/*
 * Jump_base_driver.c
 *
 *  Created on: 09-Sep-2021
 *      Author: ubuntu
 */

#include "Jump_base_driver.h"

typedef void (*pFunction)(void);



#define APP_ADDRESS			0x08008000U


void JumpToApplication(void) {
	uint32_t JumpAddress;

	pFunction Jump;

	/* Disable all interrupts */
	__disable_irq();

	/* Stop SysTick */
	SysTick->CTRL = 0;
	SysTick->LOAD = 0;
	SysTick->VAL = 0;

	/* Reset clock configuration */
	HAL_RCC_DeInit();

	/* Deinitialize HAL */
	HAL_DeInit();

	/* Clear all interrupt enable registers */
	for (uint8_t i = 0; i < 8; i++) {
		NVIC->ICER[i] = 0xFFFFFFFF;
	}

	/* Clear all pending interrupt flags */
	for (uint8_t i = 0; i < 8; i++) {
		NVIC->ICPR[i] = 0xFFFFFFFF;
	}

	/* Set Main Stack Pointer */
	__set_MSP(*(volatile uint32_t*) APP_ADDRESS);

	/* Get application's Reset Handler address */
	JumpAddress = *(volatile uint32_t*) (APP_ADDRESS + 4);

	/* Convert to function pointer */
	Jump = (pFunction) JumpAddress;

	/* Jump to application */
	Jump();
}


