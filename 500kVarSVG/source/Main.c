/*=============================================================================*
 *         Copyright(c) 2009-2011, Convertergy Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  FILENAME : 5KW_main.c 
 *   
 *  PURPOSE  : This document  include  initializtion of CPU system , ad sampling, three phase pwm output,Peripheral Interrupt Expansion equal,
 *                        system default parameter  defined , state change funtion,sofeware period  interrupt.
 *  
 *  HISTORY  :
 *    DATE            VERSION        AUTHOR            NOTE
 *    2011-02-19      V0.1           Ken      	    Created
 *
 * 	Software Development Evironment: 
 *	-- CCS 3.3.81.6
 * 	-- BIOS 5.373.06
 *	-- Code Generation Tool v5.2.4 
 *    
 *============================================================================*/


//Main including header files
#include "DSP2833x_Device.h"			       // Peripheral address definitions
#include "500kVar_SVG.h"					// Main include file

extern Uint16 RamfuncsLoadStart;
extern Uint16 RamfuncsLoadEnd;
extern Uint16 RamfuncsRunStart;
extern Uint16 IQmathLoadStart;
extern Uint16 IQmathLoadEnd;
extern Uint16 IQmathRunStart;

interrupt void PWM_AD_isr(void);       //PWM触发的AD中断申明
interrupt void Skiip4_CAN_isr(void);   //Can-A模块中断申明


void main(void)
{
	InitSysCtrl();
// Copy time critical code and Flash setup code to RAM
// This includes the following ISR functions: epwm1_timer_isr(), epwm2_timer_isr()
// epwm3_timer_isr and and InitFlash();
// The  RamfuncsLoadStart, RamfuncsLoadEnd, and RamfuncsRunStart
// symbols are created by the linker. Refer to the F28335.cmd file.

// Step 3. Clear all interrupts and initialize PIE vector table:
// Disable CPU interrupts
	DINT;
// Initialize the PIE control registers to their default state.
// The default state is all PIE interrupts disabled and flags
// are cleared.
// This function is found in the DSP2833x_PieCtrl.c file.
	InitPieCtrl();
// Disable CPU interrupts and clear all CPU interrupt flags:
	IER = 0x0000;
	IFR = 0x0000;
// Initialize the PIE vector table with pointers to the shell Interrupt
// Service Routines (ISR).
// This will populate the entire table, even if the interrupt
// is not used in this example.  This is useful for debug purposes.
// The shell ISR routines are found in DSP2833x_DefaultIsr.c.
// This function is found in DSP2833x_PieVect.c.
	InitPieVectTable();
// Interrupts that are used in this example are re-mapped to
// ISR functions found within this file.
	EALLOW;  // This is needed to write to EALLOW protected registers
	PieVectTable.SEQ1INT = &PWM_AD_isr;
	PieVectTable.ECAN0INTA = &Skiip4_CAN_isr;
	EDIS;     // This is needed to disable write to EALLOW protected registers 
	
	InitFlash();
	MemCopy(&RamfuncsLoadStart, &RamfuncsLoadEnd, &RamfuncsRunStart);
	MemCopy(&IQmathLoadStart, &IQmathLoadEnd, &IQmathRunStart);

	//ECan-A模块初始化
	InitECan();
	InitECan1();

	InitGpio();  // Skipped for this example 
	InitAdc();							// Initialize necessary ADC module, directly for SVPWM.
	InitEPwm();
  



	IER |= M_INT1; 
// Enable SEQ1_INT which is connected to PIE1.1:
    IER |= M_INT9;
// Enable eCAN0-A INT which is connected to PIE9.5:


	EINT;
} // end of main()




