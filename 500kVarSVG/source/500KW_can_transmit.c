
#include "DSP2833x_Device.h"			// Peripheral address definitions
#include "500KW_MAIN_HEADER.h"					// Main include file

void Can_Carbox_Transmit(void)    //并联传送变量
{    

	struct ECAN_REGS ECanaShadow;

		ECanaMboxes.MBOX0.MDL.all= 100 * PLLConReg.Sin_Theta;
		ECanaMboxes.MBOX0.MDH.all= 100 * PLLConReg.Cos_Theta;
		
		//*************PART I**************************
		//the next part focus on transmitting messages
		//Step 1, clear register CANTRS
         /* ECanaShadow.CANTRR.all=0xFFFFFFFF;  //? should it be ECanaRegs.CANTRS.all=0xFFFFFFFF; ***check it's right
		ECanaRegs.CANTRR.all=ECanaShadow.CANTRR.all;
		ECanaShadow.CANTA.all=0xFFFFFFFF;
		ECanaRegs.CANTA.all=ECanaShadow.CANTA.all; */
	   //Step 2, initialize mailboxes
		EALLOW;
		//Step 3, config TRS to apply for transmitting
		ECanaShadow.CANTRS.all=0;
		ECanaShadow.CANTRS.bit.TRS0=1;
		ECanaRegs.CANTRS.all=ECanaShadow.CANTRS.all;
		//Step 4, wait for response to complete transmition
			
    	while(ECanaRegs.CANTA.bit.TA0 == 0) 
   		{}
		//Step 5,reset TA and transmition flag by writing 1 to corresponding register bits
		ECanaShadow.CANTA.all=0;
		ECanaShadow.CANTA.bit.TA0=1;
		ECanaRegs.CANTA.all=ECanaShadow.CANTA.all;
		   
		EDIS;
	//*************PART I END**********************


}  

