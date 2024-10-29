/**
 * \file            main_ur.c
 * \brief           stm side hardware independent src file
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

#include "main_ur.h"
#include "extern_template_variables.h"

/* variables for app init task */
/**
 * \brief        semaphore to start app_init task
 * 
 */
SemaphoreHandle_t app_init_sem;

/**
 * \brief           pcb belong to app_init task
 */
struct udp_pcb* app_port;

/**
 * \brief           Holds and except app initialisation commend, sent to app_init_task
 */
app_data_init_commend_t app_commend;

/**
 * \brief          Holds and except remote prot number, sent to app_init_task
 */
u16_t app_sender_port;

/**
 * \brief           Holds and except remote ip address, sent to app_init_task
 */
ip_addr_t app_sender_ip;

/**
 * \brief           main function start up the software create the tasks and semaphore's
 */
void
ur_main() {

    BaseType_t err;

    /* creating semaphores for each testing task  */
    for (int i = 0; i < test_list_size; i++) {

        stm_test_list_array[i].q = xSemaphoreCreateBinary();

        if (stm_test_list_array[i].q == NULL) {
             exit(SEMAPHORE_CREATE_FAILED);
        }
    }

    /* creating semaphore for "app_data_init_task" */
    app_init_sem = xSemaphoreCreateBinary();

    if (app_init_sem == NULL)

    {
        exit(SEMAPHORE_CREATE_FAILED);
    }

    /* create "begin_task" */
    err = xTaskCreate((TaskFunction_t)begin_task, "begin_task", BEGIN_TASK_STACK_SIZE, NULL, 0, NULL);

    if (err != pdPASS) {
        exit(TASK_CREATE_FAILED);
    }

    /* create "app_data_init_task" */
    err = xTaskCreate((TaskFunction_t)app_data_init_task, "app_data_init_task", APP_INIT_TASK_STACK_SIZE, NULL,
                      UNIT_TEST_TASK_PRIORITY, NULL);

    if (err != pdPASS) {

    	exit(TASK_CREATE_FAILED);
    }

    /* crate a task's from function "testing_thread". each task, except a member from "stm_test_list_array" */
    for (uint16_t i = 0; i < test_list_size; i++) {

        err = xTaskCreate((TaskFunction_t)testing_thread, "test_task", TESTIN_THREAD_STACK_SIZE,
                          &stm_test_list_array[i], UNIT_TEST_TASK_PRIORITY, NULL);
        if (err != pdPASS) {
        	exit(TASK_CREATE_FAILED);
        }
    }

    /* start freertos */
    vTaskStartScheduler();
}
