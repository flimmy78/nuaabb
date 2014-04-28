/*=============================================================================*
 *         Copyright(c) 2009-2011, ALL RIGHTS RESERVED
 *
 *  FILENAME : EPwm.c 
 *
 *  PURPOSE  : ePWM Module configuration and Timer/PWMoutput En/Disable process..
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
 
#include "DSP2833x_Device.h"		// Peripheral address definitions
#include "500kVar_SVG.h"				// Main include file


/*=============================================================================*
 * FUNCTION: InitEPwm()
 * PURPOSE : Initializes the Enhanced PWM modules.
 *			 To precisely capture the most valuable sampling point L current 
 *			 with high frequency sampling for Energy, additional Timer5 and 
 *			 Timer6 are used to supply additional leading-type SOC and equal 
 *			 interval sampling, with :
 *			 TBCLK=SYSCLKOUT/(HSPCLKDIV*CLKDIV)=150MHz/(2*1)=75MHz
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
void InitEPwm(void)
{ // start of InitEPwm()

	/* Must disable the clock to the ePWM modules if you want all ePMW modules 
	   synchronized, according to : TMS320x2833x, 2823x System Control and 
	   Interrupts Reference Guide (Rev. C).pdf. */

	asm(" EALLOW");					// Enable EALLOW protected register access
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 0;
	asm(" EDIS");					// Disable EALLOW protected register access

	// Configure ePWM1/2/3/4/5/6 for symmetric PWM

	// ePWM1 Configuration 
	EPwm1Regs.TBCTL.bit.CTRMODE = 0x2;		// up-down mode
	EPwm1Regs.TBPRD = PWM_HALF_PERIOD;		// actual period = 2*PWM_HALF_PERIOD
	EPwm1Regs.TBPHS.half.TBPHS = 0x0;		// Set timer phase, zero
	EPwm1Regs.TBCTL.bit.PHSEN = 0x0;		// Master module
	EPwm1Regs.TBCTL.bit.PRDLD = 0x0;		// reload from shadow
	EPwm1Regs.TBCTL.bit.SYNCOSEL = 0x1; 	// sync enable by Yu
	EPwm1Regs.TBCTL.bit.SWFSYNC = 0x0;
	EPwm1Regs.TBCTL.bit.HSPCLKDIV = 0x1;	// prescaler = 2
	EPwm1Regs.TBCTL.bit.CLKDIV = 0x0;
	EPwm1Regs.TBCTL.bit.PHSDIR = 0x0;		// don't care
	EPwm1Regs.TBCTL.bit.FREE_SOFT = 0x0;	
	//bit15-14     11:     	FREE/SOFT, 11 = ignore emulation suspend
	//bit13        0:      	PHSDIR, 0 = count down after sync event, 
							//1 = count up after sync event
	//bit12-10     000:    	CLKDIV, 000 = 2^0, 001 = 2^1, ..., 111 = 2^7, 
							//TBCLK = SYSCLKOUT/(HSPCLKDIV*CLKDIV)
	//bit9-7       001:    	HSPCLKDIV, Prescaler, 000 = 1, 001 = 2, 
							//010 = 4, ..., 110 = 12, 111 = 14.
	//bit6         0:      	SWFSYNC, 0 = no software sync produced, 
							//1 = writing a 1 forces a one-time sync pulse
	//bit5-4       01:     	SYNCOSEL, sync-output-select, 00 = EPWMxSYNC, 
							//01 =>CTR=0, 10 =>CTR=CMPB, 11 = sync-out disabled
	//bit3         0:      	PRDLD, 0 = reload PRD on counter=0 from shadow, 
							//1 = without shadow
	//bit2         0:      	PHSEN, 0 = phase control disabled, master mode, 
							//1 = slave mode 
	//bit1-0       11:     	CTRMODE, Counter Mode, 00 = Up, 01 = Down, 
							//10 = Up-Down, 11 = Stop timer stopped (disabled)

	EPwm1Regs.CMPCTL.all = 0x000A;			// Compare control register
	//bit15-10	0:    	reserved
	//bit9		0:      SHDWBFULL, read-only
	//bit8		0:      SHDWAFULL, read-only
	//bit7		0:      reserved
	//bit6		0:      SHDWBMODE, 0 = shadow mode, 1= immediate
	//bit5		0:      reserved
	//bit4		0:      SHDWAMODE, 0 = shadow mode, 1= immediate
	//bit3-2	10:     LOADBMODE, 00 => CTR=0; 01 =>CTR=PRD, 
						//10 => CTR=0 or CTR=PRD, 11, disable
	//bit1-0	10:     LOADAMODE, 00 => CTR=0; 01 =>CTR=PRD, 
						//10 => CTR=0 or CTR=PRD, 11, disable

	// config EPWM1A output when(1)incrementing and (2)equals CMPA
	EPwm1Regs.AQCTLA.bit.CAU = 0x2;	                                    	
	// config EPWM1A output when(1)decrementing and (2)equals CMPA
	EPwm1Regs.AQCTLA.bit.CAD = 0x1;                                     		
	//Action-qualifier control register A	
	//bit15-12     0000:   reserved
	//bit11-10     00:     CBD, 00 = do nothing
	//bit9-8       00:     CBU, 00 = do nothing
	//bit7-6       10:     CAD, 00 = disable, 01, clear, 10 = set, 11 = toggle
	//bit5-4       01:     CAU, 00 = disable, 01, clear, 10 = set, 11 = toggle
	//bit3-2       00:     PRD, 00 = do nothing
	//bit1-0       00:     ZRO, 00 = do nothing
	// force AQC output low
	// config EPWM1B output when(1)incrementing and (2)equals CMPA
	//EPwm1Regs.AQCTLB.bit.CAU = 0x1;	                                     
	// config EPWM1A output when(1)decrementing and (2)equals CMPA
	//EPwm1Regs.AQCTLB.bit.CAD = 0x2;                                    		
	//Action-qualifier control register A	
	//bit15-12     0000:   reserved
	//bit11-10     00:     CBD, 00 = do nothing
	//bit9-8       00:     CBU, 00 = do nothing
	//bit7-6       01:     CAD, 00 = disable, 01, clear, 10 = set, 11 = toggle
	//bit5-4       10:     CAU, 00 = disable, 01, clear, 10 = set, 11 = toggle
	//bit3-2       00:     PRD, 00 = do nothing
	//bit1-0       00:     ZRO, 00 = do nothing
	// AQCSFRC load mode
	EPwm1Regs.AQSFRC.bit.RLDCSF = 0x3;			
	//00 Load on event counter equauls zero
	//01 Load on event counter equauls period
	//10 Load on event counter equauls zero or period
	//11 Load immediately
	
	EPwm1Regs.DBFED = DeadBand_Duration;		// Dead band duration for FED
	EPwm1Regs.DBRED = DeadBand_Duration;		// Dead band duration for RED
	
	EPwm1Regs.PCCTL.bit.CHPEN = 0;				// PWM chopper unit disabled

	// 0000 0001 0000 0000 Configure SOCA
	EPwm1Regs.ETPS.all = 0x1100;			
	//bit15-14	00:		EPWMxSOCB, read-only
	//bit13-12	00:		SOCBPRD, 00 = disable, 
						//01,10,11=gerneate EPWMxSOCB pulse on the 1/2/3 event.
	//bit11-10	00:	 	EPWMxSOCA, read-only
	//bit9-8	01:		SOCAPRD, 00 = disable, 
						//01,10,11=gerneate EPWMxSOCA pulse on the 1/2/3 event.
	//bit7-4	0000:	reserved
	//bit3-2	00:		INTCNT, read-only 
	//bit1-0	00:		INTPRD, 00 = disable, 
						//01,10,11=gerneate EPWMx_INT on the 1/2/3 event.

	// 1100 Enable SOCA to ADC, Enable INT and INTSEL.
	EPwm1Regs.ETSEL.all = 0xA900;			
	//bit15		0:		SOCBEN, 0 = disable SOCB
	//bit14-12	000:	SOCBSEL, don't care
	//bit11		1:		SOCAEN, 1 = enable SOCA, 0 = disable
	//bit10-8	001:	SOCASEL, EPWMxSOCA selection, 000 = reserved, 
						//001 => TBCTR=0, 010 => TBCTR=TBPRD, 011 = reserved
						//100 = CMPA inc, 101 = CMPA dec, 110 = CMPB inc, 
						//111 = CMPB dec.
	//bit7-4	0000:	reserved
	//bit3		0:		INTEN, EPWMx_INT, 0 = disable interrupt, 1 = enable
	//bit2-0	000:	INTSEL, 000 = Reserved, 001 -> TBCTR=0; 
						//010 -> TBCTR=TBPRD; 011 = Reserved; 100 = CMPA Up, 
						//101 = CMPA Down
						//110 = CMPB Up; 111 = CMPB down.


	

	// ePWM2 Configuration 
	EPwm2Regs.TBCTL.bit.CTRMODE = 0x2;		// up-down mode
	EPwm2Regs.TBPRD = PWM_HALF_PERIOD;		// actual period = 2*PWM_HALF_PERIOD
	EPwm2Regs.TBPHS.half.TBPHS = 0x0;		// Set timer phase, zero
	EPwm2Regs.TBCTL.bit.PHSEN = 0x1;		// Slave module
	EPwm2Regs.TBCTL.bit.PRDLD = 0x0;		// reload from shadow
	EPwm2Regs.TBCTL.bit.SYNCOSEL = 0x0; 	// EPWMxSYNC
	EPwm2Regs.TBCTL.bit.SWFSYNC = 0x0;
	EPwm2Regs.TBCTL.bit.HSPCLKDIV = 0x1;	// prescaler = 2
	EPwm2Regs.TBCTL.bit.CLKDIV = 0x0;
	EPwm2Regs.TBCTL.bit.PHSDIR = 0x0;		// don't care
	EPwm2Regs.TBCTL.bit.FREE_SOFT = 0x0;	
	//bit15-14     11:     	FREE/SOFT, 11 = ignore emulation suspend
	//bit13        0:      	PHSDIR, 0 = count down after sync event, 
							//1 = count up after sync event
	//bit12-10     000:    	CLKDIV, 000 = 2^0, 001 = 2^1, ..., 111 = 2^7, 
							//TBCLK = SYSCLKOUT/(HSPCLKDIV*CLKDIV)
	//bit9-7       001:    	HSPCLKDIV, Prescaler, 000 = 1, 001 = 2, 
							//010 = 4, ..., 110 = 12, 111 = 14.
	//bit6         0:      	SWFSYNC, 0 = no software sync produced, 
							//1 = writing a 1 forces a one-time sync pulse
	//bit5-4       00:     	SYNCOSEL, sync-output-select, 00 = EPWMxSYNC, 
							//01 =>CTR=0, 10 =>CTR=CMPB, 11 = sync-out disabled
	//bit3         0:      	PRDLD, 0 = reload PRD on counter=0 from shadow, 
							//1 = without shadow
	//bit2         0:      	PHSEN, 0 = phase control disabled, master mode, 
							//1 = slave mode 
	//bit1-0       11:     	CTRMODE, Counter Mode, 00 = Up, 01 = Down, 
							//10 = Up-Down, 11 = Stop timer stopped (disabled)

	EPwm2Regs.CMPCTL.all = 0x000A;			// Compare control register
	//bit15-10	0:    	reserved
	//bit9		0:      SHDWBFULL, read-only
	//bit8		0:      SHDWAFULL, read-only
	//bit7		0:      reserved
	//bit6		0:      SHDWBMODE, 0 = shadow mode, 1= immediate
	//bit5		0:      reserved
	//bit4		0:      SHDWAMODE, 0 = shadow mode, 1= immediate
	//bit3-2	10:     LOADBMODE, 00 => CTR=0; 01 =>CTR=PRD, 
						//10 => CTR=0 or CTR=PRD, 11, disable
	//bit1-0	10:     LOADAMODE, 00 => CTR=0; 01 =>CTR=PRD, 
						//10 => CTR=0 or CTR=PRD, 11, disable

	// config EPWM1A output when(1)incrementing and (2)equals CMPA
	EPwm2Regs.AQCTLA.bit.CAU = 0x2;                                   		
	// config EPWM1A output when(1)decrementing and (2)equals CMPA
	EPwm2Regs.AQCTLA.bit.CAD = 0x1;                                     		
	//Action-qualifier control register A	
	//bit15-12     0000:   reserved
	//bit11-10     00:     CBD, 00 = do nothing
	//bit9-8       00:     CBU, 00 = do nothing
	//bit7-6       01:     CAD, 00 = disable, 01, clear, 10 = set, 11 = toggle
	//bit5-4       10:     CAU, 00 = disable, 01, clear, 10 = set, 11 = toggle
	//bit3-2       00:     PRD, 00 = do nothing
	//bit1-0       00:     ZRO, 00 = do nothing
	// config EPWM1A output when(1)incrementing and (2)equals CMPA
//	EPwm2Regs.AQCTLB.bit.CBU = 0x2;			
	// config EPWM1A output when(1)decrementing and (2)equals CMPA
//	EPwm2Regs.AQCTLB.bit.CBD = 0x1;			
	//Action-qualifier control register A	
	//bit15-12     0000:   reserved
	//bit11-10     00:     CBD, 00 = do nothing
	//bit9-8       00:     CBU, 00 = do nothing
	//bit7-6       01:     CAD, 00 = disable, 01, clear, 10 = set, 11 = toggle
	//bit5-4       10:     CAU, 00 = disable, 01, clear, 10 = set, 11 = toggle
	//bit3-2       00:     PRD, 00 = do nothing
	//bit1-0       00:     ZRO, 00 = do nothing

	// AQCSFRC load mode
	EPwm2Regs.AQSFRC.bit.RLDCSF = 0x3;			
	//00 Load on event counter equauls zero
	//01 Load on event counter equauls period
	//10 Load on event counter equauls zero or period
	//11 Load immediately
	
	EPwm2Regs.DBFED = DeadBand_Duration;		// Dead band duration for FED
	EPwm2Regs.DBRED = DeadBand_Duration;		// Dead band duration for RED
	
	EPwm2Regs.PCCTL.bit.CHPEN = 0;				// PWM chopper unit disabled


	// ePWM3 Configuration 
	EPwm3Regs.TBCTL.bit.CTRMODE = 0x2;		// up-down mode
	EPwm3Regs.TBPRD = PWM_HALF_PERIOD;		// actual period = 2*PWM_HALF_PERIOD
	EPwm3Regs.TBPHS.half.TBPHS = 0x0;		// Set timer phase, zero
	EPwm3Regs.TBCTL.bit.PHSEN = 0x1;		// Slave module
	EPwm3Regs.TBCTL.bit.PRDLD = 0x0;		// reload from shadow
	EPwm3Regs.TBCTL.bit.SYNCOSEL = 0x0; 	// 
	EPwm3Regs.TBCTL.bit.SWFSYNC = 0x0;
	EPwm3Regs.TBCTL.bit.HSPCLKDIV = 0x1;	// prescaler = 2
	EPwm3Regs.TBCTL.bit.CLKDIV = 0x0;
	EPwm3Regs.TBCTL.bit.PHSDIR = 0x0;		// don't care
	EPwm3Regs.TBCTL.bit.FREE_SOFT = 0x0;	
	//bit15-14     11:     	FREE/SOFT, 11 = ignore emulation suspend
	//bit13        0:      	PHSDIR, 0 = count down after sync event, 
							//1 = count up after sync event
	//bit12-10     000:    	CLKDIV, 000 = 2^0, 001 = 2^1, ..., 111 = 2^7, 
							//TBCLK = SYSCLKOUT/(HSPCLKDIV*CLKDIV)
	//bit9-7       001:    	HSPCLKDIV, Prescaler, 000 = 1, 001 = 2, 
							//010 = 4, ..., 110 = 12, 111 = 14.
	//bit6         0:      	SWFSYNC, 0 = no software sync produced, 
							//1 = writing a 1 forces a one-time sync pulse
	//bit5-4       01:     	SYNCOSEL, sync-output-select, 00 = EPWMxSYNC, 
							//01 =>CTR=0, 10 =>CTR=CMPB, 11 = sync-out disabled
	//bit3         0:      	PRDLD, 0 = reload PRD on counter=0 from shadow, 
							//1 = without shadow
	//bit2         0:      	PHSEN, 0 = phase control disabled, master mode, 
							//1 = slave mode 
	//bit1-0       11:     	CTRMODE, Counter Mode, 00 = Up, 01 = Down, 
							//10 = Up-Down, 11 = Stop timer stopped (disabled)

	EPwm3Regs.CMPCTL.all = 0x000A;			// Compare control register
	//bit15-10	0:    	reserved
	//bit9		0:      SHDWBFULL, read-only
	//bit8		0:      SHDWAFULL, read-only
	//bit7		0:      reserved
	//bit6		0:      SHDWBMODE, 0 = shadow mode, 1= immediate
	//bit5		0:      reserved
	//bit4		0:      SHDWAMODE, 0 = shadow mode, 1= immediate
	//bit3-2	10:     LOADBMODE, 00 => CTR=0; 01 =>CTR=PRD, 
						//10 => CTR=0 or CTR=PRD, 11, disable
	//bit1-0	10:     LOADAMODE, 00 => CTR=0; 01 =>CTR=PRD, 
						//10 => CTR=0 or CTR=PRD, 11, disable

	// config EPWM3A output when(1)incrementing and (2)equals CMPA
	EPwm3Regs.AQCTLA.bit.CAU = 0x2;	                                      	
	// config EPWM3A output when(1)decrementing and (2)equals CMPA
	EPwm3Regs.AQCTLA.bit.CAD = 0x1;                                    			
	//Action-qualifier control register A	
	//bit15-12     0000:   reserved
	//bit11-10     00:     CBD, 00 = do nothing
	//bit9-8       00:     CBU, 00 = do nothing
	//bit7-6       01:     CAD, 00 = disable, 01, clear, 10 = set, 11 = toggle
	//bit5-4       10:     CAU, 00 = disable, 01, clear, 10 = set, 11 = toggle
	//bit3-2       00:     PRD, 00 = do nothing
	//bit1-0       00:     ZRO, 00 = do nothing
//	EPwm3Regs.AQCTLB.bit.CBU = 0x2;                                      			
	// config EPWM1A output when(1)decrementing and (2)equals CMPA
//	EPwm3Regs.AQCTLB.bit.CBD = 0x1;                                      		
	//Action-qualifier control register A	
	//bit15-12     0000:   reserved
	//bit11-10     00:     CBD, 00 = do nothing
	//bit9-8       00:     CBU, 00 = do nothing
	//bit7-6       01:     CAD, 00 = disable, 01, clear, 10 = set, 11 = toggle
	//bit5-4       10:     CAU, 00 = disable, 01, clear, 10 = set, 11 = toggle
	//bit3-2       00:     PRD, 00 = do nothing
	//bit1-0       00:     ZRO, 00 = do nothing
	// AQCSFRC load mode
	EPwm3Regs.AQSFRC.bit.RLDCSF = 0x3;			
	//00 Load on event counter equauls zero
	//01 Load on event counter equauls period
	//10 Load on event counter equauls zero or period
	//11 Load immediately

	
	EPwm3Regs.DBFED = DeadBand_Duration;		// Dead band duration for FED
	EPwm3Regs.DBRED = DeadBand_Duration;		// Dead band duration for RED
	
	EPwm3Regs.PCCTL.bit.CHPEN = 0;				// PWM chopper unit disabled

	// ePWM4 Configuration 
	EPwm4Regs.TBCTL.bit.CTRMODE = 0x2;		// up-down mode
	EPwm4Regs.TBPRD = PWM_HALF_PERIOD;		// actual period = 2*PWM_HALF_PERIOD
	EPwm4Regs.TBPHS.half.TBPHS = 0x0;		// Set timer phase, zero
	EPwm4Regs.TBCTL.bit.PHSEN = 0x1;		// Slave module
	EPwm4Regs.TBCTL.bit.PRDLD = 0x0;		// reload from shadow
	EPwm4Regs.TBCTL.bit.SYNCOSEL = 0x0; 	// sync when CTR to zero
	EPwm4Regs.TBCTL.bit.SWFSYNC = 0x0;
	EPwm4Regs.TBCTL.bit.HSPCLKDIV = 0x1;	// prescaler = 2
	EPwm4Regs.TBCTL.bit.CLKDIV = 0x0;
	EPwm4Regs.TBCTL.bit.PHSDIR = 0x0;		// don't care
	EPwm4Regs.TBCTL.bit.FREE_SOFT = 0x0;	
	//bit15-14     11:     	FREE/SOFT, 11 = ignore emulation suspend
	//bit13        0:      	PHSDIR, 0 = count down after sync event, 
							//1 = count up after sync event
	//bit12-10     000:    	CLKDIV, 000 = 2^0, 001 = 2^1, ..., 111 = 2^7, 
							//TBCLK = SYSCLKOUT/(HSPCLKDIV*CLKDIV)
	//bit9-7       001:    	HSPCLKDIV, Prescaler, 000 = 1, 001 = 2, 
							//010 = 4, ..., 110 = 12, 111 = 14.
	//bit6         0:      	SWFSYNC, 0 = no software sync produced, 
							//1 = writing a 1 forces a one-time sync pulse
	//bit5-4       01:     	SYNCOSEL, sync-output-select, 00 = EPWMxSYNC, 
							//01 =>CTR=0, 10 =>CTR=CMPB, 11 = sync-out disabled
	//bit3         0:      	PRDLD, 0 = reload PRD on counter=0 from shadow, 
							//1 = without shadow
	//bit2         0:      	PHSEN, 0 = phase control disabled, master mode, 
							//1 = slave mode 
	//bit1-0       11:     	CTRMODE, Counter Mode, 00 = Up, 01 = Down, 
							//10 = Up-Down, 11 = Stop timer stopped (disabled)

	EPwm4Regs.CMPCTL.all = 0x0005;			// Compare control register
	//bit15-10	0:    	reserved
	//bit9		0:      SHDWBFULL, read-only
	//bit8		0:      SHDWAFULL, read-only
	//bit7		0:      reserved
	//bit6		0:      SHDWBMODE, 0 = shadow mode, 1= immediate
	//bit5		0:      reserved
	//bit4		0:      SHDWAMODE, 0 = shadow mode, 1= immediate
	//bit3-2	01:     LOADBMODE, 00 => CTR=0; 01 =>CTR=PRD, 
						//10 => CTR=0 or CTR=PRD, 11, disable
	//bit1-0	01:     LOADAMODE, 00 => CTR=0; 01 =>CTR=PRD, 
						//10 => CTR=0 or CTR=PRD, 11, disable

	// config EPWM1A output when(1)incrementing and (2)equals CMPA
	EPwm4Regs.AQCTLA.bit.CAU = 0x2;	                                      	
	// config EPWM1A output when(1)decrementing and (2)equals CMPA
	EPwm4Regs.AQCTLA.bit.CAD = 0x1;	                                      	
	//Action-qualifier control register A	
	//bit15-12     0000:   reserved
	//bit11-10     00:     CBD, 00 = do nothing
	//bit9-8       00:     CBU, 00 = do nothing
	//bit7-6       01:     CAD, 00 = disable, 01, clear, 10 = set, 11 = toggle
	//bit5-4       10:     CAU, 00 = disable, 01, clear, 10 = set, 11 = toggle
	//bit3-2       00:     PRD, 00 = do nothing
	//bit1-0       00:     ZRO, 00 = do nothing
//	EPwm4Regs.AQCTLB.bit.CBU = 0x1;                                      			
	// config EPWM1A output when(1)decrementing and (2)equals CMPA
//	EPwm4Regs.AQCTLB.bit.CBD = 0x2;                                     		
	//Action-qualifier control register A	
	//bit15-12     0000:   reserved
	//bit11-10     00:     CBD, 00 = do nothing
	//bit9-8       00:     CBU, 00 = do nothing
	//bit7-6       01:     CAD, 00 = disable, 01, clear, 10 = set, 11 = toggle
	//bit5-4       10:     CAU, 00 = disable, 01, clear, 10 = set, 11 = toggle
	//bit3-2       00:     PRD, 00 = do nothing
	//bit1-0       00:     ZRO, 00 = do nothing
	EPwm4Regs.AQSFRC.bit.RLDCSF = 0x3;
				
	//00 Load on event counter equauls zero
	//01 Load on event counter equauls period
	//10 Load on event counter equauls zero or period
	//11 Load immediately
	// force AQC output low
	
	EPwm4Regs.DBFED = DeadBand_Duration;		// Dead band duration for FED
	EPwm4Regs.DBRED = DeadBand_Duration;		// Dead band duration for RED
	
	EPwm4Regs.PCCTL.bit.CHPEN = 0;				// PWM chopper unit disabled

	// ePWM5 Configuration 
	EPwm5Regs.TBCTL.bit.CTRMODE = 0x2;		// up-down mode
	EPwm5Regs.TBPRD = PWM_HALF_PERIOD;		// actual period = 2*PWM_HALF_PERIOD
	EPwm5Regs.TBPHS.half.TBPHS = 0x0;		// Set timer phase, zero
	EPwm5Regs.TBCTL.bit.PHSEN = 0x1;		// Slave module
	EPwm5Regs.TBCTL.bit.PRDLD = 0x0;		// reload from shadow
	EPwm5Regs.TBCTL.bit.SYNCOSEL = 0x0; 	// sync when CTR to zero
	EPwm5Regs.TBCTL.bit.SWFSYNC = 0x0;
	EPwm5Regs.TBCTL.bit.HSPCLKDIV = 0x1;	// prescaler = 2
	EPwm5Regs.TBCTL.bit.CLKDIV = 0x0;
	EPwm5Regs.TBCTL.bit.PHSDIR = 0x0;		// don't care
	EPwm5Regs.TBCTL.bit.FREE_SOFT = 0x0;	
	//bit15-14     11:     	FREE/SOFT, 11 = ignore emulation suspend
	//bit13        0:      	PHSDIR, 0 = count down after sync event, 
							//1 = count up after sync event
	//bit12-10     000:    	CLKDIV, 000 = 2^0, 001 = 2^1, ..., 111 = 2^7, 
							//TBCLK = SYSCLKOUT/(HSPCLKDIV*CLKDIV)
	//bit9-7       001:    	HSPCLKDIV, Prescaler, 000 = 1, 001 = 2, 
							//010 = 4, ..., 110 = 12, 111 = 14.
	//bit6         0:      	SWFSYNC, 0 = no software sync produced, 
							//1 = writing a 1 forces a one-time sync pulse
	//bit5-4       00:     	SYNCOSEL, sync-output-select, 00 = EPWMxSYNC, 
							//01 =>CTR=0, 10 =>CTR=CMPB, 11 = sync-out disabled
	//bit3         0:      	PRDLD, 0 = reload PRD on counter=0 from shadow, 
							//1 = without shadow
	//bit2         0:      	PHSEN, 0 = phase control disabled, master mode, 
							//1 = slave mode 
	//bit1-0       11:     	CTRMODE, Counter Mode, 00 = Up, 01 = Down, 
							//10 = Up-Down, 11 = Stop timer stopped (disabled)

	EPwm5Regs.CMPCTL.all = 0x0005;			// Compare control register
	//bit15-10	0:    	reserved
	//bit9		0:      SHDWBFULL, read-only
	//bit8		0:      SHDWAFULL, read-only
	//bit7		0:      reserved
	//bit6		0:      SHDWBMODE, 0 = shadow mode, 1= immediate
	//bit5		0:      reserved
	//bit4		0:      SHDWAMODE, 0 = shadow mode, 1= immediate
	//bit3-2	01:     LOADBMODE, 00 => CTR=0; 01 =>CTR=PRD, 
						//10 => CTR=0 or CTR=PRD, 11, disable
	//bit1-0	01:     LOADAMODE, 00 => CTR=0; 01 =>CTR=PRD, 
						//10 => CTR=0 or CTR=PRD, 11, disable

	// config EPWM1A output when(1)incrementing and (2)equals CMPA
	EPwm5Regs.AQCTLA.bit.CAU = 0x2;                                     			
	// config EPWM1A output when(1)decrementing and (2)equals CMPA
	EPwm5Regs.AQCTLA.bit.CAD = 0x1;                                     		
	//Action-qualifier control register A	
	//bit15-12     0000:   reserved
	//bit11-10     00:     CBD, 00 = do nothing
	//bit9-8       00:     CBU, 00 = do nothing
	//bit7-6       01:     CAD, 00 = disable, 01, clear, 10 = set, 11 = toggle
	//bit5-4       10:     CAU, 00 = disable, 01, clear, 10 = set, 11 = toggle
	//bit3-2       00:     PRD, 00 = do nothing
	//bit1-0       00:     ZRO, 00 = do nothing
//	EPwm5Regs.AQCTLB.bit.CBU = 0x2;			
	// config EPWM1A output when(1)decrementing and (2)equals CMPA
//	EPwm5Regs.AQCTLB.bit.CBD = 0x1;			
	//Action-qualifier control register A	
	//bit15-12     0000:   reserved
	//bit11-10     00:     CBD, 00 = do nothing
	//bit9-8       00:     CBU, 00 = do nothing
	//bit7-6       01:     CAD, 00 = disable, 01, clear, 10 = set, 11 = toggle
	//bit5-4       10:     CAU, 00 = disable, 01, clear, 10 = set, 11 = toggle
	//bit3-2       00:     PRD, 00 = do nothing
	//bit1-0       00:     ZRO, 00 = do nothing
	EPwm5Regs.AQSFRC.bit.RLDCSF = 0x3;			
	//00 Load on event counter equauls zero
	//01 Load on event counter equauls period
	//10 Load on event counter equauls zero or period
	//11 Load immediately
	// force AQC output low
	
	EPwm5Regs.DBFED = DeadBand_Duration;		// Dead band duration for FED
	EPwm5Regs.DBRED = DeadBand_Duration;		// Dead band duration for RED
	
	EPwm5Regs.PCCTL.bit.CHPEN = 0;				// PWM chopper unit disabled

	// ePWM6 Configuration 
	EPwm6Regs.TBCTL.bit.CTRMODE = 0x2;		// up-down mode
	EPwm6Regs.TBPRD = PWM_HALF_PERIOD;		// actual period = 2*PWM_HALF_PERIOD
	EPwm6Regs.TBPHS.half.TBPHS = 0x0;		// Set timer phase, zero
	EPwm6Regs.TBCTL.bit.PHSEN = 0x1;		// Slave module
	EPwm6Regs.TBCTL.bit.PRDLD = 0x0;		// reload from shadow
	EPwm6Regs.TBCTL.bit.SYNCOSEL = 0x0; 	// sync when CTR to zero
	EPwm6Regs.TBCTL.bit.SWFSYNC = 0x0;
	EPwm6Regs.TBCTL.bit.HSPCLKDIV = 0x1;	// prescaler = 2
	EPwm6Regs.TBCTL.bit.CLKDIV = 0x0;
	EPwm6Regs.TBCTL.bit.PHSDIR = 0x0;		// don't care
	EPwm6Regs.TBCTL.bit.FREE_SOFT = 0x0;	
	//bit15-14     11:     	FREE/SOFT, 11 = ignore emulation suspend
	//bit13        0:      	PHSDIR, 0 = count down after sync event, 
							//1 = count up after sync event
	//bit12-10     000:    	CLKDIV, 000 = 2^0, 001 = 2^1, ..., 111 = 2^7, 
							//TBCLK = SYSCLKOUT/(HSPCLKDIV*CLKDIV)
	//bit9-7       001:    	HSPCLKDIV, Prescaler, 000 = 1, 001 = 2, 
							//010 = 4, ..., 110 = 12, 111 = 14.
	//bit6         0:      	SWFSYNC, 0 = no software sync produced, 
							//1 = writing a 1 forces a one-time sync pulse
	//bit5-4       01:     	SYNCOSEL, sync-output-select, 00 = EPWMxSYNC, 
							//01 =>CTR=0, 10 =>CTR=CMPB, 11 = sync-out disabled
	//bit3         0:      	PRDLD, 0 = reload PRD on counter=0 from shadow, 
							//1 = without shadow
	//bit2         0:      	PHSEN, 0 = phase control disabled, master mode, 
							//1 = slave mode 
	//bit1-0       11:     	CTRMODE, Counter Mode, 00 = Up, 01 = Down, 
							//10 = Up-Down, 11 = Stop timer stopped (disabled)

	EPwm6Regs.CMPCTL.all = 0x0005;			// Compare control register
	//bit15-10	0:    	reserved
	//bit9		0:      SHDWBFULL, read-only
	//bit8		0:      SHDWAFULL, read-only
	//bit7		0:      reserved
	//bit6		0:      SHDWBMODE, 0 = shadow mode, 1= immediate
	//bit5		0:      reserved
	//bit4		0:      SHDWAMODE, 0 = shadow mode, 1= immediate
	//bit3-2	01:     LOADBMODE, 00 => CTR=0; 01 =>CTR=PRD, 
						//10 => CTR=0 or CTR=PRD, 11, disable
	//bit1-0	01:     LOADAMODE, 00 => CTR=0; 01 =>CTR=PRD, 
						//10 => CTR=0 or CTR=PRD, 11, disable

	// config EPWM1A output when(1)incrementing and (2)equals CMPA
	EPwm6Regs.AQCTLA.bit.CAU = 0x2;                                      		
	// config EPWM1A output when(1)decrementing and (2)equals CMPA
	EPwm6Regs.AQCTLA.bit.CAD = 0x1;                                    			
	//Action-qualifier control register A	
	//bit15-12     0000:   reserved
	//bit11-10     00:     CBD, 00 = do nothing
	//bit9-8       00:     CBU, 00 = do nothing
	//bit7-6       01:     CAD, 00 = disable, 01, clear, 10 = set, 11 = toggle
	//bit5-4       10:     CAU, 00 = disable, 01, clear, 10 = set, 11 = toggle
	//bit3-2       00:     PRD, 00 = do nothing
	//bit1-0       00:     ZRO, 00 = do nothing
//	EPwm6Regs.AQCTLB.bit.CBU = 0x1;                                     		
	// config EPWM1A output when(1)decrementing and (2)equals CMPA
//	EPwm6Regs.AQCTLB.bit.CBD = 0x2;                                    		
	//Action-qualifier control register A	
	//bit15-12     0000:   reserved
	//bit11-10     00:     CBD, 00 = do nothing
	//bit9-8       00:     CBU, 00 = do nothing
	//bit7-6       01:     CAD, 00 = disable, 01, clear, 10 = set, 11 = toggle
	//bit5-4       10:     CAU, 00 = disable, 01, clear, 10 = set, 11 = toggle
	//bit3-2       00:     PRD, 00 = do nothing
	//bit1-0       00:     ZRO, 00 = do nothing
	EPwm6Regs.AQSFRC.bit.RLDCSF = 0x3;			
	//00 Load on event counter equauls zero
	//01 Load on event counter equauls period
	//10 Load on event counter equauls zero or period
	//11 Load immediately
	// force AQC output low
	
	EPwm6Regs.DBFED = DeadBand_Duration;		// Dead band duration for FED
	EPwm6Regs.DBRED = DeadBand_Duration;		// Dead band duration for RED

	EPwm6Regs.PCCTL.bit.CHPEN = 0;				// PWM chopper unit disabled

	// initial TB counter
	EPwm1Regs.TBCTR = 0x0000;		// Clear timer counter
	EPwm2Regs.TBCTR = 0x0000;		// Clear timer counter
	EPwm3Regs.TBCTR = 0x0000;		// Clear timer counter
	EPwm4Regs.TBCTR = 0x0000;		// Clear timer counter
	EPwm5Regs.TBCTR = 0x0000;		// Clear timer counter
	EPwm6Regs.TBCTR = 0x0000;		// Clear timer counter
	asm(" EALLOW");					// Enable EALLOW protected register access
	SysCtrlRegs.PCLKCR0.bit.TBCLKSYNC = 1;	// HSPCLK to ePWM modules enabled
	asm(" EDIS");					// Disable EALLOW protected register access

	EPwm1Regs.AQCSFRC.bit.CSFA = 0x0;
	EPwm1Regs.AQCSFRC.bit.CSFB = 0x0;
	//00 Forcing disabled, i.e., has no effect
	//01 Forces a continuous low on output B
	//10 Forces a continuous high on output B
	//11 Software forcing is disabled and has no effect
	EPwm1Regs.DBCTL.all = 0x0B;//0x002B;//
	//Dead-Band Generator Control register 	
	//bit15-6	0000:   reserved
	//bit5-4	10:     IN_MODE, select input source for RED and FED, EPWMxA/EPWMxB
						// 00 = A->both, 01 = B->RED and A->FED, 
						// 10 = A->RED and B->FED, 11 = B->both
	//bit3-2	10:		POLSEL, polarity select for output, 00 = AH, 01 = ALC, 
						//10 = AHC, 11 = AL
	//bit1-0	11:		OUT_MODE, 00 = no DB, 01 = No RED, 10 = No FED, 
						//11 = DB fully enabled
	EPwm2Regs.AQCSFRC.bit.CSFA = 0x0;
	EPwm2Regs.AQCSFRC.bit.CSFB = 0x0;
	//00 Forcing disabled, i.e., has no effect
	//01 Forces a continuous low on output B
	//10 Forces a continuous high on output B
	//11 Software forcing is disabled and has no effect
	EPwm2Regs.DBCTL.all = 0x0B;//0x002B;

	EPwm3Regs.AQCSFRC.bit.CSFA = 0x0;
	EPwm3Regs.AQCSFRC.bit.CSFB = 0x0;
	//00 Forcing disabled, i.e., has no effect
	//01 Forces a continuous low on output B
	//10 Forces a continuous high on output B
	//11 Software forcing is disabled and has no effect
	EPwm3Regs.DBCTL.all = 0x0B;//0x002B;

	EPwm4Regs.AQCSFRC.bit.CSFA = 0x0;
	EPwm4Regs.AQCSFRC.bit.CSFB = 0x0;
	//00 Forcing disabled, i.e., has no effect
	//01 Forces a continuous low on output B
	//10 Forces a continuous high on output B
	//11 Software forcing is disabled and has no effect
	EPwm4Regs.DBCTL.all = 0x0B;

	EPwm5Regs.AQCSFRC.bit.CSFA = 0x0;
	EPwm5Regs.AQCSFRC.bit.CSFB = 0x0;
	//00 Forcing disabled, i.e., has no effect
	//01 Forces a continuous low on output B
	//10 Forces a continuous high on output B
	//11 Software forcing is disabled and has no effect
	EPwm5Regs.DBCTL.all = 0x0B;

	EPwm6Regs.AQCSFRC.bit.CSFA = 0x0;
	EPwm6Regs.AQCSFRC.bit.CSFB = 0x0;
	//00 Forcing disabled, i.e., has no effect
	//01 Forces a continuous low on output B
	//10 Forces a continuous high on output B
	//11 Software forcing is disabled and has no effect
	EPwm6Regs.DBCTL.all = 0x0B;

//	EPwm1Regs.CMPA.half.CMPA = PWM_HALF_PERIOD*0.3;	// Set init PWM duty cycle
//	EPwm1Regs.CMPB = PWM_HALF_PERIOD*0.3;	// Set init PWM duty cycle
//	EPwm2Regs.CMPA.half.CMPA = PWM_HALF_PERIOD*0.8;	// Set init PWM duty cycle
//	EPwm2Regs.CMPB = PWM_HALF_PERIOD*0.8;//PWM_HALF_PERIOD*0.6;	// Set init PWM duty cycle
//	EPwm3Regs.CMPA.half.CMPA = PWM_HALF_PERIOD*0.6;	// Set init PWM duty cycle
//	EPwm3Regs.CMPB = PWM_HALF_PERIOD*0.6;//PWM_HALF_PERIOD*0.6;	// Set init PWM duty cycle

//TZÄ£¿éµÄÅäÖÃ
    asm(" EALLOW");	

//	GpioCtrlRegs.GPAPUD.bit.GPIO13 = 0;    // Enable pull-up on 
//	GpioCtrlRegs.GPAPUD.bit.GPIO14 = 0;    // Enable pull-up on 
//	GpioCtrlRegs.GPAPUD.bit.GPIO15 = 0;    // Enable pull-up on 
//	GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;    // Enable pull-up on 
//	GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;    // Enable pull-up on 

    GpioCtrlRegs.GPAQSEL1.bit.GPIO12 = 3;  // Asynch input GPIO12 (TZ1)
//	GpioCtrlRegs.GPAQSEL1.bit.GPIO13 = 3;  // Asynch input GPIO12 (TZ1)
//    GpioCtrlRegs.GPAQSEL1.bit.GPIO14 = 3;  // Asynch input GPIO12 (TZ1)
//    GpioCtrlRegs.GPAQSEL1.bit.GPIO15 = 3;  // Asynch input GPIO12 (TZ1)
//    GpioCtrlRegs.GPAQSEL2.bit.GPIO16 = 3;  // Asynch input GPIO12 (TZ1)
//	GpioCtrlRegs.GPAQSEL2.bit.GPIO17 = 3;  // Asynch input GPIO12 (TZ1)

	EPwm1Regs.TZSEL.bit.CBC1= 1;         // EPWM1 ONCE protection for ALL TZ	
	EPwm1Regs.TZCTL.bit.TZA = 2;         // TZ Protection EPwm1A is output low    
	EPwm1Regs.TZCTL.bit.TZB = 2;         // TZ Protection EPwm1B is output low
	
	EPwm2Regs.TZSEL.bit.OSHT1= 1;         // EPWM2 TZ 	
	EPwm2Regs.TZCTL.bit.TZA= 2;         // TZ Protection EPwm2A is output low    
	EPwm2Regs.TZCTL.bit.TZB= 2;         // TZ Protection EPwm2B is output low

	EPwm3Regs.TZSEL.bit.OSHT1= 1;         // EPWM2 TZ 	
	EPwm3Regs.TZCTL.bit.TZA= 2;         // TZ Protection EPwm3A is output low    
	EPwm3Regs.TZCTL.bit.TZB= 2;         // TZ Protection EPwm3B is output low

	EPwm1Regs.TZEINT.bit.CBC = 1;	
	asm(" EDIS");

    	
	PieCtrlRegs.PIEIER2.bit.INTx1 = 1;	// Enable ADCINT in PIE group 1
//	IER |= M_INT2;
} // end of InitEPwm()




/*=============================================================================*
 * FUNCTION: PWMOutputsEnable()
 * PURPOSE : ePWM1A/B, 2A/B, and 3A/B enabled, PWM Timer5 also enabled
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
 *     Main.c or MainStatusMachine.c 
 * 
 *============================================================================*/
void PWMOutputsEnable(void)
{ // start of PWMOutputsEnable()

//	HWI_disable();
//	EPwm1Regs.AQCSFRC.bit.CSFB = 0x0;
//	EPwm2Regs.AQCSFRC.bit.CSFB = 0x0;
//	EPwm3Regs.AQCSFRC.bit.CSFB = 0x0; 
//	EPwm4Regs.AQCSFRC.bit.CSFB = 0x0;
//	EPwm5Regs.AQCSFRC.bit.CSFB = 0x0;
//	EPwm6Regs.AQCSFRC.bit.CSFB = 0x0; 	 
		
//	EPwm1Regs.AQCSFRC.bit.CSFA = 0x0;
//	EPwm2Regs.AQCSFRC.bit.CSFA = 0x0;
//	EPwm3Regs.AQCSFRC.bit.CSFA = 0x0;			
//	EPwm4Regs.AQCSFRC.bit.CSFA = 0x0;
//	EPwm5Regs.AQCSFRC.bit.CSFA = 0x0;
//	EPwm6Regs.AQCSFRC.bit.CSFA = 0x0;
	asm(" EALLOW");	 // open driver
	EPwm1Regs.TZCLR.bit.OST = 1;
	EPwm2Regs.TZCLR.bit.OST = 1;
	EPwm3Regs.TZCLR.bit.OST = 1;
	EPwm4Regs.TZCLR.bit.OST = 1;
	EPwm5Regs.TZCLR.bit.OST = 1;
	EPwm6Regs.TZCLR.bit.OST = 1;

    EPwm1Regs.TZCTL.bit.TZA = 11;
	EPwm1Regs.TZCTL.bit.TZB = 11;
	EPwm2Regs.TZCTL.bit.TZA = 11;
	EPwm2Regs.TZCTL.bit.TZB = 11;
	EPwm3Regs.TZCTL.bit.TZA = 11;
	EPwm3Regs.TZCTL.bit.TZB = 11;
	EPwm4Regs.TZCTL.bit.TZA = 11;
	EPwm4Regs.TZCTL.bit.TZB = 11;
	EPwm5Regs.TZCTL.bit.TZA = 11;
	EPwm5Regs.TZCTL.bit.TZB = 11;
	EPwm6Regs.TZCTL.bit.TZA = 11;
	EPwm6Regs.TZCTL.bit.TZB = 11;
 	asm(" EDIS");


//	HWI_enable();
	//00 Forcing disabled, i.e., has no effect
	//01 Forces a continuous low on output B
	//10 Forces a continuous high on output B
	//11 Software forcing is disabled and has no effect
} // end of PWMOutputsEnable()


/*=============================================================================*
 * FUNCTION: PWMOutputsDisable()
 * PURPOSE : ePWM1A/B, 2A/B, and 3A/B enabled, PWM Timer5 also disabled
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
 *     Main.c or MainStatusMachine.c 
 * 
 *============================================================================*/
void PWMOutputsDisable(void)
{ // start of PWMOutputsDisable()

//	HWI_disable();			
//	EPwm1Regs.AQCSFRC.bit.CSFA = 0x1;
//	EPwm2Regs.AQCSFRC.bit.CSFA = 0x1;
//	EPwm3Regs.AQCSFRC.bit.CSFA = 0x1;
//	EPwm4Regs.AQCSFRC.bit.CSFA = 0x1;
//	EPwm5Regs.AQCSFRC.bit.CSFA = 0x1;
//	EPwm6Regs.AQCSFRC.bit.CSFA = 0x1;

//	DelayUs(3);

//	EPwm1Regs.AQCSFRC.bit.CSFB = 0x1;
//	EPwm2Regs.AQCSFRC.bit.CSFB = 0x1;
//	EPwm3Regs.AQCSFRC.bit.CSFB = 0x1;
//	EPwm4Regs.AQCSFRC.bit.CSFB = 0x1;
//	EPwm5Regs.AQCSFRC.bit.CSFB = 0x1;
//	EPwm6Regs.AQCSFRC.bit.CSFB = 0x1;
	asm(" EALLOW");
	EPwm1Regs.TZFRC.bit.OST = 1;
	EPwm2Regs.TZFRC.bit.OST = 1;
	EPwm3Regs.TZFRC.bit.OST = 1;
	EPwm4Regs.TZFRC.bit.OST = 1;
	EPwm5Regs.TZFRC.bit.OST = 1;
	EPwm6Regs.TZFRC.bit.OST = 1;

	EPwm1Regs.TZCTL.bit.TZA = 10;
	EPwm2Regs.TZCTL.bit.TZA = 10;
	EPwm3Regs.TZCTL.bit.TZA = 10;
	EPwm4Regs.TZCTL.bit.TZA = 10;
	EPwm5Regs.TZCTL.bit.TZA = 10;
	EPwm6Regs.TZCTL.bit.TZA = 10;
	asm(" EDIS");

	DelayUs(3);

	asm(" EALLOW");
	EPwm1Regs.TZFRC.bit.OST = 1;
	EPwm2Regs.TZFRC.bit.OST = 1;
	EPwm3Regs.TZFRC.bit.OST = 1;
	EPwm4Regs.TZFRC.bit.OST = 1;
	EPwm5Regs.TZFRC.bit.OST = 1;
	EPwm6Regs.TZFRC.bit.OST = 1;

	EPwm1Regs.TZCTL.bit.TZB = 10;
	EPwm2Regs.TZCTL.bit.TZB = 10;
	EPwm3Regs.TZCTL.bit.TZB = 10;
	EPwm4Regs.TZCTL.bit.TZB = 10;
	EPwm5Regs.TZCTL.bit.TZB = 10;
	EPwm6Regs.TZCTL.bit.TZB = 10;
	asm(" EDIS");

//	HWI_enable();
	//00 Forcing disabled, i.e., has no effect
	//01 Forces a continuous low on output B
	//10 Forces a continuous high on output B
	//11 Software forcing is disabled and has no effect

} // end of PWMOutputsDisable()


//void F280X_PWM_Update(PWMGEN *p) 
void F2833X_PWM_Update(PWMGEN *p) 
{       
		int16 MPeriod;              
        float32 Tmp;
        p->PeriodMax = PWM_HALF_PERIOD;
      //  GpioDataRegs.GPBTOGGLE.bit.GPIO43 = 1;//test
	// Compute the timer period (Q0) from the period modulation input (Q15)
        Tmp = (float32)p->PeriodMax; // Q15 = Q0*Q15  MfuncPeriod=2^15=0x7FFF PeriodMax=2500
      //  MPeriod = (int16)(Tmp>>16) + (int16)(p->PeriodMax>>1); // Q0 = (Q15->Q0)/2 + (Q0/2)
        MPeriod = PWM_HALF_PERIOD;
        EPwm1Regs.TBPRD = MPeriod;  
        EPwm2Regs.TBPRD = MPeriod;  
        EPwm3Regs.TBPRD = MPeriod; 
//        EPwm4Regs.TBPRD = MPeriod;  
//        EPwm5Regs.TBPRD = MPeriod; 
//        EPwm6Regs.TBPRD = MPeriod; 
//***************   Clarke ******************************************************************************************
//**************   Park *******************************************************************************************/
// Compute the compare A (Q0) from the EPWM1AO & EPWM1BO duty cycle ratio (Q15)
        Tmp = (float32)MPeriod * ( p->MfuncC1);                    // Q15 = Q0*Q15
        EPwm1Regs.CMPA.half.CMPA = Tmp;   // Q0 = (Q15->Q0)/2 + (Q0/2)

// Compute the compare B (Q0) from the EPWM2AO & EPWM2BO duty cycle ratio (Q15)
        Tmp = (float32)MPeriod * p->MfuncC2;                   // Q15 = Q0*Q15
        EPwm2Regs.CMPA.half.CMPA = Tmp;  // Q0 = (Q15->Q0)/2 + (Q0/2)

// Compute the compare C (Q0) from the EPWM3AO & EPWM3BO duty cycle ratio (Q15)
        Tmp = (float32)MPeriod*( p->MfuncC3);                   // Q15 = Q0*Q15
        EPwm3Regs.CMPA.half.CMPA = Tmp;  // Q0 = (Q15->Q0)/2 + (Q0/2)


// Compute the compare D (Q0) from the EPWM4AO & EPWM4BO duty cycle ratio (Q15)
//        Tmp = (float32)MPeriod*p->MfuncC4;                   // Q15 = Q0*Q15
//        EPwm4Regs.CMPA.half.CMPA = Tmp;  // Q0 = (Q15->Q0)/2 + (Q0/2)

// Compute the compare E (Q0) from the EPWM5AO & EPWM5BO duty cycle ratio (Q15)
//        Tmp = (float32)MPeriod*( p->MfuncC5);                   // Q15 = Q0*Q15
//       EPwm5Regs.CMPA.half.CMPA = Tmp;  // Q0 = (Q15->Q0)/2 + (Q0/2)

// Compute the compare F (Q0) from the EPWM6AO & EPWM6BO duty cycle ratio (Q15)
//        Tmp = (float32)MPeriod*p->MfuncC6;                   // Q15 = Q0*Q15
//        EPwm6Regs.CMPA.half.CMPA = Tmp;  // Q0 = (Q15->Q0)/2 + (Q0/2)

//***************   Park *******************************************************************************************/                  






}


//--- end of file -----------------------------------------------------

