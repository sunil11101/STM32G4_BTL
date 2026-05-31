/*
 * UART_base_driver.c
 *
 *  Created on: 09-Sep-2021
 *      Author: ubuntu
 */

#include "UART_base_driver.h"



	extern UART_HandleTypeDef huart1;
//	extern UART_HandleTypeDef huart4;
	#define		HUART1		huart1
//	#define		HUART4		huart4



/*	FUNCTION TO RECEIVE FIXED NUMBER OF BYTES ON UART
 * buffer : buffer to store the incoming bytes
 * len : len of bytes to be received
 * delay : time to wait for the given len of bytes
 */
uint8_t uartReceive(uint8_t * buffer, uint16_t len, uint32_t delay)
{
	return HAL_UART_Receive(&UART, buffer, len, delay);
}

/**** END OF FUNCTION uartReceive  ***************/



uint8_t logSend(uint8_t * data, uint16_t len, uint32_t delay)
{

	return HAL_UART_Transmit(&HUART1, data, len, delay);
	//return HAL_UART_Transmit_IT(&UART, data, len);
}

/*	FUNCTION TO RECEIVE FIXED NUMBER OF BYTES ON UART
 * buffer : buffer to store the incoming bytes
 * len : len of bytes to be received
 * delay : time to wait for the given len of bytes
 */
uint8_t uartSend(uint8_t * data, uint16_t len, uint32_t delay)
{

	return HAL_UART_Transmit(&UART, data, len, delay);
	//return HAL_UART_Transmit_IT(&UART, data, len);
}
/**** END OF FUNCTION uartSend  ***************/



/* Function to deinit the uart
 *
 */
void uartDeinit(uint8_t sel)
{
	if(sel==1)
	{
		HAL_UART_DeInit(&huart1);
	}
	else
	{
//			HAL_UART_DeInit(&huart4);
	}
}

