/*
 * my_main.c
 *
 *  Created on: 09-Sep-2021
 *      Author: ubuntu
 */

#include "my_main.h"


char canStatus=0;
char key[16]={0};
char iv[16]={0};

char * Key=key;
char * Iv=iv;
uint32_t canIdleTime=0;

uint32_t Device_ID;
char arr[12]={0};
uint16_t vrefint_cal;

uint8_t Status=0;
uint8_t initComplete = 0;

uint16_t progStageIndex = 0;


#define 		APP_ADDR		0x8006000


#ifdef			EXTRA_MEMORY_MCU
//	uint32_t STATUS_ADDR = 0x807F800;				//For 512 kb utilization
//	uint32_t STATUSB_ADDR = 0x807F000;

	uint32_t STATUS_ADDR = 0x803F800;				//For 256 kb utilization
	uint32_t STATUSB_ADDR = 0x803F000;
#else
	uint32_t STATUS_ADDR = 0x801F800;
	uint32_t STATUSB_ADDR = 0x801F000;
//	uint32_t STATUS_ADDR = 0x803F800;
//		uint32_t STATUSB_ADDR = 0x803F000;
#endif


	uint8_t BVERSION = BVERSIONM;
//pFunction JumpToApplication;
//uint32_t JumpAddress;


void init()
{

	static uint16_t retryCounter = 0;

//#if		defined(BMS) || defined(CAN_DISPLAY) || defined(SMART_CHARGER)
////uartInit(1);
//uartDeinit(2);
//#endif


 //HAL_UART_Transmit(&huart1, "rst\n", 4, 1000);
  //HAL_GPIO_WritePin(GPIOA, CAN_IO_Pin, GPIO_PIN_SET);
  //gpioWrite(0, CAN_IO_PIN, EN);


  uint8_t buffer[5]={0};
  char status=HAL_OK;
  char ack[2]={ACK, ACK};
  char nack[2]={NACK, NACK};


  progStageIndex = 1;

	for(int i=0;i<3;i++)
	{
		uint32_t temp = readWordFromEEPROM(UID_BASE+i*4);
		//LOGIN(temp, 16);
#ifdef		NEWFM
		for(char j=0;j<4;j++)
#else
		for(char j=i*4;j<4*(i+1);j++)
#endif
		{
#ifdef		NEWFM
			arr[i*4+j] = temp>>(j*8);
#else
			arr[j]=temp<<(j%4);
#endif

		}
	}


	progStageIndex = 2;
	Device_ID=cal_crc32(arr, 12);

	// VREFINT calibration value
	vrefint_cal= *((uint16_t*)VREFINT_CAL_ADDR); // read VREFINT_CAL_ADDR memory location

	// get key for encryption of data
	genKey(arr, vrefint_cal, 0x2992, key);
	// get Iv
	memcpy(Iv, arr, 12);
	iv[12]=0x62;
	iv[13]=0x72;
	iv[14]=0x00;
	iv[15]=0xFA;

//	LOGS("KEY:\n", 5);
//	for(uint8_t i=0;i<16;i++)
//	{
//		LOGIS(key[i], 16);
//	}
//
//	LOGS("\nIV:\n", 5);
//	for(uint8_t i=0;i<16;i++)
//	{
//		LOGIS(iv[i], 16);
//	}

	uint32_t diff;

	progStageIndex = 3;

	/**************** Block to check Flash size and validate Config accordingly *************/

	LOGIN(getTick(), 10);

	LOGIN(STATUS_ADDR, 16);
	Status = BOOT_STATE_INIT_1;

	getStatus(STATUS_ADDR);

	LOGIN(Device_ID, 16);

	progStageIndex = 4;

	LOGIN(getTick(), 10);

//	//LOGIN(bootStatus.BMS_id, 16);
//  if(bootStatus.BMS_id!=Device_ID)
//  {
//	  getStatus(STATUSB_ADDR);
//	  progStageIndex = 5;
//
//	  LOGIN(bootStatus.BMS_id, 16);
//
//	  if(bootStatus.BMS_id!=Device_ID)
//	  {
////		  bootStatus.deviceId =Device_ID;
//		  LOGS("CER\n", 4);
//
//		  //HAL_UART_Transmit(&huart1, "asd2\n", 5, 1000);
//		  bootStatus.status_b=0;
//		  bootStatus.progCounter_b=0;
//		  bootStatus.progSuccessCounter_b=0;
//		  bootStatus.appAddress_b=0x8008000;
//		  bootStatus.status_b|=USER_STATUS;
//		  uint32_t endAddr = 0;
//		  if(valFlashSearch(0x8008000, &endAddr)==1)
//		  {
//			  if(endAddr>0x80A000)
//			  {
//				  bootStatus.status_b|=PROG_STATUS;
//				  bootStatus.version_b=BVERSION;
//				  bootStatus.BMS_id = Device_ID;
//			  }
//		  }
//
//
//		  uint8_t status = updateStatus(STATUS_ADDR, &bootStatus, 2000);
//		  LOGIN(status, 10);
//
//		  Status = BOOT_STATE_CNFGERR_2;
//		  delayMs(2000);
//
//	  }
//	  else
//	  {
//		  updateStatus(STATUS_ADDR, &bootStatus, 2000);
//	  }
//  }
//  else
//  {
//	  progStageIndex = 6;
//	  getStatus(STATUSB_ADDR);
//	  if(bootStatus.BMS_id!=Device_ID)
//	  {
////		  LOGS("SCER\n", 5);
//		  char status = HAL_ERROR;
//		  for(char i=0;i<3 && status!=HAL_OK;i++)
//		  {
//			  status = getStatus(STATUS_ADDR);
//			  if(status==HAL_OK)
//			  {
//				  updateStatus(STATUSB_ADDR, &bootStatus, 2000);
//			  }
//		  }
//	  }
//  }

  progStageIndex = 7;


  	//canInit(Device_ID, bootStatus.cBoud);
  //LOGS("C1\n", 3);
  //Updating version of bootloader
  bootStatus.version_b=BVERSION;



canIdleTime = getTick();

initComplete = 1;
Start:
resetWatchdog();


#ifdef			APP_BOOTLOADER
	uartSend("bapp\n", 5,1000);
#else
	LOGS("a3\n", 3);
#endif


  	status=uartReceive(buffer, 2, 700);

//LED INDICATION
//HAL_GPIO_WritePin(LED_INDICATION_GPIO_Port, LED_INDICATION_Pin, GPIO_PIN_RESET);
//gpioWrite(1, LED_INDICATION_PIN, DIS);

//delayMs(50);

//LOGS("s1\n", 3);
//LOGIN((uint32_t)status, 10);
#ifdef		UART_FLASH_EN
  if(status==HAL_OK)
  {
//	  LOGS("s1\n", 3);
	  if(buffer[0]==START_BOOT && (buffer[0] & buffer[1])==0)
	  {
//		  LOGS("En\n", 3);
		  Status = BOOT_STATE_UPLOADING_3;
		  // deinit CAN
		  //HAL_CAN_DeactivateNotification(&hcan1, CAN_IT_RX_FIFO0_MSG_PENDING |CAN_IT_RX_FIFO1_MSG_PENDING);
//		  canNotification(DIS);

		  sendAck();
		  while(uartReceive(buffer, 1, 200)==HAL_OK);
		  uint32_t tickStart=getTick();
		  while(getTick()-tickStart<7000)
		  {
			  status=uartReceive(buffer, 2, 1000);
			  if(status==HAL_OK)
			  {
//				  LOGS("S2\n", 3);
//				  LOGIN(buffer[0], 16);
				  if((buffer[0] & buffer[1]) == 0)
				  {
					  switch(buffer[0])
					  {
						  case PROGRAMME:
						  {
							  if(bootStatus.status_b & 0x0F != 0x00)
							  {
								  bootStatus.status_b&=0xF0;
								  updateStatus(STATUS_ADDR, &bootStatus, 2000);
							  }
							  sendAck();
							  uploadProgUart();
//							  getStatus(STATUS_ADDR);
							  bootStatus.version_b=BVERSION;
							  break;
						  }
						  case ERASE:
						  {
							  bootStatus.progCounter_b+=1;
//							  updateStatus(STATUS_ADDR, &bootStatus, 2000);
							  sendAck();
							  eraseFlashUart(2000);
							  //sendAck();
							  break;
						  }
//						  case GET_BID:
//						  {
//							  sendAck();
//							  char res[14]={0};
//							  char tempAr[1]={14};
//							  uartSend(tempAr, 1, 1000);
//							  memcpy(res, arr, 12);
//							  res[12]=vrefint_cal;
//							  res[13]=vrefint_cal>>8;
//							  uartSend(res, 14, 1000);
//
//							  sendAck();
//							  break;
//						  }

						  case VERSION:
						  {
							  break;
						  }
						  case VAL_FLASH:
						  {
							  sendAck();
							  if (valFlashUart()==HAL_OK)
							  {
								  bootStatus.status_b=0xB0;
								  bootStatus.status_b|=PROG_STATUS;
								  bootStatus.progSuccessCounter_b+=1;
//								  updateStatus(STATUS_ADDR, &bootStatus, 2000);
							  }
							  else
							  {
								  bootStatus.status_b&=~PROG_STATUS;
//								  updateStatus(STATUS_ADDR, &bootStatus, 2000);
							  }
							  break;
						  }
//						  case UPDATE_ADDR:
//						  {
//							  sendAck();
//							  if(updateAddress())
//							  {
//								  bootStatus.status_b&=0x0F;
//								  bootStatus.status_b|=USER_STATUS;
////								  updateStatus(STATUS_ADDR, &bootStatus, 2000);
//								  sendAck();
//							  }
//							  else
//							  {
//								  bootStatus.status_b&=~USER_STATUS;
////								  updateStatus(STATUS_ADDR, &bootStatus, 2000);
//								  sendNack();
//							  }
//							  break;
//						  }
						  case USER_PROG:
							  sendAck();
							  goto END;
							  break;
					  }
					  tickStart = getTick();
					  resetWatchdog();
				  }
				  else
				  {
					  sendNack();
					  //HAL_UART_Transmit(&huart1, nack, 1,1000);
				  }
			  }
			  else
			  {
				  //HAL_UART_Transmit(&huart1, nack, 1,1000);
			  }
		  }
	  }
	  else
	  {
		  goto Start;
		  //goto END;
	  }
  }
  else
  {
	  goto END;
  }
#endif
END:



	if(bootStatus.version_b!=BVERSION)
	{
		bootStatus.version_b=BVERSION;
//		updateStatus(STATUS_ADDR, &bootStatus, 2000);
	}


	LOGS("Sn\n", 3);
	//LOGIN(bootStatus.status_b, 16);
	if(bootStatus.status_b!=PROG_VAL_FLAG)
	{
		uint32_t endAddr = 0;
		if(valFlashSearch(APP_ADDR, &endAddr)==1)
		{
		  if(endAddr>0x800A000)
		  {
			  bootStatus.status_b|=PROG_STATUS;
			  bootStatus.version_b=BVERSION;
//			  bootStatus.BMS_id = Device_ID;
		  }
		  else
		  {
			  goto Start;
		  }
		}
	}
	LOGS("BO\n", 3);
//
	if(bootStatus.appAddress_b >=MAX_ADDRESS || bootStatus.appAddress_b<=MIN_ADDRESS)
	{
		jumpToApp(bootStatus.appAddress_b);
	}
	rstMcu();
}


/* calculating crc32 for given data set */
uint32_t cal_crc32(char * data,unsigned char  len)
{
	uint32_t crc=0xFFFFFFFF;
	for(int i=0;i<len;i++)
	{
		crc^=(uint32_t)(data[i]<<24);
		for(int j=0;j<8;j++)
		{
			if(crc & 0x80000000)
			{
				crc=(crc<<1)^ 0x04C11DB7;
			}
			else
			{
				crc=crc<<1;
			}
		}
	}
	return crc;
}







/********************** To Erase UnAligned Memory ************/
void clearCruptedFlash()
{
	updateStatus(STATUS_ADDR, &bootStatus, 2000);
}
