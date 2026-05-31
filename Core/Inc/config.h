#define		UART1_EN



#define 		STM32G4

#define 		UART_FLASH_EN


#define			NEWFM

//#if			defined(BMS) || defined(CAN_DISPLAY) || defined(STATION_SLAVE) || defined(SMART_CHARGER)
#define		UART			huart1


#define			PAGE_SIZE		2048

#define			EN		1
#define			DIS		0


#define			LOGS(X, Y)					logSend(X, Y, 50)//HAL_UART_Transmit(&huart1, X, Y, 1000)
#define			LOGI(X, Y)					{char ar[20];itoa(X, ar, Y);LOGS(ar, strlen(ar));}
#define			LOGIN(X, Y)					{char ar[20];itoa(X, ar, Y);char len = strlen(ar);ar[len]='\n';LOGS(ar, len+1);}
#define			LOGIS(X, Y)					{char ar[20];itoa(X, ar, Y);char len = strlen(ar);ar[len]=',';LOGS(ar, len+1);}


#ifndef			APP_BOOTLOADER
	#define			MIN_ADDRESS			0x8000000
	#define			MAX_ADDRESS			0x8006000
	#define			PROG_STATUS			0x0A
	#define			USER_STATUS			0xB0
	#define			PROG_VAL_FLAG		0xBA
#else
	/**** Address for bootloader upgrade bootloader ******/
	#define			MIN_ADDRESS			0x8008000
	#define			MAX_ADDRESS			0x800E000
	#define			PROG_STATUS			0x0B
	#define			USER_STATUS			0xC0
	#define			PROG_VAL_FLAG		0xCB
#endif




#define				BOOT_STATE_INIT_1			0
#define				BOOT_STATE_CNFGERR_2		1
#define				BOOT_STATE_UPLOADING_3		2
#define				BOOT_STATE_EMPTYPRG_4		3
