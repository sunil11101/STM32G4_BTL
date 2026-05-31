
#ifndef		__BOOTLOADER
#define		__BOOTLOADER

#include "config.h"
#include "aes.h"
#include "embed_secure.h"
#include "UART_base_driver.h"
#include "Flash_base_driver.h"


/*
#define		HAL_OK       0x00,
#define		HAL_ERROR    0x01,
#define		HAL_BUSY     0x02,
#define		HAL_TIMEOUT  0x03
*/



	#define		PING_CMD		0x00
	#define		START_BOOT		0x51

	#define		ACK				0x61
	#define		NACK			0x64

	#define		PROGRAMME		0x38
	#define		ERASE			0x48
//	#define		READ_PROTECT	0x63
	#define		VERSION			0x83
//	#define		PROG_COUNT		0x87
//	#define		PROG_ERROR		0x92
	#define		USER_PROG		0x97
	#define		VAL_FLASH		0xA4
//	#define		UPDATE_ADDR		0xB3
//	#define		NO_PROTECT		0xC3
//	#define		GET_BID			0xD3


	#define			BVERSIONM				1


#pragma pack (1)
	typedef struct
	{
		uint32_t timeStamp;
		uint16_t fault;
	}faultLog;



/************** Structure to save the configuration and SOC in flash ***********/
// ADD PADDING OF 1 BYTE I.E. NO PADDING
#pragma pack (1)
	typedef	struct
	{
		// Bootloader config
		char status_b;								//01
		char version_b;
		uint16_t progCounter_b;
		uint32_t appAddress_b;

		uint16_t progSuccessCounter_b;				//02
		uint16_t reserved0_b;
		uint32_t reserved1_b;


		uint64_t reserved027;			//1512

		uint64_t reserved028;			//1520

		uint64_t reserved029;			//1528

		uint64_t reserved030;			//1536

		uint64_t reserved031[63];

		uint32_t reserved032;
		uint32_t reserved033;
	}progStatus;



extern progStatus bootStatus;


static inline progStatus readStatusFromEEPROM (uint32_t address)
{
  return (*(__IO progStatus *)address);
}


void sendAck(void);
void sendNack(void);
char writeToEEPROM (uint32_t address, uint64_t  * value, int len, uint16_t timeout);
char updateStatus(uint32_t, progStatus *, uint16_t);
char getStatus(uint32_t);
//char initFlash(uint16_t);
//char flashLock(uint16_t );
//char flashUnlock(uint16_t );
char checkCRC(uint8_t *, uint32_t );
char uploadProg(uint32_t , char *, uint8_t );
char uploadProgUart();
char downloadProg(uint32_t, uint32_t, uint16_t);
char readProtectEn();
char readProtectDis();
uint8_t readVersion(uint16_t );
char readStatus(uint16_t );
char valFlash(uint32_t , uint32_t);
char valFlashSearch(uint32_t startAddr, uint32_t * endAddr);
char valFlashUart();
uint16_t readCount(uint16_t );
uint16_t readErrorCount(uint16_t );
char updateAddress(void );

char eraseFlashUart(uint16_t );
char eraseFlash(uint32_t, uint8_t , uint16_t );

uint32_t cal_crc32(char * data,unsigned char  len);

#endif
