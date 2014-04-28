//###########################################################################
//
// File Name:  isr.c
// File Discription: 中断服务程序
//###########################################################################
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  3.1| 3 July 2008 | X.C | ZJU
//###########################################################################
#include "DSP2833x_Device.h"     // DSP2833x Headerfile Include File
#include "500kVar_SVG.h"   // DSP2833x Examples Include File



#pragma CODE_SECTION(PWM_AD_isr, "ramfuncs");
#pragma CODE_SECTION(Skiip4_CAN_isr,"ramfuncs");
//#pragma DATA_SECTION(Graphviewer1,"DMARAML67");
//#pragma DATA_SECTION(Graphviewer2,"DMARAML67");
//#pragma DATA_SECTION(Graphviewer3,"DMARAML67");
//#pragma DATA_SECTION(AscalaMbuf,"DMARAML67");
//#pragma DATA_SECTION(BscalaMbuf,"DMARAML67");
//#pragma DATA_SECTION(CscalaMbuf,"DMARAML67");

extern Uint8 Skiip4_1[46] = {0,0};          //Skiip4_1模块CAN数据保存寄存器
extern Uint8 Skiip4_2[46] = {0,0};          //Skiip4_2模块CAN数据保存寄存器
extern Uint8 Skiip4_3[46] = {0,0};          //Skiip4_3模块CAN数据保存寄存器


//#pragma DATA_SECTION(Recordwave,"DMARAML67");   //数据保存


interrupt void PWM_AD_isr(void) 
{
	DINT;

	Skiip4_CanX_Send ();

				
	PieCtrlRegs.PIEACK.all = PIEACK_GROUP1;	 // Must acknowledge the PIE group

	AdcRegs.ADCTRL2.bit.RST_SEQ1 = 1;	// Reset cascaded SEQ to CONV00 state
	AdcRegs.ADCST.bit.INT_SEQ1_CLR = 1;	// Clear ADC SEQ1 interrupt flag

	EINT;
}

interrupt void Skiip4_CAN_isr(void) 
{
	Uint32 Skiip4_1_Low = 0;
	Uint32 Skiip4_1_High = 0;
	Uint32 Skiip4_2_Low = 0;
	Uint32 Skiip4_2_High = 0;
	Uint32 Skiip4_3_Low = 0;
	Uint32 Skiip4_3_High = 0;
	
	Skiip4_1_Low = ECanaMboxes.MBOX0.MDL.all;
	Skiip4_1_High = ECanaMboxes.MBOX0.MDH.all;
	Skiip4_CAN1_Receive(Skiip4_1_High,Skiip4_1_Low);

	Skiip4_2_Low = ECanaMboxes.MBOX1.MDL.all;
	Skiip4_2_High = ECanaMboxes.MBOX1.MDH.all;
	Skiip4_CAN2_Receive(Skiip4_2_High,Skiip4_2_Low);

	Skiip4_3_Low = ECanaMboxes.MBOX2.MDL.all;
	Skiip4_3_High = ECanaMboxes.MBOX2.MDH.all;
	Skiip4_CAN2_Receive(Skiip4_3_High,Skiip4_3_Low);

	PieCtrlRegs.PIEACK.all = PIEACK_GROUP9;	// Must acknowledge the PIE group
	ECanaRegs.CANGIF0.all = 0xFFFFFFFF; /* Clear all interrupt flag bits */
    ECanaRegs.CANGIF1.all = 0xFFFFFFFF;

	EINT;
}


