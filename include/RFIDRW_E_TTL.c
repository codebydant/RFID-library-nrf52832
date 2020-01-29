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

// char buff_data[20] = "";
char buff_data[20];
static size_t buff_data_size = 0;

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

    // ret_code_t ret;
    // char c;
    // Read one char from queue
    // ret = nrf_queue_read(p_serial->p_ctx->p_config->p_queues->p_rxq, &c, sizeof(c));
    // Echo char back to port

    // NRF_LOG_INFO("NRF_SERIAL_EVENT_RX_DATA\r\n");
    // NRF_LOG_INFO("RX Data ready");
    // // char c;
    // // APP_ERROR_CHECK(nrf_serial_read(p_serial, &c, sizeof(c), NULL, 500));
    // // append(buff_data, c);
    char c[2] = {0};
    size_t p_read = 0;
    APP_ERROR_CHECK(nrf_serial_read(p_serial, &c[0], sizeof(c[0]), &p_read, 500));
    // APP_ERROR_CHECK(nrf_serial_read(p_serial, &c[0], sizeof(c[0]), NULL, 500));

    p_read = 1;
    // NRF_LOG_INFO("p_read:%d", p_read);
    buff_data_size += p_read;

    if (c[0] == '\r') {
      // (void)nrf_serial_write(p_serial, &c, sizeof(c), NULL, 500);
      // p_read = 0;
      NRF_LOG_INFO("carriage return received");
      break;
    }

    strcat(buff_data, c);
    int elements_in_buff = sizeof(buff_data) / sizeof(buff_data[0]);
    buff_data[elements_in_buff - 1] = '\0';

    // Echo char back to port
    //(void)nrf_serial_write(p_serial, &c, sizeof(c), NULL, 500);
    // NRF_LOG_INFO("c[0]:%c", c[0]);

    // // APP_ERROR_CHECK(nrf_serial_write(p_serial, &c, sizeof(c[0]), NULL, 500));

    // // NRF_LOG_INFO("p_read:%s", buff_data);
    // // NRF_LOG_INFO("p_read:0x%c", c[0]);
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
// NRF_SERIAL_UART_DEF(serial_uart, 0);
APP_TIMER_DEF(serial_uart_rx_timer);
APP_TIMER_DEF(serial_uart_tx_timer);
static nrf_serial_ctx_t serial_uart_ctx;
static nrf_serial_t serial_uart = {.instance = NRF_DRV_UART_INSTANCE(0)};
static nrf_serial_config_t serial_config = {};
static nrf_drv_uart_config_t m_uart0_drv_config = {};

void RFID_init(uint8_t RX_pinNumber, uint8_t TX_pinNumber, nrf_uart_baudrate_t baudrate) {

  serial_uart.p_ctx = &serial_uart_ctx;
  serial_uart.p_tx_timer = &serial_uart_tx_timer;
  serial_uart.p_rx_timer = &serial_uart_rx_timer;

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
}

void clean_buff_data(char *buff_data) {
  for (int i = 0; i < strlen(buff_data); i++) {
    NRF_LOG_INFO("--- data from buffer:   %c", buff_data[i]);
  }
}

void RFID_sendCommand(char *command_to_RFID, char *RFID_response) {

  // Flushing TX port
  // memset(buff_data, 0, sizeof(buff_data));
  // RFID_response = malloc(sizeof(char) * 20);
  // memset(buff_data, 0, strlen(buff_data));
  // memset(RFID_response, 0, strlen(RFID_response));
  buff_data_size = 0;
  // memset(buff_data, '\0', sizeof(buff_data));
  // strcpy(RFID_response, "");
  memset(buff_data, '\0', sizeof(buff_data));
  strcpy(buff_data, "");
  // APP_ERROR_CHECK(nrf_serial_flush(&serial_uart, NRF_SERIAL_MAX_TIMEOUT));
  // memset(buff_data, 0, sizeof buff_data);

  // static char command_to_RFID[] = "SD0\r";
  // append(command_to_RFID, '\\');
  // append(command_to_RFID, 'r');
  // memset(buff_data, '\0', sizeof(buff_data));
  NRF_LOG_INFO("--- Sending command:                %s", command_to_RFID);
  APP_ERROR_CHECK(nrf_serial_write(&serial_uart, command_to_RFID, strlen(command_to_RFID), NULL, 500));
  // NRF_LOG_INFO("--- string original is:             %s", buff_data);
  RFID_waitResponse();

  // uint8_t *buf = serial_uart.p_ctx->p_config->p_buffers->p_rxb;

  // }

  // memset(RFID_response, '\0', sizeof(RFID_response));

  // memcpy(RFID_response, dest, strlen(dest) + 1);

  // RFID_response = malloc(sizeof(char) * 20);

  // clean_buff_data(buff_data);

  // memset(RFID_response, 0, strlen(RFID_response));
  strcpy(RFID_response, buff_data);
  APP_ERROR_CHECK(nrf_serial_flush(&serial_uart, 500));
  // // RFID_response[strlen(buff_data) - 1] = '\0';

  // NRF_LOG_INFO("--- char 1 copy is:             %c", buff_data[0]);
  // NRF_LOG_INFO("--- char 2 copy is:             %c", buff_data[1]);
  // NRF_LOG_INFO("--- char 3 copy is:             %c", buff_data[2]);

  // NRF_LOG_INFO("--- char 1 res is:             %c", RFID_response[0]);
  // NRF_LOG_INFO("--- char 2 res is:             %c", RFID_response[1]);
  // NRF_LOG_INFO("--- char 3 res is:             %c", RFID_response[2]);

  // // char dest[20];
  // // strncpy(dest, buff_data, elements_in_buff);
  // // strncpy(dest, buff_data, elements_in_buff);
  // int elements_in_buff = sizeof(buff_data) / sizeof(buff_data[0]);
  // dest[elements_in_buff - 1] = NULL;
  // NRF_LOG_INFO("--- string copy is:             %s", dest);

  // NRF_LOG_INFO("--- string original is:             %s", buff_data);
  // NRF_LOG_INFO("--- string original size is:             %d", elements_in_buff);
}

void RFID_waitResponse() {

  // NRF_LOG_INFO("--- Waiting for RX be ready!");

  nrf_delay_us(1);

  // bool status = false;

  // serial_queues SERIAL_FIFO_RX_SIZE

  while (buff_data_size < 3) {
    nrf_delay_ms(1);
  }

  // if (sizeof(buff_data) > 0) {
  //   NRF_LOG_INFO("--- Response from RFID:             %s", buff_data);
}

char *RFID_getResponseCommand() {

  char *myresponse;
  myresponse = malloc(sizeof(char) * 20);

  strcpy(myresponse, buff_data);
  return myresponse;
}

/*

// memset(buff_data, 0, sizeof buff_data);
  // NRF_LOG_INFO("Response final clean: 0x%s", buff_data);

  NRF_LOG_INFO("--- Flushing TX port:                       DONE");
  APP_ERROR_CHECK(nrf_serial_flush(p_serial, NRF_SERIAL_MAX_TIMEOUT));
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

  static char command4_to_RFID[] = "SB0\r";
  NRF_LOG_INFO("--- Command SEND:                   %s", command4_to_RFID);
  APP_ERROR_CHECK(nrf_serial_write(&serial_uart, command4_to_RFID, strlen(command4_to_RFID), NULL, 0));

  NRF_LOG_INFO("--- Response from RFID:             0x%s\n", buff_data);
  nrf_delay_ms(5000);

  NRF_LOG_INFO("--- Flushing TX port:                       DONE");
  APP_ERROR_CHECK(nrf_serial_flush(&serial_uart, NRF_SERIAL_MAX_TIMEOUT));
  memset(buff_data, 0, sizeof buff_data);

  static char command5_to_RFID[] = "RSD\r";
  NRF_LOG_INFO("--- Command SEND:                   %s", command5_to_RFID);
  APP_ERROR_CHECK(nrf_serial_write(&serial_uart, command5_to_RFID, strlen(command5_to_RFID), NULL, 0));

  NRF_LOG_INFO("--- Response from RFID:             0x%s\n", buff_data);
  nrf_delay_ms(5000);

  NRF_LOG_INFO("--- command received: %s", command);
  APP_ERROR_CHECK(nrf_serial_write(p_serial, command, strlen(command), NULL, NRF_SERIAL_MAX_TIMEOUT));
  NRF_LOG_INFO("--- command: SEND");

*/