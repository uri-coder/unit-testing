/**
 * \file            uart_test.h
 * \brief           hardware specific testing code file for testing uart
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

#include "uart_test.h"
#include "extern_template_variables.h"
#include "main_ur.h"
#include "test_funck.h"

/**
 * \brief           testing uart 1 and 2 peripherals
 *                  the function will be called by "testing_thread"
 * \param[in]       data: data received from testing app vie udp the test hardware
 *                  to do a data match compare
 * \param[in]       sizeof_data: size of data
 * \param[in]       interrupt_sem: semaphore in case you need to waite for an interrupt
 * \param[in]       test_select: select what test in the function to run
 * \param[in]       error_report: report to pc about test error
 */

void
UART_1_2_task(uint8_t* data, uint16_t sizeof_data, SemaphoreHandle_t* interrupt_sem, uint8_t test_select,
              uint8_t* error_report) {

    HAL_StatusTypeDef status; /* error return check */
    *error_report = NO_ERROR; /* initalized to  NO_ERROR if an error is accruing it will change it */

    /* run task test requested by pc */
    switch (test_select) {

        case 0: /* uart1 TI rx uart2 TI tx */

            /* peripheral receive testing data  */
            status = HAL_UART_Receive_IT(&huart1, data, sizeof_data);
            if (status != HAL_OK) {

                *error_report = HAL_RETURN_ERROR;

                return;
            }

            /* peripheral transmit testing data  */
            status = HAL_UART_Transmit_IT(&huart2, data, sizeof_data);
            if (status != HAL_OK) {
                /* abort transmission in case HAL returns error so callback isr function wont release semaphore */
                HAL_UART_Abort_IT(&huart1);
                *error_report = HAL_RETURN_ERROR;
            }

            break;

        case 1: /* uart2 TI rx uart1 TI tx */

            /* peripheral receive testing data  */
            status = HAL_UART_Receive_IT(&huart2, data, sizeof_data);
            if (status != HAL_OK) {

                *error_report = HAL_RETURN_ERROR;
                return;
            }
            /* peripheral transmit testing data  */
            status = HAL_UART_Transmit_IT(&huart1, data, sizeof_data);
            if (status != HAL_OK) {
                /* abort transmission in case HAL returns error so callback isr function wont release semaphore */
                HAL_UART_Abort_IT(&huart2);
                *error_report = HAL_RETURN_ERROR;
            }

            break;

        default: break;
    }

    /* waite for rx interrupt to release the semaphore, if to match time has passed report over time error  */
    if (xSemaphoreTake(*interrupt_sem, TIME_ELAPSED_ERROR) == pdFALSE && *error_report == NO_ERROR) {

        /* abort transmission in case HAL returns error so callback isr function wont release semaphore */
        HAL_UART_Abort_IT(&huart1);
        xSemaphoreTake(*interrupt_sem, TIME_ELAPSED_ERROR);

        HAL_UART_Abort_IT(&huart2);
        xSemaphoreTake(*interrupt_sem, TIME_ELAPSED_ERROR);

        *error_report = OVER_TIME;
    }
}

/**
 * \brief           testing uart 5 and 4 peripherals
 *                  the function will be called by "testing_thread"
 * \param[in]       data: data received from testing app vie udp the test hardware
 *                  to do a data match compare
 * \param[in]       sizeof_data: size of data
 * \param[in]       interrupt_sem: semaphore in case you need to waite for an interrupt
 * \param[in]       test_select: select what test in the function to run
 * \param[in]       error_report: report to pc about test error
 */
void
UART_4_5_task(uint8_t* data, uint16_t sizeof_data, SemaphoreHandle_t* interrupt_sem, uint8_t test_select,
              uint8_t* error_report) {

    HAL_StatusTypeDef status; /* error return check */
    *error_report = NO_ERROR; /* initalized to  NO_ERROR if an error is accruing it will change it */

    /* run task test requested by pc */
    switch (test_select) {

        case 0: /* uart4 TI rx uart5 TI tx */

            /* peripheral receive testing data  */
            status = HAL_UART_Receive_IT(&huart4, data, sizeof_data);
            if (status != HAL_OK) {

                *error_report = HAL_RETURN_ERROR;

                return;
            }

            /* peripheral transmit testing data  */
            status = HAL_UART_Transmit_IT(&huart5, data, sizeof_data);
            if (status != HAL_OK) {
                /* abort transmission in case HAL returns error so callback isr function wont release semaphore */
                HAL_UART_Abort_IT(&huart4);
                *error_report = HAL_RETURN_ERROR;
            }
            break;

        case 1: /* uart5 TI rx uart4 TI tx */

            /* peripheral receive testing data  */
            status = HAL_UART_Receive_IT(&huart5, data, sizeof_data);
            if (status != HAL_OK) {

                *error_report = HAL_RETURN_ERROR;
                return;
            }
            /* peripheral transmit testing data  */
            status = HAL_UART_Transmit_IT(&huart4, data, sizeof_data);
            if (status != HAL_OK) {
                /* abort transmission in case HAL returns error so callback isr function wont release semaphore */
                HAL_UART_Abort_IT(&huart5);
                *error_report = HAL_RETURN_ERROR;
            }

            break;

        case 2: /* uart4 DMA rx uart5 DMA tx */

            /* peripheral receive testing data  */
            status = HAL_UART_Receive_DMA(&huart4, data, sizeof_data);
            if (status != HAL_OK) {

                *error_report = HAL_RETURN_ERROR;
                return;
            }

            /* peripheral transmit testing data  */
            status = HAL_UART_Transmit_DMA(&huart5, data, sizeof_data);
            if (status != HAL_OK) {
                /* abort transmission in case HAL returns error so callback isr function wont release semaphore */
                HAL_UART_Abort_IT(&huart4);
                *error_report = HAL_RETURN_ERROR;
            }

            break;

        case 3: /* uart5 DMA rx uart4 DMA tx */

            /* peripheral receive testing data  */
            status = HAL_UART_Receive_DMA(&huart5, data, sizeof_data);
            if (status != HAL_OK) {

                *error_report = HAL_RETURN_ERROR;
                return;
            }

            /* peripheral transmit testing data  */
            status = HAL_UART_Transmit_DMA(&huart4, data, sizeof_data);
            if (status != HAL_OK) {
                /* abort transmission in case HAL returns error so callback isr function wont release semaphore */
                HAL_UART_Abort_IT(&huart5);
                *error_report = HAL_RETURN_ERROR;
            }
            break;

        default: break;
    }

    /* waite for rx interrupt to release the semaphore, if to match time has passed report over time error  */
    if (xSemaphoreTake(*interrupt_sem, TIME_ELAPSED_ERROR) == pdFALSE && *error_report == NO_ERROR) {

        /* abort transmission in case HAL returns error so callback isr function wont release semaphore */
        HAL_UART_Abort_IT(&huart4);
        xSemaphoreTake(*interrupt_sem, TIME_ELAPSED_ERROR);

        HAL_UART_Abort_IT(&huart5);
        xSemaphoreTake(*interrupt_sem, TIME_ELAPSED_ERROR);

        *error_report = OVER_TIME;

    } else if (*error_report == ERROR_IT) {

        /* abort transmission in case HAL returns error so callback isr function wont release semaphore */
        HAL_UART_Abort_IT(&huart4);
        xSemaphoreTake(*interrupt_sem, TIME_ELAPSED_ERROR);

        HAL_UART_Abort_IT(&huart5);
        xSemaphoreTake(*interrupt_sem, TIME_ELAPSED_ERROR);
    }
}

void
HAL_UART_RxCpltCallback(UART_HandleTypeDef* huart) {

    if (huart == &huart4 || huart == &huart5) {

        xSemaphoreGiveFromISR(stm_test_list_array[UART_4_5].q, NULL);

    } else if (huart == &huart1 || huart == &huart2) {

        xSemaphoreGiveFromISR(stm_test_list_array[UART_1_2].q, NULL);
    }
}

/* callbacks */
void
HAL_UART_ErrorCallback(UART_HandleTypeDef* huart) {
    unit_tasting_package_t* cast;

    if (huart == &huart4 || huart == &huart5) {

        cast = stm_test_list_array[UART_4_5].taskX_pack.p->payload;
        cast->error_report = ERROR_IT;

        // if (xSemaphoreGiveFromISR(stm_test_list_array[1].q, NULL) != pdTRUE){

        // exit(1);
        //}

    } else if (huart == &huart1 || huart == &huart2) {

        cast = stm_test_list_array[UART_1_2].taskX_pack.p->payload;
        cast->error_report = ERROR_IT;
        xSemaphoreGiveFromISR(stm_test_list_array[UART_1_2].q, NULL);
    }
}

void
HAL_UART_AbortCpltCallback(UART_HandleTypeDef* huart) {
    if (huart == &huart4 || huart == &huart5) {

        xSemaphoreGiveFromISR(stm_test_list_array[UART_4_5].q, NULL);

    } else if (huart == &huart1 || huart == &huart2) {

        xSemaphoreGiveFromISR(stm_test_list_array[0].q, NULL);
    }
}
