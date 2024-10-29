/**
 * \file            server.h
 * \brief           server function for hardware independent code include file
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

#ifndef INC_SERVER_H_
#define INC_SERVER_H_

#include "main_ur.h"
extern SemaphoreHandle_t app_init_sem;
extern struct udp_pcb* app_port;
extern app_data_init_commend_t app_commend;
extern u16_t app_sender_port;
extern ip_addr_t app_sender_ip;

extern const linux_app_test_setting_t pc_test_list_array[];
extern test_func_t stm_test_list_array[];
extern const uint16_t pc_test_list_size;
extern const uint16_t test_list_size;
extern card_name_and_id_t this_card_name_and_id;

#endif /* INC_SERVER_H_ */
