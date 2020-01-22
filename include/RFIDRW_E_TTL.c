/*
 * SeeedRFID.cpp
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
#include "include/RFIDRW_E_TTL.h"

char buff_data[20] = "";

static void sleep_handler(void) {
  __WFE();
  __SEV();
  __WFE();
}

void append(char *s, char c) {
  int len = strlen(s);
  s[len] = c;
  s[len + 1] = '\0';
}

static void RFID_evt_handler(struct nrf_serial_s const *p_serial, nrf_serial_event_t event) {

  switch (event) {
  case NRF_SERIAL_EVENT_RX_DATA: {
    // NRF_LOG_INFO("RX Data ready");
    char c;
    APP_ERROR_CHECK(nrf_serial_read(p_serial, &c, sizeof(c), NULL, 500));
    append(buff_data, c);
    break;
  }
  case NRF_SERIAL_EVENT_TX_DONE: {
    // NRF_LOG_INFO("TX Data done");
    break;
  }
  case NRF_SERIAL_EVENT_DRV_ERR: {
    NRF_LOG_INFO("DVR error");
    break;
  }
  case NRF_SERIAL_EVENT_FIFO_ERR: {
    NRF_LOG_INFO("FIFO error");
    break;
  }
  default:
    break;
  }
}

NRF_SERIAL_DRV_UART_CONFIG_DEF(m_uart0_drv_config, RX_PIN_NUMBER, TX_PIN_NUMBER, RTS_PIN_NUMBER, CTS_PIN_NUMBER,
                               NRF_UART_HWFC_DISABLED, NRF_UART_PARITY_EXCLUDED, NRF_UART_BAUDRATE_9600,
                               UART_DEFAULT_CONFIG_IRQ_PRIORITY);
NRF_SERIAL_QUEUES_DEF(serial_queues, SERIAL_FIFO_TX_SIZE, SERIAL_FIFO_RX_SIZE);
NRF_SERIAL_BUFFERS_DEF(serial_buffs, SERIAL_BUFF_TX_SIZE, SERIAL_BUFF_RX_SIZE);
NRF_SERIAL_CONFIG_DEF(serial_config, NRF_SERIAL_MODE_IRQ, &serial_queues, &serial_buffs, &RFID_evt_handler,
                      sleep_handler);
NRF_SERIAL_UART_DEF(serial_uart, 0);

void RFID_init(int rxPin, int txPin) {

  NRF_LOG_INFO("---	Initializing module: 			DONE");
  APP_ERROR_CHECK(nrf_serial_init(&serial_uart, &m_uart0_drv_config, &serial_config));

  NRF_LOG_INFO("--- Flushing TX port: 			DONE");
  APP_ERROR_CHECK(nrf_serial_flush(&serial_uart, NRF_SERIAL_MAX_TIMEOUT));

  static char command_to_RFID[] = "SD0\r";
  NRF_LOG_INFO("--- Sending command to RFID: 		DONE");
  NRF_LOG_INFO("--- Command SEND: 			%s", command_to_RFID);
  APP_ERROR_CHECK(nrf_serial_write(&serial_uart, command_to_RFID, strlen(command_to_RFID), NULL, 0));
  // ret = nrf_serial_write(&serial_uart, &command, strlen(command), NULL, NRF_SERIAL_MAX_TIMEOUT);

  NRF_LOG_INFO("--- Response from RFID: 		0x%s\n", buff_data);
  // memset(buff_data, 0, sizeof buff_data);
  // NRF_LOG_INFO("Response final clean: 0x%s", buff_data);
}

void sendCommand_toRFID(struct nrf_serial_s const *p_serial, char *command) {

  NRF_LOG_INFO("--- command received: %s", command);
  APP_ERROR_CHECK(nrf_serial_write(p_serial, command, strlen(command), NULL, NRF_SERIAL_MAX_TIMEOUT));
  NRF_LOG_INFO("--- command: SEND");
}
