#include "embed_secure.h"

extern progStatus bootStatus;


/******************* function to generate key using BMS paramerters *****************
 * @ id : 96 bit MCU unique id
 * @ rval : ref value
 * @ salt : salt to be mixed with key (20 bit)
 * @ key : 128 bit key to be returned
 */
void genKey(char * id, uint16_t rval, uint16_t salt, char * key)
{
	*(key)=(rval & 0x0F) | *(id);					// 4 LSB bit of rval as LSB and 4 LSB bit of id[0] as MSB form 1 byte
	for(char i=1;i<7;i++)
	{
		*(key+i)=*(id+i-1);							// next 6 byte of id from id[0]
	}

	*(key+7) = (*(id+6) & 0x0F) | (rval & 0xF0);		// 4 MID bit of rval as MSB and 4 LSB bit of id[6] as LSB form 1 byte

	for(char i=8;i<14;i++)
	{
		*(key+i) = *(id+i-2);							// next 6 byte of id from id[6]
	}

	*(key+14) = (*(id+11) & 0xF0) | (rval>>8 & 0x0F);	// 4 MSB bit of rval as LSB and 4 MSB bit of id[11] as MSB form 1 byte

	*(key+15) = salt>>4;
}



/******************* function to encrypt configuration of the bms ********************
 * @ bmsconfig :bmsconfig struct to be encrypted
 * @ key : 128 bit key to encrypt
 * @ iv : 128 bit iv to encrypt
 * @ encryptedData : encrypted data to be returned
 */
char encryptConfig(char * key,  char * iv, char * encryptedData)
{
	struct AES_ctx ctx;
	AES_init_ctx_iv(&ctx, key, iv);
	char * buffer =(uint8_t *)&bootStatus;				// get char pointer of struct data
	int len=sizeof(bootStatus);

	/* copy char array data to necryptedData array because encrypt function will encrypt data 
	* put back to the array. If we send the buffer pointer which is address of original struct 
	* then the encrypt function will change the original array data with encrypted data ******/
	for(int i=0;i<len;i++)
	{
		*(encryptedData+i)=*(buffer+i);					
	}

	AES_CBC_encrypt_buffer(&ctx, encryptedData, len);		// encrypt data
}
/*####################### End of encryptConfig function #################################*/




/******************* function to decrypt configuration of the bms ********************
 * @ encryptedData : encrypted data to be returned
 * @ key : 128 bit key to encrypt
 * @ iv : 128 bit iv to encrypt
 * @ bmsconfig :bmsconfig struct to be encrypted
 */
char decryptConfig(char * encryptedData, char * key,  char * iv)
{
	struct AES_ctx ctx;
	AES_init_ctx_iv(&ctx, key, iv);
	//char * buffer =(uint8_t *)bmsconfig;				// get char pointer of struct data
	int len=sizeof(bootStatus);

	AES_CBC_decrypt_buffer(&ctx, encryptedData, len);		// decrypt data
	char * buffer=(uint8_t *)&bootStatus;
	/* copy decryptedData array to buffer(which is pointing to bmsconfig struct ) ******/
//	len = 0xFFFF;
	for(int i=0;i<len;i++)
	{
		*(buffer+i)=*(encryptedData+i);					
	}
}
/*####################### End of decryptConfig function #################################*/

