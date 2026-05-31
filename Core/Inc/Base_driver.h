#ifndef			__BASE_DRIVER__
#define			__BASE_DRIVER__

#include "config.h"


	#include "stm32g4xx_hal.h"



#define			CAN_IO_PIN				((uint16_t)0x0100)
#define			LED_INDICATION_PIN		((uint16_t)0x0100)

//#define			VREFINT_CAL_ADDR
//#define		STATUS_ADDR		0x0801F800
//#define		STATUSB_ADDR	0x0801F000

//#define		UID_ADDR			0x1FFFF7AC
//#define		UID_ADDR			0x1FFF7590

//#define 	VREFINT_CAL_ADDR    0x1FFFF7BA
#define 	VREFINT_CAL_ADDR    0x1FFF756A


uint32_t getTick();
void gpioWrite(uint8_t port, uint32_t pin, uint8_t state);
void gpioToggle(uint8_t port, uint32_t pin);

void resetWatchdog();


#endif
