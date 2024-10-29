/**
 * \file            test_funck.h
 * \brief           hardware specific testing code include file
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

#ifndef INC_TEST_FUNCK_H_
#define INC_TEST_FUNCK_H_

#include "main_ur.h"

#include "dac_adc_test.h"
#include "flash_test.h"
#include "i2c_test.h"
#include "spi_test.h"
#include "uart_test.h"

/**
 * \brief          test task list marcos
 */
typedef enum test_task_list {

    UART_1_2, /*!< test uart 1 and 2 */
    UART_4_5, /*!< test uart 4 and 5 */
    I2C_1_2,  /*!< test i2c 1 and 2 */
    SPI_1_2,  /*!< test spi 1 and 2 */
    ADC_DAC,  /*!< test adc anc dac */
    FLASH     /*!< test flash */

} test_task_list_t;

#endif /* INC_TEST_FUNCK_H_ */
