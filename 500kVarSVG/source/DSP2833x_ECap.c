/*=============================================================================*
 *         Copyright(c) 2009-2011, Convertergy Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  FILENAME : ECap.c 
 *
 *  PURPOSE  : Capture falling/rising edge for Grid_Frequecy calculation and 
 *  		   phase sequency detection.
 *  
 *  HISTORY  :
 *    DATE            VERSION        AUTHOR            NOTE
 *    2011-02-26      V0.1           Ken      	    Created
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


#include "DSP2833x_Device.h"	      // Peripheral address definitions
#include "500kVar_SVG.h"			// Main include file



/*=============================================================================*
 * 	Local Variables declaration
 *============================================================================*/
/* Detect and check phase sequency */
//int32 g_i32PhaseSequencyTemp_Vvw = 0;
//int32 g_i32PhaseSequencyTemp_Vwu = 0; 


/* Detect period of one Grid frequency */
//int32 i32GridFrequencyPriodTemp = 0;
	


/*=============================================================================*
 * FUNCTION: InitECap()
 * PURPOSE : eCAP hardware module initialization.
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
void InitECap(void)
{ 

	//eCAP2 for test R frequency
	ECap2Regs.ECEINT.all = 0x0000;		// Disable all eCAP interrupts
	ECap2Regs.ECCLR.all = 0xFFFF;       // Clear all CAP interrupt flags
	ECap2Regs.ECCTL1.bit.CAPLDEN = 0;	// Disabled loading of capture results
	ECap2Regs.ECCTL2.bit.TSCTRSTOP = 0;	// Stop the counter

	ECap2Regs.ECCTL2.bit.CONT_ONESHT =0;
	ECap2Regs.ECCTL2.bit.STOP_WRAP = 0;
	ECap2Regs.ECCTL1.bit.CAP1POL = 0;
	ECap2Regs.ECCTL1.bit.CAP2POL = 0;
	ECap2Regs.ECCTL1.bit.CAP3POL = 0;
	ECap2Regs.ECCTL1.bit.CAP4POL = 0;
	ECap2Regs.ECCTL1.bit.CTRRST1 = 1;
	ECap2Regs.ECCTL1.bit.CTRRST2 = 1;
	ECap2Regs.ECCTL1.bit.CTRRST3 = 1;
	ECap2Regs.ECCTL1.bit.CTRRST4 = 1;
	ECap2Regs.ECCTL2.bit.SYNCI_EN = 0;
	ECap2Regs.ECCTL2.bit.SYNCO_SEL = 2;
	ECap2Regs.ECCTL1.bit.CAPLDEN = 1;

	ECap2Regs.ECCTL1.bit.PRESCALE = 0;

	ECap2Regs.ECCTL2.bit.TSCTRSTOP = 1;
	ECap2Regs.ECCTL2.bit.REARM = 1;
	ECap2Regs.ECCTL1.bit.CAPLDEN = 1;
	ECap2Regs.ECEINT.bit.CEVT1 = 1;

	PieCtrlRegs.PIEIER4.bit.INTx2 = 1;	// Enable ECAP1_INT in PIE group 4

	//eCAP3 for test S frequency
	ECap3Regs.ECEINT.all = 0x0000;		// Disable all eCAP interrupts
	ECap3Regs.ECCLR.all = 0xFFFF;       // Clear all CAP interrupt flags
	ECap3Regs.ECCTL1.bit.CAPLDEN = 0;	// Disabled loading of capture results
	ECap3Regs.ECCTL2.bit.TSCTRSTOP = 0;	// Stop the counter

	ECap3Regs.ECCTL2.bit.CONT_ONESHT =0;
	ECap3Regs.ECCTL2.bit.STOP_WRAP = 0;
	ECap3Regs.ECCTL1.bit.CAP1POL = 0;
	ECap3Regs.ECCTL1.bit.CAP2POL = 0;
	ECap3Regs.ECCTL1.bit.CAP3POL = 0;
	ECap3Regs.ECCTL1.bit.CAP4POL = 0;
	ECap3Regs.ECCTL1.bit.CTRRST1 = 1;
	ECap3Regs.ECCTL1.bit.CTRRST2 = 1;
	ECap3Regs.ECCTL1.bit.CTRRST3 = 1;
	ECap3Regs.ECCTL1.bit.CTRRST4 = 1;
	ECap3Regs.ECCTL2.bit.SYNCI_EN = 0;
	ECap3Regs.ECCTL2.bit.SYNCO_SEL = 2;
	ECap3Regs.ECCTL1.bit.CAPLDEN = 1;

	ECap3Regs.ECCTL1.bit.PRESCALE = 0;

	ECap3Regs.ECCTL2.bit.TSCTRSTOP = 1;
	ECap3Regs.ECCTL2.bit.REARM = 1;
	ECap3Regs.ECCTL1.bit.CAPLDEN = 1;
	ECap3Regs.ECEINT.bit.CEVT1 = 1;

	PieCtrlRegs.PIEIER4.bit.INTx3 = 1;	// Enable ECAP1_INT in PIE group 4


//  ecap4
	ECap4Regs.ECEINT.all = 0x0000;		// Disable all eCAP interrupts
	ECap4Regs.ECCLR.all = 0xFFFF;       // Clear all CAP interrupt flags
	ECap4Regs.ECCTL1.bit.CAPLDEN = 0;	// Disabled loading of capture results
	ECap4Regs.ECCTL2.bit.TSCTRSTOP = 0;	// Stop the counter

	ECap4Regs.ECCTL2.bit.CONT_ONESHT =0;
	ECap4Regs.ECCTL2.bit.STOP_WRAP = 0;
	ECap4Regs.ECCTL1.bit.CAP1POL = 0;
	ECap4Regs.ECCTL1.bit.CAP2POL = 0;
	ECap4Regs.ECCTL1.bit.CAP3POL = 0;
	ECap4Regs.ECCTL1.bit.CAP4POL = 0;
	ECap4Regs.ECCTL1.bit.CTRRST1 = 1;
	ECap4Regs.ECCTL1.bit.CTRRST2 = 1;
	ECap4Regs.ECCTL1.bit.CTRRST3 = 1;
	ECap4Regs.ECCTL1.bit.CTRRST4 = 1;
	ECap4Regs.ECCTL2.bit.SYNCI_EN = 0;
	ECap4Regs.ECCTL2.bit.SYNCO_SEL = 2;
	ECap4Regs.ECCTL1.bit.CAPLDEN = 1;

	ECap4Regs.ECCTL1.bit.PRESCALE = 0;

	ECap4Regs.ECCTL2.bit.TSCTRSTOP = 1;
	ECap4Regs.ECCTL2.bit.REARM = 1;
	ECap4Regs.ECCTL1.bit.CAPLDEN = 1;
	ECap4Regs.ECEINT.bit.CEVT1 = 1;

	PieCtrlRegs.PIEIER4.bit.INTx4 = 1;	// Enable ECAP1_INT in PIE group 4

	//eCAP5 for PWM Í¬²½
	ECap5Regs.ECEINT.all = 0x0000;		// Disable all eCAP interrupts
	ECap5Regs.ECCLR.all = 0xFFFF;       // Clear all CAP interrupt flags
	ECap5Regs.ECCTL1.bit.CAPLDEN = 0;	// Disabled loading of capture results
	ECap5Regs.ECCTL2.bit.TSCTRSTOP = 0;	// Stop the counter

	ECap5Regs.ECCTL2.bit.CONT_ONESHT =0;
	ECap5Regs.ECCTL2.bit.STOP_WRAP = 0;
	ECap5Regs.ECCTL1.bit.CAP1POL = 0;
	ECap5Regs.ECCTL1.bit.CAP2POL = 0;
	ECap5Regs.ECCTL1.bit.CAP3POL = 0;
	ECap5Regs.ECCTL1.bit.CAP4POL = 0;
	ECap5Regs.ECCTL1.bit.CTRRST1 = 1;
	ECap5Regs.ECCTL1.bit.CTRRST2 = 1;
	ECap5Regs.ECCTL1.bit.CTRRST3 = 1;
	ECap5Regs.ECCTL1.bit.CTRRST4 = 1;
	ECap5Regs.ECCTL2.bit.SYNCI_EN = 0;
	ECap5Regs.ECCTL2.bit.SYNCO_SEL = 2;
	ECap5Regs.ECCTL1.bit.CAPLDEN = 1;

	ECap5Regs.ECCTL1.bit.PRESCALE = 0;

	ECap5Regs.ECCTL2.bit.TSCTRSTOP = 1;
	ECap5Regs.ECCTL2.bit.REARM = 1;
	ECap5Regs.ECCTL1.bit.CAPLDEN = 1;
	ECap5Regs.ECEINT.bit.CEVT1 = 1;

	PieCtrlRegs.PIEIER4.bit.INTx5 = 1;	// Enable ECAP1_INT in PIE group 4

//	IER= M_INT4;

} // end of InitECap()


/*=============================================================================*
 * FUNCTION: DetectPhaseSequency()
 * PURPOSE : phase sequency detection by eCAP2/3
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
 *    void ECAP1_INT_ISR(void)  
 * 
 *============================================================================*/
void DetectPhaseSequency(void)
{ // start of DetectPhaseSequency
	
	ECap2Regs.TSCTR = 0;					// Clear the counter
	ECap2Regs.CTRPHS = 0;					// Clear the counter phase register
   	ECap2Regs.ECCTL2.bit.TSCTRSTOP = 1;     // Start Counter
   	ECap2Regs.ECCTL2.bit.REARM = 1;         // arm one-shot
	ECap2Regs.ECCTL1.bit.CAPLDEN = 1;       // Enable capture units

	ECap3Regs.TSCTR = 0;					// Clear the counter
	ECap3Regs.CTRPHS = 0;					// Clear the counter phase register
   	ECap3Regs.ECCTL2.bit.TSCTRSTOP = 1;     // Start Counter
   	ECap3Regs.ECCTL2.bit.REARM = 1;         // arm one-shot
	ECap3Regs.ECCTL1.bit.CAPLDEN = 1;       // Enable capture units

} // end of DetectPhaseSequency


/*=============================================================================*
 * FUNCTION: CheckPhaseSequency()
 * PURPOSE : check and decide if the phase sequency is normal or not
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
 *    void ECAP1_INT_ISR(void)  
 * 
 *============================================================================*/
/*void CheckPhaseSequency(void)
{ // start of CheckPhaseSequency
    static Uint16 s_u16PhaseSequCheckWindow = 0;
    static Uint16 s_u16PhaseSequCheckFaultWindow = 0;

        if ( g_i32PhaseSequencyTemp_Vvw > 0 )
	{	
		if ( g_i32PhaseSequencyTemp_Vvw < g_i32PhaseSequencyTemp_Vwu ) 
		{
		       s_u16PhaseSequCheckFaultWindow = 0;    
			if ( s_u16PhaseSequCheckWindow < 10 )	// 200ms phase sequence detection 
			{
			       s_u16PhaseSequCheckWindow++;
			}
			else
			{ 
				s_u16PhaseSequCheckWindow = 0;
				g_SysFaultMessage.bit.GridPhaseSeque = 0;	 //indicate normal phase sequence
				g_StateCheck.bit.PhaseSeqCheckOver = 1;    
			}

		}
		else
		{     
		        s_u16PhaseSequCheckWindow =0;	
		        s_u16PhaseSequCheckFaultWindow ++;
                     if(s_u16PhaseSequCheckFaultWindow >=10 )
                     {
                        s_u16PhaseSequCheckFaultWindow = 0;           
                        //indicate phase sequence fault or phase broken
                        g_SysFaultMessage.bit.GridPhaseSeque = 1;	 
                     }
                     
			u16PhaseSequCheckWindow =0;	
			//indicate phase sequence fault or phase broken
			g_SysFaultMessage.bit.GridPhaseSeque = 1;	  
		}
	}


} // end of CheckPhaseSequency

*/




//--- end of file -----------------------------------------------------
