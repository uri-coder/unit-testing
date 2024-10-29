/**
 * \file            spi_test.h
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

#ifndef DEVELOPER_CODE_SPI_TEST_H_
#define DEVELOPER_CODE_SPI_TEST_H_

#include "main_ur.h"

extern SPI_HandleTypeDef hspi1;
extern SPI_HandleTypeDef hspi2;
extern SPI_HandleTypeDef hspi3;
extern SPI_HandleTypeDef hspi4;
extern SPI_HandleTypeDef hspi5;
extern DMA_HandleTypeDef hdma_spi1_rx;
extern DMA_HandleTypeDef hdma_spi2_rx;
extern DMA_HandleTypeDef hdma_spi1_tx;

extern DMA_HandleTypeDef hdma_spi3_rx;
extern DMA_HandleTypeDef hdma_spi4_rx;
extern DMA_HandleTypeDef hdma_spi5_rx;

/**
 * \brief                     one peripheral already finished transaction
 * \hideinitializer
 */
#define ONE_PERIPHERAL_FINISH 1

/**
 * \brief                     no peripheral  finish transaction
 * \hideinitializer
 */
#define NO_PERIPHERAL_FINISH  0

void spi_1_2_task(uint8_t* data, uint16_t sizeof_data, SemaphoreHandle_t* interrupt_sem, uint8_t test_select,
                  uint8_t* error_report);

#endif /* DEVELOPER_CODE_SPI_TEST_H_ */
