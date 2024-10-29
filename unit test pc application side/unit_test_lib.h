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
#ifndef UNIT_TEST_LIB
#define UNIT_TEST_LIB
#define _XOPEN_SOURCE 600
#include <arpa/inet.h>
#include <errno.h>
#include <inttypes.h>
#include <limits.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/time.h>
#include <sys/types.h>
#include <time.h>
#include <unistd.h>
/**
 * \brief    size of test name array       
 * \hideinitializer
 */
#define test_name_SIZE           100

/**
 * \brief    test data buff size      
 * \hideinitializer
 */
#define MAX_BUF_LEN              100

/**
 * \brief          test's first port increment for each task 
 * \hideinitializer
 */
#define SERVER_OFFSET_PORT       50007

/**
 * \brief           test finished execution  
 * \hideinitializer
 */
#define TEST_COMPLETE            0

/**
 * \brief            test is running
 * \hideinitializer
 */
#define TEST_RUNNING             1

/**
 * \brief                    app initialisation task port
 * \hideinitializer
 */
#define APP_INIT_PORT            50006

/**
 * \brief                    payload max size 
 * \hideinitializer
 */
#define MAX_BUF_SIZE             200

/**
 * \brief                    size of array holding stm part number 
 * \hideinitializer
 */
#define PART_NUM_SIZE            100

/**
 * \brief                     size of id uint32_t array
 * \hideinitializer
 */
#define ID_SIZE                  3
/**
 * \brief                     request board uniq id
 * \hideinitializer
 */
#define ID_REQUEST               0

/**
 * \brief                      request size of task list
 * \hideinitializer
 */
#define LIST_SIZE_REQUEST        1

/**
 * \brief                      request specific task in task list
 * \hideinitializer
 */
#define LIST_MEMBER_REQUEST      2

/**
 * \brief                      number of test iterations
 * \hideinitializer
 */
#define NUM_OF_CHECKS            60

/**
 * \brief                     communication evaluation criteria
 * \hideinitializer
 */
#define COMMUNICATION_criteria   0

/**
 * \brief                     analog evaluation criteria 
 * \hideinitializer
 */
#define ADC_criteria             1

/**
 * \brief                     analog criteria error range still considered successful
 * \hideinitializer
 */
#define ADC_SUCCESS_ERROR_RANGE  200

/**
 * \brief                     test success
 * \hideinitializer
 */
#define MASSAGE_OK               0

/**
 * \brief                     test failed
 * \hideinitializer
 */
#define MASSAGE_ERROR            1

/**
 * \brief                     size of buffer for generating text for text file
 * \hideinitializer
 */
#define TXT_TRANSFER_SIZE        1000

/**
 * \brief                     stm card under test ip address
 * \hideinitializer
 */
#define STM_IP                   "192.168.8.2"

/**
 * \brief                     testing thread stack size
 * \hideinitializer
 */
#define TESTIN_THREAD_STACK_SIZE 4096

/**
 * \brief                     time elapsed value in case ther is no successful tests
 * \hideinitializer
 */
#define NO_SUCCESS               0

/**
 * \brief                     test running in pulling mode
 * \hideinitializer
 */
#define PULLING_MODE             0

/**
 * \brief                     test running in IT mode
 * \hideinitializer
 */
#define IT_MODE                  1

/**
 * \brief                     divided by to convert us to ms
 * \hideinitializer
 */
#define US_TO_MS                 1000.0

/**
 * \brief     return marcos to indicate error's     
 */
typedef enum lib_error_massages {

    ERROR_OK,                          /*!< no error */
    SOCKET_FAILED,                     /*!< failed to create socket */
    FAILED_TO_SEND_MASSAGE,            /*!< failed to transmit udp pocket */
    FAILED_TO_RECEIVE_MASSAGE,         /*!< failed to receive udp pocket */
    MALLOC_FAILED,                     /*!< malloc failed. failed to allocate memory    */
    THREAD_CREATE_FAILED,              /*!< failed to create a thread   */
    THREAD_ATTR_INIT_FAILED,           /*!< thread_attr_init failed   */
    THREAD_ATTR_SET_STACK_SIZE_FAILED, /*!< thread_attr set failed */
    GF_ERROR                           /*!< failed to generate results file */
} lib_error_massages_t;

/**
 * \brief           error report marcos
 */
typedef enum error_report {

    NO_ERROR,         /*!< report stm do not see e problem with test */
    HAL_RETURN_ERROR, /*!< report cimsis peripheral function returned error */
    ERROR_IT,         /*!< report error interrupt has jump   */
    OVER_TIME,        /*!< report waited to long for peripheral to resume action */
    MISMATCH          /*!< date received from stm card doesn't match to the data sent to it*/

} error_report_t;

/**
 * \brief           unit testing testing communication protocol struct
 */
#pragma pack(push, 1)

typedef struct uint_tasting_package {

    uint32_t id[ID_SIZE];      /*!< holds stm mcu uniq id */
    uint8_t request;           /*!< holds app request what test in a task belong to a particular port to run. */
    uint8_t data[MAX_BUF_LEN]; /*!< data for testing */
    uint8_t error_report;      /*!< stm32 fill this variable with report about transmission errors */

} unit_tasting_package_t;

#pragma pack(pop)

/**
 * \brief           holds task name and setting for linux testing app
 * \note            will be send to linux app to start the testing
 *                      
 */
#pragma pack(push, 1)
/**
 * \brief           holds task name and setting for linux testing app
 * \note            will be send to linux app to start the testing
 *                      
 */
#pragma pack(push, 1)

typedef struct linux_app_test_setting {

    uint8_t test_name[test_name_SIZE]; /*!< name of test */
    uint8_t criteria;                  /*!< how to masseur the test successes */
    uint8_t pulling_or_it; /*!< telling pc if test is in pulling mode(run separately) or it mode(run simultaneously) for accurate timing */
} linux_app_test_setting_t;

#pragma pack(pop)

/**
 * \brief           unit test system. app test initialisation communication  protocol struct
 * \note            app get from the card, card information and tests information
 *                      
 */
#pragma pack(push, 1)

typedef struct app_data_init_commend {

    uint32_t id[ID_SIZE]; /*!< card uniq id */
    uint8_t type;         /*!< commend */
    uint16_t list_index;  /*!< test request, and for other use */
} app_data_init_commend_t;

#pragma pack(pop)

/**
 * \brief            name of evaluation board under test and is uniq id
 */
#pragma pack(push, 1)

typedef struct card_name_and_id {

    uint32_t id[ID_SIZE];                     /*!< uniq id */
    uint8_t board_part_number[PART_NUM_SIZE]; /*!< name of board */

} card_name_and_id_t;

/**
 * \brief            data matching preferments
 */
#pragma pack(pop)

typedef struct test_preferments_results {

    uint8_t data_match; /*!< data matching preferments */

} test_preferments_results_t;

/**
 * \brief            test results.
 */
typedef struct single_test_result {

    test_preferments_results_t preferments; /*!< data matching preferments */
    uint16_t time_elapsed;                  /*!< test time elapsed */

} single_test_result_t;

/**
 * \brief            data structure for test.storing test information and results
 */
typedef struct test_recurses {

    linux_app_test_setting_t test_request_pack;             /*!< test information */
    single_test_result_t test_results_array[NUM_OF_CHECKS]; /*!< test results */

} test_recurses_t;

/**
 * \brief            everting needed for a socket. for unit testing tasks 
 */
typedef struct socket_handle {

    int task_port_socket_fd; /*!< task socket port fd */
    struct sockaddr_in addr; /*!< task socket sockaddr_in */
    socklen_t addr_size;     /*!< task socket  socklen_t */

} socket_handle_t;

/**
 * \brief            data structure sent to thread containing all thread instruction
 *                   and variables to store test results.
 */
typedef struct tasting_thread_pack {

    test_recurses_t*
        first_test_addr;    /*!< holds thread test list address offset. the first peripheral test to be executed  */
    uint8_t number_of_test; /*!< number of tests for thread  to run, 
                                      starting from the test at "first_test_addr" and incrementing */
    socket_handle_t thread_socket; /*!< thread socket(for communicating with stm) variables */

} tasting_thread_pack_t;

/**
 * \brief            main library structure, storing everting needed in order to use the library
 * \note             need to declared in order to use the library,
 *                   library functions except it as input 
 */
typedef struct unit_test_pack {

    test_recurses_t* test_raw_results_list; /*!< point to test list array. dynamicly allocated */
    tasting_thread_pack_t*
        test_thread_input_list; /*!< point to list of tasting_thread_pack_t excepted by the unit tasting thread.
                                                     dynamicly allocated */
    pthread_t*
        thread_handel_list; /*!< point to testing thread pthread_t,needed in order to create threads . dynamicly allocated */
    card_name_and_id_t card_id_in; /*!< holding card uniq id, received from stm card under test */
    struct sockaddr_in addr; /*!< sockaddr_in for the init socket excepting card id test list and other information,
                                                    required for unit testing */
    int sockfd;              /*!< sockfd for the init socket excepting card id test list and other information,
                                                    required for unit testing */
    socklen_t addr_size;     /*!< socklen_t  for the init socket excepting card id test list and other information,
                                                    required for unit testing */
    uint16_t list_len;       /*!< hold's the number of tests to be executed  */
    app_data_init_commend_t commend;     /*!< used to send commend's to except information about stm card
                                                    and about the tests */
    uint8_t thread_num;                  /*!< stor the number of testing threads 
                                                     (a thread per each stm freertos testing task) */
    void* (*testing_tread)(void*);       /*!< point to testing thread function (created by the library user) */
    pthread_attr_t* testing_thread_attr; /*!< point to testing thread pthread_attr_t,needed in order to configure, 
                                                    threads stack size.dynamicly allocated */
} unit_test_pack_t;

void preferments_check(test_preferments_results_t* result_out, uint8_t tast_criteria, const uint8_t* buffer_compare1,
                       const uint8_t* buffer_compare2, uint16_t sizeof_buffers);
uint8_t generate_result_file(test_recurses_t* result_list_in, uint16_t length, card_name_and_id_t* card);
void testing_thread_function_init(unit_test_pack_t* handel_in, void* (*testing_tread_in)(void*));
uint8_t create_app_init_socket(unit_test_pack_t* handel_in);
uint8_t request_card_id(unit_test_pack_t* handel_in);
uint8_t print_card_name_and_id(unit_test_pack_t* handel_in);
uint8_t request_test_list_size(unit_test_pack_t* handel_in);

uint8_t allocate_test_list_mem(unit_test_pack_t* handel_in);
uint8_t read_and_fill_test_list_from_stm(unit_test_pack_t* handel_in);

uint8_t allocating_testing_threads_handles(unit_test_pack_t* handel_in);

uint8_t initialising_testing_threads_input(unit_test_pack_t* handel_in);

uint8_t creating_and_joining_testing_threads(unit_test_pack_t* handel_in);
void close_test_threads_sockets(unit_test_pack_t* handel_in);
void free_allocated_recurses(unit_test_pack_t* handel_in);

#endif /*  UNIT_TEST_LIB */