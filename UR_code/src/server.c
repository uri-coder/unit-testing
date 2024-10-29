/**
 * \file            server.c
 * \brief           server function for hardware independent code src file
 */

/*
 * Copyright (c) 10/7/2024 Uri Fridman
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without restriction,
 * including without limitation the rights to use, copy, modify, merge,
 * publish, distribute, sublicense, and/or sell copies of the Software,
 * and to permit persons to whom the Software is furnished to do so,
 * subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE
 * AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * This file is part of library_name.
 *
 * Author:          Uri Fridman <optional_email@example.com>
 */

#include "server.h"

/**
 * \brief           udp receive callback.
 *                  directing packages, according to port number to their destination task.
 * \param[in]       arg: short for argument
 * \param[in]       upcb: udp_pcb, the received package was sent to.
 * \param[in]       p: received pocket.
 * \param[in]       addr: sender ip address.
 * \param[in]       addr: sender port number.
 */
void
udp_receive_callback(void* arg, struct udp_pcb* upcb, struct pbuf* p, const ip_addr_t* addr, u16_t port) {

    unit_tasting_package_t* payload_cast; /* for casting the payload to "unit_tasting_package_t" */
    uint16_t tast_type;                   /* destination port  */

    /* check if pocket is sent to "app_data_init_task"  and if so directing the payload to task */
    if (upcb->local_port == APP_INIT_PORT) {

        /* copy the sender payload in data structors belonging to the receiving task */
        memcpy(&app_commend, p->payload, p->len);

        /* drop the pocket after is copied   */
        pbuf_free(p);

        /* Store the sender information in data structors belonging to the receiving task */
        app_sender_port = port;
        ip_addr_copy(app_sender_ip, *addr);

        /* start the task */
        xSemaphoreGive(app_init_sem);
        return;
    } else {

        payload_cast = p->payload; /* cast the payload to  unit_tasting_package_t pointer */
        tast_type = upcb->local_port
                    - SERVER_OFFSET_PORT; /* compute and store the task receiving the pocket by the dentation port */

        /* check if the id sent matches the card id */
        if (memcmp(payload_cast->id, this_card_name_and_id.id, ID_SIZE * sizeof(uint32_t)) != 0) {

            pbuf_free(p);
            return;
        }

        /* Check if we finished with the last packet */
        if (p->len > sizeof(unit_tasting_package_t) || stm_test_list_array[tast_type].test_complete_flag == TEST_RUNNING
            || tast_type >= test_list_size) {
            /* Drop the packet */
            pbuf_free(p);
            return;
        }

        /* Store the sender information in data structors belonging to the receiving task */
        ip_addr_copy(stm_test_list_array[tast_type].taskX_pack.dst_ip_addr, *addr);
        stm_test_list_array[tast_type].taskX_pack.dst_port = port;
        stm_test_list_array[tast_type].taskX_pack.p = p;

        /* set the tasks test_complete_flag to test running */
        stm_test_list_array[tast_type].test_complete_flag = TEST_RUNNING;

        /* start the task */
        xSemaphoreGive(stm_test_list_array[tast_type].q);
    }
}

/**
 * \brief           send pocket to destination ip and port
 * \param[in]       pcb: pcb of the sender.
 * \param[in]       payload: data to be sent
 * \param[in]       payload_len: sizeof data to be sent
 * \param[in]       ipaddr: ip address to send to
 * \param[in]       port: port number to send to
 * \return          error massage
 */
err_t
send_packet(struct udp_pcb* pcb, const void* payload, u16_t payload_len, const ip_addr_t* ipaddr, u16_t port) {
    err_t err; /* except function error return */
    struct pbuf* p;

    /* allocate a pbuf for the payload */
    p = pbuf_alloc(PBUF_TRANSPORT, payload_len, PBUF_RAM);
    if (!p) {
        /* failed to allocate pbuf*/
        return ERR_MEM;
    }

    /* copy the payload into the pbuf */
    memcpy(p->payload, payload, payload_len);

    /* send the packet */
    err = udp_sendto(pcb, p, ipaddr, port);

    /* free the pbuf */
    pbuf_free(p);

    return err;
}

/**
 * \brief           creating sockets initialising  card uniq id and task's ruining flags
 */
void
udpServer_init(void) {

    err_t err; /* except function error return */
    /* crating sockets for testing tasks  */
    for (uint8_t i = 0; i < test_list_size; i++) {

        /* creating udp_pcb */
        stm_test_list_array[i].pcb_and_port_for_test = udp_new();
        /* configuring the udp_pcb,  port number is task array index + SERVER_OFFSET_PORT */
        err = udp_bind(stm_test_list_array[i].pcb_and_port_for_test, IP_ADDR_ANY, SERVER_OFFSET_PORT + i);

        /* configuring the udp_pcb  receive callback function to udp_receive_callback   */
        if (err == ERR_OK) {
            udp_recv(stm_test_list_array[i].pcb_and_port_for_test, udp_receive_callback, NULL);
        } else {
            udp_remove(stm_test_list_array[i].pcb_and_port_for_test);
        }
    }

    /* crating a udp_pcb for the app_data_init_task  */
    app_port = udp_new();

    err = udp_bind(app_port, IP_ADDR_ANY, APP_INIT_PORT);

    /* configuring the udp_pcb  receive callback function to udp_receive_callback   */
    if (err == ERR_OK) {
        udp_recv(app_port, udp_receive_callback, NULL);
    } else {
        udp_remove(app_port);
    }

    /* setting test_complete_flag to TAST_COMLITE  */
    for (uint8_t i = 0; i < test_list_size; i++) {

        stm_test_list_array[i].test_complete_flag = TEST_COMPLETE;
    }

    /* get card uniq id */
    this_card_name_and_id.id[0] = HAL_GetUIDw0();
    this_card_name_and_id.id[1] = HAL_GetUIDw1();
    this_card_name_and_id.id[2] = HAL_GetUIDw2();
}
