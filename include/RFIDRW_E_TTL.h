/*
 * SeeedRFID.h
 * A library for RFID moudle.
 *
 * Copyright (c) 2008-2014 seeed technology inc.
 * Author      : Ye Xiaobo(yexiaobo@seeedstudio.com)
 * Create Time: 2014/2/20
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
* 1. VCC support 3.3 ~ 5V
* 2. TX, RX connect to Arduino or Seeeduino
* 3. T1, T2 is the Signal port for RFID antenna
* 4. W0, W1 is for wiegand protocol, but this library not support yet.
*
* ```
* 		+-----------+
* 	----|VCC	  T1|----
* 	----|GND	  T2|----
* 	----|TX		 SER|----
* 	----|RX		 LED|----
* 	----|W0		BEEP|----
* 	----|W1		 GND|----
* 		+-----------+
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

#define SERIAL_FIFO_TX_SIZE 1024
#define SERIAL_FIFO_RX_SIZE 1024

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

void sendCommand_toRFID(struct nrf_serial_s const *p_serial, char *command);
void RFID_init(int rxPin, int txPin);

#endif // RFIDRW_E_TTL_H