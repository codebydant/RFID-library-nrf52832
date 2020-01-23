/*
 * RFIDRW_E_TTL.c
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
* 	----|RX_3V	   RX TX V+ V-    A+|---- Antenna +
* 	----|TX_3V 	                  AR|----
* 	----|V-		                    A-|---- Antenna -
* 	----|Vo		                    CT|----
* 	----|V+	                      VC|----
* 	----|		                      VR|----
* 		+-----------------------------+
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

// NRF_SERIAL_DRV_UART_CONFIG_DEF(m_uart0_drv_config, RX_PIN_NUMBER, TX_PIN_NUMBER, RTS_PIN_NUMBER, CTS_PIN_NUMBER,
//                              NRF_UART_HWFC_DISABLED, NRF_UART_PARITY_EXCLUDED, NRF_UART_BAUDRATE_9600,
//                              UART_DEFAULT_CONFIG_IRQ_PRIORITY);
NRF_SERIAL_QUEUES_DEF(serial_queues, SERIAL_FIFO_TX_SIZE, SERIAL_FIFO_RX_SIZE);
NRF_SERIAL_BUFFERS_DEF(serial_buffs, SERIAL_BUFF_TX_SIZE, SERIAL_BUFF_RX_SIZE);
// NRF_SERIAL_CONFIG_DEF(serial_config, NRF_SERIAL_MODE_IRQ, &serial_queues, &serial_buffs, &RFID_evt_handler,
//                     sleep_handler);
NRF_SERIAL_UART_DEF(serial_uart, 0);

static nrf_serial_config_t serial_config = {};
static nrf_drv_uart_config_t m_uart0_drv_config = {};

void RFID_init(uint8_t RX_pinNumber, uint8_t TX_pinNumber, nrf_uart_baudrate_t baudrate) {

  NRF_LOG_INFO("--- Setting RX pin:                 %d", RX_pinNumber);
  NRF_LOG_INFO("--- Setting TX pin:                 %d", TX_pinNumber);

  m_uart0_drv_config.pselrxd = RX_pinNumber;
  m_uart0_drv_config.pseltxd = TX_pinNumber;
  m_uart0_drv_config.pselrts = RTS_PIN_NUMBER;
  m_uart0_drv_config.pselcts = CTS_PIN_NUMBER;
  m_uart0_drv_config.hwfc = NRF_UART_HWFC_DISABLED;
  m_uart0_drv_config.parity = NRF_UART_PARITY_EXCLUDED;
  m_uart0_drv_config.baudrate = baudrate;
  m_uart0_drv_config.interrupt_priority = UART_DEFAULT_CONFIG_IRQ_PRIORITY;

  serial_config.mode = NRF_SERIAL_MODE_IRQ;
  serial_config.p_queues = &serial_queues;
  serial_config.p_buffers = &serial_buffs;
  serial_config.ev_handler = &RFID_evt_handler;
  serial_config.sleep_handler = sleep_handler;

  NRF_LOG_INFO("---	Initializing module: 			DONE");
  APP_ERROR_CHECK(nrf_serial_init(&serial_uart, &m_uart0_drv_config, &serial_config));

  NRF_LOG_INFO("--- Flushing TX port: 			DONE");
  APP_ERROR_CHECK(nrf_serial_flush(&serial_uart, NRF_SERIAL_MAX_TIMEOUT));

  static char command1_to_RFID[] = "SD0\r";
  NRF_LOG_INFO("--- Sending command to RFID: 		DONE");
  NRF_LOG_INFO("--- Command SEND: 			%s", command1_to_RFID);
  APP_ERROR_CHECK(nrf_serial_write(&serial_uart, command1_to_RFID, strlen(command1_to_RFID), NULL, 0));
  // ret = nrf_serial_write(&serial_uart, &command, strlen(command), NULL, NRF_SERIAL_MAX_TIMEOUT);

  NRF_LOG_INFO("--- Response from RFID: 		0x%s", buff_data);
  // memset(buff_data, 0, sizeof buff_data);
  // NRF_LOG_INFO("Response final clean: 0x%s", buff_data);

  NRF_LOG_INFO("--- Flushing TX port:                       DONE");
  APP_ERROR_CHECK(nrf_serial_flush(&serial_uart, NRF_SERIAL_MAX_TIMEOUT));
  memset(buff_data, 0, sizeof buff_data);

  static char command2_to_RFID[] = "LTG\r";
  NRF_LOG_INFO("--- Command SEND:                   %s", command2_to_RFID);
  APP_ERROR_CHECK(nrf_serial_write(&serial_uart, command2_to_RFID, strlen(command2_to_RFID), NULL, 0));

  NRF_LOG_INFO("--- Response from RFID:             0x%s\n", buff_data);

  //////////////////////////////////////////////////////
  NRF_LOG_INFO("--- Flushing TX port:                       DONE");
  APP_ERROR_CHECK(nrf_serial_flush(&serial_uart, NRF_SERIAL_MAX_TIMEOUT));
  memset(buff_data, 0, sizeof buff_data);

  static char command3_to_RFID[] = "SL0\r";
  NRF_LOG_INFO("--- Command SEND:                   %s", command3_to_RFID);
  APP_ERROR_CHECK(nrf_serial_write(&serial_uart, command3_to_RFID, strlen(command3_to_RFID), NULL, 0));

  NRF_LOG_INFO("--- Response from RFID:             0x%s\n", buff_data);
  nrf_delay_ms(5000);

  NRF_LOG_INFO("--- Flushing TX port:                       DONE");
  APP_ERROR_CHECK(nrf_serial_flush(&serial_uart, NRF_SERIAL_MAX_TIMEOUT));
  memset(buff_data, 0, sizeof buff_data);

  static char command4_to_RFID[] = "SL2\r";
  NRF_LOG_INFO("--- Command SEND:                   %s", command4_to_RFID);
  APP_ERROR_CHECK(nrf_serial_write(&serial_uart, command4_to_RFID, strlen(command4_to_RFID), NULL, 0));

  NRF_LOG_INFO("--- Response from RFID:             0x%s\n", buff_data);
  nrf_delay_ms(5000);
}

void sendCommand_toRFID(struct nrf_serial_s const *p_serial, char *command) {

  NRF_LOG_INFO("--- command received: %s", command);
  APP_ERROR_CHECK(nrf_serial_write(p_serial, command, strlen(command), NULL, NRF_SERIAL_MAX_TIMEOUT));
  NRF_LOG_INFO("--- command: SEND");
}
