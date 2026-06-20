/*
 * CRC_base_driver.c
 *
 *  Created on: 09-Sep-2021
 *      Author: ubuntu
 */


#include "Base_driver.h"


	extern CRC_HandleTypeDef hcrc;
	#define		HCRC		hcrc
//	extern IWDG_HandleTypeDef hiwdg;




/******* Get Common Timer value
 *
 */
uint32_t getTick()
{
	return HAL_GetTick();
}
/**************** End of function getTick ****************/

/* Function for Delay in ms
 *
 */
void delayMs(uint32_t delay)
{
	HAL_Delay(delay);
}


/* Function to write the GPIO pin state
 *
 */
void gpioWrite(uint8_t port, uint32_t pin, uint8_t state)
{
	switch(port)
	{
	case 0:
		HAL_GPIO_WritePin(GPIOA, pin, state);
		break;
	case 1:
		HAL_GPIO_WritePin(GPIOB, pin, state);
		break;
	case 2:
		HAL_GPIO_WritePin(GPIOC, pin, state);
		break;
	}
}


/* Function to write the GPIO pin state
 *
 */
void gpioToggle(uint8_t port, uint32_t pin)
{
	switch(port)
	{
	case 0:
		HAL_GPIO_TogglePin(GPIOA, pin);
		break;
	case 1:
		HAL_GPIO_TogglePin(GPIOB, pin);
		break;
	case 2:
		HAL_GPIO_TogglePin(GPIOC, pin);
		break;
	}
}


/* Function to reset the MCU ***
 *
 */
void rstMcu()
{
	NVIC_SystemReset();
}



/***** Function to init the CRC
 *
 */
void crcInit()
{

//	hcrc.Instance = CRC;
//	hcrc.Init.DefaultPolynomialUse = DEFAULT_POLYNOMIAL_ENABLE;
//	hcrc.Init.DefaultInitValueUse = DEFAULT_INIT_VALUE_ENABLE;
//	hcrc.Init.InputDataInversionMode = CRC_INPUTDATA_INVERSION_NONE;
//	hcrc.Init.OutputDataInversionMode = CRC_OUTPUTDATA_INVERSION_DISABLE;
//	hcrc.InputDataFormat = CRC_INPUTDATA_FORMAT_WORDS;
//	if (HAL_CRC_Init(&hcrc) != HAL_OK)
//	{
//	  Error_Handler();
//	}
}



/***** Function to deinit the CRC
 *
 */
void crcDeinit()
{
//	HAL_CRC_DeInit(&HCRC);
}



/****** Functiont to calculate CRC with already accumulated result
 *
 */
uint32_t crcAccumulate(uint32_t * data, uint8_t len)
{
	return HAL_CRC_Accumulate(&HCRC, data, len);
}


/********** Function to reset the watchdog couner value *******************
 *
 */
void resetWatchdog()
{
//	HAL_IWDG_Refresh(&hiwdg);
}




















