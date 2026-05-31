#ifndef			__MY_MAIN__
#define			__MY_MAIN__

#include "config.h"
#include "Jump_base_driver.h"
#include "stmBootloader.h"


	#include "stm32g4xx_hal.h"


void init();
uint32_t cal_crc32(char * data,unsigned char  len);


uint8_t getCheckSum(uint8_t * data, uint16_t size, uint8_t init);

void clearCruptedFlash();


#endif
