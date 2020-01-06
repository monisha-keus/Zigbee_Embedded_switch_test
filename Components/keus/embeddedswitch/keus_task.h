/****************************
 * ****KEUS_TASK.H********
 * **************************/

#ifndef KEUS_TASK_H_INCLUDED
#define KEUS_TASK_H_INCLUDED

#include "keus_gpio_util.h"
#include "keus_timer_util.h"
#include "ZComDef.h"
#include "OSAL_Nv.h"
#include "keus_uart.h"

#define NVIC_MEMORY_POSITION 0x10

#define LED_OFF         GPIO_HIGH
#define LED_ON          GPIO_LOW

#define LED_ACTIVE      GPIO_LOW
#define LED_INACTIVE    GPIO_HIGH
#define BUTTON_ACTIVE   GPIO_LOW
#define BUTTON_INAVTIVE GPIO_HIGH

#define KEUS_EMBEDDEDSWITCH_BUTTON_POLL_TIME 1000
#define KEUS_EMBEDDEDSWITCH_RESET_BTN_TIMEOUT 10000
//extern uint8 button_Pressed;

#define ONOFF                   1
#define DIMMING                 2
#define FAN_CONTROLLER          3

#define ON      0x01
#define OFF     0x00
#define HIGH    255
#define LOW     0x00

//*****config data 
struct CONFIG_INFO_t{
    uint8   led;
    uint8   valid_state;
    uint8   config_id; 
};

bool KeusThemeSwitchMiniMemoryInit(void);
bool KeusThemeSwitchMiniReadConfigDataIntoMemory(void);
bool KeusThemeSwitchMiniWriteConfigDataIntoMemory(void);

#endif





