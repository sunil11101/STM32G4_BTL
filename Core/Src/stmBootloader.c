#include "stmBootloader.h"


progStatus bootStatus;
extern CRC_HandleTypeDef hcrc;
extern char * Key;
extern char * Iv;
extern uint32_t Device_ID;

extern uint32_t STATUS_ADDR;
extern uint32_t STATUSB_ADDR;





/******************* function to update configuration of the bms ********************
 * @ address : memory address
 * @ bmsconfig :bmsconfig struct to be saved
 * @ timeout : max time to erase and write
 * @ return the status
 *
char updateStatus(uint32_t address, progStatus *bmsconfig, uint16_t timeout)
{
	int size= sizeof(progStatus);
	if(size%4)
		size=size/4+1;
	else
		size=size/4;

	uint32_t * data= (uint32_t *)bmsconfig;
	return writeToEEPROM(address, data, size, timeout);
}
/*####################### End of updateStatus function #################################*/


/******************* function to update configuration of the bms ********************
 * @ address : memory address
 * @ bmsconfig :bmsconfig struct to be saved
 * @ timeout : max time to erase and write
 * @ return the status
 */
char updateStatus(uint32_t address, progStatus *bmsconfig, uint16_t timeout)
{

//	LOGS("UP\n", 3);
//	return HAL_OK;
	int size= sizeof(progStatus);
		int tsize;
		if(size%64==0)
			tsize=size;
		else
			tsize=(size/64+1)*64;

		char enData[tsize];

		encryptConfig(Key, Iv, enData);
		if(size%8)
			size=size/8+1;
		else
			size/=8;

		uint64_t * data=(uint64_t *)enData;

		if(writeToEEPROM(address, data, size, timeout)==HAL_OK)
		{
//			LOGS("D1\n", 3);
			char status = HAL_OK;
			if(address!=STATUSB_ADDR)
			{
				status = writeToEEPROM(STATUSB_ADDR, data, size, timeout);
				if(status!=HAL_OK)
					status = writeToEEPROM(STATUSB_ADDR, data, size, timeout);
			}
//			LOGIN(status, 10);
			return status;
		}
		return HAL_ERROR;
}
/*####################### End of writeConfigToEEPROM function #################################*/



/******************* function to read configuration of the bms ********************
 * @ address : memory address
 * @ bmsconfig :bmsconfig struct to be saved
 * @ timeout : max time to erase and write
 * @ return the status
 */
char readFromEEPROM(uint32_t address, uint16_t timeout)
{
	int size= sizeof(progStatus);
	//BMS_saved_config tempconfig=readConfigFromEEPROM(address);
	int tsize;
	if(size%64==0)
	{
		tsize=(size/64)*64;
	}
	else
	{
		tsize=(size/64+1)*64;
	}
	char enData[tsize];			// sending array of size of multiple of 64 bytes
//	LOGS("r1\n", 3);
	progStatus  * tempconfig=(progStatus *)enData;
//	LOGS("r2\n", 3);
//	uint8_t d = (*(__IO uint8_t *)address);
//	LOGIN(d, 16);
	*tempconfig = readStatusFromEEPROM(address);
//	LOGS("r3\n", 3);
	decryptConfig(enData, Key,  Iv);

	return HAL_OK;
}
/*####################### End of writeConfigToEEPROM function #################################*/



/* Function to update application address
 * return status of operation
 */
char updateAddress()
{
	char buffer[10]={0};
	char status=uartReceive(buffer, 5, 1500);
	if(status==HAL_OK)
	{
		uint8_t crc=buffer[0];
		uint32_t address=buffer[0] | (uint32_t)buffer[1]<<8 | (uint32_t)buffer[2]<<16|
						(uint32_t)buffer[3]<<24;
		if(checkCRC(buffer, 5))
		{
			sendAck();
			bootStatus.appAddress_b=address;
			bootStatus.status_b&=0x0F;
			bootStatus.status_b|=USER_STATUS;
			if(updateStatus(STATUS_ADDR, &bootStatus, 2000)==HAL_OK)
				return 1;
			else
				return 0;
		}
	}
	return 0;
}


/* Function to send ACK packet to host
 *
 */
void sendAck()
{
	char ar[1]={ACK};
	uartSend(ar, 1, 1000);
}
/************** End of sendAck() function ********************/



/* Function to send NACK packet to host
 *
 */
void sendNack()
{
	char ar[1]={NACK};
	uartSend(ar, 1, 1000);
}
/******* End of sendNack() function **************/




/* Function to read status from EEPROM
 * timeout: max wait time for a operation
 * return status of operation
 */
char getStatus(uint32_t address)
{
	//bootStatus=readStatusFromEEPROM(address);
	char status = readFromEEPROM(address, 2000);
//	if(bootStatus.BMS_id!=Device_ID)
	{
		return readFromEEPROM(STATUSB_ADDR, 2000);
	}
	return status;
}
/************ End of getStatus() function *********************/






/* Function to erase the flash to make it ready for write operation
 * address: address from write to be performed
 * size: no of pages to be erased
 * timeout: time to wait for a operation
 * return status of operation
 */
char eraseFlashUart(uint16_t timeout)
{
	char buffer[10]={0};
	char status;

	status=uartReceive(buffer, 6, 1500);
	if(status==HAL_OK)
	{
		uint8_t crc=buffer[0];
		uint32_t address=buffer[0] | (uint32_t)buffer[1]<<8 | (uint32_t)buffer[2]<<16|
						(uint32_t)buffer[3]<<24;
		uint8_t size = buffer[4];
		if(checkCRC(buffer, 6))
		{
//			LOGIN(address, 16);
//			LOGIN(size, 10);
			sendAck();
			if(eraseFlash(address, size, timeout)==HAL_OK)
			{
				sendAck();
			}
			else
			{
				sendNack();
			}

		}
		else
		{
			sendNack();
		}
	}
	else
	{
		sendNack();
	}
}
/*********** End of initFlash() function *********************/


/* Function to validate the flash uploaded *************************
 * startAddr: start address of code
 * endAddr: end address of code to verify
 * return state of operation
 */
char valFlash(uint32_t startAddr, uint32_t endAddr)
{
	if(startAddr>=endAddr)
		return 0;
	uint32_t * tempData;
	uint32_t crcResult=0;
	uint32_t address;

	// initilize crc hardware
	HAL_CRC_Init(&hcrc);
	//hcrc.Instance->DR=0;
//	crcDeinit();
//	crcInit();
	for(address=startAddr;address<endAddr;address+=4)
	{
		tempData=(uint32_t *)((__IO uint32_t *) address);
		HAL_CRC_Accumulate(&hcrc, tempData, 1);
//		crcAccumulate(tempData, 1);
	}

	tempData=(uint32_t *)((__IO uint32_t *) address);
	crcResult=HAL_CRC_Accumulate(&hcrc, tempData, 1);
//	crcResult = crcAccumulate(tempData, 1);

	// stop crc hardware
	HAL_CRC_DeInit(&hcrc);
//	crcDeinit();
	if(crcResult==0)
		return 1;
	else
		return 0;
}
/**************** End of valFlash function ************************/


/* Function to validate the flash uploaded *************************
 * startAddr: start address of code
 * endAddr: end address of code to verify
 * return state of operation
 */
char valFlashSearch(uint32_t startAddr, uint32_t * endAddr)
{
//	if(startAddr>=endAddr)
//		return 0;
	uint32_t * tempData;
	uint32_t crcResult=0;
	uint32_t address;

	// initilize crc hardware
	HAL_CRC_Init(&hcrc);
	//hcrc.Instance->DR=0;
	tempData=(uint32_t *)((__IO uint32_t *) startAddr);
//	LOGIN(*tempData, 16);
	if(*tempData==0xFFFFFFFF)
	{
		return 0;
	}
	for(address=startAddr;address<0x0801F000;address+=4)
	{
		tempData=(uint32_t *)((__IO uint32_t *) address);
		crcResult = HAL_CRC_Accumulate(&hcrc, tempData, 1);
//		LOGIN(address, 16);
		if(crcResult==0)
		{
			*endAddr=address;
			break;
		}
	}

//	tempData=(uint32_t *)((__IO uint32_t *) address);
//	crcResult=HAL_CRC_Accumulate(&hcrc, tempData, 1);
//	LOGIN(crcResult, 16);

	// stop crc hardware
	HAL_CRC_DeInit(&hcrc);
	if(crcResult==0)
		return 1;
	else
		return 0;
}
/**************** End of valFlash function ************************/




/* Function to validate the flash uploaded by verifying CRC checksum
 * return status of operation
 */
char valFlashUart()
{
	HAL_StatusTypeDef flash_ok = HAL_ERROR;
	uint32_t tickStart=getTick();
	char buffer[10]={0};
	char ack[1]={ACK};
	char nack[1]={NACK};
	char status;

	status=uartReceive(buffer, 5, 1500);
	if(status==HAL_OK)
	{
		uint32_t startAddress=buffer[2] | (uint32_t)buffer[3]<<8 | (uint32_t)buffer[1]<<16|
						(uint32_t)buffer[0]<<24;
		if(checkCRC(buffer, 5))
		{
			sendAck();

			status=uartReceive(buffer, 5, 1500);
			if(status==HAL_OK)
			{
				uint32_t endAddress=buffer[2] | (uint32_t)buffer[3]<<8 | (uint32_t)buffer[1]<<16|
						(uint32_t)buffer[0]<<24;
				if(checkCRC(buffer, 5))
				{
					sendAck();
					if(valFlash(startAddress, endAddress))
					{
						sendAck();
						return HAL_OK;
					}
					else
					{
						sendNack();
						return HAL_ERROR;
					}
					//HAL_CRC_Accumulate(data, 1);
				}
				else
				{
					sendNack();
					return HAL_ERROR;
				}
			}
			else
			{
				sendNack();
				return HAL_TIMEOUT;
			}
		}
		else
		{
			sendNack();
			return HAL_ERROR;
		}
	}
	else
	{
		sendNack();
		return HAL_TIMEOUT;
	}
}
/***************** End of valFlash() function **********************/








/*  Validates the checksum of the message.
 *  Validates the received message through a simple checksum.
 *
 *  Each byte of the message (including the received checksum) is XOR'd with
 *  the previously calculated checksum value. The result should be 0x00.
 *  Note: The first byte of the message is XOR'd with an initial value of 0xFF
 *
 *  \param  *pBuffer    The buffer where the message is stored.
 *  \param  len         The length of the message;
 *  \retval uint8_t     The result of the validation. 1 = OK. 0 = FAIL
 */
char checkCRC(uint8_t *pBuffer, uint32_t len)
{
    uint8_t initial = 0xFF;
    uint8_t result = 0x7F; /* some random result value */

    result = initial ^ *pBuffer++;
    len--;
    while(len--)
    {
        result ^= *pBuffer++;
    }

    //result ^= 0xFF;
//    LOGIN(result, 16);

    if(result == 0x00)
    {
        return 1;
    }
    else
    {
        return 0;
    }
}
/********* End of checkChecksum() function ****************/


/*************Function to write programme to EEPROM
 *	buffer: buffer of data to be uploaded
 *	len: length of buffer
 *	return status of operation
 */
char uploadProg(uint32_t address, char * buffer, uint8_t len)
{
	struct AES_ctx ctx;

	uint8_t key[]={42, 37, 59, 37, 78, 13, 30, 82, 189, 54, 22, 25, 120, 61, 69, 75};
	uint8_t iv[]={21, 77, 104, 101, 79, 34, 92, 62, 36, 60, 31, 69, 47, 42, 29, 57};

	AES_init_ctx_iv(&ctx, key, iv);
	AES_CBC_decrypt_buffer(&ctx, buffer, len);

	if(flashUnlock(2000)!=HAL_OK)
	{
//		LOGS("FU\n", 3);
		return HAL_ERROR;
	}

	HAL_StatusTypeDef flash_ok=HAL_ERROR;
	uint32_t tickStart=HAL_GetTick();
	uint64_t data=0;
	uint16_t i=0;
	while(len>7)
	{
		data=buffer[i]|(uint64_t)buffer[i+1]<<8|(uint64_t)buffer[i+2]<<16 |
				(uint64_t)buffer[i+3]<<24 | (uint64_t)buffer[i+4]<<32 |(uint64_t)buffer[i+5]<<40 |
				(uint64_t)buffer[i+6]<<48 | (uint64_t)buffer[i+7]<<56;
		char status = HAL_FLASH_Program (FLASH_TYPEPROGRAM_DOUBLEWORD, address+i, data);
		//LOGIN(address+i, 16);
		if(status!=HAL_OK)
		{
			LOGS("ER\n", 3);
			flashLock(2000);
			return status;
		}
		//LOGIN(i, 10);
		//HAL_FLASH_Program (0x00, address+i, buffer[i]);
		i+=8;
		len-=8;
		//i++;
	}

	if(len)
	{
		for(char j=0;j<len;j++)
		{
			data=(uint32_t)buffer[i+j]<<(j*8);
		}
		if(HAL_FLASH_Program (FLASH_TYPEPROGRAM_DOUBLEWORD, address+i, data)!=HAL_OK)
			return HAL_ERROR;
	}
	if(flashLock(2000)!=HAL_OK)
	{
		LOGS("FL\n", 3);
		return HAL_ERROR;
	}
	return HAL_OK;
}
/**************** End of uploadProg function ******************************/



/* Function to downlaod programme to flash reading from UART
 * using hal uart functions
 * return status of operation
 */
char uploadProgUart()
{
	uint8_t buffer[255]={0};
	char ack[1]={ACK};
	char nack[1]={NACK};
	char status;

	// Read starting address
	status=uartReceive(buffer, 6, 1500);
	if(status==HAL_OK)
	{
		uint8_t crc=buffer[0];
		uint32_t address=buffer[0] | (uint32_t)buffer[1]<<8 | (uint32_t)buffer[2]<<16|
						(uint32_t)buffer[3]<<24;
		uint8_t len = buffer[4];
		if(checkCRC(buffer, 6) && address % 4==0)
		{
//			LOGIN(address, 16);
			sendAck();
			status=uartReceive(buffer, len+1, 2000);
			if(status!=HAL_OK)
			{
				return HAL_TIMEOUT;
			}
			if(checkCRC(buffer, len+1))
			{
				if(uploadProg(address, buffer, len)==HAL_OK)
					sendAck();
				else
					sendNack();
			}
			else
			{
				sendNack();
			}
		}
		else
		{
			sendNack();
		}
	}
	else
	{

	}
}

/* Function to Enable read protection of Flash
 *	return status of operation
 */
char readProtectEn()
{
	/*
	if(READ_BIT(FLASH->OBR, FLASH_OBR_RDPRT1)==0)
	{

		//HAL_UART_Transmit(&huart1, "C\n",2, 1000);
		if(flashUnlock(2000)!=HAL_OK)
		{
			return HAL_ERROR;
		}
		if(HAL_FLASH_OB_Unlock()!=HAL_OK)
		{
			return HAL_ERROR;
		}

		// Erase the option bytes
		SET_BIT(FLASH->CR, FLASH_CR_OPTER);
		SET_BIT(FLASH->CR, FLASH_CR_STRT);

		// wait for operation to complete
		if(FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE)!=HAL_OK)
		{
			return HAL_ERROR;
		}
		//HAL_UART_Transmit(&huart1, "D\n",2, 1000);
		// clear erase bit
		CLEAR_BIT(FLASH->CR, FLASH_CR_OPTER);

		// set option byte programming bit
		SET_BIT(FLASH->CR, FLASH_CR_OPTPG);

		// write rdb option byte
		WRITE_REG(OB->RDP, 0xBB);

		// wait for operation to complete
		if(FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE)!=HAL_OK)
		{
			return HAL_ERROR;
		}
		//HAL_UART_Transmit(&huart1, "E\n",2, 1000);
		// clear programming bit
		CLEAR_BIT(FLASH->CR, FLASH_CR_OPTPG);

		HAL_FLASH_OB_Lock();
		if(flashLock(2000)!=HAL_OK)
		{
			return HAL_ERROR;
		}
		//sendAck();
		if(HAL_FLASH_OB_Launch()!=HAL_OK)
		{
			return HAL_ERROR;
		}
		//HAL_UART_Transmit(&huart1, "F\n",2, 1000);
	}
	*/
	return HAL_OK;
	//sendAck();
}
/************* End of readProtectionEn() function ****************/




/* Function to Enable read protection of Flash
 *	return status of operation
 */
char readProtectDis()
{
	/*
	if(READ_BIT(FLASH->OBR, FLASH_OBR_RDPRT1)!=0)
	{
		flashUnlock(2000);
		HAL_FLASH_OB_Unlock();

		// Erase the option bytes
		SET_BIT(FLASH->CR, FLASH_CR_OPTER);
		SET_BIT(FLASH->CR, FLASH_CR_STRT);

		// wait for operation to complete
		FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

		// clear erase bit
		CLEAR_BIT(FLASH->CR, FLASH_CR_OPTER);

		// set option byte programming bit
		SET_BIT(FLASH->CR, FLASH_CR_OPTPG);

		// write rdb option byte
		WRITE_REG(OB->RDP, 0xAA);

		// wait for operation to complete
		FLASH_WaitForLastOperation((uint32_t)FLASH_TIMEOUT_VALUE);

		// clear programming bit
		CLEAR_BIT(FLASH->CR, FLASH_CR_OPTPG);

		HAL_FLASH_OB_Lock();
		flashLock(2000);
		//sendAck();
		HAL_FLASH_OB_Launch();
	}*/
	//sendAck();
}
/************* End of readProtectionEn() function ****************/




/* Function to read version of bootloader
 * return version in range 0-255
 */
uint8_t readVersion(uint16_t delay)
{

}
/************* End of readVersion() function ****************/



/* Function to read Status of current programme i.e. if it was uploaded successfully
 * return 1 if programme is ok other wise currupted of download is in process
 */
char readStatus(uint16_t delay)
{

}
/************* End of readStatus() function ****************/

/* Function to read no of time programme updated in MCU
 * return counter value
 */
uint16_t readCount(uint16_t delay)
{

}
/************* End of downloadProg() function ****************/

/* Function to read no time programme failed to download in Flash
 * return counter value
 */
uint16_t readErrorCount(uint16_t delay)
{

}
/************* End of readErrorCount() function ****************/







