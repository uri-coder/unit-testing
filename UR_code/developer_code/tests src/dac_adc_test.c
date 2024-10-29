/**
 * \file            dac_adc_test.c
 * \brief           hardware specific testing code file for testing dac and adc
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

#include "dac_adc_test.h"
#include "extern_template_variables.h"
#include "main_ur.h"
#include "test_funck.h"

/**
 * \brief           testing dac adc1 adc2 and adc3 adc peripherals
 *                  the function will be called by "testing_thread"
 * \param[in]       data: data received from testing app vie udp the test hardware
 *                  to do a data match compare
 * \param[in]       sizeof_data: size of data
 * \param[in]       interrupt_sem: semaphore in case you need to waite for an interrupt
 * \param[in]       test_select: select what test in the function to run
 * \param[in]       error_report: report to pc about test error
 */
void
adc_1_2_3_dac1_task(uint8_t* data, uint16_t sizeof_data, SemaphoreHandle_t* interrupt_sem, uint8_t test_select,
                    uint8_t* error_report) {

    HAL_StatusTypeDef status; /* error return check */
    *error_report = NO_ERROR; /* initalized to  NO_ERROR if an error is accruing it will change it */
    /* run task test requested by pc */
    switch (test_select) {

        case 0: /* dec1 to adc1 */

            HAL_DAC_Start(&hdac, DAC1_CHANNEL_1);

            /* peripheral transmit testing data  */
            status = HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_12B_R, data[0]);
            if (status != HAL_OK) {
                *error_report = HAL_RETURN_ERROR;
                return;
            }

            /* peripheral receive testing data  */
            HAL_ADC_Start(&hadc1);
            data[0] = HAL_ADC_GetValue(&hadc1);

            break;

        case 1: /* dec1 to adc2 */

            HAL_DAC_Start(&hdac, DAC1_CHANNEL_1);

            /* peripheral transmit testing data  */
            status = HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_12B_R, data[0]);
            if (status != HAL_OK) {
                *error_report = HAL_RETURN_ERROR;
                return;
            }

            /* peripheral receive testing data  */
            HAL_ADC_Start(&hadc2);
            data[0] = HAL_ADC_GetValue(&hadc2);

            break;

        case 2: /* dec1 to adc3 */

            HAL_DAC_Start(&hdac, DAC1_CHANNEL_1);

            /* peripheral transmit testing data  */
            status = HAL_DAC_SetValue(&hdac, DAC1_CHANNEL_1, DAC_ALIGN_12B_R, data[0]);
            if (status != HAL_OK) {
                *error_report = HAL_RETURN_ERROR;
                return;
            }

            /* peripheral receive testing data  */
            HAL_ADC_Start(&hadc3);
            data[0] = HAL_ADC_GetValue(&hadc3);

            break;

        default: break;
    }
}
