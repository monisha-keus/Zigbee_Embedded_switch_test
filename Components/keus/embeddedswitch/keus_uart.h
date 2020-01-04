#ifndef KEUS_UART_H_INCLUDED
#define KEUS_UART_H_INCLUDED

#include "keus_gpio_util.h"
#include "hal_uart.h"
#include "OSAL.h"

#define MAX_UART_BUF_LEN    20

#define UART_INITIATOR  0x28
#define UART_TERNINATOR 0x29

#define TASK_UART_ACK 5

void uart0RxCb( uint8 port, uint8 event );
void initUart0(void);
void initUart1(void);
void uart1RxCb( uint8 port, uint8 event );



// void uart1RxCb( uint8 port, uint8 event );
// void initUart1(void);


#endif  //KEUS_UART_H_INCLUDED
