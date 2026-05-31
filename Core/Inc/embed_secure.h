#ifndef		__EMBED_SECURE__
#define		__EMBED_SECURE__


//#include "EEPROM_custom.h"
#include "aes.h"
#include "stmBootloader.h"

void genKey(char * id, uint16_t rval, uint16_t salt, char * key);
char encryptConfig(char * key,  char * iv, char * encryptedData);
char decryptConfig(char * encryptedData, char * key,  char * iv);

#endif
