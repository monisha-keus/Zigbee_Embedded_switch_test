#include "keus_gpio_util.h"
#include "keus_timer_util.h"
#include "hal_mcu.h"
#include "hal_uart.h"
#include "osal.h"

extern void ledTimerCbk(uint8 timerId);
void leddebounceCbk(uint8 timerId);
void uart1RxCb( uint8 port, uint8 event );
void initUart1(void);

uint8 led1_state = 1,led2_state =1,led3_state  = 1 ;
bool toggle1 = false,toggle2 = false,toggle3 = false;

uint8 count1 = 0,count2 = 0,count3 = 0;
bool debounce = true;

KeusGPIOPin ledPin1 = {0, 5, GPIO_OUTPUT, false, GPIO_HIGH};
KeusGPIOPin ledPin2 = {0, 4, GPIO_OUTPUT, false, GPIO_HIGH};
KeusGPIOPin ledPin3 = {0, 1, GPIO_OUTPUT, false, GPIO_HIGH};

KeusGPIOPin buttonPin1 = {1, 2, GPIO_INPUT, true, GPIO_LOW};
KeusGPIOPin buttonPin2 = {1, 5, GPIO_INPUT, true, GPIO_LOW};
KeusGPIOPin buttonPin3 = {1, 4, GPIO_INPUT, true, GPIO_LOW};


KeusTimerConfig intervalTimer = {
  &ledTimerCbk,
  100,
  true,
  -1,
  0
};

KeusTimerConfig debounceTimer = {
  &leddebounceCbk,
  200,
  true,
  -1,
  0
};

void ledTimerCbk(uint8 timerId) {
  if(toggle1){
    count1++;
  }

  if(toggle2){
    count2++;
  }

  if(toggle3){
    count3++;
  }
  
}


void leddebounceCbk(uint8 timerId) {
  debounce = true;
}


int main( void )
{
  HalUARTInit();
  HAL_BOARD_INIT();
  HAL_ENABLE_INTERRUPTS(); // enable global interrupts
  KeusTimerUtilInit();
  KeusTimerUtilStartTimer();

  KeusGPIOSetDirection(&ledPin1);
  KeusGPIOSetDirection(&buttonPin1);
  KeusGPIOEdgeConfiguration (&buttonPin1,GPIO_FALLING_EDGE);
  KeusGPIOReadPinValue(&buttonPin1);
  KeusGPIOSetPinValue(&ledPin1);
  

  KeusGPIOSetDirection(&ledPin2);
  KeusGPIOSetDirection(&buttonPin2);
  KeusGPIOEdgeConfiguration (&buttonPin2,GPIO_FALLING_EDGE);
  KeusGPIOReadPinValue(&buttonPin2);
  KeusGPIOSetPinValue(&ledPin2);

  KeusGPIOSetDirection(&ledPin3);
  KeusGPIOSetDirection(&buttonPin3);
  KeusGPIOEdgeConfiguration (&buttonPin3,GPIO_FALLING_EDGE);
  KeusGPIOReadPinValue(&buttonPin3);
  KeusGPIOSetPinValue(&ledPin3);

  KeusTimerUtilAddTimer(&intervalTimer);
  KeusGPIOInterruptEnable(&buttonPin1);
  KeusGPIOInterruptEnable(&buttonPin2);
  KeusGPIOInterruptEnable(&buttonPin3);

  initUart1();
  HalUARTWrite( HAL_UART_PORT_1, "KEUS INIT", (byte)osal_strlen("KEUS INIT"));


  while (true) {
   
    if(count1 >= 5){
      count1 = 0;
      ledPin1.state = KeusGPIOToggledState(ledPin1.state); // LED BLINK RATE IS 500 ms
      KeusGPIOSetPinValue(&ledPin1);
    }
    if(count2 >= 10){
      count2 = 0;
      ledPin2.state = KeusGPIOToggledState(ledPin2.state);//LED BLINK RATE IS 1000 ms
      KeusGPIOSetPinValue(&ledPin2);
    }

    if(count3 >= 15){                                   
      count3 = 0;
      ledPin3.state = KeusGPIOToggledState(ledPin3.state);//LED BLINK RATE IS 1500 ms 
      KeusGPIOSetPinValue(&ledPin3);
    }
  }
}

//*******************************************************************************

HAL_ISR_FUNCTION( halKeusPort1Isr, P1INT_VECTOR )
{
  HAL_ENTER_ISR();
  if(debounce){
    debounce = false;
    if (P1IFG & BV(buttonPin1.bit)) {
      if(led1_state){
        ledPin1.state  = GPIO_LOW;
        KeusGPIOSetPinValue(&ledPin1);
        toggle1 = true;
      } 
      else
      {
        ledPin1.state  = GPIO_HIGH;
        KeusGPIOSetPinValue(&ledPin1);
        toggle1 = false;
      } 
      led1_state = !led1_state;

    }

    if (P1IFG & BV(buttonPin2.bit)) {
        if(led2_state){
        ledPin2.state  = GPIO_LOW;
        KeusGPIOSetPinValue(&ledPin2);
        toggle2 = true;
      } 
      else
      {
        ledPin2.state  = GPIO_HIGH;
        KeusGPIOSetPinValue(&ledPin2);
        toggle2 = false;
      } 
      led2_state = !led2_state;
    }

      if (P1IFG & BV(buttonPin3.bit)) {
          if(led3_state){
        ledPin3.state  = GPIO_LOW;
        KeusGPIOSetPinValue(&ledPin3);
        toggle3 = true;
      } 
      else
      {
        ledPin3.state  = GPIO_HIGH;
        KeusGPIOSetPinValue(&ledPin3);
        toggle3 = false;
      } 
      led3_state = !led3_state;
      }
  }
    KeusTimerUtilAddTimer(&debounceTimer);

  /*
    Clear the CPU interrupt flag for Port_0
    PxIFG has to be cleared before PxIF
  */
  P1IFG = 0;
  P1IF = 0;
  
  CLEAR_SLEEP_MODE();
  HAL_EXIT_ISR();
}

//*************************************************************

void initUart1() {
 halUARTCfg_t uart1Config;
 uart1Config.configured           = TRUE;
 uart1Config.baudRate             = HAL_UART_BR_9600;
 uart1Config.flowControl          = FALSE;
 uart1Config.flowControlThreshold = 48;
 uart1Config.rx.maxBufSize        = 128;
 uart1Config.tx.maxBufSize        = 128;
 uart1Config.idleTimeout          = 6;  
 uart1Config.intEnable            = TRUE;
 uart1Config.callBackFunc         = uart1RxCb;
 HalUARTOpen (HAL_UART_PORT_1, &uart1Config);
}

void uart1RxCb( uint8 port, uint8 event ) {
 uint8  ch;

   while (Hal_UART_RxBufLen(port))
 {
   // Read one byte from UART to ch
   HalUARTRead (port, &ch, 1);
   
   if (ch == 's') {
     ledPin1.state = GPIO_LOW;
     KeusGPIOSetPinValue(&ledPin1);
     HalUARTWrite( HAL_UART_PORT_1, "LOW", (byte)osal_strlen("LOW"));
 
   } else if (ch == 'r') {
     ledPin1.state = GPIO_HIGH;
     KeusGPIOSetPinValue(&ledPin1);
     HalUARTWrite( HAL_UART_PORT_1, "HIGH", (byte)osal_strlen("HIGH"));

   }
 }
}
