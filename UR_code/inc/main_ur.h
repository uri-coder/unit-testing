/**
 * \file            template.h
 * \brief           Template include file
 */

/*
 * Copyright (c)  10/7/2024 Uri Fridman
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

#ifndef INC_MAIN_UR_H_
#define INC_MAIN_UR_H_

#include "main.h"
//#include "stm32f7xx_hal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "FreeRTOS.h"
#include "inet.h"
#include "lwip.h"
#include "semphr.h"
#include "task.h"
#include "udp.h"

void app_data_init_task(void* arg);

/* startup code functions */
void ur_main(void);

/* server fuctions */
void udpServer_init();                                    // Initialize the UDP server
struct udp_pcb* udp_new(void);                            // Allocate a new udp_pcb structure
err_t udp_bind(struct udp_pcb*, const ip_addr_t*, u16_t); // Bind the UDP server to an IP address and port number

void udp_receive_callback(void* arg, struct udp_pcb* upcb, struct pbuf* p, const ip_addr_t* addr, u16_t port);
void begin_task(void const* arg);
void app_data_init_task(void* arg);
err_t send_packet(struct udp_pcb* pcb, const void* payload, u16_t payload_len, const ip_addr_t* ipaddr, u16_t port);

/**
 * \brief    size of test name array       
 * \hideinitializer
 */
#define TEST_NAME_SIZE           100

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
 * \brief                      sizeof error message
 * \hideinitializer
 */
#define SIZEOF_ERR_MASSAGE       30

/**
 * \brief                      communication evaluation criteria
 * \hideinitializer
 */
#define COMMUNICATION            0

/**
 * \brief                      sempre_take waite for isr to give the semaphore no more then TIME_ELAPSED_ERROR
 * \hideinitializer
 */
#define TIME_ELAPSED_ERROR       100

/**
 * \brief                     communication evaluation criteria
 * \hideinitializer
 */
#define COMMUNICATION_CRITERIA   0

/**
 * \brief                     analog evaluation criteria
 * \hideinitializer
 */
#define ADC_CRITERIA             1
/**
 * \brief                     app_data_init_task stack size
 * \hideinitializer
 */
#define APP_INIT_TASK_STACK_SIZE configMINIMAL_STACK_SIZE * 10

/**
 * \brief                     testing_thread stack size
 * \hideinitializer
 */
#define TESTIN_THREAD_STACK_SIZE configMINIMAL_STACK_SIZE * 10

/**
 * \brief                     begin_task stack size
 * \hideinitializer
 */
#define BEGIN_TASK_STACK_SIZE    configMINIMAL_STACK_SIZE * 10

/**
 * \brief                     unit test task priority
 * \hideinitializer
 */
#define UNIT_TEST_TASK_PRIORITY  -3

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
 * \brief         test error report marcos
 */
typedef enum error_report {

    NO_ERROR,         /*!< report stm do not see e problem with test */
    HAL_RETURN_ERROR, /*!< report cimsis peripheral function return error */
    ERROR_IT,         /*!< report error interrupt has jump   */
    OVER_TIME         /*!< report waited to long for peripheral to resume action */

} error_report_t;

/**
 * \brief           program returns error report marcos
 */
typedef enum main_error_report {

    PROGRAM_FINISHED_WITH_NO_ERROR, /*!< no error during execution */
    SEMAPHORE_CREATE_FAILED,        /*!< failed to create a semaphore  */
    TASK_CREATE_FAILED,             /*!< faild to create a task  */
    UDP_SEND_FAILED,                /*!< failed to send udp package  */
} main_error_report_t;

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
 * \brief           holds udp pocket send to the port of the task  
 * \note            each task have a struct like this and the udp receive callback according to the 
 *                  destination port fill the struct of the port task 
 *                      
 */
typedef struct task_package {

    ip_addr_t dst_ip_addr; /*!< ip address of sender */
    u16_t dst_port;        /*!< port of sender */

    struct pbuf* p; /*!< sender received pocket */

} task_package_t;

/**
 * \brief           holds all test task resources
 * \note            the type is send to tasting_thread for each test function
 *                     
 */
typedef struct test_func {

    void (*task_ptr)(uint8_t*, uint16_t, SemaphoreHandle_t*, uint8_t,
                     uint8_t*);            /*!< holds thread test function address written by the developer */
    SemaphoreHandle_t q;                   /*!< hold thread semaphore */
    task_package_t taskX_pack;             /*!< except sender package */
    uint8_t test_complete_flag;            /*!< signal flag if test has finish execution  */
    struct udp_pcb* pcb_and_port_for_test; /*!< udp pcb with a port number for the specific task  */

} test_func_t;

/**
 * \brief           holds task name and setting for linux testing app
 * \note            will be send to linux app to start the testing
 *                      
 */
#pragma pack(push, 1)

typedef struct linux_app_test_setting {

    uint8_t test_name[TEST_NAME_SIZE];   /*!< name of test */
    uint8_t results_evaluation_criteria; /*!< how to masseur the test successes */
    uint8_t pulling_or_it;               /*!< telling pc if test is in pulling mode(run separately)
                                          or it mode(run simultaneously) for accurate timing */
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

#pragma pack(pop)

void testing_thread(void const* arg);

#endif /* INC_MAIN_UR_H_ */
