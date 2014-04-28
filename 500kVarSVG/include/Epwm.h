/*=============================================================================*
 *         Copyright(c) 2009-2011, Convertergy Co., Ltd.
 *                          ALL RIGHTS RESERVED
 *
 *  FILENAME : Epwm.h 
 *
 *  PURPOSE  : Header file for EPwm.c
 *  
 *  HISTORY  :
 *    DATE            VERSION        AUTHOR            NOTE
 *    2011-02-16      V0.1           Ken      	    Created
 *
 *
 *============================================================================*/

#include "DSP2833x_Device.h"			// Peripheral address definitions
#include "5KW_MAINHEADER.h"				// Main include file

#ifndef EPWM_H
#define EPWM_H

/*=============================================================================*
 * 	Extern functions declaration
 *============================================================================*/
/* ePWM module initialization */


/* Start ePWM timer5 for ADC SOC by PWM module */
//extern void StartEPWM5_TimerForADC(void);

/* Start ePWM timer6 for GeneralADC */
//extern void StartEPWM6_Timer(void);

/* Enable PWM output */
extern void PWMOutputsEnable(void);

/* Disable PWM output */
extern void PWMOutputsDisable(void);

/*-----------------------------------------------------------------------------
Define the structure of the PWM Driver Object 
-----------------------------------------------------------------------------*/
typedef struct {   
        Uint32 PeriodMax;     // Parameter: PWM Half-Period in CPU clock cycles (Q0)
        float32 MfuncPeriod;    // Input: Period scaler (Q15) 2^15 
        float32 MfuncC1;        // Input: EPWM1 A&B Duty cycle ratio (Q15)
        float32 MfuncC2;        // Input: EPWM2 A&B Duty cycle ratio (Q15) 
        float32 MfuncC3;        // Input: EPWM3 A&B Duty cycle ratio (Q15)
        float32 MfuncC4;        // Input: EPWM4 A&B Duty cycle ratio (Q15)
        float32 MfuncC5;        // Input: EPWM5 A&B Duty cycle ratio (Q15)
        float32 MfuncC6;        // Input: EPWM6 A&B Duty cycle ratio (Q15)
        float32 TaOffset;
        float32 TbOffset;
        float32 TcOffset;
        void (*update)();     // Pointer to the update function 
        } PWMGEN ;    

/*-----------------------------------------------------------------------------
Define a PWMGEN_handle
-----------------------------------------------------------------------------*/
typedef PWMGEN *PWMGEN_handle;

/*------------------------------------------------------------------------------
Default Initializers for the F2833X PWMGEN Object 
------------------------------------------------------------------------------*/
#define F2833X_FC_PWM_GEN    {1000,   \
                              0x7FFF, \
                              0x4000, \
                              0x4000, \
                              0x4000, \
                              0x4000, \
                              0x4000, \
                              0x4000, \
                              0x0000,\
                              0x0000,\
                              0x0000,\
                           (void (*)(Uint32))F2833X_PWM_Update \
                             }

#define PWMGEN_DEFAULTS 	F2833X_FC_PWM_GEN
/*------------------------------------------------------------------------------
 Prototypes for the functions in F2833XPWM.C
------------------------------------------------------------------------------*/
//void F2833X_PWM_Init(PWMGEN_handle);
void F2833X_PWM_Update(PWMGEN_handle);


#endif

//--- end of file -----------------------------------------------------

