/*
 * my_main.c
 *
 *  Created on: 09-Sep-2021
 *      Author: ubuntu
 */

#include "my_main.h"

uint8_t Status = 0;

#define 		APP_ADDR		0x8008000

	uint8_t BVERSION = BVERSIONM;
//pFunction JumpToApplication;
//uint32_t JumpAddress;


void init()
{

//initComplete = 1;
Start:
resetWatchdog();



#ifdef			APP_BOOTLOADER
	uartSend("bapp\n", 5,1000);
#else
	LOGS("a3\n", 3);
#endif

	char buffer[5]={0};
  	uint8_t status=uartReceive(buffer, 2, 700);


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
//							  if(bootStatus.status_b & 0x0F != 0x00)
//							  {
//								  bootStatus.status_b&=0xF0;
//								  updateStatus(STATUS_ADDR, &bootStatus, 2000);
//							  }
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


	uint32_t endAddr = 0;
	if(valFlashSearch(APP_ADDR, &endAddr)==1)
	{
		LOGS("S2\n", 3);
	  if(endAddr>0x8007000)
	  {
//		  bootStatus.status_b|=PROG_STATUS;
//		  bootStatus.version_b=BVERSION;
//			  bootStatus.BMS_id = Device_ID;
	  }
	  else
	  {
		  goto Start;
	  }
	}else
	{
		goto Start;
	}

	LOGS("BO\n", 3);
//
//	if(bootStatus.appAddress_b >=MAX_ADDRESS || bootStatus.appAddress_b<=MIN_ADDRESS)
	{
//		jumpToApp(APP_ADDR);
		JumpToApplication();
	}
//	rstMcu();
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


