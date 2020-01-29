/*
 * RFIDRW_E_TTL.h
 * A library for the RFIDRW-E-TTL Animal tag and RFID reader writer with external antenna, and TTL output.
 *
 * Copyright (c) 2008-2014 seeed technology inc.
 * Author      : Daniel Tobon(danielTobon43 Github)
 * Create Time: 2020/1/23
 *
 * The MIT License (MIT)
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

/**************************************************************************
* Pins
* ====
*
* 1. V+ support 5.5 ~ 15V
* 2. TX_3V, RX_3V connect to nRF52832
* 3. A+, A- is the Signal port for RFID antenna
*
* ```
* 		+-----------------------------+
* 	----|RX_3V	   RX TX V+ V-    	A+|---- Antenna +
* 	----|TX_3V 	                 	AR|----
* 	----|V-		                    A-|---- Antenna -
* 	----|Vo		                    CT|----
* 	----|V+	                        VC|----
* 	----|		                    VR|----
* 		+-----------------------------+
* ```
***************************************************************************/

#ifndef RFIDRW_E_TTL_H
#define RFIDRW_E_TTL_H

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>
#include <stdio.h>
#include <string.h>

#include "app_scheduler.h"
#include "app_timer.h"
#include "nrf.h"
#include "nrf_delay.h"
#include "nrf_drv_clock.h"
#include "nrf_drv_power.h"
#include "nrf_gpio.h"
#include "nrf_log.h"
#include "nrf_log_ctrl.h"
#include "nrf_pwr_mgmt.h"
#include "nrf_serial.h"

#include "app_error.h"
#include "app_util.h"
#include "boards.h"

#include "nrf_log_default_backends.h"

#define OP_QUEUES_SIZE 3
#define APP_TIMER_PRESCALER NRF_SERIAL_APP_TIMER_PRESCALER

#define SERIAL_FIFO_TX_SIZE 32
#define SERIAL_FIFO_RX_SIZE 32

#define SERIAL_BUFF_TX_SIZE 1
#define SERIAL_BUFF_RX_SIZE 1

struct RFIDdata {
  int dataLen;
  // byte chk;
  char chk;
  bool valid;
  unsigned char raw[5];
};

struct RFIDdata _data;

void RFID_sendCommand(char *command_to_RFID, char *RFID_response);
char *RFID_getResponseCommand();
void RFID_init(uint8_t RX_pinNumber, uint8_t TX_pinNumber, nrf_uart_baudrate_t baudrate);
void RFID_waitResponse();

#endif // RFIDRW_E_TTL_H