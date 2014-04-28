
//* Define system level clocks 
//* System Clock frequency 150MHz
//* Switching freqnecy 3kHz
//* control frequency 6kHz
// High Speed Clock 150MHz

#ifndef MAIN_HEADER_H
#define MAIN_HEADER_H

#define SYSCLOCK_FREQ		150000000
#define HSPCLK_FREQ			(SYSCLOCK_FREQ/2)		// defined in SysCtrlRegs, this is parameter
#define LSPCLK_FREQ			(SYSCLOCK_FREQ/4)		// defined in SysCtrlRegs

#define PWM_FREQ			3000					//PWM frequency
	
/* (period/2) for SPWM with 75MHz TBCLK */		 
#define PWM_PERIOD	    	(HSPCLK_FREQ/PWM_FREQ)  //PWM Frequency counter value(25000)
#define DeadBand_Duration	250	 //250*2=5us,375-->7.5us  // Deadband time (3.33uS)
#define PWM_HALF_PERIOD		((HSPCLK_FREQ/PWM_FREQ)/2)    // PWM frequency(up-down model) (12500) /* (period/2) for SPWM with 75MHz TBCLK */		 


/* DSP/BIOS Macros for ADC calibration */

#define CPU_RATE      6.667L   // for a 150MHz CPU clock speed (SYSCLKOUT)
//#define CPU_RATE    7.143L   // for a 140MHz CPU clock speed (SYSCLKOUT)
//#define CPU_RATE    8.333L   // for a 120MHz CPU clock speed (SYSCLKOUT)
//#define CPU_RATE   10.000L   // for a 100MHz CPU clock speed (SYSCLKOUT)
//#define CPU_RATE   13.330L   // for a 75MHz CPU clock speed (SYSCLKOUT)
//#define CPU_RATE   20.000L   // for a 50MHz CPU clock speed  (SYSCLKOUT)
//#define CPU_RATE   33.333L   // for a 30MHz CPU clock speed  (SYSCLKOUT)
//#define CPU_RATE   41.667L   // for a 24MHz CPU clock speed  (SYSCLKOUT)
//#define CPU_RATE   50.000L   // for a 20MHz CPU clock speed  (SYSCLKOUT)
//#define CPU_RATE   66.667L   // for a 15MHz CPU clock speed  (SYSCLKOUT)
//#define CPU_RATE  100.000L   // for a 10MHz CPU clock speed  (SYSCLKOUT)

#define DelayUs(A)  DSP28x_usDelay(((((long double) A * 1000.0L) / (long double)CPU_RATE) - 9.0L) / 5.0L)

/*=============================================================================*
 * 	Includings 
 *============================================================================*/

/* Include Standard C Language Header Files */
#include <string.h>

/* Include 自定义 generated Header Files */
#include "C28x_FPU_FastRTS.h" 
#include "500kVarSVG_DataAcquisition.h"
#include "500kVarSVG_Controlloop.h"
#include "500kVarSVG_Clarke.h"
#include "500kVarSVG_Park.h"
#include "500kVarSVG_ipark.h"
#include "500kVarSVG_Protection.h"
#include "500kVarSVG_StateUpdate.h"
//#include "500KW_Chebyshev_LP.h"
#include "500kVarSVG_SVPWM.h"
#include "500kVarSVG_f28335pwm.h"
#include "500kVarSVG_Sci_LCD.h"


/* Include user defined Header Files */
//#include "Adc.h"
//#include "Ecan.h"



/*=============================================================================*
 * 	Global Variables for system level initialization
 *============================================================================*/

/* BIOS symbols defined in the linker command file */

/* User defined symbols in the linker command file */




extern void InitEPwm(void);
extern void InitECap(void);
extern void InitAdc(void);

/*=============================================================================*
 * 	Global functions for system level initialization
 *============================================================================*/

/* System level initialization */
extern void InitGpio(void);
extern void InitPieCtrl(void);
extern void InitSysCtrl(void);
extern void InitWatchdog(void);
extern void SetDBGIER(Uint16);
extern void InitFlash(void);
extern void UserInit(void);
extern void InitECan(void);
extern void InitECan1(void);
//extern void InitI2C(void);
extern void InitXintf(void);

extern void InitSpi(void);          //初始化Spi模块，用于主副DSP之间通讯
extern void Spi_fifo_init(void);    //初始化Spi模块，用于主副DSP之间通讯

#endif /*500KW_MAIN_HEADER_*/
