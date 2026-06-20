/*
 * Flash_base_driver.c
 *
 *  Created on: 09-Sep-2021
 *      Author: ubuntu
 */

#include "Flash_base_driver.h"

//extern uint32_t STATUSB_ADDR;


/******************* function to write data to EEPROM ********************
 * @ address : memory address
 * @ value :array of uint32_t to be stored
 * @ len : length of array
 * @ timeout : max time to erase and write
 * @ return the status
 */
char writeToEEPROM (uint32_t address, uint64_t  * value, int len, uint16_t timeout)
{
	if(address>=MIN_ADDRESS && address<MAX_ADDRESS /*|| (address>=STATUSB_ADDR)*/)
			return HAL_ERROR;

//If MCU is STM32L4
#ifdef			STM32L4
	HAL_StatusTypeDef flash_ok = HAL_ERROR;
	uint32_t tickStart=HAL_GetTick();
	while (flash_ok != HAL_OK)
	{
		if(HAL_GetTick()-tickStart >timeout)
			return HAL_TIMEOUT;
		flash_ok = HAL_FLASH_Unlock();
	}
	flash_ok = HAL_ERROR;
	FLASH_EraseInitTypeDef config;
	config.TypeErase=0;				//pages
	config.Banks= FLASH_BANK_1;
	config.Page = (address-0x8000000)/2048;
	config.NbPages=1;
	uint32_t erase_status=0;

	tickStart=HAL_GetTick();
	while (flash_ok != HAL_OK)
	{
		if(HAL_GetTick()-tickStart >timeout)
	  		 return HAL_TIMEOUT;
		flash_ok =  HAL_FLASHEx_Erase(&config, &erase_status);
	}

	if(erase_status==0)
		return HAL_ERROR;


	for(int i=0;i<len;i++)
	{
	#ifdef IWDG_EN
		HAL_IWDG_Refresh(&hiwdg);
	#endif
		flash_ok = HAL_ERROR;
		tickStart=HAL_GetTick();
		while (flash_ok != HAL_OK)
		{
		  if((HAL_GetTick()-tickStart) >timeout)
				 return HAL_TIMEOUT;
		  flash_ok = HAL_FLASH_Program (FLASH_TYPEPROGRAM_DOUBLEWORD, address+(i*8), *(value+i));
		}
	}

	flash_ok = HAL_ERROR;
	while (flash_ok != HAL_OK)
	{
	 if(HAL_GetTick()-tickStart >timeout)
	  		 return HAL_TIMEOUT;
	flash_ok = HAL_FLASH_Lock ();
	}
	//LOGS("C\n", 2);
	return flash_ok;

//If MCU is G0
#elif			defined(STM32G0) || defined(STM32C0)
	HAL_StatusTypeDef flash_ok = HAL_ERROR;
	uint32_t tickStart = getTick();
	while (flash_ok != HAL_OK)
	{
		if(getTick()-tickStart >timeout)
			return HAL_TIMEOUT;
		flash_ok = HAL_FLASH_Unlock();
	}
	//LOGS("C1\n", 3);
	flash_ok = HAL_ERROR;
	FLASH_EraseInitTypeDef config;
	config.TypeErase=FLASH_TYPEERASE_PAGES;				//pages
//	config.Banks= FLASH_BANK_1;
	config.Page = (address-0x8000000)/2048;
	config.NbPages=1;
	uint32_t erase_status=0;

//	if(config.Page>=FLASH_PAGE_NB)
//	{
////		config.Banks = FLASH_BANK_2;
//		config.Page -= FLASH_PAGE_NB;
//	}

	tickStart=getTick();
	while (flash_ok != HAL_OK)
	{
		if(getTick()-tickStart >timeout)
	  		 return HAL_TIMEOUT;
		flash_ok =  HAL_FLASHEx_Erase(&config, &erase_status);
	}

	//LOGS("C2\n", 3);

	if(erase_status==0)
		return HAL_ERROR;

//	LOGS("C3\n", 3);
//	delayMs(500);
//	LOGS("C2\n", 3);
//	HAL_GPIO_WritePin(GPIOB, GPIO_PIN_12, GPIO_PIN_RESET);
//	delayMs(10);
	for(int i=0;i<len;i++)
	{
	#ifdef IWDG_EN
		HAL_IWDG_Refresh(&hiwdg);
	#endif
		flash_ok = HAL_ERROR;
		tickStart=getTick();
		while (flash_ok != HAL_OK)
		{
		  if((getTick()-tickStart) >timeout)
				 return HAL_TIMEOUT;

		  flash_ok = HAL_FLASH_Program (FLASH_TYPEPROGRAM_DOUBLEWORD, address+(i*8), *(value+i));

//		  LOGS("FWe\n", 4);
		}

		//LOGS("C4\n", 3);
	}
//	LOGS("C4\n", 3);

	flash_ok = HAL_ERROR;
	while (flash_ok != HAL_OK)
	{
	 if(getTick()-tickStart >timeout)
	  		 return HAL_TIMEOUT;
	flash_ok = HAL_FLASH_Lock ();
	}
	//LOGS("C\n", 2);
	return flash_ok;

//If MCU is MM32
#elif			defined(MM32F1)

#endif
}
/*####################### End of writeToEEPROM function #########################*/




/*** Function to erase flash to make it ready for write operation
 * address: address from write to be performed
 * size: no of pages to be erased
 * timeout: time to wait for a operation
 * return status of operation
 */
char eraseFlash(uint32_t address, uint8_t pages, uint16_t timeout)
{
	//LOGS("ME\n", 3);
//	return HAL_ERROR;
#ifdef			APP_BOOTLOADER
	if(address>=MIN_ADDRESS && address<MAX_ADDRESS ||(address>=STATUSB_ADDR && pages>1))
		return HAL_ERROR;
#else
	if(/*address>=MIN_ADDRESS && */address<MAX_ADDRESS)
		return HAL_ERROR;
#endif
	uint32_t tickStart=getTick();
	HAL_StatusTypeDef flash_ok = HAL_ERROR;
	FLASH_EraseInitTypeDef config;
	config.TypeErase = FLASH_TYPEERASE_PAGES;
	config.Page=(address-0x8000000)/2048;
	config.Banks= FLASH_BANK_1;
	config.NbPages=pages;
	uint32_t erase_status=0;

	__HAL_FLASH_CLEAR_FLAG(
		  FLASH_FLAG_OPTVERR |
	      FLASH_FLAG_PGAERR  |
	      FLASH_FLAG_PGSERR  |
	      FLASH_FLAG_WRPERR  |
	      FLASH_FLAG_PROGERR);

//	LOGIS(config.NbPages, 10);
//	LOGIN(pages, 10);
//	LOGS("P:", 2);
//	LOGIS(config.Page, 10);
//	LOGIN(address, 10);

//	if((address + ((config.NbPages-1) * 2048))>=STATUSB_ADDR && pages>1)
//	{
//		return HAL_ERROR;
//	}


	uint8_t extraFlag = 0;
	uint32_t extraPage = 0;
	uint32_t extraNbPages = 0;
//	if(config.Page>=FLASH_PAGE_NB)
//	{
////		LOGS("2B\n", 3);
//		config.Banks = FLASH_BANK_2;
//		config.Page -= FLASH_PAGE_NB;
//	}
//	else if((config.Page+pages)>FLASH_PAGE_NB)		//if no of pages to erase is from both banks
//	{
////		LOGS("bB\n", 3);
//		config.NbPages = FLASH_PAGE_NB - config.Page;
//		extraFlag = 1;
//		extraPage = 0;
//		extraNbPages = pages - config.NbPages;
//	}




//	LOGIS(config.Banks, 10);
//	LOGIS(config.NbPages, 10);
//	LOGIN(config.Page, 10);
	if (flashUnlock(timeout)!=HAL_OK)
	{
		//LOGS("FU\n", 3);
		return HAL_ERROR;
	}

	FLASH_PageErase(16, 0);
	tickStart=getTick();
	while (flash_ok != HAL_OK)
	{
//		LOGIN(config.Banks, 10);
//		LOGS("E\n",2);
		if((getTick()-tickStart) >timeout)
			 return HAL_TIMEOUT;
		flash_ok =  HAL_FLASHEx_Erase(&config, &erase_status);
	}
//	LOGIN(erase_status, 16);

	if(erase_status!=0xFFFFFFFF)
		return HAL_ERROR;

//	if(extraFlag)
//	{
//		config.Page=extraPage;
//		config.Banks= FLASH_BANK_2;
//		config.NbPages=extraNbPages;
//		flash_ok = HAL_ERROR;
//		while (flash_ok != HAL_OK)
//		{
//			LOGS("Ex\n",3);
//			if((getTick()-tickStart) >timeout)
//				 return HAL_TIMEOUT;
//			flash_ok =  HAL_FLASHEx_Erase(&config, &erase_status);
//		}
//	}
	flashLock(timeout);
	return HAL_OK;
}
/******************* End of reaseFlash function *****************/


/* Function to Lock the flash for write operation
 * timeout: max wait time for operation
 * return status of operation
 */
char flashLock(uint16_t timeout)
{
	HAL_StatusTypeDef flash_ok = HAL_ERROR;
	uint32_t tickStart=getTick();
	while (flash_ok != HAL_OK)
	{
		if((getTick()-tickStart) >timeout)
			return HAL_TIMEOUT;
		flash_ok = HAL_FLASH_Lock ();
	}
	return flash_ok;
}
/************ End of flashLock() Function ********************/



/* Function to unlock the flash for write operation
 *	return status of operation
 */
char flashUnlock(uint16_t timeout)
{
	HAL_StatusTypeDef flash_ok = HAL_ERROR;
	uint32_t tickStart = getTick();
	while (flash_ok != HAL_OK)
	{
		if((getTick()-tickStart) >timeout)
			return HAL_TIMEOUT;
		flash_ok = HAL_FLASH_Unlock ();
	}
	return flash_ok;
}
/********** End of flashUnlock() function **************/
