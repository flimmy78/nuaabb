/*=============================================================================*
 *         
 *                          ALL RIGHTS RESERVED
 *
 *  FILENAME : 17KW_EEPROM.c 
 *
 *  PURPOSE  : EEPROM with I2C interface operation functions.
 *  
 *  HISTORY  :
 *    DATE            VERSION        AUTHOR            NOTE
 *    
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


#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "500KW_MAIN_HEADER.h"			// Main include file

// Note: I2C Macros used in this example can be found in the
// DSP2833x_I2C_defines.h file

// Global variables
//I2cMsgOut is used in transmission from DSP to EEPROM
TypeI2cMsg I2cMsgOut =	{	I2C_MSGSTAT_INACTIVE,
							I2C_SLAVE_ADDR,
							I2C_NUMBYTES,
							I2C_EEPROM_HIGH_ADDR,
							I2C_EEPROM_LOW_ADDR
						};
//I2cMsgIn is used in transmission from EEPROM to DSP
TypeI2cMsg I2cMsgIn = 	{	I2C_MSGSTAT_INACTIVE,
							I2C_SLAVE_ADDR,
							I2C_NUMBYTES,
							I2C_EEPROM_HIGH_ADDR,
							I2C_EEPROM_LOW_ADDR
						};

TypeI2cMsg *CurrentMsgPtr;				// Used in interrupts
Uint16 u16IntSource = 0;
volatile Uint16 u16I2cIsrError = 0;	//used in ISR, record process result.

/*=============================================================================*
 * FUNCTION: 
 *		I2C_ConvAddr()
 *
 * PURPOSE : 
 *		Decompose EEPROM address to block address and byte address.
 *
 * INPUT: 
 *		u16I2cAddr----EEPROM address, from 0 to 0x7FE.
 *		pRtI2cAddr----pointer of a TypeI2cAddr variable, contains the result.
 *
 * RETURN: 
 *     void
 *
 * CALLS: 
 *     void
 *
 * CALLED BY: 
 *		I2C.c
 * 
 *============================================================================*/
void I2C_ConvAddr(Uint16 u16I2cAddr, TypeI2cAddr* pRtI2cAddr)
{
	pRtI2cAddr->u16ByteAddr  = u16I2cAddr & 0x00FF;
	pRtI2cAddr->u16BlockAddr = (u16I2cAddr & 0xFF00)>>8;
}

/*=============================================================================*
 * FUNCTION: 
 *		InitI2C()
 *
 * PURPOSE : 
 *		Initialize the I2C module of TMS320F28335:
 *		100KHz. Clock Pulse Width High: 4.7us, Clock Pulse Width Low: 5.3us
 *		Only enable SCD(Stop Condition Detected) and ARDY(Register Access Ready)
 *	interrupts.
 *		Take I2C out of reset.
 *		Enable FIFO mode.
 *		Enable TXFIFO, clear TXFFINT, disable TXFFINT.
 *		Enable RXFIFO, clear RXFFINT, disable RXFFINT.
 *
 * INPUT: 
 *     void
 *
 * RETURN: 
 *     void
 *
 * CALLS: 
 *     void
 *
 * CALLED BY: 
 *     Main.c  
 * 
 *============================================================================*/
/*void InitI2C(void)
{
	EALLOW;

	GpioCtrlRegs.GPBPUD.bit.GPIO32 = 0;    // Enable pull-up for GPIO32 (SDAA)
	GpioCtrlRegs.GPBPUD.bit.GPIO33 = 0;	   // Enable pull-up for GPIO33 (SCLA)
	GpioCtrlRegs.GPBQSEL1.bit.GPIO32 = 3;  // Asynch input GPIO32 (SDAA)
    GpioCtrlRegs.GPBQSEL1.bit.GPIO33 = 3;  // Asynch input GPIO33 (SCLA)
	//GPIO32 has been configured for SDAA in InitGPIO()
	//GPIO33 has been configured for SCLA in InitGPIO()
	GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1;	//enable write protect pin 		added at 2013.1.30			
	GpioDataRegs.GPBSET.bit.GPIO50 = 1;		//enable Multibyte write Mode 	added at 2013.1.30		
	GpioDataRegs.GPBSET.bit.GPIO51 = 1;		//enable I2C Control		 	added at 2013.1.30		

	
    EDIS;

	I2caRegs.I2CSAR = I2C_SLAVE_ADDR;		// EEPROM control code
*/
/*
#if (CPU_FRQ_150MHZ)             			// Default - For 150MHz SYSCLKOUT
	I2caRegs.I2CPSC.all = 14;   			// Prescaler - need 7-12 Mhz on module clk (150/15 = 10MHz)
#endif
#if (CPU_FRQ_100MHZ)             			// For 100 MHz SYSCLKOUT
	I2caRegs.I2CPSC.all = 9;				// Prescaler - need 7-12 Mhz on module clk (100/10 = 10MHz)
#endif
*/
/*	I2caRegs.I2CMDR.bit.IRS = 0;   			// clear I2CSTR
	DelayUs(1000);
	I2caRegs.I2CPSC.all = 14;  				//IPSC must be initialized while I2C module in reset

	I2caRegs.I2CMDR.bit.IRS = 1;
	DelayUs(1500);

	I2caRegs.I2CCLKL = 48;					// 48+5=53, tcl=5.3us  farmersun
	I2caRegs.I2CCLKH = 42;					// farmersun  42+5=47, tch=4.7us, and fscl=100KHz(M24C16 MAX Fscl=400KHz)
	I2caRegs.I2CIER.all = 0x24;				// Enable SCD & ARDY interrupts
	
//	I2caRegs.I2CMDR.all = 0x6E20;			
	I2caRegs.I2CMDR.all = 0x0020;			// enable I2C module
											// 

//	I2caRegs.I2CFFTX.all = 0x2040;			
	I2caRegs.I2CFFTX.all = 0x6040;			// Enable FIFO mode and TXFIFO, clear TXFFINT, disable TXFFINT
	I2caRegs.I2CFFRX.all = 0x2040;			// Enable RXFIFO, clear RXFFINT, disable RXFFINT
	
	u16IntSource = I2C_NO_ISRC;
	u16I2cIsrError = I2C_SUCCESS;

	PieCtrlRegs.PIEIER8.bit.INTx1 = 1;
//	IER = M_INT8;
	return;
}
*/
/*=============================================================================*
 * FUNCTION: 
 *		I2C_Reset()
 *
 * PURPOSE : 
 *		Reset 28335 I2C Module.
 *		Then, if I2CSTR[BB] is 1 (not busy), initialize I2C module.
 *
 * INPUT: 
 *		void
 *
 * RETURN: 
 *		Uint16 type value:
 *		I2C_ERROR----after reset, I2CSTR[BB] remains 0 (i.e, still busy)
 *		I2C_SUCCESS--after reset, I2CSTR[BB] is 1 (i.e, not busy)
 *
 * CALLS: 
 *		InitI2C()
 *		DELAY_US()
 *
 * CALLED BY: 
 *     Main.c  
 * 
 *============================================================================*/
/*Uint16 I2C_Reset(void)
{
	I2caRegs.I2CMDR.bit.IRS = 0;
	DelayUs(1000);
	I2caRegs.I2CMDR.bit.IRS = 1;
	DelayUs(2000);

	if(0 == I2caRegs.I2CSTR.bit.BB)
	{
		InitI2C();
		return I2C_SUCCESS;
	}
	else
	{
		return I2C_ERROR;
	}
}
*/
/*=============================================================================*
 * FUNCTION: 
 *		I2C_PollACK()
 *
 * PURPOSE : 
 *		Check if ST25C16 EEPROM is ready for next operation.
 *		Minimize system delay by Polling On ACK.
 *
 * INPUT: 
 *		TypeI2cMsg *msg: Address of a TypeI2cMsg type structure.
 *		It should be address of I2cMsgIn.
 *
 * RETURN: 
 *		Uint16 type value:
 *		I2C_ERROR----FATAL ERROR, caused by I2C module cannot enter ISR, or
 *	I2C module can not ACK after 100 times retry. when return this, Inverter
 *	must be Power on again.
 *		I2C_SUCCESS--ST25C16 is ready for next operation now.
 *
 * CALLS: 
 *		SEM_pendBinary()
 *
 * CALLED BY: 
 *     Main.c  
 *
 * MORE INFORMATION:
 *		After "I2C_Write_Page()" ends, I2cMsgOut.MsgStatus is
 *	I2C_MSGSTAT_INACTIVE.
 *
 *		Test shows that when executed in RAM, one most out loop consumes 
 *	212us when ACK is lost. As the longest write time is 10ms(47.2*212us),
 *	47.2 loops is ok. For enough margin, 100 loops is applied.
 *============================================================================*/
Uint16 I2C_PollACK(TypeI2cMsg *msg)
{
	TypeI2cMsg *pCurMsgPtrTmp;
	Uint16 u16Error;
	Uint16 i;

	pCurMsgPtrTmp = CurrentMsgPtr;
	CurrentMsgPtr = msg;
	for(i=0; i<100; i++)
	{	//---- START+DEVICE SELECT
		//GpioDataRegs.GPBTOGGLE.bit.GPIO54 = 1;	//for test
		msg->MsgStatus = I2C_MSGSTAT_SEND_NOSTOP_BUSY;
		I2caRegs.I2CSAR = msg->SlaveAddress;	
		I2caRegs.I2CCNT = 1;
		I2caRegs.I2CDXR = msg->MemoryLowAddr;
		I2caRegs.I2CMDR.all = 0x2620;				//dummy WRITE
		//1--- WAIT FOR ISR RETRUN
		u16Error = SEM_pendBinary(&SEM_I2C_isr, I2C_MAX_MS_SEMPEND);
		//2_1--dummy WRITE ISR returns
		if(u16Error)				
		{
			//3_1--dummy WRITE---NACK
			if(I2C_NACK_ERROR == u16I2cIsrError)//STOP will be set in ISR			
			{	//wait for dummy WRITE---NACK----STOP ISR returns SCD
				u16Error = SEM_pendBinary(&SEM_I2C_isr, I2C_MAX_MS_SEMPEND);
				//3_1--4_1--dummy WRITE---NACK----STOP ISR returns SCD
				if(u16Error)
				{
					if(I2caRegs.I2CFFTX.bit.TXFFST)	//clear TXFFST
					{	
						I2caRegs.I2CFFTX.bit.TXFFRST = 0;
						while(I2caRegs.I2CFFTX.bit.TXFFST);
						I2caRegs.I2CFFTX.bit.TXFFRST = 1;
					}
					continue;
				}
				//3_1--4_2--dummy WRITE---NACK----STOP not enter ISR
				else
				{
					CurrentMsgPtr = pCurMsgPtrTmp;
					return I2C_ERROR;
				}
			}
			//3_2--dummy WRITE---OK
			else if(I2C_MSGSTAT_RESTART == msg->MsgStatus)
			{	
				msg->MsgStatus = I2C_MSGSTAT_READ_BUSY;
				I2caRegs.I2CSAR = msg->SlaveAddress;	
				I2caRegs.I2CCNT = 1;
				I2caRegs.I2CMDR.all = 0x2C20;		// RESTART
				
				u16Error = SEM_pendBinary(&SEM_I2C_isr, I2C_MAX_MS_SEMPEND);

				//3_2--4_1--dummy WRITE---OK----RESTART ISR returns SCD
				if(u16Error)		
				{	//3_2--4_1--dummy WRITE---OK----RESTART----OK
					if(I2C_MSGSTAT_INACTIVE == msg->MsgStatus)	
					{
						CurrentMsgPtr = pCurMsgPtrTmp;
						return I2C_SUCCESS;
					}
					//3_2--4_1--dummy WRITE---OK----RESTART----OTHER
					else
					{
						continue;
					}
				}
				//3_2--4_2--dummy WRITE---OK----RESTART not enter ISR
				else
				{
					CurrentMsgPtr = pCurMsgPtrTmp;
					return I2C_ERROR;
				}
			}
			//3_3--dummy WRITE---OTHER
			else
			{
				continue;
			}
		}
		//2_2--dummy WRITE not enter ISR, FATAL ERROR, must be power on again
		else
		{
			CurrentMsgPtr = pCurMsgPtrTmp;
			return I2C_ERROR;
		}
	}

	//no ACK after 100 times retry, FATAL ERROR, must be power on again
	CurrentMsgPtr = pCurMsgPtrTmp;
	return I2C_ERROR;
}

/*=============================================================================*
 * FUNCTION: 
 *		I2C_Write_InByte()
 *
 * PURPOSE : 
 *		Provide EEPROM write interface to other threads.
 *		Analyze EEPROM address and data amount to be wrote in, split write zone
 *	in Row of ST25C16, then call "I2C_Write_Page()" to complete every row writing.
 *		
 * INPUT: 
 *		Uint16 u16StartAddr----EEPROM address to be wrote in,
 *		Uint16* pSrcAddr-------address of data to be wrote in, each Uint16-type
 * memory contains 1 byte data in 8 LSBs.
 *		Uint16 u16ByteNum------byte amount of data to be wrote in.
 *
 * RETURN: 
 *		Uint16 type value:
 *		I2C_W_R_OK
 *		I2C_W_R_EXCEED_LIMIT
 *		I2C_W_R_DATA_AMOUNT_IS_ZERO
 *		I2C_W_FAILED
 *
 * CALLS: 
 *		I2C_ConvAddr()
 *		I2C_Write_Page()
 *		I2C_PollACK()
 *
 * CALLED BY: 
 *		Main.c  
 * 
 * MORE INFORMATION:
 *		Internal I2C error check and retry is handled by I2C_Write_Page().
 *		Every time after I2C_Write_Page() ends, it calls I2C_PollACK() to wait
 *	for the completion of write.
 *		This function has all data written.
 *
 *		For ST25C16, one Row contains 16 bytes at most. If user want to sent 16
 *	bytes one time, the byte amount should be 17, because byte address is the
 *	plused one. 28335 can transmit at most 16 bytes one time, so in fact we can
 *	not write 16 bytes into ST25C16 in one write cycle. To solve this problem, we
 *	reduce the size of Row to 8 bytes. Every time the data bytes we write into
 *	ST25C16 are all located in the same 'half row' in memory: that is the same
 *	Block Address bits(b3,b2,b1 of Device Select code) and the same 5 MSBs in
 *	the Byte Address.
 *============================================================================*/
Uint16 I2C_Write_InByte(  Uint16 u16StartAddr, 
                          Uint16* pSrcAddr, 
                          Uint16 u16ByteNum    )
{
	Uint16 i;
	Uint16 u16Addr_cur;
	Uint16 u16RowAddr_next;
	Uint16 u16BlockAddr_next;
	Uint16 u16ByteNum_remain;
	Uint16 u16EachNum;
	Uint16 u16Error;
	TypeI2cAddr i2cStartAddr;
	
	//check if u16StartAddr is a valid ST25C16 address
	if ((u16StartAddr + u16ByteNum) >= ST25C16_ADDR_LIMIT)
	{
		return I2C_W_R_EXCEED_LIMIT;	//Error: write area exceeds EEPROM limit
	}
	else if (0 == u16ByteNum)
	{
		return I2C_W_R_DATA_AMOUNT_IS_ZERO;	//Error: byte of data amount is 0.
	}

	for(i = 0; i < I2C_MAX_BUFFER_SIZE; i++)
	{
		I2cMsgOut.MsgBuffer[i] = 0;
	}
	
	//every outer while loop writes a BLOCK (upto 256 bytes),
	//every inner while loop writes a half ROW (upto 8 bytes),
	//u16ByteNum_remain shows how many bytes are left. When it is 0, jump out.
	CurrentMsgPtr = &I2cMsgOut;
	u16Addr_cur = u16StartAddr;
	u16ByteNum_remain = u16ByteNum;
	
	while(u16ByteNum_remain > 0)
	{
		u16BlockAddr_next = \
			(u16Addr_cur & ST25C16_ADDR_BLOCK_MASK) + ST25C16_ADDR_BLOCK_LEN;
		
		while(u16Addr_cur < u16BlockAddr_next)
		{
			u16RowAddr_next = \
				(u16Addr_cur & ST25C16_ADDR_ROW_MASK_B) + ST25C16_ADDR_ROW_LEN_B;
			u16EachNum = u16RowAddr_next - u16Addr_cur;
			
			if(u16ByteNum_remain < u16EachNum)
			{
				u16EachNum = u16ByteNum_remain;
				u16ByteNum_remain = 0;
			}
			else
			{
				u16ByteNum_remain -= u16EachNum;
			}
			
			I2C_ConvAddr(u16Addr_cur, &i2cStartAddr);
			I2cMsgOut.SlaveAddress = I2C_SLAVE_ADDR | i2cStartAddr.u16BlockAddr;
			I2cMsgOut.NumOfBytes = u16EachNum;
			I2cMsgOut.MemoryLowAddr = i2cStartAddr.u16ByteAddr;
			for(i = 0; i < u16EachNum; i++)
			{
				I2cMsgOut.MsgBuffer[i] = *pSrcAddr;
				pSrcAddr++;
			}
			u16Error = I2C_Write_Page(&I2cMsgOut);
			
			if(I2C_SUCCESS == u16Error)
			{	//wait for ST25C16 completing internal write.
				u16Error = I2C_PollACK(&I2cMsgIn);
				if(I2C_ERROR == u16Error)
				{
					return I2C_W_FAILED;	//WRITE EEPROM FAILED
				}
				else
				{	
					if(u16ByteNum_remain > 0)
					{
						u16Addr_cur += u16EachNum;
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				return I2C_W_FAILED;	//WRITE EEPROM FAILED
			}
		}
	}
	return I2C_W_R_OK;
}

/*=============================================================================*
 * FUNCTION: 
 *		I2C_Write_InWord()
 *
 * PURPOSE : 
 *		Provide EEPROM write interface to other threads.
 *		Analyze EEPROM address and data amount to be wrote in, split write zone
 *	in Row of M24C16, then call "I2C_Write_Page()" to complete every row writing.
 *		
 * INPUT: 
 *		Uint16 u16StartAddr----EEPROM address to be wrote in,
 *		Uint16* pSrcAddr-------address of data to be wrote in,
 *		Uint16 u16WordNum------word amount of data to be wrote in.
 *
 * RETURN: 
 *		Uint16 type value:
 *		I2C_W_R_OK
 *		I2C_W_R_EXCEED_LIMIT
 *		I2C_W_R_DATA_AMOUNT_IS_ZERO
 *		I2C_W_FAILED
 *
 * CALLS: 
 *		I2C_ConvAddr()
 *		I2C_Write_Page()
 *		I2C_PollACK()
 *
 * CALLED BY: 
 *		Main.c  
 * 
 * MORE INFORMATION:
 *		Internal I2C error check and retry is handled by I2C_Write_Page().
 *		Every time after I2C_Write_Page() ends, it calls I2C_PollACK() to wait
 *	for the completion of write.
 *		This function has all data written.
 *
 *		For M24C16, one Row contains 16 bytes at most. If user want to sent 16
 *	bytes one time, the byte amout should be 17, because byte address is the
 *	plused one. 28335 can transmit at most 16 bytes one time, so in fact we can
 *	not write 16 bytes into M24C16 in one write cycle. To solve this problem, we
 *	reduce the size of Row to 8 bytes(4 words). Every time the data bytes we 
 *	write into M24C16 are all located in the same 'half row' in memory: that is 
 *	the same Block Address bits(b3,b2,b1 of Device Select code) and the same 5 
 *	MSBs in the Byte Address.
 *============================================================================*/
Uint16 I2C_Write_InWord(  Uint16 u16StartAddr, 
                          Uint16* pSrcAddr, 
                          Uint16 u16WordNum   )
{
	Uint16 i;
	Uint16 u16Addr_cur;
	Uint16 u16RowAddr_next;
	Uint16 u16BlockAddr_next;
	Uint16 u16WordNum_remain;
	Uint16 u16EachNum;
	Uint16 u16Error;
	TypeI2cAddr i2cStartAddr;
	
	//check if u16StartAddr is a valid ST25C16 address
	if ((u16StartAddr + 2*u16WordNum) >= ST25C16_ADDR_LIMIT)
	{
		return I2C_W_R_EXCEED_LIMIT;	//Error: write area exceeds EEPROM limit
	}
	else if (0 == u16WordNum)
	{
		return I2C_W_R_DATA_AMOUNT_IS_ZERO;	//Error: byte of data amount is 0.
	}

	for(i = 0; i < I2C_MAX_BUFFER_SIZE; i++)
	{
		I2cMsgOut.MsgBuffer[i] = 0;
	}
	
	//every outer while loop writes a BLOCK (upto 256 bytes),
	//every inner while loop writes a half ROW (upto 8 bytes),
	//u16WordNum_remain shows how many words are left. When it is 0, jump out.
	CurrentMsgPtr = &I2cMsgOut;
	u16Addr_cur = u16StartAddr;
	u16WordNum_remain = u16WordNum;
	
	while(u16WordNum_remain > 0)
	{
		u16BlockAddr_next = \
			(u16Addr_cur & ST25C16_ADDR_BLOCK_MASK) + ST25C16_ADDR_BLOCK_LEN;
		
		while(u16Addr_cur < u16BlockAddr_next)
		{
			u16RowAddr_next = \
				(u16Addr_cur & ST25C16_ADDR_ROW_MASK_W) + ST25C16_ADDR_ROW_LEN_W;
			u16EachNum = (u16RowAddr_next - u16Addr_cur) >> 1;
			
			if(u16WordNum_remain < u16EachNum)
			{
				u16EachNum = u16WordNum_remain;
				u16WordNum_remain = 0;
			}
			else
			{
				u16WordNum_remain -= u16EachNum;
			}
			
			I2C_ConvAddr(u16Addr_cur, &i2cStartAddr);
			I2cMsgOut.SlaveAddress = I2C_SLAVE_ADDR | i2cStartAddr.u16BlockAddr;
			I2cMsgOut.NumOfBytes = 2 * u16EachNum;
			I2cMsgOut.MemoryLowAddr = i2cStartAddr.u16ByteAddr;
			for(i = 0; i < 2 * u16EachNum; i += 2)
			{	//low address storage lower 8 bits
				//high address storage higher 8 bits
				I2cMsgOut.MsgBuffer[i] = (*pSrcAddr)&0x00FF;
				I2cMsgOut.MsgBuffer[i+1] = ((*pSrcAddr)&0xFF00)>>8;
				pSrcAddr++;
			}
			u16Error = I2C_Write_Page(&I2cMsgOut);
			
			if(I2C_SUCCESS == u16Error)
			{	//wait for ST25C16 completing internal write.
				u16Error = I2C_PollACK(&I2cMsgIn);
				if(I2C_ERROR == u16Error)
				{
					return I2C_W_FAILED;	//WRITE EEPROM FAILED
				}
				else
				{	
					if(u16WordNum_remain > 0)
					{
						u16Addr_cur += 2 * u16EachNum;
					}
					else
					{
						break;
					}
				}
			}
			else
			{
				return I2C_W_FAILED;	//WRITE EEPROM FAILED
			}
		}
	}
	return I2C_W_R_OK;
}

/*=============================================================================*
 * FUNCTION: 
 *		I2C_Write_Page()
 *		
 * PURPOSE : 
 *		Write a Page.
 *		M24C16's Page Write mode allows up to 16 bytes to be written in a single
 *	write cycle, provided that they are all located in the same 'row' in memory:
 *	that is the same Block Address bits(b3,b2,b1 of Device Select code) and the
 *	same 4 MSBs in the Byte Address.
 *		In this firmware, every time the data bytes we write into M24C16 are all
 *	located in the same 'half row' in memory. See comments of I2C_Write() for
 *	more information.
 *
 * INPUT: 
 *		TypeI2cMsg *msg----Address of a TypeI2cMsg type structure.
 *		It should be address of I2cMsgOut.
 *
 * RETURN: 
 *		Uint16 type value:
 *		I2C_SUCCESS
 *		I2C_ERROR
 *
 * CALLS: 
 *		I2C_Reset()
 *		I2C_WriteData()
 *		SEM_pendBinary()
 *
 * CALLED BY: 
 *		I2C_Write()
 *		
 * MORE INFORMATION:
 *		Destination address and source data are ready in I2cMsgOut before it is 
 *	called.
 *		It Handles Internal I2C error check and retry operation.
 *============================================================================*/
Uint16 I2C_Write_Page(TypeI2cMsg *msg)
{
/*	Uint16 u16Error;
	Uint16 u16RetryTimes_PageW;	//Page WRITE retry times
	Uint16 u16RetryTimes_Reset;	//I2C Reset retry times
	Uint16 u16ResetFlag;		//I2C_NO--not need RESET, I2C_YES--need RESET
	
	u16RetryTimes_PageW = 0;
	u16RetryTimes_Reset = 0;
	u16ResetFlag = I2C_NO;
	
	if (msg->MsgStatus != I2C_MSGSTAT_INACTIVE)
	{
		u16Error = I2C_Reset();
		if(I2C_SUCCESS == u16Error)
			msg->MsgStatus = I2C_MSGSTAT_INACTIVE;
		else
			return I2C_ERROR;
	}
	
	msg->MsgStatus = I2C_MSGSTAT_SEND_WITHSTOP;
	
	while(u16RetryTimes_Reset < I2C_MAX_TIMES_RESET)
	{	//--------dummy WRITE--------
		while(u16RetryTimes_PageW < I2C_MAX_TIMES_PAGE_WRITE)
		{	
			u16Error = I2C_WriteData(msg);
			
			if(I2C_SUCCESS != u16Error)
			{	//retry WRITE
				u16RetryTimes_PageW++;
				continue;
			}
			else
			{	
				u16Error = SEM_pendBinary(&SEM_I2C_isr, I2C_MAX_MS_SEMPEND);
				
				if(u16Error)
				{
					if(I2C_SUCCESS == u16I2cIsrError)
					{
						if(I2C_MSGSTAT_INACTIVE == msg->MsgStatus)
						{	//write success! jump out.
							u16RetryTimes_PageW = 0;
							break;
						}
						else
						{	//write failed! retry WRITE.
							u16RetryTimes_PageW++;
							continue;
						}
					}
					else if(I2C_NACK_ERROR == u16I2cIsrError)
					{	//ISR will generate a STOP,then a SCD will happen.
						u16Error = SEM_pendBinary(&SEM_I2C_isr, \
													I2C_MAX_MS_SEMPEND);
						if(u16Error)
						{	
							u16RetryTimes_PageW++;	//retry WRITE.
							continue;
						}
						else
						{	//FATAL ERROR: can't enter ISR
							msg->MsgStatus = I2C_MSGSTAT_INACTIVE;
							return I2C_ERROR;
						}
					}
					else	//----illegal interrupt source
					{	//enable I2C Reset
						u16ResetFlag = I2C_YES;
						break;
					}
				}
				else
				{	//FATAL ERROR: can't enter ISR
					msg->MsgStatus = I2C_MSGSTAT_INACTIVE;
					return I2C_ERROR;
				}
			}
		}	//----WRITE at most I2C_MAX_TIMES_PAGE_WRITE time ends here!

		
		if(u16RetryTimes_PageW >= 3)		//3=I2C_MAX_TIMES_PAGE_WRITE
			u16ResetFlag = I2C_YES;
		if((I2C_NO == u16ResetFlag) && (u16RetryTimes_PageW < 3))
			break;

		//--------RESET I2C--------
		if(I2C_YES == u16ResetFlag)
		{
			u16ResetFlag = I2C_NO;
			u16RetryTimes_Reset++;
			u16RetryTimes_PageW = 0;
			
			u16Error = I2C_Reset();
			
			if(I2C_ERROR == u16Error)
			{
				msg->MsgStatus = I2C_MSGSTAT_INACTIVE;
				SEM_pendBinary(&SEM_I2C_isr, 0);	//clear unexpected SEM
				return I2C_ERROR;
			}
			else
			{
				msg->MsgStatus = I2C_MSGSTAT_SEND_WITHSTOP;
				continue;
			}
		}
	}	//----I2C Reset I2C_MAX_TIMES_RESET times end here!
	
	u16RetryTimes_Reset = 0;
	u16RetryTimes_PageW = 0;

//	SEM_pendBinary(&SEM_I2C_isr, 0);	//clear unexpected SEM

	if(u16RetryTimes_Reset < I2C_MAX_TIMES_RESET)
	{
		if(I2C_MSGSTAT_INACTIVE == msg->MsgStatus)
		{
			return I2C_SUCCESS;
		}
		else
		{
			msg->MsgStatus = I2C_MSGSTAT_INACTIVE;
			return I2C_ERROR;
		}
	}
	else
	{
		msg->MsgStatus = I2C_MSGSTAT_INACTIVE;
		return I2C_ERROR;
	}
*/
return(0);
}

/*=============================================================================*
 * FUNCTION: 
 *		I2C_WriteData()
 *
 * PURPOSE : 
 *		Operate registers of I2C Module to execute data writing.
 *
 * INPUT: 
 *		TypeI2cMsg *msg----Address of a TypeI2cMsg type structure.
 *		It should be address of I2cMsgOut.
 *
 * RETURN: 
 *		Uint16 type value:
 *		I2C_STP_NOT_READY_ERROR
 *		I2C_BUS_BUSY_ERROR
 *		I2C_SUCCESS
 *
 * CALLS: 
 *		void
 *
 * CALLED BY: 
 *		I2C_Write_Page()
 *		
 *============================================================================*/
Uint16 I2C_WriteData(TypeI2cMsg *msg)
{
	Uint16 i;

	// Wait until the STP bit is cleared from any previous master communication.
	// Clearing of this bit by the module is delayed until after the SCD bit is
	// set. If this bit is not checked prior to initiating a new message, the
	// I2C could get confused.
	if (1 == I2caRegs.I2CMDR.bit.STP)
	{
		return I2C_STP_NOT_READY_ERROR;
	}
	else if (1 == I2caRegs.I2CSTR.bit.BB)
	{	// Check if bus busy
		return I2C_BUS_BUSY_ERROR;
	}
	else
	{	
		if(I2caRegs.I2CFFTX.bit.TXFFST)	//clear TXFIFO
		{	//Test shows that after last write operation, I2CFFTX[TXFFST] is
			//not 0 but 15, which causes error on next write operation. To fix
			//this bug, manually clear I2CFFTX[TXFFST] before every write.
			I2caRegs.I2CFFTX.bit.TXFFRST = 0;
			while(I2caRegs.I2CFFTX.bit.TXFFST);
			I2caRegs.I2CFFTX.bit.TXFFRST = 1;
		}

		msg->MsgStatus = I2C_MSGSTAT_WRITE_BUSY;
		// Setup slave address
		I2caRegs.I2CSAR = msg->SlaveAddress;
		// Setup number of bytes to send
		I2caRegs.I2CCNT = msg->NumOfBytes + 1;	//plus BYTE ADDRESS
		// Setup data to send
		I2caRegs.I2CDXR = msg->MemoryLowAddr;
		for (i=0; i<msg->NumOfBytes; i++)                                    
		{                                         
			I2caRegs.I2CDXR = *(msg->MsgBuffer+i); 
		}                                         

		// Send start as master transmitter
		I2caRegs.I2CMDR.all = 0x6E20;
		//bit15,   0,   NACKMOD
		//bit14,   1,   FREE 	when encounter a breakpoint: 0=stop immediately;
// 1=run free
		//bit13,   1,   STT		1=generate a START condition. The combination of
// RM, STT and STP see Table 6
		//bit12,   0,   reserved

		//bit11,   1,   STP		1=generate a STOP condition, see Table 6
		//bit10,   1,   MST		0=slave, 1=master
		//bit9,    1,   TRX		0=receiver, 1=transmitter
		//bit8,    0,   XA		0=7 bit address, 1=10 bit address

		//bit7,    0,   RM		0=Nonrepeat mode, 1=Repeat mode
		//bit6,    0,   DLB		0=disable digital loopback, 1=enable
		//bit5,    1,   IRS		0=reset/disable I2C module, 1=enable
		//bit4,    0,   STB		0=not in START byte mode, 1=in. This bit is to 
//help a slave that need extra time to detect START condition

		//bit3,    0,   FDF		0=disable free data mode, 1=enable
		//bit2-0,  000, BC		000=8 bits per data byte
		//						001=1 bits per data byte
		//						010=2 bits per data byte
		//						011=3 bits per data byte
		//						100=4 bits per data byte
		//						101=5 bits per data byte
		//						110=6 bits per data byte
		//						111=7 bits per data byte
	}
	return I2C_SUCCESS;
}

/*=============================================================================*
 * FUNCTION: 
 *		I2C_Read_InByte()
 *
 * PURPOSE : 
 *		Provide EEPROM read interface to other threads.
 *		Analyze EEPROM address and data amount to be read, split read zone with
 *	16 bytes as one groups, then call "I2C_Read_MultiByte()" to complete every 
 *	group reading.
 *
 * INPUT: 
 *		Uint16 u16StartAddr----EEPROM address start to be read out,
 *		Uint16* pSrcAddr-------storage address of data be read out, each Uint16-
 * type memory contains 1 byte data in 8 LSBs.
 *		Uint16 u16ByteNum------byte amount of data to be read out.
 *
 * RETURN: 
 *		Uint16 type value:
 *		I2C_W_R_OK
 *		I2C_W_R_EXCEED_LIMIT
 *		I2C_W_R_DATA_AMOUNT_IS_ZERO
 *		I2C_R_FAILED
 *
 * CALLS: 
 *		I2C_ConvAddr()
 *		I2C_Read_MultiByte()
 *		I2C_PollACK()
 *
 * CALLED BY: 
 *     Main.c  
 *
 * MORE INFORMATION:
 *		Internal I2C error check and retry is handled by I2C_Read_MultiByte().
 *		This function has all data read.
 *============================================================================*/
Uint16 I2C_Read_InByte(  Uint16 u16StartAddr, 
                         Uint16* pDstiAddr, 
                         Uint16 u16ByteNum   )
{
	Uint16 i;
	Uint16 u16EachNum;
	Uint16 u16EEPROMAddr;
	Uint16 u16Error;
	TypeI2cAddr i2cStartAddr;

	//check if u16StartAddr is a valid ST25C16 address
	if ((u16StartAddr + u16ByteNum) >= ST25C16_ADDR_LIMIT)
	{
		return I2C_W_R_EXCEED_LIMIT;
	}
	else if (0 == u16ByteNum)
	{
		return I2C_W_R_DATA_AMOUNT_IS_ZERO;
	}
	
	//prepare for first reading.
	CurrentMsgPtr = &I2cMsgIn;
	u16EEPROMAddr = u16StartAddr;
	I2C_ConvAddr(u16EEPROMAddr, &i2cStartAddr);
	I2cMsgIn.SlaveAddress = I2C_SLAVE_ADDR | i2cStartAddr.u16BlockAddr;
	I2cMsgIn.MemoryLowAddr = i2cStartAddr.u16ByteAddr;
	u16EachNum = u16ByteNum;

	while (u16EachNum)
	{	
		if (u16EachNum / I2C_NUMBYTES > 0)
		{
			I2cMsgIn.NumOfBytes = I2C_NUMBYTES;
			u16EachNum -= I2C_NUMBYTES;
			u16EEPROMAddr += I2C_NUMBYTES;	//address for next reading
		}
		else
		{
			I2cMsgIn.NumOfBytes = u16EachNum;
			u16EachNum = 0;
		}
		u16Error = I2C_Read_MultiByte(&I2cMsgIn);
		if (0 == u16Error)	//read success!
		{	//copy data, clear buffer
			for(i = 0; i < I2cMsgIn.NumOfBytes; i++)
			{
				*pDstiAddr = I2cMsgIn.MsgBuffer[i];
				pDstiAddr++;
				I2cMsgIn.MsgBuffer[i] = 0;
			}
			if(0 != u16EachNum)
			{	//prepare for next reading.
				I2C_ConvAddr(u16EEPROMAddr, &i2cStartAddr);
				I2cMsgIn.MemoryLowAddr = i2cStartAddr.u16ByteAddr;
				I2cMsgIn.SlaveAddress = I2C_SLAVE_ADDR | i2cStartAddr.u16BlockAddr;
			}
		}
		else
		{
			return I2C_R_FAILED;	//Error: read EEPROM failed
		}
	}
	return I2C_W_R_OK;
}
/*=============================================================================*
 * FUNCTION: 
 *		I2C_Read_InWord()
 *
 * PURPOSE : 
 *		Provide EEPROM read interface to other threads.
 *		Analyze EEPROM address and data amount to be read, split read zone with
 *	16 bytes as one groups, then call "I2C_Read_MultiByte()" to complete every 
 *	group reading.
 *
 * INPUT: 
 *		Uint16 u16StartAddr----EEPROM address start to be read out,
 *		Uint16* pSrcAddr-------storage address of data be read out,
 *		Uint16 u16WordNum------word amount of data to be read out.
 *
 * RETURN: 
 *		Uint16 type value:
 *		I2C_W_R_OK
 *		I2C_W_R_EXCEED_LIMIT
 *		I2C_W_R_DATA_AMOUNT_IS_ZERO
 *		I2C_R_FAILED
 *
 * CALLS: 
 *		I2C_ConvAddr()
 *		I2C_Read_MultiByte()
 *		I2C_PollACK()
 *
 * CALLED BY: 
 *     Main.c  
 *
 * MORE INFORMATION:
 *		Internal I2C error check and retry is handled by I2C_Read_MultiByte().
 *		This function has all data read.
 *============================================================================*/
Uint16 I2C_Read_InWord(  Uint16 u16StartAddr, 
                         Uint16* pDstiAddr, 
                         Uint16 u16WordNum   )
{
	Uint16 i;
	Uint16 u16EachNum;
	Uint16 u16EEPROMAddr;
	Uint16 u16Error;
	TypeI2cAddr i2cStartAddr;

	//check if u16StartAddr is a valid ST25C16 address
	if ((u16StartAddr + 2 * u16WordNum) >= ST25C16_ADDR_LIMIT)
	{
		return I2C_W_R_EXCEED_LIMIT;
	}
	else if (0 == u16WordNum)
	{
		return I2C_W_R_DATA_AMOUNT_IS_ZERO;
	}
	
	//prepare for first reading.
	CurrentMsgPtr = &I2cMsgIn;
	u16EEPROMAddr = u16StartAddr;
	I2C_ConvAddr(u16EEPROMAddr, &i2cStartAddr);
	I2cMsgIn.SlaveAddress = I2C_SLAVE_ADDR | i2cStartAddr.u16BlockAddr;
	I2cMsgIn.MemoryLowAddr = i2cStartAddr.u16ByteAddr;
	u16EachNum = 2 * u16WordNum;

	while (u16EachNum)
	{	
		if (u16EachNum / I2C_NUMBYTES > 0)
		{
			I2cMsgIn.NumOfBytes = I2C_NUMBYTES;
			u16EachNum -= I2C_NUMBYTES;
			u16EEPROMAddr += I2C_NUMBYTES;	//address for next reading
		}
		else
		{
			I2cMsgIn.NumOfBytes = u16EachNum;
			u16EachNum = 0;
		}
		u16Error = I2C_Read_MultiByte(&I2cMsgIn);
		if (0 == u16Error)	//read success!
		{	//copy data, clear buffer
			for(i = 0; i < I2cMsgIn.NumOfBytes; i+=2)
			{
				*pDstiAddr = (I2cMsgIn.MsgBuffer[i+1]<<8) | I2cMsgIn.MsgBuffer[i];
				pDstiAddr++;
				I2cMsgIn.MsgBuffer[i+1] = I2cMsgIn.MsgBuffer[i] = 0;
			}
			if(0 != u16EachNum)
			{	//prepare for next reading.
				I2C_ConvAddr(u16EEPROMAddr, &i2cStartAddr);
				I2cMsgIn.MemoryLowAddr = i2cStartAddr.u16ByteAddr;
				I2cMsgIn.SlaveAddress = I2C_SLAVE_ADDR | i2cStartAddr.u16BlockAddr;
			}
		}
		else
		{
			return I2C_R_FAILED;	//Error: read EEPROM failed
		}
	}
	return I2C_W_R_OK;
}

/*=============================================================================*
 * FUNCTION: 
 *		I2C_Read_MultiByte()
 *
 * PURPOSE : 
 *		Read at most 16 bytes.
 *		M24C16 supports more than 16 bytes to be read in Sequential Random Read
 *	mode, but in this firmware, the buffer size is 16, so we read at most 16
 *	bytes in this function.	
 *
 * INPUT: 
 *		TypeI2cMsg *msg----Address of a TypeI2cMsg type structure.
 *		It should be address of I2cMsgIn.
 *
 * RETURN: 
 *		Uint16 type value:
 *		I2C_SUCCESS
 *		I2C_ERROR
 *
 * CALLS: 
 *		I2C_Reset()
 *		I2C_ReadData()
 *		SEM_pendBinary()
 *
 * CALLED BY: 
 *		I2C_Read()
 *	
 * MORE INFORMATION:
 *		Destination address and source data are ready in I2cMsgIn before it is 
 *	called.
 *		It Handles Internal I2C error check and retry operation.
 * 
 *============================================================================*/
Uint16 I2C_Read_MultiByte(TypeI2cMsg *msg)
{	
/*	Uint16 u16Error;
	Uint16 u16RetryTimes_dW;	//dummy WRITE retry times
	Uint16 u16RetryTimes_Reset;	//I2C Reset retry times
	Uint16 u16ResetFlag;		//I2C_NO--not need RESET, I2C_YES--need RESET
	
	u16RetryTimes_dW = 0;
	u16RetryTimes_Reset = 0;
	u16ResetFlag = I2C_NO;
	
	if (msg->MsgStatus != I2C_MSGSTAT_INACTIVE)
	{
		u16Error = I2C_Reset();
		if(I2C_ERROR == u16Error)
		{
			return I2C_ERROR;
		}
		else
		{
			msg->MsgStatus = I2C_MSGSTAT_INACTIVE;
		}
	}
	
	msg->MsgStatus = I2C_MSGSTAT_SEND_NOSTOP;
	
	while(u16RetryTimes_Reset < I2C_MAX_TIMES_RESET)
	{	//--------dummy WRITE--------
		while(u16RetryTimes_dW < I2C_MAX_TIMES_DYMMY_WRITE)
		{	
			u16Error = I2C_ReadData(msg);
			
			if(I2C_SUCCESS != u16Error)
			{	//retry dummy WRITE
				u16RetryTimes_dW++;
				continue;
			}
			else
			{	
				u16Error = SEM_pendBinary(&SEM_I2C_isr, I2C_MAX_MS_SEMPEND);

				if(u16Error)	//return from ISR
				{
    				if(I2C_SUCCESS == u16I2cIsrError)
    				{
    					if(I2C_MSGSTAT_RESTART == msg->MsgStatus)
    					{
    						u16RetryTimes_dW = 0;
    						break;		//dummy WRITE success, jump to RESTART
    					}
    					else
    					{	
    						u16RetryTimes_dW++;
    						continue;	//dummy WRITE failed, retry dummy WRITE
    					}
    				}
    				else if(I2C_NACK_ERROR == u16I2cIsrError)//retry dummy WRITE
    				{	//wait for SCD interrupt return
    					u16Error = SEM_pendBinary(&SEM_I2C_isr, I2C_MAX_MS_SEMPEND);
						if(u16Error)
    					{	
    						u16RetryTimes_dW++;
    						continue;
						}
						else
						{
							msg->MsgStatus = I2C_MSGSTAT_INACTIVE;
							return I2C_ERROR;
						}
    				}
    				else	//----illegal interrupt source
    				{	
    					u16ResetFlag = I2C_YES;	//retry I2C Reset
    					break;
    				}
				}
				else	//FATAL ERROR----can't enter ISR
				{
					msg->MsgStatus = I2C_MSGSTAT_INACTIVE;
					return I2C_ERROR;
				}
			}
		}	//----dummy WRITE I2C_MAX_TIMES_DYMMY_WRITE time ends here!
		
		if(u16RetryTimes_dW >= I2C_MAX_TIMES_DYMMY_WRITE)
		{	//retry I2C Reset
			u16ResetFlag = I2C_YES;
		}
		else if(I2C_NO == u16ResetFlag)
		{	//--------RESTART--------	
			u16Error = I2C_ReadData(msg);

			if(I2C_SUCCESS != u16Error)
			{	//retry I2C Reset
				u16ResetFlag = I2C_YES;
			}
			else
			{	
				u16Error = SEM_pendBinary(&SEM_I2C_isr, I2C_MAX_MS_SEMPEND);
				if(u16Error)
				{
    				if(I2C_SUCCESS == u16I2cIsrError)	
    				{	
    					if(I2C_MSGSTAT_INACTIVE == msg->MsgStatus)
    					{
    						break;	// read success!
    					}
    					else
    					{	//RESTART failed
    						u16ResetFlag = I2C_YES;	//retry I2C Reset
    					}
    				}
    				else//----illegal interrupt source
    				{	
    					u16ResetFlag = I2C_YES;	//retry I2C Reset
    				}
				}
				else	//FATAL ERROR----can't enter ISR
				{
					msg->MsgStatus = I2C_MSGSTAT_INACTIVE;
					return I2C_ERROR;
				}
			}
		}
		//note: no else here!

		//--------RESET I2C--------
		if(I2C_YES == u16ResetFlag)
		{
			u16ResetFlag = I2C_NO;
			u16RetryTimes_Reset++;
			u16RetryTimes_dW = 0;
			
			u16Error = I2C_Reset();
			
			if(I2C_ERROR == u16Error)
			{
				msg->MsgStatus = I2C_MSGSTAT_INACTIVE;
				SEM_pendBinary(&SEM_I2C_isr, 0);	//clear unexpected SEM
				return I2C_ERROR;
			}
			else
			{
				msg->MsgStatus = I2C_MSGSTAT_SEND_NOSTOP;
				continue;
			}
		}
	}	//----I2C Reset 3 times end here!

	u16RetryTimes_dW = 0;
	SEM_pendBinary(&SEM_I2C_isr, 0);	//clear unexpected SEM

	if(u16RetryTimes_Reset < I2C_MAX_TIMES_RESET)
	{	
		u16RetryTimes_Reset = 0;

		if(I2C_MSGSTAT_INACTIVE == msg->MsgStatus)
		{
			return I2C_SUCCESS;
		}
		else
		{
			msg->MsgStatus = I2C_MSGSTAT_INACTIVE;
			return I2C_ERROR;
		}
	}
	else
	{
		u16RetryTimes_Reset = 0;
		msg->MsgStatus = I2C_MSGSTAT_INACTIVE;
		return I2C_ERROR;
	}
*/
return(0);
}

/*=============================================================================*
 * FUNCTION: 
 *		I2C_ReadData()
 *
 * PURPOSE : 
 *		Operate registers of I2C Module to execute data reading.
 *
 * INPUT: 
 *		TypeI2cMsg *msg----Address of a TypeI2cMsg type structure.
 *		It should be address of I2cMsgIn.
 *
 * RETURN: 
 *		Uint16 type value:
 *		I2C_STP_NOT_READY_ERROR
 *		I2C_BUS_BUSY_ERROR
 *		I2C_SUCCESS
 *
 * CALLS: 
 *     void
 *
 * CALLED BY: 
 *		I2C_Read_MultiByte()
 * 
 *============================================================================*/
Uint16 I2C_ReadData(TypeI2cMsg *msg)
{
	// Wait until the STP bit is cleared from any previous master communication.
	// Clearing of this bit by the module is delayed until after the SCD bit is
	// set. If this bit is not checked prior to initiating a new message, the
	// I2C could get confused.
	if (1 == I2caRegs.I2CMDR.bit.STP)
	{
		return I2C_STP_NOT_READY_ERROR;
	}

	if(I2C_MSGSTAT_SEND_NOSTOP == msg->MsgStatus)
	{
		// Check if bus busy
		if (1 == I2caRegs.I2CSTR.bit.BB)
		{
			return I2C_BUS_BUSY_ERROR;
		}

		if(I2caRegs.I2CFFTX.bit.TXFFST)	//clear TXFIFO
		{	//Test shows that after last write operation, I2CFFTX[TXFFST] is
			//not 0 but 15, which causes error on next write operation. To fix
			//this bug, manually clear I2CFFTX[TXFFST] before every write.
			I2caRegs.I2CFFTX.bit.TXFFRST = 0;
			while(I2caRegs.I2CFFTX.bit.TXFFST);
			I2caRegs.I2CFFTX.bit.TXFFRST = 1;
		}

		msg->MsgStatus = I2C_MSGSTAT_SEND_NOSTOP_BUSY;
		//----dummy write
		I2caRegs.I2CSAR = msg->SlaveAddress;	
		I2caRegs.I2CCNT = 1;
		I2caRegs.I2CDXR = msg->MemoryLowAddr;
		I2caRegs.I2CMDR.all = 0x2620;				// Send data to setup EEPROM address
	}
	else if(I2C_MSGSTAT_RESTART == msg->MsgStatus)
	{	
		msg->MsgStatus = I2C_MSGSTAT_READ_BUSY;
		//----read
		I2caRegs.I2CSAR = msg->SlaveAddress;	
		I2caRegs.I2CCNT = msg->NumOfBytes;			// Setup how many bytes to expect
		I2caRegs.I2CMDR.all = 0x2C20;				// Send restart as master receiver
	}

	return I2C_SUCCESS;
}

/*=============================================================================*
 * FUNCTION: 
 *		SWI_I2C_handle()
 *
 * PURPOSE : 
 *		DSP/BIOS SWI function, handles I2C interrupt routine.
 *
 * INPUT: 
 *     void
 *
 * RETURN: 
 *     void
 *
 * CALLS: 
 *     void
 *
 * CALLED BY: 
 *     DSP/BIOS shedule routine.
 * 
 *============================================================================*/
void SWI_I2C_handle(void)     // I2C-A
{
	Uint16  i;

	u16I2cIsrError = I2C_SUCCESS;
	
	// Read interrupt source
	u16IntSource = I2caRegs.I2CISRC.all;
	
	// Interrupt source = stop condition detected
	if(I2C_SCD_ISRC == u16IntSource)
	{
		// If completed message was writing data, reset msg to inactive state
		if (I2C_MSGSTAT_WRITE_BUSY == CurrentMsgPtr->MsgStatus)
		{
			if(0 != I2caRegs.I2CFFTX.bit.TXFFST)
			{	
				u16I2cIsrError = I2C_NACK_ERROR;
			}

			CurrentMsgPtr->MsgStatus = I2C_MSGSTAT_INACTIVE;
		}
		else
		{
			// If a message receives a NACK during the address setup portion of the
			// EEPROM read, the code further below included in the register access ready
			// interrupt source code will generate a stop condition. After the stop
			// condition is received (here), set the message status to try again.
			// User may want to limit the number of retries before generating an error.
			if(I2C_MSGSTAT_SEND_NOSTOP_BUSY == CurrentMsgPtr->MsgStatus)
			{
				CurrentMsgPtr->MsgStatus = I2C_MSGSTAT_SEND_NOSTOP;
				u16I2cIsrError = I2C_NACK_ERROR;
			}
			// If completed message was reading EEPROM data, reset msg to inactive state
			// and read data from FIFO.
			else if (I2C_MSGSTAT_READ_BUSY == CurrentMsgPtr->MsgStatus)
			{
				CurrentMsgPtr->MsgStatus = I2C_MSGSTAT_INACTIVE;
				for(i = 0; i < CurrentMsgPtr->NumOfBytes; i++)
				{
					CurrentMsgPtr->MsgBuffer[i] = I2caRegs.I2CDRR;
				}
			}
		}
	}  // end of stop condition detected

	// Interrupt source = Register Access Ready
	// This interrupt is used to determine when the EEPROM address setup portion of the
	// read data communication is complete. Since no stop bit is commanded, this flag
	// tells us when the message has been sent instead of the SCD flag. If a NACK is
	// received, clear the NACK bit and command a stop. Otherwise, move on to the read
	// data portion of the communication.
	else if(I2C_ARDY_ISRC == u16IntSource)
	{
		if(1 == I2caRegs.I2CSTR.bit.NACK)
		{
			I2caRegs.I2CMDR.bit.STP = 1;
			I2caRegs.I2CSTR.all = I2C_CLR_NACK_BIT;
			
			if(I2C_MSGSTAT_SEND_NOSTOP_BUSY == CurrentMsgPtr->MsgStatus)
			{
				u16I2cIsrError = I2C_NACK_ERROR;
			}
			else if(I2C_MSGSTAT_WRITE_BUSY == CurrentMsgPtr->MsgStatus)
			{
				u16I2cIsrError = I2C_NACK_ERROR;
			}
		}
		else if(I2C_MSGSTAT_SEND_NOSTOP_BUSY == CurrentMsgPtr->MsgStatus)
		{
			CurrentMsgPtr->MsgStatus = I2C_MSGSTAT_RESTART;
		}
	}  // end of register access ready

	else if(I2C_NO_ISRC != u16IntSource)
	{	// Generate some error due to invalid interrupt source
		u16I2cIsrError = I2C_ERROR;
	}
	if(I2C_NO_ISRC != u16IntSource)
	{
		SEM_postBinary(&SEM_I2C_isr);
	}
}


//--- end of file -----------------------------------------------------
