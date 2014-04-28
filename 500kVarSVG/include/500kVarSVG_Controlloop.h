/*=============================================================================*
 *  Copyright(c) 2012-2013, ALL RIGHTS RESERVED
 *
 *  FILENAME : 500KW_Controlloop.h 
 *  PURPOSE  : header files 500KW_Controlloop.c 
 *			       define constant, struct declaration, extern varibles 
 *
 *  HISTORY  :
 *    DATE:2012.12.20      VERSION:V1.0        AUTHOR:WangWanBao            NOTE
 *    
 *
 ******************************************************************************/

#ifndef REGULATOR_H
#define REGULATOR_H


/* General Purpose PID Controller, Data type */
// PID regulator with inner staturation, parameters definition
struct CURRENTCONTREG	
{  
//	float32  f32Ialpha;   		// Input: Reference input 
//	float32  f32Ibeta;   		// Input: Feedback input 
//	float32  f32Id;				// Variable: Error 
//	float32  f32Iq;				// Parameter: Proportional gain  
	float32	 f32Id_5thHar_Out;
	float32	 f32Id_5thHar_k_2;
	float32	 f32Id_5thHar_k_1;
	float32	 f32Id_5thHar_k;
	float32	 f32Id_11thHar_Out;
	float32	 f32Id_11thHar_k_2;
	float32	 f32Id_11thHar_k_1;
	float32	 f32Id_11thHar_k;
	float32	 f32Id_k;
	float32	 f32Id_Filter;
//	float32	 f32Id_Filter_k_1;
	float32  f32Id_Harmonic;    //d axis harmonic
	float32  f32Id_Error_k;		// Parameter: Integral correction gain
	float32  f32Id_Error_k_1; 	// Parameter: Derivative gain 
	float32  f32Id_Error_k_2; 	// Parameter: Derivative Error 
	float32  f32DutyD_Con;		// History: Previous proportional output
	float32  f32DutyD_Dec;		// Decoupling of D axis

//	float32	 f32Iq_Filter_k_1;
	float32	 f32Iq_5thHar_Out;
	float32	 f32Iq_5thHar_k_2;
	float32	 f32Iq_5thHar_k_1;
	float32	 f32Iq_5thHar_k;
	float32	 f32Iq_11thHar_Out;
	float32	 f32Iq_11thHar_k_2;
	float32	 f32Iq_11thHar_k_1;
	float32	 f32Iq_11thHar_k;
	float32	 f32Iq_k;
	float32	 f32Iq_Filter;
	float32  f32Iq_Harmonic;
	float32  f32Iq_Error_k;		// Parameter: Integral correction gain
	float32  f32Iq_Error_k_1; 	// Parameter: Derivative gain
	float32  f32Iq_Error_k_2; 	// Parameter: Derivative Error  
	float32  f32DutyQ_Con;		// History: Previous proportional output
	float32  f32DutyQ_Dec;      // Decoupling of Q axis
	
	float32  f32DutyDP;			// Variable: Saturated difference
	float32  f32DutyQP;			// Parameter: Integral gain
//	float32  f32DutyDN;			// Variable: Saturated difference
//	float32  f32DutyQN;
//	float32  f32DutyDQ;			// Parameter: Integral correction gain
	float32  f32DutyDP_ff; 		// Parameter: Derivative gain(forward feed grid voltage) 
	float32  f32DutyQP_ff; 		// Parameter: Derivative gain(forward feed grid voltage) 
	float32  f32DutyDN_ff; 		// Parameter: Derivative gain(forward feed grid voltage) 
	float32  f32DutyQN_ff; 		// Parameter: Derivative gain(forward feed grid voltage) 
//	float32  f32DutyAlpha;		// Parameter: Integral correction gain Kp-Ki
//	float32  f32DutyBeta; 		// Parameter: Proportional gain 
//	float32  f32DutyKd; 		// Parameter: Derivative gain 
//	float32  f32Duty_3d;		// History: Previous proportional output
	float32  f32Ta;				// Parameter: Integral correction gain
	float32  f32Tb; 			// Parameter: Derivative gain 
	float32  f32Tc;				// History: Previous proportional output
	Uint16	 u16Flg_DriveOpen;
	float32	 f32HarmCoff;
};	            
extern	struct	CURRENTCONTREG	CurrConReg;

struct PLLCONTREG	
{  
	float32  f32Valpha;   		// Middle Parameter:PLL Clark trans output f32Valpha
	float32  f32Vbeta;   	  	// Middle Parameter:PLL Clark trans output f32Vbeta
	float32  f32VdP;		    // Middle Parameter:PLL PARK trans output f32VdP
	float32  f32VqP;			// Middle Parameter:PLL PARK trans output f32VqP
	float32	 f32VdN;			// Middle Parameter:PLL PARK trans output f32VdN
	float32  f32VqN;			// Middle Parameter:PLL PARK trans output f32VqN
	float32	 f32VGrid_Amp;      // Middle Parameter: grid 
	float32	 f32VGrid_Sin_Theta;// Middle Parameter: grid theta's sin
	float32	 f32VGrid_Cos_Theta;// Middle Parameter: grid theta's cos
	float32  f32ThetaErr_Old;	// Variable: PLL err(k-1) input  
	float32  f32ThetaErr_New;	// Variable: PLL err(k) input  
	float32  f32Theta_Step;		// Output: PLL output Theta step
	float32  f32Theta;   		// Output: PLL output theta
	float32  Sin_Theta;		    // Output: PID output theta sin
	float32  Cos_Theta;			// Output: PID output theta cos
//	float32	 Sin_5Theta;
//	float32	 Cos_5Theta; 
//	float32  Sin_AITheta;		// Variable: Saturated difference
//	float32  Cos_AITheta;		// Parameter: Integral gain
//	float32  f32Theta_120;   	// Output: PID output
//	float32  f32Theta120;   	// Output: PID output
//	float32  Sin_Theta_120;		// Variable: Saturated difference
//	float32  Cos_Theta_120;		// Parameter: Integral gain
//	float32  Sin_Theta120;		// Variable: Saturated difference
//	float32  Cos_Theta120;		// Parameter: Integral gain
//	float32  f32Fre_Max;
//	float32  f32Fre_Min;
	float32  f32Fre_Delta_k;   // frequency err parameter for protection
//	float32  f32Fre_Delta_k_1;
	float32	 f32DQ_PLL_Lockin; // bit to check PLL is ok, Vqp
};	            
extern	struct	PLLCONTREG	PLLConReg;


/*=============================================================================*
 * 	Extern functions declaration
 *============================================================================*/

/* Initialize the starting Duty and loop PID parameters */
extern void InverterStage_Init(void);     //Inverter parameter Initialization
extern void ADC_INT_Control(void);
extern void Can_Carbox_Transmit(void);
extern int16 swGetStartADIsrPoint(void);
extern int16 swGetEndADIsrPoint(void);
extern int16 swGetEndCONPoint(void);
extern int16 STATUS_AC_RELAY1_ON;
//extern int16 Test_Start_of_SEQISR;

#endif

//--- end of file -----------------------------------------------------
