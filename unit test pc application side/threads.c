/**
 * \file            thread.c
 * \brief           unit test thread src file
 */

/*
 * Copyright (c) 10/19/2024 Uri Fridman 
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
#include "threads.h"
extern unit_test_pack_t my_test;

/**
 * \brief           thread one for each testing freertos task, on stm. to test multiple peripherals simultaneously
 * \param[in]       arg: "tasting_thread_pack_t" struct pointer
 * \param[out]      return: NULL
 */
void*
testing_thread(void* arg) {

    uint16_t check; /* return error check */
    tasting_thread_pack_t* test_setting =
        (tasting_thread_pack_t*)arg;  /* for casting  arg to tasting_thread_pack_t pointer */
    unit_tasting_package_t rx_pocket; /* udp receive buffer */
    unit_tasting_package_t tx_pocket; /* udp transmit buffer */
    struct timeval t1, t2;            /* store udp massage tranmission time */

    /* copy card uniq id to transmit buffer, in order for the test request to be recived  by stm card  */
    memcpy(tx_pocket.id, my_test.commend.id, sizeof(tx_pocket.id));

    /* run tests */
    for (uint8_t peripheral_test = 0; peripheral_test < test_setting->number_of_test; peripheral_test++) {

        tx_pocket.request = peripheral_test; /* test number to be run */

        /* test is rerun for a number of iterations */
        for (uint8_t test_iteration = 0; test_iteration < NUM_OF_CHECKS; test_iteration++) {

            /* generate random data to check peripheral  */
            for (uint8_t i = 0; i < sizeof(tx_pocket.data); i++) {

                tx_pocket.data[i] = rand() % UINT8_T_RANGE;
            }

            tx_pocket.error_report = MISMATCH;
            /* request from stm card to ran a peripheral test on data and the send the data tested back */
            check = sendto(test_setting->thread_socket.task_port_socket_fd, (void*)&tx_pocket, sizeof(tx_pocket), 0,
                           (struct sockaddr*)&test_setting->thread_socket.addr,
                           sizeof(test_setting->thread_socket.addr));
            if (check == -1) {

                perror("send filed");
                exit(FAILED_TO_SEND_MASSAGE);
            }
            /* take the time of the beginning of the test */
            gettimeofday(&t1, NULL);

            /* receive from stm data tested  */
            check = recvfrom(test_setting->thread_socket.task_port_socket_fd, (void*)&rx_pocket, sizeof(rx_pocket), 0,
                             (struct sockaddr*)&test_setting->thread_socket.addr,
                             &test_setting->thread_socket.addr_size);
            if (check == -1) {

                perror("receive filed");
                exit(FAILED_TO_RECEIVE_MASSAGE);
            }

            /* compute and store test time elapsed */
            gettimeofday(&t2, NULL);
            test_setting->first_test_addr[peripheral_test].test_results_array[test_iteration].time_elapsed =
                (t2.tv_usec - t1.tv_usec);

            /* evaluating test result */
            if (rx_pocket.error_report != NO_ERROR) {

                test_setting->first_test_addr[peripheral_test]
                    .test_results_array[test_iteration]
                    .preferments.data_match = rx_pocket.error_report;

            } else {

                preferments_check(
                    &test_setting->first_test_addr[peripheral_test].test_results_array[test_iteration].preferments,
                    test_setting->first_test_addr[peripheral_test].test_request_pack.criteria, rx_pocket.data,
                    tx_pocket.data, sizeof(tx_pocket.data));
            }
        }
    }
    return NULL;
}
