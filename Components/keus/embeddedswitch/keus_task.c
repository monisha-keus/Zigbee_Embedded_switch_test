
/********************************************************
 * @fn  This function is to declare all GPIOs
 * *****************************************************/

#include "keus_task.h"

#define UART0     HAL_UART_PORT_0
//#define UART1     HAL_UART_PORT_1


bool init_status = 0;
bool read_status = 0;
bool write_status = 0;

bool debounce = false;
//bool toggle = false, toggle1 = false, toggle3 = false;
uint8 count = 0, count1 = 0, count2 = 0;//led_state;
uint8 button_Pressed =0,button_state = 0;

void ledTimerCbk(uint8 timerId);
void leddebounceCbk(uint8 timerId);

//Debounce configuration
KeusTimerConfig debounceTimer = {
    &leddebounceCbk,
    200,
    true,
    -1,
    0};

//Timer Configuration
KeusTimerConfig intervalTimer = {
    &ledTimerCbk,
    100,
    true,
    -1,
    0};
/****************************************************************************
 * **************************FUNCTION DECLARATION****************************
 * ***************************************************************************/
void KEUS_init_fnc(void);

//void update_led_array(uint8 led_no,uint8 led_state);
void KEUS_loop(void);
void update_config_to_memory(void);
void uart_send_button_ack(void);

/*******************************************************
 * @brief  Callback function for timer
           Called every after 100ms

*********************************************************/
void ledTimerCbk(uint8 timerId)
{
  // if (switch_state_buffer)
  // { //To control Blinking led rate
    HalUARTWrite(HAL_UART_PORT_0,&button_Pressed,1);

    // count1++;
    // if(count1 == 5){
    //   count1 = 0;    }
  // }

  if (config_state_buffer)
  { 
    count2++;
    if(count2 == 5){
      count2 = 0;
    }
  }
}

/**************************************************************
 * @fn      leddebounceCbk
 * @brief   callback function for Debounce,called every after 200ms
 * @param   none
 * @return  None
 * ***********************************************************/

void leddebounceCbk(uint8 timerId)
{
  debounce = true;
}

//********LED and button initilization
extern KeusGPIOPin ledPin1 = {0, 0, GPIO_OUTPUT, false, GPIO_LOW};
extern KeusGPIOPin ledPin2 = {0, 1, GPIO_OUTPUT, false, GPIO_LOW};
extern KeusGPIOPin ledPin3 = {0, 4, GPIO_OUTPUT, false, GPIO_LOW};
extern KeusGPIOPin ledPin4 = {0, 5, GPIO_OUTPUT, false, GPIO_LOW};

extern KeusGPIOPin buttonPin1 = {1, 2, GPIO_INPUT, true, GPIO_LOW};
extern KeusGPIOPin buttonPin2 = {1, 3, GPIO_INPUT, true, GPIO_LOW};
extern KeusGPIOPin buttonPin3 = {1, 4, GPIO_INPUT, true, GPIO_LOW};
extern KeusGPIOPin buttonPin4 = {1, 5, GPIO_INPUT, true, GPIO_LOW};

/*************************************************************
 * @fn      KEUS_init_fnc
 * @brief   All Initilization
 * @return  None
 * @param   None  
 * ***********************************************************/
void KEUS_init_fnc(void)
{

  //****Setting GPIOs direction and edge
  KeusGPIOSetDirection(&ledPin1);
  KeusGPIOSetDirection(&buttonPin1);
  KeusGPIOEdgeConfiguration(&buttonPin1, GPIO_FALLING_EDGE);
  KeusGPIOReadPinValue(&buttonPin1);
  KeusGPIOSetPinValue(&ledPin1);

  KeusGPIOSetDirection(&ledPin2);
  KeusGPIOSetDirection(&buttonPin2);
  KeusGPIOEdgeConfiguration(&buttonPin2, GPIO_RISING_EDGE);
  KeusGPIOReadPinValue(&buttonPin2);
  KeusGPIOSetPinValue(&ledPin2);

  KeusGPIOSetDirection(&ledPin3);
  KeusGPIOSetDirection(&buttonPin3);
  KeusGPIOEdgeConfiguration(&buttonPin3, GPIO_RISING_EDGE);
  KeusGPIOReadPinValue(&buttonPin3);
  KeusGPIOSetPinValue(&ledPin3);

  KeusGPIOSetDirection(&ledPin4);
  KeusGPIOSetDirection(&buttonPin4);
  KeusGPIOEdgeConfiguration(&buttonPin4, GPIO_RISING_EDGE);
  KeusGPIOReadPinValue(&buttonPin4);
  KeusGPIOSetPinValue(&ledPin4);

  //*****Enabling Interrupts
  KeusGPIOInterruptEnable(&buttonPin1);
  KeusGPIOInterruptEnable(&buttonPin2);
  KeusGPIOInterruptEnable(&buttonPin3);
  KeusGPIOInterruptEnable(&buttonPin4);

  //******Enabling Timer and Debounce
  KeusTimerUtilAddTimer(&intervalTimer);
  KeusTimerUtilAddTimer(&debounceTimer);

  // init_status = KeusThemeSwitchMiniMemoryInit();
  // read_status = KeusThemeSwitchMiniReadConfigDataIntoMemory();
  //*****Timer Initialization
  KeusTimerUtilInit();
  //******Timer Start
  KeusTimerUtilStartTimer();

  // //*****UART Initialization
  initUart0();

  HalUARTWrite(HAL_UART_PORT_0, "KEUS INIT", (byte)osal_strlen("KEUS INIT"));

  

  //********At bootup send config and switch state to mini-58
  //uart_send_switch_sate_ack(UART1,0x04,4,0x0B);

  KEUS_loop();
}

/**********************************************************************
 * @fn      KEUS_loop
 * @brief   Infinite loop
 * @return  No return after this
 * @param   None
 * *********************************************************************/
void KEUS_loop(void)
{
  while (1)
  {
    HalUARTPoll();
    
  }
}

/*******************************************************************************
 * @fn      HAL_ISR_FUNCTION
 * @brief   ISR for button pressed
 * ****************************************************************************/

HAL_ISR_FUNCTION(halKeusPort1Isr, P1INT_VECTOR)
{
  HAL_ENTER_ISR();
  if (debounce)
  {
    debounce = false;
    if (P1IFG & BV(buttonPin1.bit))
    {
      ledPin1.state = KeusGPIOToggledState(ledPin1.state); //toggle led at button pressed
      KeusGPIOSetPinValue(&ledPin1);
     // config_data[0].valid_state =  ledPin1.state;
      button_state = !ledPin1.state;
      button_Pressed++;

    }

    if (P1IFG & BV(buttonPin2.bit))
    {
      ledPin2.state = KeusGPIOToggledState(ledPin2.state);
      KeusGPIOSetPinValue(&ledPin2);
      //config_data[1].valid_state =  ledPin2.state;
      button_state = !ledPin2.state;
      event_t = 0x10;
      button_Pressed = SWITCH_2;

    }

    if (P1IFG & BV(buttonPin3.bit))
    {
      ledPin3.state = KeusGPIOToggledState(ledPin3.state);
      KeusGPIOSetPinValue(&ledPin3);
      //config_data[2].valid_state =  ledPin3.state;
      button_state = !ledPin3.state;
      event_t = 0x10;
      button_Pressed = SWITCH_3;
    }
    if (P1IFG & BV(buttonPin4.bit))
    {
      ledPin4.state = KeusGPIOToggledState(ledPin4.state);
      KeusGPIOSetPinValue(&ledPin4);
      //config_data[3].valid_state =  ledPin4.state;
      button_state = !ledPin4.state;
      event_t = 0x10;
      button_Pressed = SWITCH_4;


    }
  }

  /*
    Clear the CPU interrupt flag for Port_0
    PxIFG has to be cleared before PxIF
  */
  P1IFG = 0;
  P1IF = 0;

  CLEAR_SLEEP_MODE();
  HAL_EXIT_ISR();
}

//*************************NVIC MEMORY

// /*******************************************************
//  * @fn          KeusThemeSwitchMiniMemoryInit
//  * @brief       nvic memory init
//  * @return
//  * @param
//  * *****************************************************/
// bool KeusThemeSwitchMiniMemoryInit(void)
// {
//   //  for (uint8 i = 0; i < CLICK_TYPES; i++)
//   //  {
//   //    themeManager.btnThemeMap[i] = 255;
//   //  }

//   uint8 res = osal_nv_item_init(NVIC_MEMORY_POSITION, sizeof(config_data), (void *)config_data);

//   if (res == SUCCESS || res == NV_ITEM_UNINIT)
//   {
//     return true;
//   }
//   else
//   {
//     return false;
//   }
// }
// /*******************************************************
//  * @fn      KeusThemeSwitchMiniReadConfigDataIntoMemory
//  * @brief   NVIC memory read 
//  * @return
//  * @param  
//  * ****************************************************/
// bool KeusThemeSwitchMiniReadConfigDataIntoMemory(void)
// {
//   uint8 res = osal_nv_read(NVIC_MEMORY_POSITION, 0, sizeof(config_data), (void *)config_data);

//   if (res == SUCCESS)
//   {
//     return true;
//   }
//   else
//   {
//     return false;
//   }
// }
// /********************************************************
//  * @fn      KeusThemeSwitchMiniWriteConfigDataIntoMemory
//  * @brief   NVIC Memory write
//  * @return
//  * @param
//  * *****************************************************/
// bool KeusThemeSwitchMiniWriteConfigDataIntoMemory(void)
// {
//   uint8 res = osal_nv_write(NVIC_MEMORY_POSITION, 0, sizeof(config_data), (void *)config_data);

//   if (res == SUCCESS)
//   {
//     return true;
//   }
//   else
//   {
//     return false;
//   }
// }