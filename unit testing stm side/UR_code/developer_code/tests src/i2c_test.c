/**
 * \file            i2c_test.c
 * \brief           hardware specific testing code file for testing i2c
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

#include "i2c_test.h"
#include "extern_template_variables.h"
#include "main_ur.h"
#include "test_funck.h"

/**
 * \brief           testing i2c 1 and 2 peripherals
 *                  the function will be called by "testing_thread"
 * \param[in]       data: data received from testing app vie udp the test hardware
 *                  to do a data match compare
 * \param[in]       sizeof_data: size of data
 * \param[in]       interrupt_sem: semaphore in case you need to waite for an interrupt
 * \param[in]       test_select: select what test in the function to run
 * \param[in]       error_report: report to pc about test error
 */
void
I2C_1_2_task(uint8_t* data, uint16_t sizeof_data, SemaphoreHandle_t* interrupt_sem, uint8_t test_select,
             uint8_t* error_report) {

    HAL_StatusTypeDef status; /* error return check */
    *error_report = NO_ERROR; /* initalized to  NO_ERROR if an error is accruing it will change it */

    /* run task test requested by pc */
    switch (test_select) {

        case 0: /* i2c1 TI rx i2c2 TI rx */

            /* peripheral receive testing data  */
            status = HAL_I2C_Master_Receive_IT(&hi2c1, 0, data, sizeof_data);
            if (status != HAL_OK) {

                *error_report = HAL_RETURN_ERROR;
                return;
            }

            /* peripheral transmit testing data  */
            status = HAL_I2C_Slave_Transmit_IT(&hi2c2, data, sizeof_data);
            if (status != HAL_OK) {
                /* abort transmission in case HAL returns error so callback isr function wont release semaphore */
                HAL_I2C_Master_Abort_IT(&hi2c1, 0);

                *error_report = HAL_RETURN_ERROR;
            }

            break;

        case 1: /* i2c2 TI rx i2c1 TI rx */

            /* peripheral receive testing data  */
            status = HAL_I2C_Master_Receive_IT(&hi2c2, 0, data, sizeof_data);
            if (status != HAL_OK) {

                *error_report = HAL_RETURN_ERROR;
                return;
            }

            /* peripheral transmit testing data  */
            status = HAL_I2C_Slave_Transmit_IT(&hi2c1, data, sizeof_data);
            if (status != HAL_OK) {
                /* abort transmission in case HAL returns error so callback isr function wont release semaphore */
                HAL_I2C_Master_Abort_IT(&hi2c2, 0);

                *error_report = HAL_RETURN_ERROR;
            }

            break;
        case 2: /* i2c1 DMA rx i2c2 TI rx */

            /* peripheral receive testing data  */
            status = HAL_I2C_Master_Receive_DMA(&hi2c1, 0, data, sizeof_data);
            if (status != HAL_OK) {

                *error_report = HAL_RETURN_ERROR;
            }
            /* peripheral transmit testing data  */
            status = HAL_I2C_Slave_Transmit_IT(&hi2c2, data, sizeof_data);
            if (status != HAL_OK) {
                /* abort transmission in case HAL returns error so callback isr function wont release semaphore */
                HAL_I2C_Master_Abort_IT(&hi2c1, 0);

                *error_report = HAL_RETURN_ERROR;
            }

            break;

        default: break;
    }
    /* waite for rx interrupt to release the semaphore, if to match time has passed report over time error  */
    if (xSemaphoreTake(*interrupt_sem, TIME_ELAPSED_ERROR) == pdFALSE && *error_report == NO_ERROR) {

        /* abort transmission in case HAL returns error so callback isr function wont release semaphore */
        HAL_I2C_Master_Abort_IT(&hi2c1, 0);
        xSemaphoreTake(*interrupt_sem, TIME_ELAPSED_ERROR);
        HAL_I2C_Master_Abort_IT(&hi2c2, 0);
        xSemaphoreTake(*interrupt_sem, TIME_ELAPSED_ERROR);

        *error_report = OVER_TIME;

    } else if (*error_report == ERROR_IT) {

        /* abort transmission in case HAL returns error so callback isr function wont release semaphore */
        HAL_I2C_Master_Abort_IT(&hi2c1, 0);
        xSemaphoreTake(*interrupt_sem, TIME_ELAPSED_ERROR);
        HAL_I2C_Master_Abort_IT(&hi2c2, 0);
        xSemaphoreTake(*interrupt_sem, TIME_ELAPSED_ERROR);
    }
}

void
HAL_I2C_MasterRxCpltCallback(I2C_HandleTypeDef* hi2c) {

    if (hi2c == &hi2c1 || hi2c == &hi2c2) {

        xSemaphoreGiveFromISR(stm_test_list_array[I2C_1_2].q, NULL);
    }
}

/**
  * @brief  I2C error callback.
  * @param  hi2c Pointer to a I2C_HandleTypeDef structure that contains
  *                the configuration information for the specified I2C.
  * @retval None
  */
void
HAL_I2C_ErrorCallback(I2C_HandleTypeDef* hi2c) {

    unit_tasting_package_t* cast;

    if (hi2c == &hi2c1 || hi2c == &hi2c2) {

        cast = stm_test_list_array[I2C_1_2].taskX_pack.p->payload;

        cast->error_report = ERROR_IT;

        xSemaphoreGiveFromISR(stm_test_list_array[I2C_1_2].q, NULL);
    }
}

void
HAL_I2C_AbortCpltCallback(I2C_HandleTypeDef* hi2c) {
    if (hi2c == &hi2c1 || hi2c == &hi2c2) {

        xSemaphoreGiveFromISR(stm_test_list_array[I2C_1_2].q, NULL);
    }
}
