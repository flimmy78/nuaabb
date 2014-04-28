/*=============================================================================*
 *         Copyright(c) 2010-2011, ALL RIGHTS RESERVED
 *                          
 *
 *  FILENAME : 17KW_Sci_Driver.h 

 *  PURPOSE  : header files 17KW_Scib_Interface.c 
 *			       Define base structure and driver fuction for SCI 
 *
 *  HISTORY  :
 *    DATE            VERSION        AUTHOR            NOTE
 *    
 *
 ******************************************************************************/


#ifndef SCI_DRIVER_H
#define SCI_DRIVER_H

typedef char               int8;
typedef unsigned char      Uint8;

#define	OS_ENTER_CRITICAL()		asm("	SETC	INTM")
#define	OS_EXIT_CRITICAL()		asm("	CLRC	INTM")

#define SCIB_BaudRate		9600			// 9600bps
#define SCIB_BRR 	 		((LSPCLK_FREQ/SCIB_BaudRate)/8)-1

#define false                   0
#define true                    1

/************************************************************************************
*Maxmam	Sci Number																	*
*************************************************************************************/

#define	con_MAX_SCI_NO				1

/************************************************************************************
*SCI type constant definition														*
*As the input parameter of sInitialSci												*
*************************************************************************************/

#define	con_SciTypeSci				0			/*		normal sci moule 			*/

  
/************************************************************************************
*return value of write operation													*
*************************************************************************************/
#define	con_SciTxRdy				0
#define	con_SciTxBusy				1

/************************************************************************************
*return value of read operation														*
*************************************************************************************/
#define	con_SciRxRdy				0
#define	con_SciRxBufEmpty			1

/************************************************************************************
*input value of change baudrate														*
*************************************************************************************/
#define	con_BaudRate1200			1
#define	con_BaudRate2400			2
#define	con_BaudRate4800			3
#define	con_BaudRate9600			4

/************************************************************************************
*Queue constant define																	*
*************************************************************************************/
#define	con_QBufNormal				 0
#define	con_QBufFull				 1
#define	con_QBufEmpty				 2

/************************************************************************************
*function declaration																*
*************************************************************************************/


extern void  Scib_ClearRx(Uint8 uusciid);
extern Uint8 Scib_GetTxStatus(Uint8 u8sciid);


extern void Scib_TaskInitial(Uint8 u8sciid, Uint8 *pbRxBuf, Uint16 u16RxSize, Uint8 u8Type);									
extern void  Scib_RxISR(Uint8 u8sciid);
extern Uint8 Scib_Read(Uint8 u8sciid,Uint8 *pBuf);
extern Uint8 Scib_SnatchGraph(void);

extern void Scib_Write(Uint8 *pstart,Uint16 u16length);
extern void Scib_WriteBinary(Uint16 *pstart,Uint16 u16Length);
extern Uint8 sbNumToAscii(Uint16 u16Number,int8 i8Exponent,Uint8 *pbBuffer);

extern void InitScib();
extern void SWI_ScibRXISR();

//fuction declaration for IPOMS


/*
 extern int16 swGetPLLTheta(void) ;
 extern int16 swGetVolta(void);
 extern  int16 swGetVoltb(void);
 extern int16 swGetVoltd(void);
 extern int16 swGetVoltq(void);  */
#endif

//--- end of file -----------------------------------------------------
