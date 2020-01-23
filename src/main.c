#include "include/RFIDRW_E_TTL.h"

char buff_received_uart_badge[30] = "";
char tmp_buff_debug[50] = "";
char UID_badge[10] = "";

static void log_init(void) {
  ret_code_t err_code = NRF_LOG_INIT(NULL);
  APP_ERROR_CHECK(err_code);

  NRF_LOG_DEFAULT_BACKENDS_INIT();
}

static void idle_state_handle(void) {
  if (NRF_LOG_PROCESS() == false) {
    nrf_pwr_mgmt_run();
  }
}

int main(void) {

  log_init();
  NRF_LOG_PROCESS();
  NRF_LOG_INFO("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");
  NRF_LOG_INFO("***      MAIN PROGRAM     ***");
  NRF_LOG_INFO("~~~~~~~~~~~~~~~~~~~~~~~~~~~~~");

  NRF_LOG_INFO("---	Setting drv clock:                      DONE");
  NRF_LOG_INFO("--- Setting drv power:                      DONE");
  APP_ERROR_CHECK(nrf_drv_clock_init());
  APP_ERROR_CHECK(nrf_drv_power_init(NULL));

  NRF_LOG_INFO("---	Setting app timer:                      DONE");
  nrf_drv_clock_lfclk_request(NULL);
  APP_ERROR_CHECK(app_timer_init());

  NRF_LOG_INFO("---	Setting leds ON:                        DONE");
  bsp_board_init(BSP_INIT_LEDS | BSP_INIT_BUTTONS);

  // RFID RX, TX and Baudrate parameters
  NRF_LOG_INFO("---	Setting RFID:                           DONE");
  RFID_init(RX_PIN_NUMBER, TX_PIN_NUMBER, NRF_UART_BAUDRATE_9600);

  /*
  NRF_UART_BAUDRATE_1200 	1200 baud.
  NRF_UART_BAUDRATE_2400 	2400 baud.
  NRF_UART_BAUDRATE_4800 	4800 baud.
  NRF_UART_BAUDRATE_9600 	9600 baud.
  NRF_UART_BAUDRATE_14400 	14400 baud.
  NRF_UART_BAUDRATE_19200 	19200 baud.
  NRF_UART_BAUDRATE_28800 	28800 baud.
  NRF_UART_BAUDRATE_38400 	38400 baud.
  NRF_UART_BAUDRATE_57600 	57600 baud.
  NRF_UART_BAUDRATE_76800 	76800 baud.
  NRF_UART_BAUDRATE_115200 	115200 baud.
  NRF_UART_BAUDRATE_230400 	230400 baud.
  NRF_UART_BAUDRATE_250000 	250000 baud.
  NRF_UART_BAUDRATE_460800 	460800 baud.
  NRF_UART_BAUDRATE_921600 	921600 baud.
  NRF_UART_BAUDRATE_1000000 1000000 baud
  */
  while (true) {
    nrf_delay_ms(100);
    idle_state_handle();
  }
}

/*
https://devzone.nordicsemi.com/f/nordic-q-a/21776/get-data-from-uart-receive-buffer/85505#85505
*/
