/**
 * \file            test_funck.c
 * \brief           hardware specific testing code file template src file
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

#include "test_funck.h"

/* instruction to hardware specific programming start */

/* prototype functions  for the user to write hardware specific code */

/*


void test_thread_prototype_fuction(uint8_t* data, uint16_t sizeof_data,
SemaphoreHandle_t* interrupt_sem){


}



*/

/** 
* instruction to hardware specific programming 
*
*/

/* arrays for stm32 testing software and  linux pc tasting app start .(template filled by hardware specific code, developer)  */

/**
 * \brief           holds the tests information that wil be sent to testing app
 * \note            developer need to write a member for each test in the thread
 *                  at the last member of the thread. developer initialise the test_name with '#'
 *                  at the start.
 *                  the developer need to initialise test_name and result_evaluation_criteria
 *                  members need to be filed by the order of the functions in  "stm_test_list_array"
 *                  array and by the order of the tests in the function.
 */
const linux_app_test_setting_t pc_test_list_array[] = {

    {.test_name = " uart1 TI rx uart2 TI tx",
     .results_evaluation_criteria = COMMUNICATION_CRITERIA,
     .pulling_or_it = IT_MODE},
    {.test_name = "#uart2 TI rx uart1 TI tx ",
     .results_evaluation_criteria = COMMUNICATION_CRITERIA,
     .pulling_or_it = IT_MODE},

    {.test_name = " uart4 TI rx uart5 TI tx",
     .results_evaluation_criteria = COMMUNICATION_CRITERIA,
     .pulling_or_it = IT_MODE},
    {.test_name = " uart5 TI rx uart4 TI tx ",
     .results_evaluation_criteria = COMMUNICATION_CRITERIA,
     .pulling_or_it = IT_MODE},
    {.test_name = " uart4 DMA rx uart5 DMA tx",
     .results_evaluation_criteria = COMMUNICATION_CRITERIA,
     .pulling_or_it = IT_MODE},
    {.test_name = "#uart5 DMA rx uart4 DMA tx",
     .results_evaluation_criteria = COMMUNICATION_CRITERIA,
     .pulling_or_it = IT_MODE},

    {.test_name = " i2c1 TI rx i2c2 TI rx",
     .results_evaluation_criteria = COMMUNICATION_CRITERIA,
     .pulling_or_it = IT_MODE},
    {.test_name = " i2c2 TI rx i2c1 TI rx",
     .results_evaluation_criteria = COMMUNICATION_CRITERIA,
     .pulling_or_it = IT_MODE},
    {.test_name = "#i2c1 DMA rx i2c2 TI rx",
     .results_evaluation_criteria = COMMUNICATION_CRITERIA,
     .pulling_or_it = IT_MODE},

    {.test_name = " spi1 TI rx spi2 TI tx",
     .results_evaluation_criteria = COMMUNICATION_CRITERIA,
     .pulling_or_it = IT_MODE},
    {.test_name = " spi2 TI rx spi1 TI tx",
     .results_evaluation_criteria = COMMUNICATION_CRITERIA,
     .pulling_or_it = IT_MODE},
    {.test_name = " spi1 DMA rx spi2 TI tx",
     .results_evaluation_criteria = COMMUNICATION_CRITERIA,
     .pulling_or_it = IT_MODE},
    {.test_name = " spi2 DMA rx spi1 DMA tx",
     .results_evaluation_criteria = COMMUNICATION_CRITERIA,
     .pulling_or_it = IT_MODE},
    {.test_name = "#spi2 IT rx/tx spi1 it rx/tx",
     .results_evaluation_criteria = COMMUNICATION_CRITERIA,
     .pulling_or_it = IT_MODE},

    {.test_name = " dec1 to adc1 ", .results_evaluation_criteria = ADC_CRITERIA, .pulling_or_it = PULLING_MODE},
    {.test_name = " dec1 to adc2 ", .results_evaluation_criteria = ADC_CRITERIA, .pulling_or_it = PULLING_MODE},
    {.test_name = "#dec1 to adc3 ", .results_evaluation_criteria = ADC_CRITERIA, .pulling_or_it = PULLING_MODE},

    {.test_name = "#flash ", .results_evaluation_criteria = COMMUNICATION_CRITERIA, .pulling_or_it = PULLING_MODE},

};

/**
 * \brief           storing card name and id that will be sent to tasting app.
 *
 * \note            name need to be filed by the the developer of the test.
 */
card_name_and_id_t this_card_name_and_id = {.board_part_number = "nucleo-f746zg"};

/**
 * \brief           hold the resources for all unit testing thread
 *
 * \note            developer need to write a member for each testing function and initialise the
 *                  task_ptr struct member, with the test function address.
 *
 * \note                 later the program will start threads each thread will receive a member
 *                       and will call the task_ptr function ie the test function
 */
test_func_t stm_test_list_array[] = {

    [UART_1_2].task_ptr = UART_1_2_task,
    [UART_4_5].task_ptr = UART_4_5_task,
    [I2C_1_2].task_ptr = I2C_1_2_task,
    [SPI_1_2].task_ptr = spi_1_2_task,
    [ADC_DAC].task_ptr = adc_1_2_3_dac1_task,
    [FLASH].task_ptr = flash_task

};
/**
 * \brief           Holds number of tasks
 *                  
 */
const uint16_t test_list_size = sizeof(stm_test_list_array) / sizeof(test_func_t);
/**
 * \brief           Holds number of tests
 *                  
 */
const uint16_t pc_test_list_size = sizeof(pc_test_list_array) / sizeof(linux_app_test_setting_t); /* number of tasks */

/* arrays for stm32 testing software and  linux pc tasting app end  */

/* tasks code start developer code */

/* tasks code end */
