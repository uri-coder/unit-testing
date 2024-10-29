/**
 * \file            flash_test.c
 * \brief           hardware specific testing code file for testing the flash
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

#include "flash_test.h"
#include "extern_template_variables.h"
#include "main_ur.h"
#include "test_funck.h"

/**
 * \brief           testing flash
 *                  the function will be called by "testing_thread"
 * \param[in]       data: data received from testing app vie udp the test hardware
 *                  to do a data match compare
 * \param[in]       sizeof_data: size of data
 * \param[in]       interrupt_sem: semaphore in case you need to waite for an interrupt
 * \param[in]       test_select: select what test in the function to run
 * \param[in]       error_report: report to pc about test error
 */
void
flash_task(uint8_t* data, uint16_t sizeof_data, SemaphoreHandle_t* interrupt_sem, uint8_t test_select,
           uint8_t* error_report) {

    HAL_StatusTypeDef status; /* error return check */
    *error_report = NO_ERROR; /* initalized to  NO_ERROR if an error is accruing it will change it */
    FLASH_EraseInitTypeDef erase_init;
    uint32_t page_error;
    switch (test_select) {

        case 0: /* flash test */

            /* no irq while writing to the flash */
            __disable_irq();

            /* erase flash */
            erase_init.TypeErase = FLASH_TYPEERASE_SECTORS;
            erase_init.Sector = FLASH_SECTOR_6;
            erase_init.NbSectors = 1;
            erase_init.VoltageRange = FLASH_VOLTAGE_RANGE_3;

            HAL_FLASH_Unlock();
            __HAL_FLASH_CLEAR_FLAG(FLASH_FLAG_EOP | FLASH_FLAG_OPERR | FLASH_FLAG_WRPERR | FLASH_FLAG_PGAERR
                                   | FLASH_FLAG_PGPERR);

            status = HAL_FLASHEx_Erase(&erase_init, &page_error);
            if (status != HAL_OK) {
                *error_report = HAL_RETURN_ERROR;
                HAL_FLASH_Lock();
                __enable_irq();

                return;
            }

            /* write to flash */
            for (uint8_t j = 0; j < sizeof_data / sizeof(uint32_t); j++) {

                status = HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, FLASH_ADDR + j * sizeof(uint32_t),
                                           *(uint32_t*)(data + j * sizeof(uint32_t)));
                if (status != HAL_OK) {
                    *error_report = HAL_RETURN_ERROR;
                    HAL_FLASH_Lock();
                    __enable_irq();

                    return;
                }
            }

            /* read from flash */
            for (uint8_t j = 0; j < sizeof_data / sizeof(uint32_t); j++) {

                *((uint32_t*)(data + j * sizeof(uint32_t))) = *((uint32_t*)(FLASH_ADDR + j * sizeof(uint32_t)));
            }

            break;

        default: break;
    }
    HAL_FLASH_Lock();
    __enable_irq();
}
