/**
 * \file            unit_test_lib.c
 * \brief           unit test library inc  file
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
#include "unit_test_lib.h"

/**
 * \brief           generate result's file from unit test result's, "test_recurses_t" array
 * \note            this function should be only after unit test is finished
 * \param[in]       result_list_in: list of test and their results 
 * \param[in]       length: number of test's
 * \param[in]       card: card name and id   
 * \param[out]      error:error massage
 */
uint8_t
generate_result_file(test_recurses_t* result_list_in, uint16_t length, card_name_and_id_t* card) {

    FILE* fptr;                                     /* result output file pointer  */
    uint8_t txt_transfer_buffer[TXT_TRANSFER_SIZE]; /* buffer fore generating result text */
    uint8_t number_of_success_tx_rx;                /* number of perineal test success */
    uint16_t avr_time_elapsed;                      /* average test time elapsed */
    uint8_t over_time_error_num;                    /* over time error number */
    uint8_t return_error_num;                       /* HAL function returned error, error number */
    uint8_t interrupt_error_num;                    /* callback error , error number */
    uint32_t sum_of_time_elapsed;                   /* sum of single peripheral tests  time elapsed */
    int check;                                      /* function return error check  */

    /* generating the name of the file, include card name end uniq id  */
    sprintf((char*)txt_transfer_buffer,
            "unit test results of card: %s UID:  %" PRIu32 "  ::  %" PRIu32 "  ::  %" PRIu32 ".txt",
            card->board_part_number, card->id[0], card->id[1], card->id[2]);

    /* opening or creating result file */
    fptr = fopen((char*)txt_transfer_buffer, "w");
    if (fptr == NULL) {

        return GF_ERROR;
    }

    /* generating and writing in to results file results text of all the tests */
    for (uint16_t test_index = 0; test_index < length; test_index++) {

        /* initialising the counters to 0  */
        number_of_success_tx_rx = 0;
        sum_of_time_elapsed = 0;
        over_time_error_num = 0;
        return_error_num = 0;
        interrupt_error_num = 0;

        /* going to a single peripheral tests an counting the results */
        for (uint8_t test_iteration = 0; test_iteration < sizeof(result_list_in->test_results_array)
                                                              / sizeof(result_list_in->test_results_array[0]);
             test_iteration++) {

            if (result_list_in[test_index].test_results_array[test_iteration].preferments.data_match == NO_ERROR) {
                number_of_success_tx_rx++;

                sum_of_time_elapsed = sum_of_time_elapsed
                                      + result_list_in[test_index].test_results_array[test_iteration].time_elapsed;
            } else if (result_list_in[test_index].test_results_array[test_iteration].preferments.data_match
                       == OVER_TIME) {

                over_time_error_num++;

            } else if (result_list_in[test_index].test_results_array[test_iteration].preferments.data_match
                       == HAL_RETURN_ERROR) {

                return_error_num++;

            } else if (result_list_in[test_index].test_results_array[test_iteration].preferments.data_match
                       == ERROR_IT) {

                interrupt_error_num++;
            }
        }

        /* if ther ware success devid the sum of time elapsed by number of success, else put marco NO_SUCCESS */
        if (number_of_success_tx_rx > 0) {
            avr_time_elapsed = sum_of_time_elapsed / number_of_success_tx_rx;
        } else {
            avr_time_elapsed = NO_SUCCESS;
        }
        /* generate result text for a single perineal  */
        sprintf(
            (char*)txt_transfer_buffer,
            "test %s data match = %u/%u.  average time elapsed =  %6.3fms. over time error = %u/%u. error interrupt = "
            "%u/%u. HAL return error = %u/%u.\n\n ",
            &result_list_in[test_index].test_request_pack.test_name[1], (unsigned int)NUM_OF_CHECKS,
            number_of_success_tx_rx, (float)avr_time_elapsed / US_TO_MS, (unsigned int)NUM_OF_CHECKS,
            over_time_error_num, (unsigned int)NUM_OF_CHECKS, interrupt_error_num, (unsigned int)NUM_OF_CHECKS,
            return_error_num);

        /* write to file result text for a single perineal  */
        fputs((char*)txt_transfer_buffer, fptr);
    }

    /* close results file */
    check = fclose(fptr);
    if (check != 0) {
        return GF_ERROR;
    }

    return ERROR_OK;
}

/**
 * \brief           take 2 buffers and comparing their data base on test evaluation criteria
 * \note            this function should be only after the single test that beeing evaluated is completed
 * \param[in]       result_out: list of test and their results 
 * \param[in]       tast_criteria:  test evaluation criteria
 * \param[in]       buffer_compare1: buffer 1 to compare with buffer 2 
 * \param[in]       buffer_compare1: buffer 2 to compare with buffer 1    
 * \param[in]       sizeof_buffers:  size of buffers to be compared    
 */
void
preferments_check(test_preferments_results_t* result_out, uint8_t tast_criteria, const uint8_t* buffer_compare1,
                  const uint8_t* buffer_compare2, uint16_t sizeof_buffers) {

    /* compere results by test criteria */
    switch (tast_criteria) {

        case COMMUNICATION_criteria:

            if (memcmp(buffer_compare2, buffer_compare1, sizeof_buffers) != 0) {

                result_out->data_match = MISMATCH;

            } else {
                result_out->data_match = NO_ERROR;
            }

            break;

        case ADC_criteria:

            if (buffer_compare2[0] >= buffer_compare1[0] - ADC_SUCCESS_ERROR_RANGE
                && buffer_compare2[0] <= buffer_compare1[0] + ADC_SUCCESS_ERROR_RANGE) {

                result_out->data_match = MISMATCH;

            } else {
                result_out->data_match = NO_ERROR;
            }

            break;
        default: break;
    }
}

/**
 * \brief           initialise the testing thread function written by the user of the library
 * \note            this function needs to be called at the beginning of unit test code
 * \param[in]       handel_in: library structure with the testing thread pointer to be initialised
 * \param[in]       testing_tread_in:  testing thread   
 */
void
testing_thread_function_init(unit_test_pack_t* handel_in, void* (*testing_tread_in)(void*)) {

    handel_in->testing_tread = testing_tread_in;
}

/**
 * \brief           crating the socket for communicating with stm card to get name id and test information
 * \note            the function arrange in this library file in the same order that they need to be called
 *                  do not call this function befor privies are called
 * \param[in]       handel_in: library structure with the testing thread pointer to be initialised
 * \param[out]      error:error massage
 */
uint8_t
create_app_init_socket(unit_test_pack_t* handel_in) {

    /* create a socket */
    handel_in->sockfd = socket(AF_INET, SOCK_DGRAM, 0);
    if (handel_in->sockfd == -1) {

        return SOCKET_FAILED;
    }

    /* initialise socket fildes */
    memset(&handel_in->addr, '\0', sizeof(handel_in->addr));
    handel_in->addr.sin_family = AF_INET;
    handel_in->addr.sin_port = htons(APP_INIT_PORT);
    handel_in->addr.sin_addr.s_addr = inet_addr(STM_IP);
    handel_in->addr_size = sizeof(handel_in->addr);

    return ERROR_OK;
}

/**
 * \brief           requesting stm card name and uniq id, from stm card
 * \note            the function arrange in this library file in the same order that they need to be called
 *                  do not call this function befor privies are called
 * \param[in]       handel_in: library structure with the with the resources to communicate with card
 * \param[out]      error:error massage
 */
uint8_t
request_card_id(unit_test_pack_t* handel_in) {

    int check; /* check function return error */

    /* request card name and id from stm card */
    check = sendto(handel_in->sockfd, (void*)&handel_in->commend, sizeof(handel_in->commend), 0,
                   (struct sockaddr*)&handel_in->addr, sizeof(handel_in->addr));
    if (check == -1) {

        return FAILED_TO_SEND_MASSAGE;
    }
    /* receive card name and id from stm card */
    check = recvfrom(handel_in->sockfd, (void*)&handel_in->card_id_in, sizeof(handel_in->card_id_in), 0,
                     (struct sockaddr*)&handel_in->addr, &handel_in->addr_size);
    if (check == -1) {

        return FAILED_TO_RECEIVE_MASSAGE;
    }

    /* copy  card uniq id to be used later when communicating with card */
    memcpy(handel_in->commend.id, handel_in->card_id_in.id, sizeof(handel_in->card_id_in.id));

    return ERROR_OK;
}

/**
 * \brief           printing stm card, part number and uniq id
 * \note            the function arrange in this library file in the same order that they need to be called
 *                  do not call this function befor privies are called
 * \param[in]       handel_in: library structure with the with the name and id of the stm card
 * \param[out]      error:error massage
 */
uint8_t
print_card_name_and_id(unit_test_pack_t* handel_in) {

    printf("card part number: %s\n", handel_in->card_id_in.board_part_number);
    printf("card UID %" PRIu32 "  ::  %" PRIu32 "  ::  %" PRIu32 " \n", handel_in->card_id_in.id[0],
           handel_in->card_id_in.id[1], handel_in->card_id_in.id[2]);

    return ERROR_OK;
}

/**
 * \brief           requesting from stm the number of tests to be run
 * \note            the function arrange in this library file in the same order that they need to be called
 *                  do not call this function befor privies are called
 * \param[in]       handel_in: library structure storing the number of test
 * \param[out]      error:error massage
 */
uint8_t
request_test_list_size(unit_test_pack_t* handel_in) {

    int check; /* check function return error */

    /* request number of tests to be run, from stm card */
    handel_in->commend.type = LIST_SIZE_REQUEST;
    check = sendto(handel_in->sockfd, (void*)&handel_in->commend, sizeof(handel_in->commend), 0,
                   (struct sockaddr*)&handel_in->addr, sizeof(handel_in->addr));
    if (check == -1) {

        return FAILED_TO_SEND_MASSAGE;
    }

    /* receive number of tests to be run, from stm card */
    check = recvfrom(handel_in->sockfd, (void*)&handel_in->list_len, sizeof(handel_in->list_len), 0,
                     (struct sockaddr*)&handel_in->addr, &handel_in->addr_size);
    if (check == -1) {

        return FAILED_TO_RECEIVE_MASSAGE;
    }

    return ERROR_OK;
}

/**
 * \brief           allocating "test_recurses_t" list, one for each test
 * \note            the function arrange in this library file in the same order that they need to be called
 *                  do not call this function befor privies are called
 * \param[in]       handel_in: library structure with the pointer that point to "test_recurses_t" list 
 *                  and also th number of tests
 * \param[out]      error:error massage
 */
uint8_t
allocate_test_list_mem(unit_test_pack_t* handel_in) {

    handel_in->test_raw_results_list = (test_recurses_t*)malloc(sizeof(test_recurses_t) * handel_in->list_len);
    if (handel_in->test_raw_results_list == NULL) {
        return MALLOC_FAILED;
    }

    return ERROR_OK;
}

/**
 * \brief           fills the "test_recurses_t" test list with test name's and settings received from stm card under test
 * \note            the function arrange in this library file in the same order that they need to be called
 *                  do not call this function befor privies are called
 * \param[in]       handel_in: library structure with the pointer that point to "test_recurses_t" list 
 *                  and also the socket resources, required to communicate with stm card
 * \param[out]      error:error massage
 */
uint8_t
read_and_fill_test_list_from_stm(unit_test_pack_t* handel_in) {

    int check; /* check function return error */
    handel_in->commend.type =
        LIST_MEMBER_REQUEST;   /* initialising app test init commend, type segment, with request a test member */
    handel_in->thread_num = 0; /* set to 0 struct member holding the number  testing thread to be run */

    /* requesting and receiving tests information from stm card */
    for (uint16_t i = 0; i < handel_in->list_len; i++) {

        handel_in->commend.list_index = i; /* requesting member "i" from the list */

        /* request test to be run, from stm card */
        check = sendto(handel_in->sockfd, (void*)&handel_in->commend, sizeof(handel_in->commend), 0,
                       (struct sockaddr*)&handel_in->addr, sizeof(handel_in->addr));
        if (check == -1) {
            return FAILED_TO_SEND_MASSAGE;
        }

        /* receive test to be run, from stm card */
        check = recvfrom(handel_in->sockfd, (void*)&handel_in->test_raw_results_list[i].test_request_pack,
                         sizeof(linux_app_test_setting_t), 0, (struct sockaddr*)&handel_in->addr,
                         &handel_in->addr_size);
        if (check == -1) {

            return FAILED_TO_RECEIVE_MASSAGE;
        }

        /* count number of thread's every time the name of test begin with "#"
         (indicating the lest test belonging to task (or thread on pc app side) )*/
        if (handel_in->test_raw_results_list[i].test_request_pack.test_name[0] == '#') {
            handel_in->thread_num++;
        }
    }

    return ERROR_OK;
}

/**
 * \brief           allocating pthread_t list, for testing thread 
 * \note            the function arrange in this library file in the same order that they need to be called
 *                  do not call this function befor privies are called
 * \param[in]       handel_in: library structure with the pointer that point to pthread_t list 
 * \param[out]      error:error massage
 */
uint8_t
allocating_testing_threads_handles(unit_test_pack_t* handel_in) {

    handel_in->test_thread_input_list = (tasting_thread_pack_t*)malloc(sizeof(tasting_thread_pack_t)
                                                                       * handel_in->thread_num);
    if (handel_in->test_thread_input_list == NULL) {
        return MALLOC_FAILED;
    }

    handel_in->thread_handel_list = (pthread_t*)malloc(sizeof(pthread_t) * handel_in->thread_num);
    if (handel_in->thread_handel_list == NULL) {
        return MALLOC_FAILED;
    }

    handel_in->testing_thread_attr = (pthread_attr_t*)malloc(sizeof(pthread_attr_t) * handel_in->thread_num);
    if (handel_in->thread_handel_list == NULL) {
        return MALLOC_FAILED;
    }
    return ERROR_OK;
}

/**
 * \brief           creating the socket's and initialising thread's settings
 * \note            the function arrange in this library file in the same order that they need to be called
 *                  do not call this function befor privies are called
 * \param[in]       handel_in: library structure with thread input pointer, pointing to thread input list
 * \param[out]      error:error massage
 */
uint8_t
initialising_testing_threads_input(unit_test_pack_t* handel_in) {

    test_recurses_t* temp_ptr =
        handel_in->test_raw_results_list; /* temporary pointer used to compute the start test address of a thread */
    uint8_t index;                        /* counts the number of test's of a thread */

    /* initializing threads threads input packages */
    for (uint16_t thread_handle_num = 0; thread_handle_num < handel_in->thread_num; thread_handle_num++) {

        index = 0; /* setting  to 0, the number of test's of a thread  */

        /* counting the number of test belonging to a thread  */
        while (temp_ptr[index].test_request_pack.test_name[0] != '#') {
            index++;
        }

        index++;

        /* init the number of test  */
        handel_in->test_thread_input_list[thread_handle_num].number_of_test = index;

        /* init the start address of the tests */
        handel_in->test_thread_input_list[thread_handle_num].first_test_addr = temp_ptr;

        /* creating socket for thread */
        handel_in->test_thread_input_list[thread_handle_num].thread_socket.task_port_socket_fd = socket(AF_INET,
                                                                                                        SOCK_DGRAM, 0);
        if (handel_in->test_thread_input_list[thread_handle_num].thread_socket.task_port_socket_fd == -1) {
            return SOCKET_FAILED;
        }

        /* init thread socket fields  */
        memset(&handel_in->test_thread_input_list[thread_handle_num].thread_socket.addr, '\0', sizeof(handel_in->addr));
        handel_in->test_thread_input_list[thread_handle_num].thread_socket.addr.sin_family = AF_INET;

        /* setting socket port value to the thread number offset by  SERVER_OFFSET_PORT*/
        handel_in->test_thread_input_list[thread_handle_num].thread_socket.addr.sin_port = htons(SERVER_OFFSET_PORT
                                                                                                 + thread_handle_num);
        handel_in->test_thread_input_list[thread_handle_num].thread_socket.addr.sin_addr.s_addr = inet_addr(STM_IP);
        handel_in->test_thread_input_list[thread_handle_num].thread_socket.addr_size = sizeof(struct sockaddr_in);

        /* initializing the next thread  tests start address */
        temp_ptr = &temp_ptr[index];
    }

    return ERROR_OK;
}

/**
 * \brief           creating and joining test threads
 * \note            the function arrange in this library file in the same order that they need to be called
 *                  do not call this function befor privies are called
 * \param[in]       handel_in: library structure with  pointer, pointing to thread's pthread_t list
 * \param[out]      error:error massage
 */
uint8_t
creating_and_joining_testing_threads(unit_test_pack_t* handel_in) {

    int check; /* check function return error */

    /* creating and joining threads */
    for (uint16_t thread_set_stack_index = 0; thread_set_stack_index < handel_in->thread_num;
         thread_set_stack_index++) {

        /* setting thread stack size */
        check = pthread_attr_init(&handel_in->testing_thread_attr[thread_set_stack_index]);
        if (check == -1) {

            return THREAD_ATTR_INIT_FAILED;
        }

        check = pthread_attr_setstacksize(&handel_in->testing_thread_attr[thread_set_stack_index],
                                          TESTIN_THREAD_STACK_SIZE);
        if (check == -1) {

            return THREAD_ATTR_SET_STACK_SIZE_FAILED;
        }

        /* setting thread guard size */
        check = pthread_attr_setguardsize(&handel_in->testing_thread_attr[thread_set_stack_index], _SC_PAGESIZE);
        if (check == -1) {

            return THREAD_ATTR_SET_STACK_SIZE_FAILED;
        }
    }

    /* if test run in pulling mode join the threads separately, run them separately */
    for (uint16_t thread_create_index = 0; thread_create_index < handel_in->thread_num; thread_create_index++) {

        if (handel_in->test_thread_input_list[thread_create_index].first_test_addr->test_request_pack.pulling_or_it
            == PULLING_MODE) {
            check = pthread_create(&handel_in->thread_handel_list[thread_create_index],
                                   &handel_in->testing_thread_attr[thread_create_index], handel_in->testing_tread,
                                   &handel_in->test_thread_input_list[thread_create_index]);
            if (check != 0) {

                return THREAD_CREATE_FAILED;
            }
            check = pthread_join(handel_in->thread_handel_list[thread_create_index], NULL);
            if (check != 0) {

                return THREAD_CREATE_FAILED;
            }
        }
    }

    /* if test run in interrupt mode join the threads together, run them simultaneously */
    for (uint16_t thread_create_index = 0; thread_create_index < handel_in->thread_num; thread_create_index++) {

        if (handel_in->test_thread_input_list[thread_create_index].first_test_addr->test_request_pack.pulling_or_it
            == IT_MODE) {
            check = pthread_create(&handel_in->thread_handel_list[thread_create_index],
                                   &handel_in->testing_thread_attr[thread_create_index], handel_in->testing_tread,
                                   &handel_in->test_thread_input_list[thread_create_index]);
            if (check != 0) {

                return THREAD_CREATE_FAILED;
            }
        }
    }

    /* joining  threads */
    for (uint16_t thread_join_index = 0; thread_join_index < handel_in->thread_num; thread_join_index++) {

        if (handel_in->test_thread_input_list[thread_join_index].first_test_addr->test_request_pack.pulling_or_it
            == IT_MODE) {

            check = pthread_join(handel_in->thread_handel_list[thread_join_index], NULL);
            if (check != 0) {

                return THREAD_CREATE_FAILED;
            }
        }
    }

    return ERROR_OK;
}

/**
 * \brief           closing all the socket used
 * \note            the function arrange in this library file in the same order that they need to be called
 *                  do not call this function befor privies are called
 * \param[in]       handel_in: library structure with  pointer, pointing to socket fd list
 * \param[out]      error:error massage
 */
void
close_test_threads_sockets(unit_test_pack_t* handel_in) {

    for (uint16_t thread_socket = 0; thread_socket < handel_in->thread_num; thread_socket++) {

        close(handel_in->test_thread_input_list[thread_socket].thread_socket.task_port_socket_fd);
    }

    close(handel_in->sockfd);
}

/**
 * \brief           free all the memory allocated
 * \note            the function arrange in this library file in the same order that they need to be called
 *                  do not call this function befor privies are called
 * \param[in]       handel_in: library structure with  pointers to all the memoirs used in unit test
 * \param[out]      error:error massage
 */
void
free_allocated_recurses(unit_test_pack_t* handel_in) {

    free(handel_in->test_thread_input_list);
    handel_in->test_thread_input_list = NULL;

    free(handel_in->thread_handel_list);
    handel_in->thread_handel_list = NULL;

    free(handel_in->test_raw_results_list);
    handel_in->test_raw_results_list = NULL;

    free(handel_in->testing_thread_attr);
    handel_in->testing_thread_attr = NULL;
}
