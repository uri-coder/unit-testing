/**
 * \file            spi_test.c
 * \brief           hardware specific testing code file for testing spi
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

#include "spi_test.h"
#include "extern_template_variables.h"
#include "main_ur.h"
#include "test_funck.h"

/**
 * \brief           to make sure that the to spi peripherals finish transmit and receive before releasing the semaphore.
 */
uint8_t spi_tx_rx;

/**
 * \brief           testing spi 1 and 2 peripherals
 *                  the function will be called by "testing_thread"
 * \param[in]       data: data received from testing app vie udp the test hardware
 *                  to do a data match compare
 * \param[in]       sizeof_data: size of data
 * \param[in]       interrupt_sem: semaphore in case you need to waite for an interrupt
 * \param[in]       test_select: select what test in the function to run
 * \param[in]       error_report: report to pc about test error
 */
void
spi_1_2_task(uint8_t* data, uint16_t sizeof_data, SemaphoreHandle_t* interrupt_sem, uint8_t test_select,
             uint8_t* error_report) {

    HAL_StatusTypeDef status; /* error return check */
    *error_report = NO_ERROR; /* initalized to  NO_ERROR if an error is accruing it will change it */

    /* run task test requested by pc */
    switch (test_select) {

        case 0: /* spi1 TI rx spi2 TI tx */

            /* peripheral receive testing data  */
            status = HAL_SPI_Receive_IT(&hspi1, data, sizeof_data);
            if (status != HAL_OK) {

                *error_report = HAL_RETURN_ERROR;
                return;
            }

            /* peripheral transmit testing data  */
            status = HAL_SPI_Transmit_IT(&hspi2, data, sizeof_data);
            if (status != HAL_OK) {
                /* abort transmission in case HAL returns error so callback isr function wont release semaphore */
                HAL_SPI_Abort_IT(&hspi1);
                *error_report = HAL_RETURN_ERROR;
            }

            break;

        case 1: /* spi2 TI rx spi1 TI tx */

            /* peripheral receive testing data  */
            status = HAL_SPI_Receive_IT(&hspi2, data, sizeof_data);
            if (status != HAL_OK) {
                HAL_SPI_Abort_IT(&hspi2);
                *error_report = HAL_RETURN_ERROR;
                break;
            }

            /* peripheral transmit testing data  */
            status = HAL_SPI_Transmit_IT(&hspi1, data, sizeof_data);
            if (status != HAL_OK) {
                /* abort transmission in case HAL returns error so callback isr function wont release semaphore */
                HAL_SPI_Abort_IT(&hspi1);
                xSemaphoreTake(*interrupt_sem, TIME_ELAPSED_ERROR);

                HAL_SPI_Abort_IT(&hspi2);
                *error_report = HAL_RETURN_ERROR;
            }

            break;

        case 2: /* spi1 DMA rx spi2 TI tx */

            /* peripheral receive testing data  */
            status = HAL_SPI_Receive_DMA(&hspi1, data, sizeof_data);
            if (status != HAL_OK) {

                *error_report = HAL_RETURN_ERROR;
                return;
            }
            /* peripheral transmit testing data  */
            status = HAL_SPI_Transmit_IT(&hspi2, data, sizeof_data);
            if (status != HAL_OK) {
                /* abort transmission in case HAL returns error so callback isr function wont release semaphore */
                HAL_SPI_Abort_IT(&hspi1);
                *error_report = HAL_RETURN_ERROR;
            }

            break;

        case 3: /* spi2 DMA rx spi1 DMA tx */

            status = HAL_SPI_Receive_DMA(&hspi2, data, sizeof_data);
            if (status != HAL_OK) {
                HAL_SPI_Abort_IT(&hspi2);

                *error_report = HAL_RETURN_ERROR;
                break;
            }
            /* peripheral transmit testing data  */
            status = HAL_SPI_Transmit_DMA(&hspi1, data, sizeof_data);
            if (status != HAL_OK) {
                /* abort transmission in case HAL returns error so callback isr function wont release semaphore */
                HAL_SPI_Abort_IT(&hspi1);
                xSemaphoreTake(*interrupt_sem, TIME_ELAPSED_ERROR);

                HAL_SPI_Abort_IT(&hspi2);
                *error_report = HAL_RETURN_ERROR;
            }

            break;

        case 4: /* spi2 IT rx/tx spi1 it rx/tx */

            spi_tx_rx = NO_PERIPHERAL_FINISH;

            /* peripheral tx/rx testing data  */
            status = HAL_SPI_TransmitReceive_IT(&hspi1, data, data + sizeof_data / 2, sizeof_data / 2);
            if (status != HAL_OK) {

                *error_report = HAL_RETURN_ERROR;
                return;
            }
            /* peripheral tx/rx testing data  */
            status = HAL_SPI_TransmitReceive_IT(&hspi2, data + sizeof_data / 2, data, sizeof_data / 2);
            if (status != HAL_OK) {
                /* abort transmission in case HAL returns error so callback isr function wont release semaphore */
                HAL_SPI_Abort_IT(&hspi1);
                *error_report = HAL_RETURN_ERROR;
            }

            break;

        default: break;
    }
    /* waite for rx interrupt to release the semaphore, if to match time has passed report over time error  */
    if (xSemaphoreTake(*interrupt_sem, TIME_ELAPSED_ERROR) == pdFALSE && *error_report == NO_ERROR) {

        /* abort transmission in case HAL returns error so callback isr function wont release semaphore */
        HAL_SPI_Abort_IT(&hspi2);
        xSemaphoreTake(*interrupt_sem, TIME_ELAPSED_ERROR);

        HAL_SPI_Abort_IT(&hspi1);
        xSemaphoreTake(*interrupt_sem, TIME_ELAPSED_ERROR);

        *error_report = OVER_TIME;

    } else if (*error_report == ERROR_IT) {

        /* abort transmission in case HAL returns error so callback isr function wont release semaphore */
        HAL_SPI_Abort_IT(&hspi2);
        xSemaphoreTake(*interrupt_sem, TIME_ELAPSED_ERROR);

        HAL_SPI_Abort_IT(&hspi1);
        xSemaphoreTake(*interrupt_sem, TIME_ELAPSED_ERROR);
    }
}

/* peripheral isr callback function`s begin */

void
HAL_SPI_RxCpltCallback(SPI_HandleTypeDef* hspi) {

    if (hspi == &hspi1 || hspi == &hspi2) {

        xSemaphoreGiveFromISR(stm_test_list_array[SPI_1_2].q, NULL);
    }
}

void
HAL_SPI_TxRxCpltCallback(SPI_HandleTypeDef* hspi) {
    /* Prevent unused argument(s) compilation warning */

    if ((hspi == &hspi1 || hspi == &hspi2) && spi_tx_rx == ONE_PERIPHERAL_FINISH) {

        xSemaphoreGiveFromISR(stm_test_list_array[SPI_1_2].q, NULL);
    } else {

        spi_tx_rx = ONE_PERIPHERAL_FINISH;
    }
    /* NOTE : This function should not be modified, when the callback is needed,
            the HAL_SPI_TxRxHalfCpltCallback() should be implemented in the user file
   */
}

/**
  * @brief  SPI Abort Complete callback.
  * @param  hspi SPI handle.
  * @retval None
  */
void
HAL_SPI_AbortCpltCallback(SPI_HandleTypeDef* hspi) {
    /* releases semaphore and report errors*/
    if (hspi == &hspi1 || hspi == &hspi2) {

        xSemaphoreGiveFromISR(stm_test_list_array[SPI_1_2].q, NULL);
    }
}

/**
   * @brief  SPI error callback.
   * @param  hspi pointer to a SPI_HandleTypeDef structure that contains
   *               the configuration information for SPI module.
   * @retval None
   */
void
HAL_SPI_ErrorCallback(SPI_HandleTypeDef* hspi) {

    unit_tasting_package_t* cast; /* for casting  */

    /* releases semaphore and report errors */
    if (hspi == &hspi1 || hspi == &hspi2) {

        cast = stm_test_list_array[SPI_1_2].taskX_pack.p->payload;
        cast->error_report = ERROR_IT;
        xSemaphoreGiveFromISR(stm_test_list_array[SPI_1_2].q, NULL);
    }
}

/* peripheral isr callback function end */
