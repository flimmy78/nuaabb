/*=============================================================================*
 *         Copyright(c) 2009-2010, ALL RIGHTS RESERVED
 *
 *  FILENAME : Gpio.c 
 *
 *  PURPOSE  : Gpio configuration according to application board for 5KW
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


#include "DSP2833x_Device.h"		    // Peripheral address definitions
#include "500kVar_SVG.H"				// Main include file

/*=============================================================================*
 * FUNCTION: InitGpio()
 * PURPOSE : I/O hardware module initialization.
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
void InitGpio(void)
{
	asm(" EALLOW");					// Enable EALLOW protected register access

	/* Group A pins */


	GpioCtrlRegs.GPACTRL.all  = 0x00000000;		// QUALPRD = SYSCLKOUT for all group A GPIO
		
	GpioCtrlRegs.GPAQSEL1.all = 0x00000000;		// No qualification for all group A GPIO 0-15

	GpioCtrlRegs.GPAQSEL2.all = 0x00000000;		// No qualification for all group A GPIO 16-31

	GpioCtrlRegs.GPADIR.all   = 0x00000000;		// All group A GPIO are inputs
	
	GpioCtrlRegs.GPAPUD.all   = 0xFFFFFFFF;		// All Pullups disabled due to external pull down

//	GpioCtrlRegs.GPAPUD.bit.GPIO16 = 0;	
//	GpioCtrlRegs.GPAPUD.bit.GPIO17 = 0;	
	// GPIO 0-12 are used as ePWM port
	GpioCtrlRegs.GPAMUX1.bit.GPIO0  = 1;		// 0=GPIO  1=EPWM1A     2=rsvd       
												// 3=rsvd
	GpioCtrlRegs.GPAMUX1.bit.GPIO1  = 1;		// 0=GPIO  1=EPWM1B     2=ECAP6      
												// 3=rsvd
	GpioCtrlRegs.GPAMUX1.bit.GPIO2  = 1;		// 0=GPIO  1=EPWM2A     2=rsvd       
												// 3=rsvd
	GpioCtrlRegs.GPAMUX1.bit.GPIO3  = 1;		// 0=GPIO  1=EPWM2B     2=ECAP5      
												// 3=rsvd
	GpioCtrlRegs.GPAMUX1.bit.GPIO4  = 1;		// 0=GPIO  1=EPWM3A     2=rsvd       
												// 3=rsvd
	GpioCtrlRegs.GPAMUX1.bit.GPIO5  = 1;		// 0=GPIO  1=EPWM3B     2=MFSRA      
												// 3=ECAP1
	GpioCtrlRegs.GPAMUX1.bit.GPIO6  = 1;		// 0=GPIO  1=EPWM4A     2=EPWMSYNCI  
												// 3=EPWMSYNCO
	GpioCtrlRegs.GPAMUX1.bit.GPIO7  = 1;		// 0=GPIO  1=EPWM4B     2=MCLKRA     
												// 3=ECAP2
	GpioCtrlRegs.GPAMUX1.bit.GPIO8  = 1;		// 0=GPIO  1=EPWM5A     2=CANTXB     
												// 3=ADCSOCAO
	GpioCtrlRegs.GPAMUX1.bit.GPIO9  = 1;		// 0=GPIO  1=EPWM5B     2=SCITXDB    
												// 3=ECAP3
	GpioCtrlRegs.GPAMUX1.bit.GPIO10 = 1;		// 0=GPIO  1=EPWM6A     2=CANRXB     
												// 3=ADCSOCBO
	GpioCtrlRegs.GPAMUX1.bit.GPIO11 = 1;		// 0=GPIO  1=EPWM6B     2=SCIRXDB    



	// GPIO12-17 are used as TZ1-TZ6,
	GpioCtrlRegs.GPAMUX1.bit.GPIO12 = 0x1;		// 0=GPIO  1=TZ1        2=CANTXB
												// 3=SPISIMOB
	GpioCtrlRegs.GPAPUD.bit.GPIO12 = 0;         // Enable pull-up on 

	GpioCtrlRegs.GPAMUX1.bit.GPIO13 = 0;		// 0=GPIO  1=TZ2        2=CANRXB     
												// 3=SPISOMIB
	GpioCtrlRegs.GPADIR.bit.GPIO13 = 1;	        // output
	GpioDataRegs.GPACLEAR.bit.GPIO13 = 1;       // 初始为0

	GpioCtrlRegs.GPAMUX1.bit.GPIO14 = 0;		// 0=GPIO  1=TZ3/XHOLD  2=SCITXDB    
												// 3=SPICLKB
	GpioCtrlRegs.GPAMUX1.bit.GPIO15 = 0;		// 0=GPIO  1=TZ4/XHOLDA 2=SCIRXDB    
												// 3=SPISTEB
	GpioCtrlRegs.GPAMUX2.bit.GPIO16 = 0x0;		// 0=GPIO  1=SPISIMOA   2=CANTXB     
												// 3=TZ5
	GpioCtrlRegs.GPAMUX2.bit.GPIO17 = 0x0;		// 0=GPIO  1=SPISOMIA   2=CANRXB     
												// 3=TZ6
	// GPIO18-19,PViso Control port 
	GpioCtrlRegs.GPAMUX2.bit.GPIO18 = 0;		// 0=GPIO  1=SPICLKA    2=SCITXDB    
												// 3=CANRXA											
	GpioCtrlRegs.GPAMUX2.bit.GPIO19 = 0;		// 0=GPIO  1=SPISTEA    2=SCIRXDB    
												// 3=CANTXA
	// GPIO20-21 are used for DCI and Vinv measurement
	GpioCtrlRegs.GPAMUX2.bit.GPIO20 = 3;		// 0=GPIO  1=EQEP1A     2=MDXA       
												// 3=CANTXB
	GpioCtrlRegs.GPAMUX2.bit.GPIO21 = 3;		// 0=GPIO  1=EQEP1B     2=MDRA       
												// 3=CANRXB

    //GPIO22-24 are Used for temperature 
	GpioCtrlRegs.GPAMUX2.bit.GPIO22 = 0;		// 0=GPIO  1=EQEP1S     2=MCLKXA     
												// 3=SCITXDB
	GpioCtrlRegs.GPAMUX2.bit.GPIO23 = 0;		// 0=GPIO  1=EQEP1I     2=MFSXA      
												// 3=SCIRXDB
	GpioCtrlRegs.GPAMUX2.bit.GPIO24 = 0;		// 0=GPIO  1=ECAP1      2=EQEP2A     
												// 3=MDXB
//GPIO25-26, are Used for test R,S,T grid frequency. 25 GPIO,26-27 eCAPS.
	GpioCtrlRegs.GPAMUX2.bit.GPIO25 = 1;		// 0=GPIO  1=ECAP2      2=EQEP2B     
												// 3=MDRB
	GpioCtrlRegs.GPAMUX2.bit.GPIO26 = 1;		// 0=GPIO  1=ECAP3      2=EQEP2I     
												// 3=MCLKXB
	GpioCtrlRegs.GPAMUX2.bit.GPIO27 = 1;		// 0=GPIO  1=ECAP4      2=EQEP2S     
												// 3=MFSXB
	// GPIO28 is used as Output port CNTLR4, for 交流接触器.												  
    GpioCtrlRegs.GPAMUX2.bit.GPIO28 = 0;		// 0=GPIO  1=SCIRXDA    2=XZCS6      3=XZCS6   */

	// GPIO29 input pin for test FPGA is OK 
    GpioCtrlRegs.GPAMUX2.bit.GPIO29 = 0;		// 0=GPIO  1=SCITXDA    2=XA19       3=XA19	   */

	// GPIO30-31 are used for CAN communication
	GpioCtrlRegs.GPAMUX2.bit.GPIO30 = 1;		// 0=GPIO  1=CANRXA     2=XA18       3=XA18
	GpioCtrlRegs.GPAMUX2.bit.GPIO31 = 1;		// 0=GPIO  1=CANTXA     2=XA17       3=XA17



	/* Group B pins */
	
	// QUALPRD = SYSCLKOUT for all group B GPIO
	GpioCtrlRegs.GPBCTRL.all  = 0x00000000;		
	// No qualification for all group B GPIO 32-47
	GpioCtrlRegs.GPBQSEL1.all = 0x00000000;		
	// No qualification for all group B GPIO 48-63
	GpioCtrlRegs.GPBQSEL2.all = 0x00000000;		
	// All group B GPIO are inputs
	GpioCtrlRegs.GPBDIR.all   = 0x00000000;		
	// All group B pullups disabled due to external pull down
	GpioCtrlRegs.GPBPUD.all   = 0xFFFFFFFF;		

	// GPIO32/33 are used as I2C port
	GpioCtrlRegs.GPBMUX1.bit.GPIO32 = 1;		// 0=GPIO  1=SDAA      2=EPWMSYNCI  
												// 3=ADCSOCAO
	GpioCtrlRegs.GPBMUX1.bit.GPIO33 = 0;		// 0=GPIO  1=SCLA      2=EPWMSYNCO  
												// 3=ADCSOCBO
    GpioCtrlRegs.GPBDIR.bit.GPIO33 = 1;			// Configures the GPIO pin as an output		

	// GPIO34 is used for input capcitor charging(one) DC+. CNTLR1
	GpioCtrlRegs.GPBMUX1.bit.GPIO34 = 0;		// 0=GPIO  1=ECAP1     2=XREADY     
												// 3=XREADY
	// used for SCIA for test or flash
    GpioCtrlRegs.GPBMUX1.bit.GPIO35 = 0x2;		// 0=GPIO  1=SCITXDA   2=XR/W       3=XR/W     */
   
    // GPIO36 is used for ipput capcitor charging(two) DC-. CNTLR2
    GpioCtrlRegs.GPBMUX1.bit.GPIO36 = 0;		// 0=GPIO  1=SCIRXDA   2=XZCS0      3=XZCS0	   */

	// GPIO37 Input pin unused.   
	GpioCtrlRegs.GPBMUX1.bit.GPIO37 = 0x2;		// 0=GPIO  1=ECAP2     2=XZCS7      
												// 3=XZCS7
	// GPIO38 is unused input pin.
	GpioCtrlRegs.GPBMUX1.bit.GPIO38 = 0x2;		// 0=GPIO  1=rsvd      2=XWE0       
												// 3=XWE0
	// GPIO39 input pin for FPGA status
	GpioCtrlRegs.GPBMUX1.bit.GPIO39 = 0;		// 0=GPIO  1=rsvd      2=XA16       
												// 3=XA16
	// GPIO40-47 is used for communication with FPGA
	GpioCtrlRegs.GPBMUX1.bit.GPIO40 = 0x2;		// 0=GPIO  1=rsvd      
												// 2=XA0/XWE1   3=XA0/XWE1
	GpioCtrlRegs.GPBMUX1.bit.GPIO41 = 0x2;		// 0=GPIO  1=rsvd      2=XA1        
												// 3=XA1
	GpioCtrlRegs.GPBMUX1.bit.GPIO42 = 0x2;		// 0=GPIO  1=rsvd      2=XA2        
												// 3=XA2
	GpioCtrlRegs.GPBMUX1.bit.GPIO43 = 0x2;		// 0=GPIO  1=rsvd      2=XA3        
												// 3=XA3
	GpioCtrlRegs.GPBMUX1.bit.GPIO44 = 0x2;		// 0=GPIO  1=rsvd      2=XA4        
												// 3=XA4
	GpioCtrlRegs.GPBMUX1.bit.GPIO45 = 0x2;		// 0=GPIO  1=rsvd      2=XA5        
												// 3=XA5
	GpioCtrlRegs.GPBMUX1.bit.GPIO46 = 0x2;		// 0=GPIO  1=rsvd      2=XA6        
												// 3=XA6
	GpioCtrlRegs.GPBMUX1.bit.GPIO47 = 0x2;		// 0=GPIO  1=rsvd      2=XA7        
												// 3=XA7

	// GPIO48 is used for DSP_A_SYNC_IN
	GpioCtrlRegs.GPBMUX2.bit.GPIO48 = 0x1;		// 0=GPIO  1=ECAP5     2=XD31       
												// 3=XD31
	GpioCtrlRegs.GPBPUD.bit.GPIO48 = 0;	

    // GPIO49,50 are used for EEPROM Read/write control
	GpioCtrlRegs.GPBMUX2.bit.GPIO49 = 0;		// 0=GPIO  1=ECAP6     2=XD30       
												// 3=XD30
	GpioCtrlRegs.GPBMUX2.bit.GPIO50 = 0;		// 0=GPIO  1=EQEP1A    2=XD29       
												// 3=XD29
	// GPIO51 is used for control I2C
	GpioCtrlRegs.GPBMUX2.bit.GPIO51 = 0;		// 0=GPIO  1=EQEP1B    2=XD28       
												// 3=XD28
    // GPIO52 is used for BUSOVP input 
	GpioCtrlRegs.GPBMUX2.bit.GPIO52 = 0;		// 0=GPIO  1=EQEP1S    2=XD27       
												// 3=XD27
    // GPIO53 is used for AC OUTOCP input 
	GpioCtrlRegs.GPBMUX2.bit.GPIO53 = 0;		// 0=GPIO  1=EQEP1I    2=XD26   3=XD26     

	//GPIO54 is unused										
	GpioCtrlRegs.GPBMUX2.bit.GPIO54 = 1;		// 0=GPIO  1=SPISIMOA  2=XD25   3=XD25     
	
	//GPIO55 is unused											
	GpioCtrlRegs.GPBMUX2.bit.GPIO55 = 1;		// 0=GPIO  1=SPISOMIA  2=XD24  	3=XD24     
	
	//GPIO56 is unused										
	GpioCtrlRegs.GPBMUX2.bit.GPIO56 = 1;		// 0=GPIO  1=SPICLKA   2=XD23 	3=XD23

	//GPIO57 is unused
	GpioCtrlRegs.GPBMUX2.bit.GPIO57 = 1;		// 0=GPIO  1=SPISTEA   2=XD22       
												// 3=XD22
	//CNTLR15, is unused.
	GpioCtrlRegs.GPBMUX2.bit.GPIO58 = 0;		// 0=GPIO  1=MCLKRA    2=XD21       
												// 3=XD21
	//CNTLR14, fan3 High dang.
	GpioCtrlRegs.GPBMUX2.bit.GPIO59 = 0;		// 0=GPIO  1=MFSRA     2=XD20       
												// 3=XD20
	//CNTLR13, fan3 Middle dang.
	GpioCtrlRegs.GPBMUX2.bit.GPIO60 = 0;		// 0=GPIO  1=MCLKRB    2=XD19       
												// 3=XD19
	//CNTLR11, fan2 High dang.
	GpioCtrlRegs.GPBMUX2.bit.GPIO61 = 0;		// 0=GPIO  1=MFSRB     2=XD18       
												// 3=XD18
	// GPIO62/63 are used as SCIcRX/TX, to redunadant
	GpioCtrlRegs.GPBMUX2.bit.GPIO62 = 1;		// 0=GPIO  1=SCIRXDC   2=XD17       
												// 3=XD17
	GpioCtrlRegs.GPBMUX2.bit.GPIO63 = 1;		// 0=GPIO  1=SCITXDC   2=XD16       
												// 3=XD16

	/* Group C pins */	
	GpioCtrlRegs.GPCDIR.all = 0x00000000;		// All group C GPIO are inputs
	GpioCtrlRegs.GPCPUD.all = 0xFFFFFFFF;		// All group C pullups disabled
												// due to external pull down
//	GpioCtrlRegs.GPCPUD.bit.GPIO83 = 0;		  // All group C pullups disabled
	
	// CNTLR12, used for fan3 Low dang
	GpioCtrlRegs.GPCMUX1.bit.GPIO64 = 0;		// 0=GPIO  1=GPIO      2=XD15       
												// 3=XD15
    //CNTLR9, fan2 Low dang.
	GpioCtrlRegs.GPCMUX1.bit.GPIO65 = 0;		// 0=GPIO  1=GPIO      2=XD14       
												// 3=XD14
	//CNTLR10, fan2 Middle dang.
	GpioCtrlRegs.GPCMUX1.bit.GPIO66 = 0;		// 0=GPIO  1=GPIO      2=XD13       
												// 3=XD13
    //CNTLR7, fan1 Middle dang.
	GpioCtrlRegs.GPCMUX1.bit.GPIO67 = 0;		// 0=GPIO  1=GPIO      2=XD12       
	//CNTLR8, fan1 High dang.											// 3=XD12
	GpioCtrlRegs.GPCMUX1.bit.GPIO68 = 0;		// 0=GPIO  1=GPIO      2=XD11       
												// 3=XD11

	//is unused now, CNTLR5.
	GpioCtrlRegs.GPCMUX1.bit.GPIO69 = 0;		// 0=GPIO  1=GPIO      2=XD10       
												// 3=XD10
    //GPIO70-71 is used for FAN1, Low dang,CNTLR6
	GpioCtrlRegs.GPCMUX1.bit.GPIO70 = 0;		// 0=GPIO  1=GPIO      2=XD9        
												// 3=XD9
	//电容泄放回路,CNTLR3 for discharge of the input Capacitor.
	GpioCtrlRegs.GPCMUX1.bit.GPIO71 = 0;		// 0=GPIO  1=GPIO      2=XD8        
												// 3=XD8

    //GPIO72-79 are used for A_XD7-0.
	GpioCtrlRegs.GPCMUX1.bit.GPIO72 = 0x2;		// 0=GPIO  1=GPIO      2=XD7        
												// 3=XD7
	GpioCtrlRegs.GPCMUX1.bit.GPIO73 = 0x2;		// 0=GPIO  1=GPIO      2=XD6        
												// 3=XD6
	GpioCtrlRegs.GPCMUX1.bit.GPIO74 = 0x2;		// 0=GPIO  1=GPIO      2=XD5        
												// 3=XD5
	GpioCtrlRegs.GPCMUX1.bit.GPIO75 = 0x2;		// 0=GPIO  1=GPIO      2=XD4        
												// 3=XD4
	GpioCtrlRegs.GPCMUX1.bit.GPIO76 = 0x2;		// 0=GPIO  1=GPIO      2=XD3        
												// 3=XD3
	GpioCtrlRegs.GPCMUX1.bit.GPIO77 = 0x2;		// 0=GPIO  1=GPIO      2=XD2        
												// 3=XD2
	GpioCtrlRegs.GPCMUX1.bit.GPIO78 = 0x2;		// 0=GPIO  1=GPIO      2=XD1        
												// 3=XD1
	GpioCtrlRegs.GPCMUX1.bit.GPIO79 = 0x2;		// 0=GPIO  1=GPIO      2=XD0        
												// 3=XD0

	// GPIO80-81 are used for A_XA8-9
	GpioCtrlRegs.GPCMUX2.bit.GPIO80 = 0x2;		// 0=GPIO  1=GPIO      2=XA8        
												// 3=XA8
	GpioCtrlRegs.GPCMUX2.bit.GPIO81 = 0x2;		// 0=GPIO  1=GPIO      2=XA9        
												// 3=XA9
	// GPIO82-83 are used for light2,1.
	GpioCtrlRegs.GPCMUX2.bit.GPIO82 = 0;		// 0=GPIO  1=GPIO      2=XA10, unused       
												// 3=XA10
	GpioCtrlRegs.GPCMUX2.bit.GPIO83 = 0;		// 0=GPIO  1=GPIO      2=XA11       
												// 3=XA11
	// GPIO84-86 are used for state back for fan10-9.
	GpioCtrlRegs.GPCMUX2.bit.GPIO84 = 0;		// 0=GPIO  1=GPIO      2=XA12       
												// 3=XA12
	GpioCtrlRegs.GPCMUX2.bit.GPIO85 = 0;		// 0=GPIO  1=GPIO      2=XA13       
												// 3=XA13
	GpioCtrlRegs.GPCMUX2.bit.GPIO86 = 0;		// 0=GPIO  1=GPIO      2=XA14       
												// 3=XA14
    // GPIO87 is used for FPGA_ACK.
	GpioCtrlRegs.GPCMUX2.bit.GPIO87 = 0;		// 0=GPIO  1=GPIO      2=XA15       
												// 3=XA15
/*------------------------------------------------------------------------
    GPIO direction and initial status settings
  -----------------------------------------------------------------------*/
	// GPIO18 Output pin for ISO ground relay2 												
	GpioDataRegs.GPACLEAR.bit.GPIO18 = 1;		// output 0 by default	 
	//GpioDataRegs.GPASET.bit.GPIO18 = 1;		    // output 0 by default	
	GpioCtrlRegs.GPADIR.bit.GPIO18 = 1;			// Configures the GPIO pin as an output

	// GPIO19 Output pin for ISO ground relay1												
	GpioDataRegs.GPACLEAR.bit.GPIO19 = 1;		// output 0 by default	
	//GpioDataRegs.GPASET.bit.GPIO19 = 1;		// output 0 by default		 
	GpioCtrlRegs.GPADIR.bit.GPIO19 = 1;			// Configures the GPIO pin as an output

    // GPIO22 Output pin for AD 温度采样多路选择1											
	GpioDataRegs.GPACLEAR.bit.GPIO22 = 1;		// output 0 by default	
    //GpioDataRegs.GPASET.bit.GPIO22 = 1;		// output 0 by default	
	GpioCtrlRegs.GPADIR.bit.GPIO22 = 1;		// Configures the GPIO pin as an output

	// GPIO23 Output pin for AD 温度采样多路选择2										
	GpioDataRegs.GPACLEAR.bit.GPIO23 = 1;		// output 0 by default
	//GpioDataRegs.GPASET.bit.GPIO23 = 1;		// output 0 by default		
	GpioCtrlRegs.GPADIR.bit.GPIO23 = 1;			// Configures the GPIO pin as an output

	// GPIO24 Output pin for AD 温度采样多路选择3										
	GpioDataRegs.GPACLEAR.bit.GPIO24 = 1;		// output 0 by default
	//GpioDataRegs.GPASET.bit.GPIO24 = 1;		// output 0 by default		
	GpioCtrlRegs.GPADIR.bit.GPIO24 = 1;			// Configures the GPIO pin as an output

	// GPIO28 Output pin for 继电器4 signal，交流接触器
	GpioDataRegs.GPACLEAR.bit.GPIO28 = 1;		// output 0 by default	
	//GpioDataRegs.GPASET.bit.GPIO28 = 1;		    // output 0 by default		
	GpioCtrlRegs.GPADIR.bit.GPIO28 = 1;			// Configures the GPIO pin as an output 


	// GPIO34 Output pin for  继电器1 signal, DC capacitor charge1
	GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;			// output 0 by default	
	//GpioDataRegs.GPBSET.bit.GPIO34 = 1;		// output 0 by default		
	GpioCtrlRegs.GPBDIR.bit.GPIO34 = 1;				// Configures the GPIO pin as an output 

	// GPIO36 Output pin for 继电器2 signal,DC capacitor charge2
	GpioDataRegs.GPBCLEAR.bit.GPIO36 = 1;				// output 1 by default	 
	//GpioDataRegs.GPBSET.bit.GPIO36 = 1;		// output 0 by default		
	GpioCtrlRegs.GPBDIR.bit.GPIO36 = 1;				// Configures the GPIO pin as an output 

	GpioDataRegs.GPBCLEAR.bit.GPIO39 = 1;				// output 1 by default	 
	//GpioDataRegs.GPBSET.bit.GPIO39 = 1;		// output 0 by default		
	GpioCtrlRegs.GPBDIR.bit.GPIO39 = 1;				// Configures the GPIO pin as an output 

	// GPIO47 Output pin for input2 Insulation test relay control 
//	GpioDataRegs.GPBCLEAR.bit.GPIO47 = 1;		// output 1 by default	 
//	GpioCtrlRegs.GPBDIR.bit.GPIO47 = 1;			// RS485 TX enable Control, 
												
	// GPIO49,50 output pin to control EEPROM W/R,
	GpioDataRegs.GPBCLEAR.bit.GPIO49 = 1;		// output 0 by default	 
	//GpioDataRegs.GPBSET.bit.GPIO49 = 1;		// output 0 by default		
	GpioCtrlRegs.GPBDIR.bit.GPIO49 = 1;	
		
	GpioDataRegs.GPBCLEAR.bit.GPIO50 = 1;		// output 0 by default	 
	//GpioDataRegs.GPBSET.bit.GPIO50 = 1;		// output 0 by default		
	GpioCtrlRegs.GPBDIR.bit.GPIO50 = 1;		
	//Relay
	
	//I2C control GPIO51.
	GpioDataRegs.GPBCLEAR.bit.GPIO51 = 1;		// output 0 by default	 
	//GpioDataRegs.GPBSET.bit.GPIO51 = 1;		// output 0 by default		
	GpioCtrlRegs.GPBDIR.bit.GPIO51 = 1;		
	//Relay

	// GPIO58,59,60,61,64,65,66,67,68,69,70,71, output pin to control 继电器控制板,
	GpioDataRegs.GPBCLEAR.bit.GPIO58 = 1;		// output 0 by default	 
	//GpioDataRegs.GPBSET.bit.GPIO58 = 1;		// output 0 by default		
	GpioCtrlRegs.GPBDIR.bit.GPIO58 = 1;	    	// GPIO58 Output pin for  继电器15 signal

	GpioDataRegs.GPBCLEAR.bit.GPIO59 = 1;		// output 0 by default	 
	//GpioDataRegs.GPBSET.bit.GPIO59 = 1;		// output 0 by default		
	GpioCtrlRegs.GPBDIR.bit.GPIO59 = 1;	        // GPIO59 Output pin for  继电器14 signal
		
	GpioDataRegs.GPBCLEAR.bit.GPIO60 = 1;		// output 0 by default	 
	//GpioDataRegs.GPBSET.bit.GPIO60 = 1;		// output 0 by default		
	GpioCtrlRegs.GPBDIR.bit.GPIO60 = 1;	        // GPIO60 Output pin for  继电器13 signal
		
	GpioDataRegs.GPBCLEAR.bit.GPIO61 = 1;		// output 0 by default	 
	//GpioDataRegs.GPBSET.bit.GPIO61 = 1;		// output 0 by default		
	GpioCtrlRegs.GPBDIR.bit.GPIO61 = 1;	        // GPIO61 Output pin for  继电器11 signal

	GpioDataRegs.GPCCLEAR.bit.GPIO64 = 1;		// output 0 by default	 
	//GpioDataRegs.GPCSET.bit.GPIO64 = 1;		// output 0 by default		
	GpioCtrlRegs.GPCDIR.bit.GPIO64 = 1;		    // GPIO64 Output pin for  继电器12 signal

	GpioDataRegs.GPCCLEAR.bit.GPIO65 = 1;		// output 0 by default	 
	//GpioDataRegs.GPCSET.bit.GPIO65 = 1;		// output 0 by default		
	GpioCtrlRegs.GPCDIR.bit.GPIO65 = 1;	        // GPIO65 Output pin for  继电器9 signal

	GpioDataRegs.GPCCLEAR.bit.GPIO66 = 1;		// output 0 by default	 
	//GpioDataRegs.GPCSET.bit.GPIO66 = 1;		// output 0 by default		
	GpioCtrlRegs.GPCDIR.bit.GPIO66 = 1;	        // GPIO66 Output pin for  继电器10 signal
		
	GpioDataRegs.GPCCLEAR.bit.GPIO67 = 1;		// output 0 by default	 
	//GpioDataRegs.GPCSET.bit.GPIO67 = 1;		// output 0 by default		
	GpioCtrlRegs.GPCDIR.bit.GPIO67 = 1;	        // GPIO67 Output pin for  继电器7signal

	GpioDataRegs.GPCCLEAR.bit.GPIO68 = 1;		// output 0 by default	 
	//GpioDataRegs.GPCSET.bit.GPIO68 = 1;		// output 0 by default		
	GpioCtrlRegs.GPCDIR.bit.GPIO68 = 1;	        // GPIO68 Output pin for  继电器8 signal
		
	GpioDataRegs.GPCCLEAR.bit.GPIO69 = 1;		// output 0 by default	 
	//GpioDataRegs.GPCSET.bit.GPIO69 = 1;		// output 0 by default		
	GpioCtrlRegs.GPCDIR.bit.GPIO69 = 1;         // GPIO69 Output pin for  继电器5signal

	//AC connecttor GPIO70-71---2,1
	GpioDataRegs.GPCCLEAR.bit.GPIO70 = 1;		// output 0 by default	 
	//GpioDataRegs.GPCSET.bit.GPIO70 = 1;		// output 0 by default		
	GpioCtrlRegs.GPCDIR.bit.GPIO70 = 1;	        // GPIO70 Output pin for  继电器6 signal
		
	GpioDataRegs.GPCCLEAR.bit.GPIO71 = 1;		// output 0 by default	 
	//GpioDataRegs.GPCSET.bit.GPIO71 = 1;		// output 0 by default		
	GpioCtrlRegs.GPCDIR.bit.GPIO71 = 1;		    // GPIO71 Output pin for  继电器3 signal

	// GPIO82,83 output pin to control 继电器控制板灯,
	GpioDataRegs.GPCCLEAR.bit.GPIO82 = 1;		// output 0 by default	 
	//GpioDataRegs.GPCSET.bit.GPIO82 = 1;		// output 0 by default		
	GpioCtrlRegs.GPCDIR.bit.GPIO82 = 1;	      // GPIO70 Output pin for  继电器指示灯 signal
		
	GpioDataRegs.GPCCLEAR.bit.GPIO83 = 1;		// output 0 by default	 
	//GpioDataRegs.GPCSET.bit.GPIO83 = 1;		// output 0 by default		
	GpioCtrlRegs.GPCDIR.bit.GPIO83 = 1;		  // GPIO71 Output pin for  继电器指示灯signal


	/* Re-assign Input qualification for every digital input according to above 
	settings:
 	(1) 6 samples, (2) no qualification is assigned to Voltage zero crossing 
 	detection port(GPIO23/24/25), and (3)
 	00-Syn with SysClock, 01- 3 samples, 10- 6 samples, 11- Asyn  */
	// GPIO12 input pin for AC OverCurrentProtection.may be can be used as TZ1,
//	GpioCtrlRegs.GPAQSEL1.bit.GPIO12 = 0x2;
	// GPIO13 input pin for boost1 OverCurrentProtection 			
//	GpioCtrlRegs.GPAQSEL1.bit.GPIO13 = 0x2;	

	// GPIO29 input pin for FPGA Configuration	
	GpioCtrlRegs.GPAQSEL2.bit.GPIO29 = 0x2;	

	// GPIO39 Input pin for test FPGA status.		
	GpioCtrlRegs.GPBQSEL1.bit.GPIO39 = 0x2;		

	// GPIO52 input pin used to detect Aux ok or not		
	GpioCtrlRegs.GPBQSEL2.bit.GPIO52 = 0x2;	

	// GPIO53 input pin used to detect Vc frequency		
	GpioCtrlRegs.GPBQSEL2.bit.GPIO53 = 0x2;	
	


	/* External interrupt selection */
	GpioIntRegs.GPIOXINT1SEL.all = 0x0000;		// GPIO0 is XINT1 source
	GpioIntRegs.GPIOXINT2SEL.all = 0x0000;		// GPIO0 is XINT2 source
	GpioIntRegs.GPIOXINT3SEL.all = 0x0000;		// GPIO32 is XINT3 source
	GpioIntRegs.GPIOXINT4SEL.all = 0x0000;		// GPIO32 is XINT4 source
	GpioIntRegs.GPIOXINT5SEL.all = 0x0000;		// GPIO32 is XINT5 source
	GpioIntRegs.GPIOXINT6SEL.all = 0x0000;		// GPIO32 is XINT6 source
	GpioIntRegs.GPIOXINT7SEL.all = 0x0000;		// GPIO32 is XINT7 source
	GpioIntRegs.GPIOXNMISEL.all = 0x0000;		  // GPIO0 is XNMI source

	XIntruptRegs.XINT1CR.all = 0x0000;			// XINT1 disabled
	XIntruptRegs.XINT2CR.all = 0x0000;			// XINT2 disabled
	XIntruptRegs.XINT3CR.all = 0x0000;			// XINT3 disabled
	XIntruptRegs.XINT4CR.all = 0x0000;			// XINT4 disabled
	XIntruptRegs.XINT5CR.all = 0x0000;			// XINT5 disabled
	XIntruptRegs.XINT6CR.all = 0x0000;			// XINT6 disabled
	XIntruptRegs.XINT7CR.all = 0x0000;			// XINT7 disabled
	XIntruptRegs.XNMICR.all = 0x0000;			  // XNMI disabled

	/* Low-power mode selection */
	// Any one of GPIO 0-31 can be used as LPM wakeup pin.
	// No pin selected for HALT and STANBY wakeup (reset default)
	GpioIntRegs.GPIOLPMSEL.all = 0x00000000;	



	/* Finish up */
	asm(" EDIS");			// Disable EALLOW protected register access								

} // end InitGpio()






//--- end of file -----------------------------------------------------
