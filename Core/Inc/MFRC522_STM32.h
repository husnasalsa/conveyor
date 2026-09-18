#ifndef MFRC522_STM32_MIN_H
#define MFRC522_STM32_MIN_H

#include "stm32f4xx_hal.h"
#include <stdint.h>
#include <stdio.h>

#define ENABLE_USER_LOG   1
#define ENABLE_DEBUG_LOG  0 // Test with this disabled

#if ENABLE_USER_LOG
  #define USER_LOG(fmt, ...) printf("[USER] " fmt "\r\n", ##__VA_ARGS__)
#else
  #define USER_LOG(fmt, ...)
#endif

#if ENABLE_DEBUG_LOG
  #define DEBUG_LOG(fmt, ...) printf("[DEBUG] " fmt "\r\n", ##__VA_ARGS__)
#else
  #define DEBUG_LOG(fmt, ...)
#endif

// Essential registers
#define PCD_CommandReg     0x01
#define PCD_ComIrqReg      0x04
#define PCD_ErrorReg       0x06
#define PCD_Status2Reg     0x08
#define PCD_FIFODataReg    0x09
#define PCD_FIFOLevelReg   0x0A
#define PCD_BitFramingReg  0x0D
#define PCD_TxControlReg   0x14
#define PCD_TxAutoReg      0x15
#define PCD_RFCfgReg       0x26
#define PCD_TModeReg       0x2A
#define PCD_TPrescalerReg  0x2B
#define PCD_TReloadRegL    0x2C
#define PCD_TReloadRegH    0x2D
#define PCD_DemodReg       0x19
#define PCD_VersionReg     0x37

// Commands
#define PCD_Idle           0x00
#define PCD_Transceive     0x0C
#define PCD_SoftReset      0x0F
#define PCD_IDLE           0x00               // no action, cancels current command execution
#define PCD_AUTHENT        0x0E               // performs the MIFARE standard authentication as a reader
#define PCD_RECEIVE        0x08               // activates the receiver circuits
#define PCD_TRANSMIT       0x04               // transmits data from the FIFO buffer
#define PCD_TRANSCEIVE     0x0C               // transmits data from FIFO buffer to antenna and automatically activates the receiver after transmission
#define PCD_RESETPHASE     0x0F               // resets the MFRC522
#define PCD_CALCCRC        0x03               // activates the CRC coprocessor or performs a self-test


// PICC commands
#define PICC_REQA          0x26
#define PICC_SEL_CL1       0x93
#define PICC_REQIDL        0x26               // REQuest command, Type A. Invites PICCs in state IDLE to go to READY and prepare for anticollision or selection. 7 bit frame.
#define PICC_REQALL        0x52               // Wake-UP command, Type A. Invites PICCs in state IDLE and HALT to go to READY(*) and prepare for anticollision or selection. 7 bit frame.
#define PICC_ANTICOLL      0x93               // Anti collision/Select, Cascade Level 1
#define PICC_SElECTTAG     0x93               // Anti collision/Select, Cascade Level 2
#define PICC_AUTHENT1A     0x60               // Perform authentication with Key A
#define PICC_AUTHENT1B     0x61               // Perform authentication with Key B
#define PICC_READ          0x30               // Reads one 16 byte block from the authenticated sector of the PICC. Also used for MIFARE Ultralight.
#define PICC_WRITE         0xA0               // Writes one 16 byte block to the authenticated sector of the PICC. Called "COMPATIBILITY WRITE" for MIFARE Ultralight.
#define PICC_DECREMENT     0xC0               // Decrements the contents of a block and stores the result in the internal data register.
#define PICC_INCREMENT     0xC1               // Increments the contents of a block and stores the result in the internal data register
#define PICC_RESTORE       0xC2               // Reads the contents of a block into the internal data register.
#define PICC_TRANSFER      0xB0               // Writes the contents of the internal data register to a block.
#define PICC_HALT          0x50               // HaLT command, Type A. Instructs an ACTIVE PICC to go to state HALT.

// Status
#define STATUS_OK          0
#define STATUS_ERROR       1
#define STATUS_TIMEOUT     2

typedef struct {
    SPI_HandleTypeDef *hspi;
    GPIO_TypeDef *csPort;
    uint16_t csPin;
    GPIO_TypeDef *rstPort;
    uint16_t rstPin;
} MFRC522_t;

// Prototypes
void MFRC522_Init(MFRC522_t *dev);
void MFRC522_AntennaOff(MFRC522_t *dev);
void MFRC522_AntennaOn(MFRC522_t *dev);
uint8_t MFRC522_ReadReg(MFRC522_t *dev, uint8_t reg);
void MFRC522_WriteReg(MFRC522_t *dev, uint8_t reg, uint8_t value);
void MFRC522_SetBitMask(MFRC522_t *dev, uint8_t reg, uint8_t mask);
void MFRC522_ClearBitMask(MFRC522_t *dev, uint8_t reg, uint8_t mask);
uint8_t MFRC522_RequestA(MFRC522_t *dev, uint8_t *atqa);
uint8_t MFRC522_Anticoll(MFRC522_t *dev, uint8_t *uid);
uint8_t MFRC522_ReadUid(MFRC522_t *dev, uint8_t *uid);
uint8_t waitcardRemoval (MFRC522_t *dev);
uint8_t waitcardDetect (MFRC522_t *dev);

#endif
