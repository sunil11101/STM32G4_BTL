#ifndef			__UART_BASE_DRIVER__
#define			__UART_BASE_DRIVER__

#include "config.h"


	#include "stm32g4xx_hal.h"

uint8_t uartReceive(uint8_t * buffer, uint16_t len, uint32_t delay);
uint8_t logSend(uint8_t * data, uint16_t len, uint32_t delay);
uint8_t uartSend(uint8_t * data, uint16_t len, uint32_t delay);

void uartDeinit(uint8_t sel);
void uartInit(uint8_t sel);


#endif
