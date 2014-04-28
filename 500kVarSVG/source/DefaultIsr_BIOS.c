/*=============================================================================*
 *         Copyright(c) 2009-2011, ALL RIGHTS RESERVED
 *
 *  FILENAME : DefaultIsr_BIOS.c 
 *
 *  PURPOSE  : ADC, EPWMx_TRIP, ECAP1 and SCIRXINT ISR frames.
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
 

#include "DSP2833x_Device.h"
#include "500kVar_SVG.h"		    // Peripheral address definitions
				// Main include file

void FAN_LOW_OFF(void);
void FAN_MIDDLE_OFF(void);
void FAN_HIGH_OFF(void);
//int16	Test_Start_of_SEQISR;
//float32     program_time1 = 0;


//---------------------------------------------------------------------
void INT13_ISR(void)// 0x000D1A  INT13 - XINT13 (or CPU Timer1, reserved for TI)
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void RTOSINT_ISR(void)				// 0x000D20  RTOSINT - CPU RTOS interrupt
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void NMI_ISR(void)							// 0x000D24  NMI - XNMI interrupt
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void ILLEGAL_ISR(void)			// 0x000D26  ILLEGAL - illegal operation trap
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void USER1_ISR(void)				// 0x000D28  USER1 - software interrupt #1
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void USER2_ISR(void)				// 0x000D2A  USER2 - software interrupt #2
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void USER3_ISR(void)				// 0x000D2C  USER3 - software interrupt #3
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void USER4_ISR(void)				// 0x000D2E  USER4 - software interrupt #4
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void USER5_ISR(void)				// 0x000D30  USER5 - software interrupt #5
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void USER6_ISR(void)				// 0x000D32  USER6 - software interrupt #6
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void USER7_ISR(void)				// 0x000D34  USER7 - software interrupt #7
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void USER8_ISR(void)				// 0x000D36  USER8 - software interrupt #8
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void USER9_ISR(void)				// 0x000D38  USER9 - software interrupt #9
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void USER10_ISR(void)				// 0x000D3A  USER10 - software interrupt #10
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void USER11_ISR(void)				// 0x000D3C  USER11 - software interrupt #11
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void USER12_ISR(void)				// 0x000D3E  USER12 - software interrupt #12
{
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
//#ifdef SVPWM2009_FLASH
//#endif
void SEQ1INT_ISR(void)					// PIE1.1 @ 0x000D40  SEQ1INT (ADC SEQ1)
{ 
	// start of SEQ1INT_ISR 
	static Uint32 MasterDSP_State = 0;  //主DSP运行状态判断
	 
	DINT;

	if( EPwm1Regs.TBCTR < 500)            //同步信号的输出脚，
	{
		GpioDataRegs.GPBSET.bit.GPIO33 = 1;   //上升沿计数时输出为高
		GpioDataRegs.GPASET.bit.GPIO13 = 1;   //上升沿计数时输出为高，主副DSP互锁
	}
	else
	{
		GpioDataRegs.GPBCLEAR.bit.GPIO33 = 1; //下降沿计数时输出为低
		GpioDataRegs.GPACLEAR.bit.GPIO13 = 1; //下降沿计数时输出为低，主副DSP互锁
	}


//	Test_Start_of_SEQISR = EPwm1Regs.TBCTR;
/*
// program_time1 is defined for Program stop time
    program_time1++;
	if(program_time1 > (g_Ctrl_Para.Temp_Lim * 6000 * 60))
	{

		PWMOutputsDisable();
    	AC_RELAY1_OFF;
		FAN_HIGH_OFF();
		//FAN_MIDDLE_OFF();
//		FAN_LOW_OFF();
		while(1)
		{
		program_time1 = 520000;
		}
	}
*/
// AD interrupt main math, ADC_INT_Control conculde 电流闭环，锁相等主功率控制程序
	ADC_INT_Control(); 
//	Can_Carbox_Transmit();   //并联模式，发送数据

	if(MasterDSP_LEVEL == 0)
	{
		MasterDSP_State++;
		if((MasterDSP_State >= 4) && (g_Sys_Current_State != WaitState))
			g_SysFaultMessage.bit.MasterDSPFault = 1;		
	}
	else 
		MasterDSP_State = 0;
  
	//add at 2013.06.21, 副DSP传输
//	s_SysFaultMessage.Word.byte3 =	SpiaRegs.SPIRXBUF;
	if(s_SysFaultMessage.bit.Rsvd12 == 0)
	{
		s_SysFaultMessage.Word.byte0 = s_SysFaultMessage.Word.byte3;
		g_SysFaultMessage.bit.SlaveDSPFault = s_SysFaultMessage.bit.SlaveDSPFault;
		s_SysFaultMessage.Word.byte0 = (s_SysFaultMessage.Word.byte0 & 0x3F);
	}

	if((s_SysFaultMessage.bit.Rsvd12 == 1) && (s_SysFaultMessage.bit.Rsvd11 == 0))
	{
		s_SysFaultMessage.Word.byte1 = s_SysFaultMessage.Word.byte3;
		s_SysFaultMessage.Word.byte1 = (s_SysFaultMessage.Word.byte1 & 0x3F);
	}

	if((s_SysFaultMessage.bit.Rsvd12 == 1) && (s_SysFaultMessage.bit.Rsvd11 == 1))
	{
		s_SysFaultMessage.Word.byte2 = (s_SysFaultMessage.Word.byte3  & 0x3F);
	}
	//add at 2013.06.21, 副DSP传输
			
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;	 // Must acknowledge the PIE group

	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;	// Reset cascaded SEQ to CONV00 state
	AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;	// Clear ADC SEQ1 interrupt flag

	EINT;

} // end of SEQ1INT_ISR 

//---------------------------------------------------------------------
void SEQ2INT_ISR(void)					// PIE1.2 @ 0x000D42  SEQ2INT (ADC SEQ2)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;	 // Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
												// PIE1.3 @ 0x000D44 reserved

//---------------------------------------------------------------------
void XINT1_ISR(void)							// PIE1.4 @ 0x000D46  XINT1
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}     

//---------------------------------------------------------------------
void XINT2_ISR(void)							// PIE1.5 @ 0x000D48  XINT2
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void ADCINT_ISR(void)						// PIE1.6 @ 0x000D4A  ADCINT (ADC)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);

} // end of ADCINT_ISR

//---------------------------------------------------------------------
void TINT0_ISR(void)				// PIE1.7 @ 0x000D4C  TINT0 (CPU TIMER 0)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void WAKEINT_ISR(void)					// PIE1.8 @ 0x000D4E  WAKEINT (LPM/WD)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void EPWM1_TZINT_ISR(void)			// PIE2.1 @ 0x000D50  EPWM1_TZINT (EPWM1)
{

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;	// Must acknowledge the PIE group
    

    PWMOutputsDisable();
	AC_RELAY1_OFF; 
	g_Sys_Current_State = PermanentFaultState; 

	EPwm1Regs.TZFLG.bit.INT = 1;
	EPwm1Regs.TZFLG.bit.CBC = 1;

// Next two lines for debug only - remove after inserting your ISR
//	asm (" ESTOP0");							// Emulator Halt instruction
//	while(1);

}

//---------------------------------------------------------------------
void EPWM2_TZINT_ISR(void)			// PIE2.2 @ 0x000D52  EPWM2_TZINT (EPWM2)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;	// Must acknowledge the PIE group

	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void EPWM3_TZINT_ISR(void)			// PIE2.3 @ 0x000D54  EPWM3_TZINT (EPWM3)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;	// Must acknowledge the PIE group
  
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);

	
}

//---------------------------------------------------------------------
void EPWM4_TZINT_ISR(void)			// PIE2.4 @ 0x000D56  EPWM4_TZINT (EPWM4)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;	// Must acknowledge the PIE group
  
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
	
}

//---------------------------------------------------------------------
void EPWM5_TZINT_ISR(void)			// PIE2.5 @ 0x000D58  EPWM5_TZINT (EPWM5)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;	// Must acknowledge the PIE group

	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void EPWM6_TZINT_ISR(void)			// PIE2.6 @ 0x000D5A  EPWM6_TZINT (EPWM6)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP2;	// Must acknowledge the PIE group

	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);

}

//---------------------------------------------------------------------
											// PIE2.7 @ 0x000D5C reserved
											// PIE2.8 @ 0x000D5E reserved
     
//---------------------------------------------------------------------
void EPWM1_INT_ISR(void)			// PIE3.1 @ 0x000D60  EPWM1_INT (EPWM1)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void EPWM2_INT_ISR(void)			// PIE3.2 @ 0x000D62  EPWM2_INT (EPWM2)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void EPWM3_INT_ISR(void)			// PIE3.3 @ 0x000D64  EPWM3_INT (EPWM3)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);


}

//---------------------------------------------------------------------
void EPWM4_INT_ISR(void)			// PIE3.4 @ 0x000D66  EPWM4_INT (EPWM4)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void EPWM5_INT_ISR(void)			// PIE3.5 @ 0x000D68  EPWM5_INT (EPWM5)
{

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
	
	

}

//---------------------------------------------------------------------
void EPWM6_INT_ISR(void)			// PIE3.6 @ 0x000D6A  EPWM6_INT (EPWM6)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP3;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);


}

//---------------------------------------------------------------------
											// PIE3.7 @ 0x000D6C reserved
											// PIE3.8 @ 0x000D6E reserved

//---------------------------------------------------------------------
void ECAP1_INT_ISR(void)			// PIE4.1 @ 0x000D70  ECAP1_INT (ECAP1)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");						// Emulator Halt instruction
	while(1);

}

//---------------------------------------------------------------------
void ECAP2_INT_ISR(void)			// PIE4.2 @ 0x000D72  ECAP2_INT (ECAP2)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;	// Must acknowledge the PIE group

	if((ECap2Regs.CAP1 <= 3500000) && (ECap2Regs.CAP1 >= 2500000))
		VgridRFreqCnt =  ECap2Regs.CAP1 ; 	//eCAP计数

	VgridRZero_Cnt++;

	if(VgridRZero_Cnt >= 10)
		VgridRZero_Cnt = 0;

	ECap2Regs.ECCLR.bit.INT = 1;
	ECap2Regs.ECCLR.bit.CEVT1 = 1;
// Next two lines for debug only - remove after inserting your ISR
//	asm (" ESTOP0");						// Emulator Halt instruction
//	while(1);


}

//---------------------------------------------------------------------
void ECAP3_INT_ISR(void)			// PIE4.3 @ 0x000D74  ECAP3_INT (ECAP3)
{
    PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;	// Must acknowledge the PIE group
	
	if((ECap3Regs.CAP1 <= 3500000) && (ECap3Regs.CAP1 >= 2500000))
		VgridSFreqCnt =  ECap3Regs.CAP1 ; 	//eCAP计数

	VgridSZero_Cnt++;

	if(VgridSZero_Cnt >= 10)
		VgridSZero_Cnt = 0;

	ECap3Regs.ECCLR.bit.INT = 1;
	ECap3Regs.ECCLR.bit.CEVT1 = 1;
}

//---------------------------------------------------------------------
void ECAP4_INT_ISR(void)			// PIE4.4 @ 0x000D76  ECAP4_INT (ECAP4)
{ 
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;	// Must acknowledge the PIE group

	if((ECap4Regs.CAP1 <= 3500000) && (ECap4Regs.CAP1 >= 2500000))	//滤除严重偏离50Hz的计数值
		VgridTFreqCnt = ECap4Regs.CAP1;

	VgridTZero_Cnt++;

	if(VgridTZero_Cnt >= 10)
		VgridTZero_Cnt = 0;

	ECap4Regs.ECCLR.bit.INT = 1;			// Clear the ECAP1 interrupt flag
	ECap4Regs.ECCLR.bit.CEVT1 = 1;			// Clear the CEVT4 flag
}

//---------------------------------------------------------------------
void ECAP5_INT_ISR(void)					// PIE4.5 @ 0x000D78  ECAP5_INT (ECAP5)
{
//  static  Uint32 PWM_SYN_TIME = 0;
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;	// Must acknowledge the PIE group	

//	PWM_SYN_TIME = ECap5Regs.CAP1;//150000000.0/(float32)ECap6Regs.CAP1*100.0;
	if(EPwm1Regs.TBSTS.bit.CTRDIR == 1)
	{
	    if(EPwm1Regs.TBCTR > 2400)	
			EPwm1Regs.TBPRD = PWM_HALF_PERIOD + 10;
		else if(EPwm1Regs.TBCTR <2200 )	
			EPwm1Regs.TBPRD = PWM_HALF_PERIOD + 10;
		else 
		    EPwm1Regs.TBPRD = EPwm1Regs.TBPRD;
	}

	if(EPwm1Regs.TBSTS.bit.CTRDIR == 0)
		EPwm1Regs.TBPRD = PWM_HALF_PERIOD - 10;

	EPwm2Regs.TBPRD = EPwm1Regs.TBPRD;
	EPwm3Regs.TBPRD = EPwm1Regs.TBPRD;

	ECap5Regs.ECCLR.bit.INT = 1;			// Clear the ECAP1 interrupt flag
	ECap5Regs.ECCLR.bit.CEVT1 = 1;			// Clear the CEVT4 flag
}

//---------------------------------------------------------------------
void ECAP6_INT_ISR(void)			// PIE4.6 @ 0x000D7A  ECAP6_INT (ECAP6)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP4;	// Must acknowledge the PIE group
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);

}

//---------------------------------------------------------------------
											// PIE4.7 @ 0x000D7C reserved
											// PIE4.8 @ 0x000D7E reserved

//---------------------------------------------------------------------
void EQEP1_INT_ISR(void)			// PIE5.1 @ 0x000D80  EQEP1_INT (EQEP1)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP5;	// Must acknowledge the PIE group
 
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void EQEP2_INT_ISR(void)			// PIE5.2 @ 0x000D82  EQEP2_INT (EQEP2)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP5;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
												// PIE5.3 @ 0x000D84 reserved
												// PIE5.4 @ 0x000D86 reserved
												// PIE5.5 @ 0x000D88 reserved
												// PIE5.6 @ 0x000D8A reserved
												// PIE5.7 @ 0x000D8C reserved
												// PIE5.8 @ 0x000D8E reserved

//---------------------------------------------------------------------
void SPIRXINTA_ISR(void)			// PIE6.1 @ 0x000D90  SPIRXINTA (SPI-A)
{ // Not used Ken
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;	// The PIE send this interrupt request to the CPU

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void SPITXINTA_ISR(void)			// PIE6.2 @ 0x000D92  SPITXINTA (SPI-A)
{ //May use SWI_post(&SWI_SPIRXINT) to enable LCD drive
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;	// The PIE send this interrupt request to the CPU

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void MRINTB_ISR(void)				// PIE6.3 @ 0x000D94  MRINTB (McBSP-B)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void MXINTB_ISR(void)				// PIE6.4 @ 0x000D96  MXINTB (McBSP-B)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void MRINTA_ISR(void)				// PIE6.5 @ 0x000D98  MRINTA (McBSP-A)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void MXINTA_ISR(void)				// PIE6.6 @ 0x000D9A  MXINTA (McBSP-A)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP6;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
											// PIE6.7 @ 0x000D9C reserved
											// PIE6.8 @ 0x000D9E reserved

//---------------------------------------------------------------------
void DINTCH1_ISR(void)					// PIE7.1 @ 0x000DA0  DINTCH1 (DMA)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void DINTCH2_ISR(void)					// PIE7.2 @ 0x000DA2  DINTCH2 (DMA)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void DINTCH3_ISR(void)				// PIE7.3 @ 0x000DA4  DINTCH3 (DMA)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void DINTCH4_ISR(void)				// PIE7.4 @ 0x000DA6  DINTCH4 (DMA)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void DINTCH5_ISR(void)				// PIE7.5 @ 0x000DA8  DINTCH5 (DMA)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void DINTCH6_ISR(void)					// PIE7.6 @ 0x000DAA  DINTCH6 (DMA)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP7;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
												// PIE7.7 @ 0x000DAC reserved
												// PIE7.8 @ 0x000DAE reserved

//---------------------------------------------------------------------
void I2CINT1A_ISR(void)				// PIE8.1 @ 0x000DB0  I2CINT1A (I2C-A)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;	// Must acknowledge the PIE group
	
	//asm (" ESTOP0");							// Emulator Halt instruction
	//while(1);
	SWI_post(&SWI_I2C_isr);
}

//---------------------------------------------------------------------
void I2CINT2A_ISR(void)				// PIE8.2 @ 0x000DB2  I2CINT2A (I2C-A)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
											// PIE8.3 @ 0x000DB4 reserved
											// PIE8.4 @ 0x000DB6 reserved

//---------------------------------------------------------------------
void SCIRXINTC_ISR(void)			// PIE8.5 @ 0x000DB8  SCIRXINTC (SCI-C)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;		// The PIE send this interrupt request to the CPU

	PieCtrlRegs.PIEIER8.bit.INTx5 = 0; // Disable INT for a while
	SWI_post(&SWI_SCICRXINT);

// Next two lines for debug only - remove after inserting your ISR
//	asm (" ESTOP0");							// Emulator Halt instruction
//	while(1);
}

//---------------------------------------------------------------------
void SCITXINTC_ISR(void)			// PIE8.6 @ 0x000DBA  SCIRXINTC (SCI-C)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP8;	// The PIE send this interrupt request to the CPU

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
											// PIE8.7 @ 0x000DBC reserved
											// PIE8.8 @ 0x000DBE reserved

//---------------------------------------------------------------------
void SCIRXINTA_ISR(void)			// PIE9.1 @ 0x000DC0  SCIRXINTA (SCI-A)
{

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;	// The PIE send this interrupt request to the CPU

//	PieCtrlRegs.PIEIER9.bit.INTx1 = 0;     // Disable INT for a while
//	SWI_post(&SWI_SCIARXINT);

	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);

   // SWI_post(&SWI_SCIARXINT);

}

//---------------------------------------------------------------------
void SCITXINTA_ISR(void)			// PIE9.2 @ 0x000DC2  SCITXINTA (SCI-A)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;	// The PIE send this interrupt request to the CPU

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void SCIRXINTB_ISR(void)			// PIE9.3 @ 0x000DC4  SCIRXINTB (SCI-B)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;		// The PIE send this interrupt request to the CPU

//	PieCtrlRegs.PIEIER9.bit.INTx3 = 0;  // Disable INT for a while
//	SWI_post(&SWI_SCIBRXINT);
// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void SCITXINTB_ISR(void)			// PIE9.4 @ 0x000DC6  SCITXINTB (SCI-B)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;	// The PIE send this interrupt request to the CPU

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void ECAN0INTA_ISR(void)			// PIE9.5 @ 0x000DC8  ECAN0_INTA (ECAN-A)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;	// Must acknowledge the PIE group

//	static Uint16 Rec_l = 0;
//	static Uint16 Rec_h = 0;

	//*************PART II**************************
	//the next part focus on acceptting messages
	//Step 1, config local acceptting mask register
	//enable local acceptting mask bit
	ECanaMboxes.MBOX0.MSGID.bit.AME=1;
	//Step 2, initialize mailboxes
		
	//Step 3, wait for response
	while(ECanaRegs.CANRMP.all!=0x00000001);
	ECanaRegs.CANRMP.all=0x00000001;	
	//Step 4, CPU read from mailboxes
//	Rec_l = ECanaMboxes.MBOX0.MDL.all;
//	Rec_h = ECanaMboxes.MBOX0.MDH.all;

//	ECanaMboxes.MBOX0.MDL.all = Rec_l;
//    ECanaMboxes.MBOX0.MDL.all = Rec_h;
	    
	ECanaRegs.CANGIF0.all = 0xFFFFFFFF; /* Clear all interrupt flag bits */
    ECanaRegs.CANGIF1.all = 0xFFFFFFFF;
}

//---------------------------------------------------------------------
void ECAN1INTA_ISR(void)			// PIE9.6 @ 0x000DCA  ECAN1_INTA (ECAN-A)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void ECAN0INTB_ISR(void)			// PIE9.7 @ 0x000DCC  ECAN0_INTB (ECAN-B)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
void ECAN1INTB_ISR(void)			// PIE9.8 @ 0x000DCE  ECAN1_INTB (ECAN-B)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;	// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}

//---------------------------------------------------------------------
											// PIE10.1 @ 0x000DD0 reserved
											// PIE10.2 @ 0x000DD2 reserved
											// PIE10.3 @ 0x000DD4 reserved
											// PIE10.4 @ 0x000DD6 reserved
											// PIE10.5 @ 0x000DD8 reserved
											// PIE10.6 @ 0x000DDA reserved
											// PIE10.7 @ 0x000DDC reserved
											// PIE10.8 @ 0x000DDE reserved

//---------------------------------------------------------------------
											// PIE11.1 @ 0x000DE0 reserved
											// PIE11.2 @ 0x000DE2 reserved
											// PIE11.3 @ 0x000DE4 reserved
											// PIE11.4 @ 0x000DE6 reserved
											// PIE11.5 @ 0x000DE8 reserved
											// PIE11.6 @ 0x000DEA reserved
											// PIE11.7 @ 0x000DEC reserved
											// PIE11.8 @ 0x000DEE reserved

//---------------------------------------------------------------------
void XINT3_ISR(void)				// PIE12.1 @ 0x000DF0  XINT3
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}     

//---------------------------------------------------------------------
void XINT4_ISR(void)				// PIE12.2 @ 0x000DF2  XINT4
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}     

//---------------------------------------------------------------------
void XINT5_ISR(void)				// PIE12.3 @ 0x000DF4  XINT5
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}     

//---------------------------------------------------------------------
void XINT6_ISR(void)				// PIE12.4 @ 0x000DF6  XINT6
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}     

//---------------------------------------------------------------------
void XINT7_ISR(void)				// PIE12.5 @ 0x000DF8  XINT7
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}     
//---------------------------------------------------------------------
											// PIE12.6 @ 0x000DFA reserved

//---------------------------------------------------------------------
void LVF_ISR(void)					// PIE12.7 @ 0x000DFC  LVF (FPU)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}     

//---------------------------------------------------------------------
void LUF_ISR(void)					// PIE12.8 @ 0x000DFE  LUF (FPU)
{
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP12;// Must acknowledge the PIE group

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}     

//---------------------------------------------------------------------
void rsvd_ISR(void)								// Reserved PIE vectors
{
// This ISR is for reserved PIE vectors.  It should never be reached by
// properly executing code.  If you get here, it means something is wrong.

// Next two lines for debug only - remove after inserting your ISR
	asm (" ESTOP0");							// Emulator Halt instruction
	while(1);
}


//--- end of file -----------------------------------------------------
