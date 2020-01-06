
/********************************************************
 * @fn  This function is to declare all GPIOs
 * *****************************************************/

#include "keus_task.h"

#define UART0     HAL_UART_PORT_0
//#define UART1     HAL_UART_PORT_1

bool debounce = false;
//bool toggle = false, toggle1 = false, toggle3 = false;
uint8 count = 0, count1 = 0, count2 = 0;//led_state;
uint8 button_Pressed =0,button_state = 0;

void ledTimerCbk(uint8 timerId);
void leddebounceCbk(uint8 timerId);
//void check_sw_pressed(void);


typedef void (*KeusBtnCbk)(void);
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
/****************************************************************************
 * **************************FUNCTION DECLARATION****************************
 * ***************************************************************************/
void KEUS_init_fnc(void);
void KEUS_loop(void);

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
  if (buttonPin1.state == BUTTON_ACTIVE) {
    buttonManager.elapsedTime += KEUS_EMBEDDEDSWITCH_BUTTON_POLL_TIME;
    count++;
    if(count >= 100){
      ledPin1.state = LED_ON;
      KeusGPIOSetPinValue(&ledPin1);
    }
    else if (buttonManager.elapsedTime >= buttonManager.btnDelay) {
      buttonManager.elapsedTime = 0;
      ledPin1.state = KeusGPIOToggledState(ledPin1.state); //toggle led at button pressed
      KeusGPIOSetPinValue(&ledPin1);
    }
  } 
  else {
    buttonManager.elapsedTime = 0;
    count = 0;
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
  buttonManager.btnDelay = KEUS_EMBEDDEDSWITCH_RESET_BTN_TIMEOUT;
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
    //check_sw_pressed();
    
  }
}

// void check_sw_pressed(void){
// if (buttonPin1.state == BUTTON_ACTIVE) {
//     buttonManager.elapsedTime += KEUS_EMBEDDEDSWITCH_BUTTON_POLL_TIME;
    
//     if (buttonManager.elapsedTime >= buttonManager.btnDelay) {
//       buttonManager.elapsedTime = 0;
//       ledPin1.state = KeusGPIOToggledState(ledPin1.state); //toggle led at button pressed
//       KeusGPIOSetPinValue(&ledPin1);
//     }
//   } else {
//     buttonManager.elapsedTime = 0;
//   }
// }