/**
 * \file            main.c
 * \brief           unit test application main src  file
 */

/*
 * Copyright (c) 10/19/2024 Uri Fridman 
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
#include "main.h"

/**
 * \brief  holds library resources for unit test      
 */
unit_test_pack_t my_test;

/**
 * \brief           unit testing application main function
 * \param[out]      return: software error massage
 */
int
main() {

    int check;

    testing_thread_function_init(&my_test, testing_thread);

    check = create_app_init_socket(&my_test);
    if (check != ERROR_OK) {
        return check;
    }

    check = request_card_id(&my_test);

    if (check != ERROR_OK) {
        return check;
    }

    check = print_card_name_and_id(&my_test);

    if (check != ERROR_OK) {
        return check;
    }

    check = request_test_list_size(&my_test);

    if (check != ERROR_OK) {
        return check;
    }

    check = allocate_test_list_mem(&my_test);

    if (check != ERROR_OK) {
        return check;
    }

    check = read_and_fill_test_list_from_stm(&my_test);

    if (check != ERROR_OK) {
        return check;
    }

    check = allocating_testing_threads_handles(&my_test);

    if (check != ERROR_OK) {
        return check;
    }

    check = initialising_testing_threads_input(&my_test);

    if (check != ERROR_OK) {
        return check;
    }

    check = creating_and_joining_testing_threads(&my_test);

    if (check != ERROR_OK) {
        return check;
    }

    check = generate_result_file(my_test.test_raw_results_list, my_test.list_len, &my_test.card_id_in);

    if (check != ERROR_OK) {
        return check;
    }

    close_test_threads_sockets(&my_test);

    free_allocated_recurses(&my_test);

    printf("program end\n");

    //while(1){}

    return ERROR_OK;
}