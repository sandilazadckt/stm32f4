/*
 * spi.h
 *
 *  Created on: Dec 29, 2025
 *      Author: sandil
 */

#ifndef INC_SPI_H_
#define INC_SPI_H_
#include <stm32f401xe.h>
#include <stdint.h>

typedef struct {

	uint32_t mode;
	uint32_t Direction;
	uint32_t DataSize;
	uint32_t CLKPolarity;
	uint32_t CLKPhase;
	uint32_t NSS;
	uint32_t BuadRatePreScaler;
	uint32_t FristBit;
	uint32_t TIMode;
	uint32_t CRCCalculation;
} SPIInitTypeDef;

typedef enum {
	SPI_STATE_RESET = 0x00U,
	SPI_STATE_READY = 0x01U,
	SPI_STATE_BUSY = 0x02U,
	SPI_STATE_BUSY_TX = 0x03U,
	SPI_STATE_BUSY_RX = 0x04U,
} SPI_StateTypeDef;

typedef enum {
	DEV_OK = 0x00U, DEV_ERROR = 0x01U, DEV_BUSY = 0x02U, DEV_TIMEOUT = 0x03U,
} StatusTypeDef;

typedef struct {
	SPIInitTypeDef Init;
	uint8_t *pTxBuffPtr;
	uint16_t TxXferSize;
	__IO uint16_t TxXferCount;
	uint8_t *pRxBuffPtr;
	uint16_t RxXferSize;
	__IO uint16_t RxXferCount;
	SPI_StateTypeDef state;
	__IO uint32_t Errorcode;
} SPI_HandleTypeDef;

#define SPI_ERROR_NONE   (0x00000000U);
#define SPI_ERROR_SLAVE  (0x00000000U);
#define SPI_ERROR_MASTER (SPI_CR1_MSTR | SPI_CR1_SSI);
#define SPI_ERROR_2LINES (0x00000000U);
#define SPI_ERROR_1LINES SPI_CR1_BIDIMODE;
#define SPI_DATASIZE_8BIT (0x00000000U);
#define SPI_DATASIZE_16BIT SPI_CR1_DFF;

#endif /* INC_SPI_H_ */
