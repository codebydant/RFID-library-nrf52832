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

  NRF_LOG_INFO("---	Setting RFID:                           DONE");
  RFID_init(10, 11);

  while (true) {
    nrf_delay_ms(100);
    idle_state_handle();
  }
}

/*
https://devzone.nordicsemi.com/f/nordic-q-a/21776/get-data-from-uart-receive-buffer/85505#85505
*/
