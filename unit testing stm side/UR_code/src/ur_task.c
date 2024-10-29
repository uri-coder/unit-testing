/**
 * \file            ur_tasks.c
 * \brief           tasks of hardware independent code src file
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

#include <ur_tasks.h>

//"main_task" Initialize the lwip and TCP server;

/**
 * \brief           task executed at program startup.
 *                  initialising the server.
 * \param[in]       arg: NULL
 */
void
begin_task(void const* arg) {

    MX_LWIP_Init();
    udpServer_init();

    for (;;) {

        osDelay(1);
    }
}

/**
 * \brief           task starting at the beginning of the program
 *                  excepting form linux pc  request of card and test's information and send the information
 *                  to linux pc app
 * \param[in]       arg: NULL
 */
void
app_data_init_task(void* arg) {

    err_t check; /* error return check */

    for (;;) {

        /* semaphore will be released and received pocket will be directed from  udp_receive_callback when a pocket is sent to the udp_pcb port belonging to this task*/
        if (xSemaphoreTake(app_init_sem, portMAX_DELAY) == pdFALSE) {
            continue;
        }

        /* if the sender asking for card name and uniq id */
        if (app_commend.type == ID_REQUEST) {

            check = send_packet(app_port, &this_card_name_and_id, sizeof(this_card_name_and_id), &app_sender_ip,
                                app_sender_port);

            if (check != ERR_OK) {

            	exit(UDP_SEND_FAILED);
            }

            continue;
        }
        /* check if the id sent matches the card id */
        if (memcmp(app_commend.id, this_card_name_and_id.id, ID_SIZE * sizeof(uint32_t)) != 0) {

            continue;
        }

        /* check witch request was sent and execute it */
        switch (app_commend.type) {

                /* if requesting for number of tests */
            case LIST_SIZE_REQUEST:

                /* send number of tests */
                check = send_packet(app_port, &pc_test_list_size, sizeof(pc_test_list_size), &app_sender_ip,
                                    app_sender_port);

                if (check != ERR_OK) {

                	exit(UDP_SEND_FAILED);
                }
                break;

                /* if requesting for test list member */
            case LIST_MEMBER_REQUEST:

                /* send test list member */
                check = send_packet(app_port, &pc_test_list_array[app_commend.list_index],
                                    sizeof(linux_app_test_setting_t), &app_sender_ip, app_sender_port);

                if (check != ERR_OK) {

                	exit(UDP_SEND_FAILED);
                }
                break;
        }
    }
}

/**
 * \brief           task start this function for each member of "stm_test_list_array".
 *                  executing the test function written by the developer of hardware specific test, code       
 * 
 * \param[in]       arg: for each task starting the function, this function except test_func_t instants created by the developer,
 *                  in "stm_test_list_array".
 *                  also containing test function written  by the developer of hardware specific test, code
 */
void
testing_thread(void const* arg) {

    err_t check;                                    /* error return check */
    test_func_t* thread_packeg = (test_func_t*)arg; /* for casting arg into a test_func_t pointer */
    unit_tasting_package_t* uint_test_cast; /* for casting the received payload address to unit_tasting_package_t */
    uint8_t* data;                          /* will point to data for testing */

    for (;;) {

        /* semaphore will be released and received pocket will be directed from  udp_receive_callback when a pocket is sent to the udp_pcb port belonging to that task*/
        if (xSemaphoreTake(thread_packeg->q, portMAX_DELAY) == pdFALSE) {
            continue;
        }

        uint_test_cast = thread_packeg->taskX_pack.p->payload;
        data = uint_test_cast->data;
        /* peripheral   tx to rx call the developer written, test function */
        thread_packeg->task_ptr(data, MAX_BUF_LEN, &thread_packeg->q, uint_test_cast->request,
                                &uint_test_cast->error_report);

        /* send back the data to pc after being tested */
        check = udp_sendto(thread_packeg->pcb_and_port_for_test, thread_packeg->taskX_pack.p,
                           &thread_packeg->taskX_pack.dst_ip_addr, thread_packeg->taskX_pack.dst_port);

        if (check != ERR_OK) {

            exit(UDP_SEND_FAILED);
        }
        /* free pbuf received ofter being sent */
        pbuf_free(thread_packeg->taskX_pack.p);

        /* indicate to  udp_receive_callback that task is finished  */
        thread_packeg->test_complete_flag = TEST_COMPLETE;
    }
}
