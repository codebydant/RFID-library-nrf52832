/*
Animal tag: http://www.getsmartcard.net/content/?548.html

Product Name			RFID Animal Ear Tag
SKU						GSC-ETR3015O
Material				TPU
Dimension				30x15mm
Frequency				125khz, 134.2khz, 13.56mhz etc
Chip					EM4305, TK4100, HITAGS, I-CODE2/MiFare One etc
Read range				1-5meters
Working Temperature		(-20 ℃～+85℃)


https://devzone.nordicsemi.com/f/nordic-q-a/21776/get-data-from-uart-receive-buffer/85505#85505
*/

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

  char RFID_response1[20];
  char RFID_response2[20];
  char RFID_response3[20];
  char RFID_response4[20];
  char RFID_response5[20];
  char RFID_response6[20];
  char RFID_response7[20];

  // Setting default transponder EM4305 Read/Write Animal Tag
  // Command: [SD4/r] according to the datasheet
  NRF_LOG_INFO("---------------------------------------------------");
  NRF_LOG_INFO("---	Setting default transponder: 	EM4305");
  static char command_to_RFID1[] = "SD4\r";
  RFID_sendCommand(command_to_RFID1, RFID_response1);
  NRF_LOG_INFO("---	the RFID response 1 is: 	%s\n", RFID_response1);

  /*
  EM4100Read Only 								SD0<crn>
  T55xx Read/Write tag 							SD1<crn>
  FDX-B/HDX (default) Read/Write Tag 			SD2<crn>
  TIRIS 64 bit / Multi-page Read/Write Tag 		SD3<crn>
  EM4205/EM4305 Read/Write Tag 					SD4<crn>
  */

  /*
  ?0 <crn> 		Command not understood.
  ?1 <crn> 		Tag not present.
  ?2 <crn> 		Tag failure to Read/Write.
  ?3 <crn> 		Access to Block 0 not allowed
  ?4 <crn> 		Page address invalid for this tag.
  OK <crn> 		Function Performed Successfully.
  */

  // Setting buzzer function to beep on read
  // Command: [SB0/r] according to the datasheet
  NRF_LOG_INFO("---------------------------------------------------");
  NRF_LOG_INFO("---	Setting buzzer function: 	BEEP ON READ");
  static char command_to_RFID2[] = "SB0\r";
  RFID_sendCommand(command_to_RFID2, RFID_response2);
  NRF_LOG_INFO("---	the RFID response 2 is: 	%s\n", RFID_response2);

  // Setting LED function to GREEN while scanning and RED on transponder read
  // Command: [SL1/r] according to the datasheet
  NRF_LOG_INFO("---------------------------------------------------");
  NRF_LOG_INFO("---	Setting LED function: 		GREEN-RED");
  static char command_to_RFID3[] = "SL2\r";
  RFID_sendCommand(command_to_RFID3, RFID_response3);
  NRF_LOG_INFO("---	the RFID response 3 is: 	%s\n", RFID_response3);

  // Setting locate transponder
  // Command: [LTG/r] according to the datasheet
  NRF_LOG_INFO("---------------------------------------------------");
  NRF_LOG_INFO("---	Setting tag type: 		EM4205/EM4305");
  char command_to_RFID5[] = "ST4\r";
  RFID_sendCommand(command_to_RFID5, RFID_response5);
  NRF_LOG_INFO("---	the RFID response 5 is: 	%s\n", RFID_response5);

  // Setting locate transponder
  // Command: [LTG/r] according to the datasheet
  NRF_LOG_INFO("---------------------------------------------------");
  NRF_LOG_INFO("---	Setting RF field turn: 		ON");
  static char command_to_RFID4[] = "SRA\r";
  RFID_sendCommand(command_to_RFID4, RFID_response4);
  NRF_LOG_INFO("---	the RFID response 4 is: 	%s\n", RFID_response4);

  // Setting locate transponder
  // Command: [LTG/r] according to the datasheet
  NRF_LOG_INFO("---------------------------------------------------");
  NRF_LOG_INFO("---	Scanning if animal tag is present!");
  char command_to_RFID6[] = "LTG\r";
  RFID_sendCommand(command_to_RFID6, RFID_response6);
  NRF_LOG_INFO("---	the RFID response 6 is: 	%s\n", RFID_response6);

  // Setting locate transponder
  // Command: [LTG/r] according to the datasheet
  NRF_LOG_INFO("---------------------------------------------------");
  NRF_LOG_INFO("---	Reading standard data!");
  char command_to_RFID7[] = "RSD\r";
  RFID_sendCommand(command_to_RFID7, RFID_response7);
  NRF_LOG_INFO("---	the RFID response 7 is: 	%s\n", RFID_response7);

  /*
  ?1 <crn> 		Tag not present
  OK <crn> 		Tag present
  */

  // char *RFID_response = RFID_getResponseCommand();
  while (true) {
    nrf_delay_ms(100);
    // if (*RFID_response == "3F31\r") {
    if (strcmp(RFID_response6, "?1") == 0) {

      // NRF_LOG_INFO("---	Animal tag is not present!");
      nrf_delay_ms(1);
      // RFID_sendCommand(command_to_RFID5, RFID_response5);
    }
    // } else {
    //   NRF_LOG_INFO("---	Animal tag is present!  OK OK");
    // }
    idle_state_handle();
  }
}


