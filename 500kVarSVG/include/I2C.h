/*=============================================================================*
 *         Copyright(c) 2009-2011, Convertergy Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  FILENAME : I2C.h 
 *
 *  PURPOSE  : provide Head file for I2C.c
 *  
 *  HISTORY  :
 *    DATE            VERSION        AUTHOR            NOTE
 *    2011-02-19      V0.1           Ken      	    Created
 *
 *----------------------------------------------------------------------------
 *  GLOBAL VARIABLES
 *    NAME                                    DESCRIPTION
 *      
 *----------------------------------------------------------------------------
 *  GLOBAL FUNCTIONS
 *    NAME                                    DESCRIPTION
 *    
 *============================================================================*/


/* Define to prevent recursive inclusion -------------------------------------*/
#ifndef I2C_H
#define I2C_H

//#include "DSP2833x_Examples.h"   // DSP2833x Examples Include File
#include "DSP2833x_I2C_defines.h"


struct DEF_I2CADDR
{
	Uint16 u16BlockAddr;
	Uint16 u16ByteAddr;
};

typedef struct DEF_I2CADDR TypeI2cAddr;


typedef struct I2CMSG TypeI2cMsg;

// Prototype statements for functions found within this file.
void   InitI2C(void);
Uint16 I2C_Reset(void);
void   I2C_ConvAddr(Uint16, TypeI2cAddr * );
Uint16 I2C_PollACK(TypeI2cMsg * );
Uint16 I2C_Write_InByte(Uint16, Uint16* , Uint16);
Uint16 I2C_Write_InWord(Uint16, Uint16* , Uint16);
Uint16 I2C_Write_Page(TypeI2cMsg * );
Uint16 I2C_WriteData(TypeI2cMsg * );
Uint16 I2C_Read_InByte(Uint16, Uint16* , Uint16);
Uint16 I2C_Read_InWord(Uint16, Uint16* , Uint16);
Uint16 I2C_Read_MultiByte(TypeI2cMsg * );
Uint16 I2C_ReadData(TypeI2cMsg * );

//note: Although the address of ST25C16 is 0xA0, but in 7-bit address mode,
//the address is 0x50. see I2CSAR Field Descriptions.
#define I2C_SLAVE_ADDR        0x50
#define I2C_NUMBYTES          16
#define I2C_EEPROM_HIGH_ADDR  0x00
#define I2C_EEPROM_LOW_ADDR   0x00

#define	ST25C16_ADDR_LIMIT			0x07FF
#define ST25C16_ADDR_MASK			0x07FF
#define ST25C16_ADDR_ROW_MASK_W		0xFFFC
#define ST25C16_ADDR_ROW_MASK_B		0xFFF8
#define	ST25C16_ADDR_BLOCK_MASK		0xFF00

#define ST25C16_ADDR_ROW_LEN_W		0x04
#define ST25C16_ADDR_ROW_LEN_B		0x08
#define	ST25C16_ADDR_BLOCK_LEN		0x100

#define I2C_W_R_OK						0
#define I2C_W_R_EXCEED_LIMIT			1
#define	I2C_W_R_DATA_AMOUNT_IS_ZERO		2
#define	I2C_R_FAILED					3
#define I2C_W_FAILED					4

#define	I2C_MAX_TIMES_PAGE_WRITE		3
#define	I2C_MAX_TIMES_DYMMY_WRITE		3
#define	I2C_MAX_TIMES_RESET				3
#define	I2C_MAX_MS_SEMPEND	10
#endif

//--- end of file -----------------------------------------------------

