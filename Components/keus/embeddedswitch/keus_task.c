
/********************************************************
 * @fn  This file is to declare all GPIOs
 * *****************************************************/

#include "keus_task.h"

#define UART0     HAL_UART_PORT_0
//#define UART1     HAL_UART_PORT_1

void ledTimerCbk(uint8 timerId);
void leddebounceCbk(uint8 timerId);
void update_led(uint8 led_no, uint8 led_state_t);
void update_config_struct(uint8 led_no, uint8 led_state);
void update_config_to_memory(void);
void uart_send_switch_sate_ack(uint8 cmnd_id,uint8 data_len);
void uart_send_config_sate_ack(uint8 cmnd_id,uint8 data_len);

bool debounce = false;
uint8 count1 = 0,timer1 = 0;
uint8 button_Pressed =0,button_state = 0;
uint8 contineous_on1 =0,blink_led = 0;
uint32  APP_EVENT = 0;
//Variables for NVIC Memory
bool init_status = 0;
bool read_status = 0;
bool write_status = 0;

uint8 get_state(uint8* buff);
uint8 get_config(uint8* buff);



void KeusEmbeddedSwitchLedBlinkHandler(uint8 timerId);

typedef void (*KeusBtnCbk)(void);

struct CONFIG_INFO_t config_data[4];
//
typedef struct {
  uint16 btnDelay;
  uint16 elapsedTime;
  KeusBtnCbk btnCbk;
} KeusBtnManager;

KeusBtnManager buttonManager;

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

  KeusTimerConfig ledTimerConfig = {
  &KeusEmbeddedSwitchLedBlinkHandler,
  500,
  true,
  -1,
  0
};
/****************************************************************************
 * **************************FUNCTION DECLARATION****************************
 * ***************************************************************************/
void KEUS_init_fnc(void);
uint8 KEUS_loop(void);

//********LED and button initilization
extern KeusGPIOPin ledPin1 = {0, 0, GPIO_OUTPUT, false, LED_OFF};
extern KeusGPIOPin ledPin2 = {0, 1, GPIO_OUTPUT, false, LED_OFF};
extern KeusGPIOPin ledPin3 = {0, 4, GPIO_OUTPUT, false, LED_OFF};
extern KeusGPIOPin ledPin4 = {0, 5, GPIO_OUTPUT, false, LED_OFF};

extern KeusGPIOPin buttonPin1 = {1, 2, GPIO_INPUT, true, LED_OFF};
extern KeusGPIOPin buttonPin2 = {1, 3, GPIO_INPUT, true, LED_OFF};
extern KeusGPIOPin buttonPin3 = {1, 4, GPIO_INPUT, true, LED_OFF};
extern KeusGPIOPin buttonPin4 = {1, 5, GPIO_INPUT, true, LED_OFF};
/*******************************************************
 * @brief  Callback function for timer
           Called every after 100ms

*********************************************************/
void ledTimerCbk(uint8 timerId)
{
  KeusGPIOReadPinValue(&buttonPin1);
  KeusGPIOReadPinValue(&buttonPin2);
  KeusGPIOReadPinValue(&buttonPin3);
  KeusGPIOReadPinValue(&buttonPin4);

  if (buttonPin1.state == BUTTON_ACTIVE){
    buttonManager.elapsedTime += KEUS_EMBEDDEDSWITCH_BUTTON_POLL_TIME;
    count1++;
    button_Pressed = 1;

    APP_EVENT = EVENT_3;
    
    if(count1 >= 50){
      contineous_on1 = 1;
      ledPin1.state = LED_ON;
      KeusGPIOSetPinValue(&ledPin1);
    }
    else if (buttonManager.elapsedTime >= buttonManager.btnDelay) {
      contineous_on1 = 0;
      buttonManager.elapsedTime = 0;
      ledPin1.state = LED_OFF;
      KeusGPIOSetPinValue(&ledPin1);
    }
  } 
   
  else {
    buttonManager.elapsedTime = 0;
    count1 = 0;

    if(contineous_on1 ==0){
      ledPin1.state = LED_OFF;
      KeusGPIOSetPinValue(&ledPin1);
    }
    button_Pressed =0;
  }
}

/************************************************************************************
 * @brief   Timer Function to Blink(500ms) Led1 contineously from start. 
 * 
 * ***********************************************************************************/
void KeusEmbeddedSwitchLedBlinkHandler(uint8 timerId){
  if(blink_led == START && button_Pressed == 0){
    ledPin1.state = KeusGPIOToggledState(ledPin1.state); //toggle led at button pressed
    KeusGPIOSetPinValue(&ledPin1);
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
  KeusGPIOEdgeConfiguration(&buttonPin1, GPIO_RISING_EDGE);
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

  //******Enabling Timer and Debounce
  KeusTimerUtilAddTimer(&intervalTimer);
  KeusTimerUtilAddTimer(&debounceTimer);
  KeusTimerUtilAddTimer(&ledTimerConfig);


  init_status = KeusThemeSwitchMiniMemoryInit();
  read_status = KeusThemeSwitchMiniReadConfigDataIntoMemory();
  //*****Timer Initialization
  KeusTimerUtilInit();
  //******Timer Start
  KeusTimerUtilStartTimer();

  // //*****UART Initialization
  initUart0();

  HalUARTWrite(HAL_UART_PORT_0, "KEUS INIT", (byte)osal_strlen("KEUS INIT"));

  
  buttonManager.btnDelay = KEUS_EMBEDDEDSWITCH_RESET_BTN_TIMEOUT;

  config_data[0].config_id = 1;
  config_data[0].led = 1;
  config_data[0].valid_state = 1;

  config_data[1].config_id = 1;
  config_data[1].led = 2;
  config_data[1].valid_state = 1;

  config_data[2].config_id = 1;
  config_data[2].led = 3;
  config_data[2].valid_state = 1;

  config_data[3].config_id = 1;
  config_data[3].led = 4;
  config_data[3].valid_state = 1;

  APP_EVENT = EVENT_1;
  KEUS_loop();
}

/**********************************************************************
 * @fn      KEUS_loop
 * @brief   Infinite loop
 * @return  No return after this
 * @param   None
 * *********************************************************************/
uint8 KEUS_loop(void)
{
  while (1)
  {
    HalUARTPoll();
    if(APP_EVENT & EVENT_1){
      blink_led = START;
      return APP_EVENT ^ EVENT_1;
    }
    if(APP_EVENT & EVENT_2){
      blink_led = STOP;
      return APP_EVENT ^ EVENT_2;
    }
    if(APP_EVENT & EVENT_3){
      return APP_EVENT ^ EVENT_3;
    }
    if(APP_EVENT & EVENT_4){
      return APP_EVENT ^ EVENT_4;
    }
    if(APP_EVENT & EVENT_5){
      return APP_EVENT ^ EVENT_5;
    } 
  }
}
/*******************************************************************
 * @fn      update_led
 * @brief   Function to receive LED state
 * *****************************************************************/

void update_led(uint8 led_no, uint8 led_state_t){
  uint8 led_state;
  led_state = led_state_t;
  //LED1
  if (led_no == 1)
  {
    if (config_data[0].config_id == ONOFF)
    {
      if (led_state == 0)
      {
        config_data[0].valid_state = LOW;
      }
      else
      {
        config_data[0].valid_state = HIGH;
      }
    }
    else if (config_data[0].config_id == DIMMING)
    {
      config_data[0].valid_state = led_state;
    }
    else if (config_data[0].config_id == FAN_CONTROLLER)
    {
      if (led_state == 0 || led_state == 50 || led_state == 100 || led_state == 150 || led_state == 200 || led_state == 255)
      {
        config_data[0].valid_state = led_state;
      }
      else if (led_state > 0 && led_state < 50)
      {
        config_data[0].valid_state = 50;
      }
      else if (led_state > 50 && led_state < 100)
      {
        config_data[0].valid_state = 100;
      }
      else if (led_state > 100 && led_state < 150)
      {
        config_data[0].valid_state = 150;
      }
      else if (led_state > 150 && led_state < 200)
      {
        config_data[0].valid_state = 200;
      }
      else if (led_state > 200 && led_state < 255)
      {
        config_data[0].valid_state = 255;
      }
    }
    config_data[0].led = led_no;
  }

  //LED == 2
  else if (led_no == 2)
  {
    if (config_data[1].config_id == ONOFF)
    {
      if (led_state == 0)
      {
        config_data[1].valid_state = LOW;
      }
      else
      {
        config_data[1].valid_state = HIGH;
      }
    }
    else if (config_data[1].config_id == DIMMING)
    {
      config_data[1].valid_state = led_state;
    }
    
    else if (config_data[1].config_id == FAN_CONTROLLER)
    {
      if (config_data[1].valid_state == 0 || config_data[1].valid_state == 50 || config_data[1].valid_state == 100 || config_data[1].valid_state == 150 || config_data[1].valid_state == 200 || config_data[1].valid_state == 255)
      {
        config_data[1].valid_state = led_state;
      }
      else if (led_state > 0 && led_state < 50)
      {
        config_data[1].valid_state = 50;
      }
      else if (led_state > 50 && led_state < 100)
      {
        config_data[1].valid_state = 100;
      }
      else if (led_state > 100 && led_state < 150)
      {
        config_data[1].valid_state = 150;
      }
      else if (led_state > 150 && led_state < 200)
      {
        config_data[1].valid_state = 200;
      }
      else if (led_state > 200 && led_state < 255)
      {
        config_data[1].valid_state = 255;
      }
    }
    config_data[1].led = led_no;
  }
  //LED == 3
  else if (led_no == 3)
  {
    if (config_data[2].config_id == ONOFF)
    {
      if (led_state == 0)
      {
        config_data[2].valid_state = LOW;
      }
      else
      {
        config_data[2].valid_state = HIGH;
      }
    }
    else if (config_data[2].config_id == DIMMING)
    {
      config_data[2].valid_state = led_state;
      
    }
    else if (config_data[2].config_id == FAN_CONTROLLER)
    {
      if (led_state == 0 || led_state == 50 || led_state == 100 || led_state == 150 || led_state == 200 || led_state == 255)
      {
        config_data[2].valid_state = led_state;
      }
      else if (led_state > 0 && led_state < 50)
      {
        config_data[2].valid_state = 50;
      }
      else if (led_state > 50 && led_state < 100)
      {
        config_data[2].valid_state = 100;
      }
      else if (led_state > 100 && led_state < 150)
      {
        config_data[2].valid_state = 150;
      }
      else if (led_state > 150 && led_state < 200)
      {
        config_data[2].valid_state = 200;
      }
      else if (led_state > 200 && led_state < 255)
      {
        config_data[2].valid_state = 255;
      }
    }
    config_data[2].led = led_no;
  }
  //LED == 4
  else if (led_no == 4)
  {
    if (config_data[3].config_id == ONOFF)
    {
      if (led_state == 0)
      {
        config_data[3].valid_state = LOW;
      }
      else
      {
        config_data[3].valid_state = HIGH;
      }
    }
    else if (config_data[3].config_id == DIMMING)
    {
      config_data[3].valid_state = led_state;
      
    }
    else if (config_data[3].config_id == FAN_CONTROLLER)
    {
      if (led_state == 0 || led_state == 50 || led_state == 100 || led_state == 150 || led_state == 200 || led_state == 255)
      {
        config_data[3].valid_state = led_state;
      }
      else if (led_state > 0 && led_state < 50)
      {
        config_data[3].valid_state = 50;
      }
      else if (led_state > 50 && led_state < 100)
      {
        config_data[3].valid_state = 100;
      }
      else if (led_state > 100 && led_state < 150)
      {
        config_data[3].valid_state = 150;
      }
      else if (led_state > 150 && led_state < 200)
      {
        config_data[3].valid_state = 200;
      }
      else if (led_state > 200 && led_state < 255)
      {
        config_data[3].valid_state = 255;
      }
    }
    config_data[3].led = led_no;
  }
  update_config_to_memory();
}


/***********************************************************************
 * @fn      update_config_struct
 * @brief   update config structure
 * 
 * ********************************************************************/
void update_config_struct(uint8 led_no, uint8 led_state)
{
  if (led_no == 1)
  {
    config_data[0].valid_state = led_state;
    config_data[0].led = led_no;
  }
  else if (led_no == 2)
  {
    config_data[1].led = led_no;
    config_data[1].valid_state = led_state;
  }
  else if (led_no == 3)
  {
    config_data[2].led = led_no;
    config_data[2].valid_state = led_state;
  }
  else if (led_no == 4)
  {
    config_data[3].led = led_no;
    config_data[3].valid_state = led_state;
  }
  update_config_to_memory();
}

/***************************************************************************
 * @brief   Function to ge state and corresponding led_no
 * ************************************************************************/
uint8 get_state(uint8* buff){
  uint8 index = 0,no_of_led = 4;
  buff[index++] = no_of_led;
  for(uint8 i = 0;i<no_of_led;i++){
    buff[index++] = config_data[i].led;
    buff[index++] = config_data[i].valid_state;
  }
  return index-1;
}

/***************************************************************************
 * @brief   Function to ge config_id and corresponding led_no
 * **************************************************************************/
uint8 get_config(uint8* buff){
  uint8 index = 0,no_of_led = 4;
  buff[index++] = no_of_led;
  for(uint8 i = 0;i<no_of_led;i++){
    buff[index++] = config_data[i].led;
    buff[index++] = config_data[i].config_id;
  }
  return index-1;
}

/*****************************************************************************
 * @fn      uart_send_sate_ack
 * @brief   Send back State of all switch to uart
 * @return
 * @param 
 * **************************************************************************/
void uart_send_switch_sate_ack(uint8 cmnd_id,uint8 data_len){
  uint8 Tx_buff[20] = {0};
  uint8 index =0,no_of_leds=4;
  Tx_buff[index++] = 0x28;
  Tx_buff[index++] = data_len;
  Tx_buff[index++] = cmnd_id;
  Tx_buff[index++] = 0x01;
  Tx_buff[index++] = no_of_leds;

  for(uint8 i = 0;i<4;i++){
  Tx_buff[index++] = config_data[i].led;
  Tx_buff[index++] = config_data[i].valid_state;
  }
  Tx_buff[index++] = 0x29;
  HalUARTWrite(HAL_UART_PORT_0, Tx_buff, index);
}

/*****************************************************************************
 * @fn      uart_send_config_sate_ack
 * @brief   Send back State of all config_id to uart
 * @return
 * @param 
 * **************************************************************************/
void uart_send_config_sate_ack(uint8 cmnd_id,uint8 data_len){
  uint8 Tx_buff[20] = {0};
  uint8 index =0,no_of_leds=4;
  Tx_buff[index++] = 0x28;
  Tx_buff[index++] = data_len;
  Tx_buff[index++] = cmnd_id;
  Tx_buff[index++] = 0x01;
  Tx_buff[index++] = no_of_leds;

  for(uint8 i = 0;i<no_of_leds;i++){
  Tx_buff[index++] = config_data[i].led;
  Tx_buff[index++] = config_data[i].config_id;
  }
  Tx_buff[index++] = 0x29;

  HalUARTWrite(HAL_UART_PORT_0, Tx_buff, index);

}
/*****************************************************************************
 * @fn        update_config_to_memory
 * **************************************************************************/
void update_config_to_memory(void)
{
  write_status = KeusThemeSwitchMiniWriteConfigDataIntoMemory();
}


//*************************NVIC MEMORY

/*******************************************************
 * @fn          KeusThemeSwitchMiniMemoryInit
 * @brief       nvic memory init
 * @return
 * @param
 * *****************************************************/
bool KeusThemeSwitchMiniMemoryInit(void)
{
  //  for (uint8 i = 0; i < CLICK_TYPES; i++)
  //  {
  //    themeManager.btnThemeMap[i] = 255;
  //  }

  uint8 res = osal_nv_item_init(NVIC_MEMORY_POSITION, sizeof(config_data), (void *)config_data);

  if (res == SUCCESS || res == NV_ITEM_UNINIT)
  {
    return true;
  }
  else
  {
    return false;
  }
}
/*******************************************************
 * @fn      KeusThemeSwitchMiniReadConfigDataIntoMemory
 * @brief   NVIC memory read 
 * @return
 * @param  
 * ****************************************************/
bool KeusThemeSwitchMiniReadConfigDataIntoMemory(void)
{
  uint8 res = osal_nv_read(NVIC_MEMORY_POSITION, 0, sizeof(config_data), (void *)config_data);

  if (res == SUCCESS)
  {
    return true;
  }
  else
  {
    return false;
  }
}


/********************************************************
 * @fn      KeusThemeSwitchMiniWriteConfigDataIntoMemory
 * @brief   NVIC Memory write
 * @return
 * @param
 * *****************************************************/
bool KeusThemeSwitchMiniWriteConfigDataIntoMemory(void)
{
  uint8 res = osal_nv_write(NVIC_MEMORY_POSITION, 0, sizeof(config_data), (void *)config_data);

  if (res == SUCCESS)
  {
    return true;
  }
  else
  {
    return false;
  }
}