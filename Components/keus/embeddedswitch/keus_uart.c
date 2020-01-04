

#include "keus_uart.h"

/******************************************************************************************************
 * ***************************VARIABLES DECLARATION****************************************************
 * ****************************************************************************************************/

uint8 data_length = 0,cmd_id = 0,uart_cmd_end = 0,rcv_ptr = 0,task_type =0;
uint8 uart1_data_length =0; 
uint8 data_buff[20] = {0};  //buffer to hold only data comming through uart0.
uint16 event_t = 0x00;      //variable to set differen event.
uint8 uart1_buff[5] = {0};  //buffer to hold data comming through uart1.
uint8 switch_state_buffer = 0;
uint8 config_state_buffer = 0;

extern KeusGPIOPin ledPin1 ;//= {0, 0, GPIO_OUTPUT, false, GPIO_LOW};

/*************************************************************************
 * @brief   Callback function for Uart0
 * @param
 * @return
 * ***********************************************************************/
void uart0RxCb( uint8 port, uint8 event ) {
  uint8  ch,uart_cmd_started=0,uart_data_ptr = 0;
  while (Hal_UART_RxBufLen(port))
  {
    // Read one byte from UART to ch
    HalUARTRead (port, &ch, 1);
    //check for the start byte
    if (ch == UART_INITIATOR && uart_cmd_started == 0) {
      uart_cmd_started = 1;
      uart_data_ptr=0;
    }
    else if(uart_cmd_started == 1){
      data_length = ch;
      uart_cmd_started = 2;
      uart_data_ptr = 0; 
    }
    else if(uart_cmd_started == 2  && (uart_data_ptr == data_length) && ch == UART_TERNINATOR){ 
      uart_cmd_end = 1;
      uart_cmd_started = 0;
      uart_data_ptr =0;

    }
    else if(uart_cmd_started == 2 && (uart_data_ptr+1) > data_length ){
      uart_cmd_started = 0;
      uart_data_ptr =0;
    }
    else if(uart_cmd_started == 2)
    {
      data_buff[uart_data_ptr++] = ch;
    }
    else
    {
      uart_cmd_started = 0;
      uart_data_ptr =0;
    }
    if(uart_cmd_end){
      cmd_id = data_buff[0];
      switch (cmd_id){
        case 0x01:
          event_t = 0x02;
          break;
        case 0x02:
          event_t = 0x04;
          break;
        case 0x03:
          event_t = 0x08;
          break;
        case 0x04:
          event_t = 0x01;
          break;
        default:
        break;
      

      }
    }
    
  }
}

/*
* Uart0 initialization
*/
void initUart0(void) {
 halUARTCfg_t uart0Config;
 uart0Config.configured           = TRUE;
 uart0Config.baudRate             = HAL_UART_BR_115200;
 uart0Config.flowControl          = FALSE;
 uart0Config.flowControlThreshold = 48;
 uart0Config.rx.maxBufSize        = 64;
 uart0Config.tx.maxBufSize        = 64;
 uart0Config.idleTimeout          = 6;  
 uart0Config.intEnable            = TRUE;
 uart0Config.callBackFunc         = uart0RxCb;
 HalUARTOpen (HAL_UART_PORT_0, &uart0Config);
}
 
 /*************************************************************************
 * @brief   Callback function for Uart1
 *          This UART1 is for communicate between Zigbee and Mini-58. 
 * @param
 * @return
 * ***********************************************************************/
void uart1RxCb( uint8 port, uint8 event ) {
    uint8  ch,uart_cmd_started=0,uart_data_ptr = 0,uart1_cmd_end =0,mgs=0;

  while (Hal_UART_RxBufLen(port))
  {
    //*****Read one byte from UART to ch
    HalUARTRead (port, &ch, 1);
    //*****check for Start byte reseived
    if (ch == UART_INITIATOR && uart_cmd_started == 0) {
      uart_cmd_started = 1;
      uart_data_ptr=0;
    }
    else if(uart_cmd_started == 1){
      uart1_data_length = ch;
      uart_cmd_started = 2;
      uart_data_ptr = 0; 
    }

    //******check for end byte received
    else if(uart_cmd_started == 2  && (uart_data_ptr == uart1_data_length) && ch == UART_TERNINATOR){ 
      uart1_cmd_end = 1;
      uart_cmd_started = 0;
      uart_data_ptr =0;

    }

    //********everytime check for data length
    else if(uart_cmd_started == 2 && (uart_data_ptr+1) > uart1_data_length ){
      uart_cmd_started = 0;
      uart_data_ptr =0;
    }
    //*******fill data_buffer with incomming data 
    else if(uart_cmd_started == 2)
    {
      uart1_buff[uart_data_ptr++] = ch;
    }
    else
    {
      uart_cmd_started = 0;
      uart_data_ptr =0;
    }
 //*******After end byte received check for command id
  if(uart1_cmd_end == 1){
    mgs = uart1_buff[0];

//******Based on command id make that particular command ack buffer 0
  if (mgs == TASK_UART_ACK) {
    switch_state_buffer = 0;
    config_state_buffer = 0;
    //ledPin1.state = GPIO_LOW;
    //  KeusGPIOSetPinValue(&ledPin1);
    //  HalUARTWrite( HAL_UART_PORT_0, "LOW", (byte)osal_strlen("LOW")); 
   } 
   
  //  else if(mgs == 0x02) {
  //    config_state_buffer = 0;
    //  ledPin1.state = GPIO_HIGH;
    //  KeusGPIOSetPinValue(&ledPin1);
    //  HalUARTWrite( HAL_UART_PORT_0, "HIGH", (byte)osal_strlen("HIGH"));
   //}
   }
  }
}
 /*
* Uart1 initialization
* P1-7 : Rx
* P1-6 : Tx
*/
void initUart1(void) {
 halUARTCfg_t uart1Config;
 uart1Config.configured           = TRUE;
 uart1Config.baudRate             = HAL_UART_BR_115200;
 uart1Config.flowControl          = FALSE;
 uart1Config.flowControlThreshold = 48;
 uart1Config.rx.maxBufSize        = 64;
 uart1Config.tx.maxBufSize        = 64;
 uart1Config.idleTimeout          = 6;  
 uart1Config.intEnable            = TRUE;
 uart1Config.callBackFunc         = uart1RxCb;
 HalUARTOpen (HAL_UART_PORT_1, &uart1Config);
}