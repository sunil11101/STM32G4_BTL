/*
 * Jump_base_driver.c
 *
 *  Created on: 09-Sep-2021
 *      Author: ubuntu
 */

#include "Jump_base_driver.h"

typedef void (*pFunction)(void);



//#define START_ADDR			0x08006000U



/************** Function to jump to main application ******************
 *address : Memory address where programe to be jump
 */
void jumpToApp(uint32_t address)
{
	if (((*(__IO uint32_t*)address) & 0x2FFE0000 ) == 0x20000000)
	{

	pFunction JumpToApplication;
	uint32_t JumpAddress;
	  /* Jump to user application */
	  JumpAddress = *(__IO uint32_t*) (address + 4);
	  JumpToApplication = (pFunction) JumpAddress;
	  /* Initialize user application's Stack Pointer */
	  __set_MSP(*(__IO uint32_t*) address);

	  __disable_irq();
	  SCB->VTOR = FLASH_BASE | 0x8000;

	  HAL_RCC_DeInit();
	  SysTick->CTRL = 0;
	  SysTick->LOAD = 0;
	  SysTick->VAL = 0;
	  __set_PRIMASK(1);

	  printf("Starting Firmware 2 . . .  \r\n");
	  HAL_DeInit();

	  JumpToApplication();
	}
	else
	{
		NVIC_SystemReset();
	}
}
/************* End of function jumpToApp *****************/




