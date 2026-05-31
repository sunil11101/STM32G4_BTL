#ifndef		__FLASH_BASE_DRIVER__
#define		__FLASH_BASE_DRIVER__


#include "config.h"
#include "Base_driver.h"


	#include "stm32g4xx_hal.h"
	#include "stm32g4xx_hal_flash.h"



static inline uint32_t readWordFromEEPROM (uint32_t address)
{
  return (*(__IO uint32_t *)address);
}




char writeToEEPROM (uint32_t address, uint64_t  * value, int len, uint16_t timeout);
char eraseFlash(uint32_t address, uint8_t pages, uint16_t timeout);
char flashLock(uint16_t timeout);
char flashUnlock(uint16_t timeout);

#endif
