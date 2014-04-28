/*=============================================================================*
 *  Copyright(c) 
 *                          ALL RIGHTS RESERVED
 *
 *  FILENAME : 500KW_Controlloop.c 
 *   
 *  PURPOSE  : SPWM algorithm process for control loop and 28335 specific board
 *				     for 500KW PV Inverter.
 *  
 *  HISTORY  :
 *    DATE            VERSION        AUTHOR            NOTE
 *    
 *
 *----------------------------------------------------------------------------
 *  GLOBAL VARIABLES
 *    NAME                                    DESCRIPTION
 *              
 *============================================================================*/


#include "DSP2833x_Device.h"			// Peripheral address definitions
#include "500KW_MAIN_HEADER.h"				// Main include file


void VoltagePIcontroller(void);		//µçÑ¹»·º¯Êý
void Calc_Curr_PI_Para(void);		//¼ÆËãµçÁ÷»·PI²ÎÊý
void CurrentPIDcontroller(void);	//µçÁ÷»·º¯Êý
void dqPLLcontroller(void);			//PLL º¯Êý
void OutputWattCtrl(void);			//Êä³öÓÐ¹¦¹¦ÂÊ¿ØÖÆº¯Êý
void OutputVarCtrl(void);			//Êä³öÎÞ¹¦¹¦ÂÊ¿ØÖÆº¯Êý
void OutputPF_Ctrl(void);			//Êä³ö¹¦ÂÊÒòÊý¿ØÖÆº¯Êý
/* Check if dqPLL has locked in with Line voltage or not */
void Check_If_dqPLL_LockedIn(void);	

float fSat(float fxinput,float fMax,float fMin);  //º¯Êý£¬ÏÞÖÆÖµÔÚ×î´óÓë×îÐ¡Ö®¼ä


struct	CURRENTCONTREG	CurrConReg;
struct	PLLCONTREG	PLLConReg;

//-----------SVPWM struct-----------
SVGENDQ svgen_dq1 = SVGENDQ_DEFAULTS;

// Instance a few transform objects
CLARKE IGrid_Clarke = CLARKE_DEFAULTS;
CLARKE VGrid_Clarke = CLARKE_DEFAULTS;
PARK IGrid_Park = PARK_DEFAULTS;          // for Output Current
PARK VGrid_Park = PARK_DEFAULTS;          // for Output Voltagge
IPARK IGrid_IPark = IPARK_DEFAULTS;       // for tiao zhi bo
PWMGEN pwm1 = PWMGEN_DEFAULTS;

void InverterStage_Init(void);  //Äæ±äÆ÷¿ØÖÆ»·Â·ÖÐµÄ²ÎÊý³õÊ¼»¯
	

#pragma CODE_SECTION(ADC_INT_Control, "ControlLoopInRAM")
void ADC_INT_Control(void);

float32	Test_Start_of_ADISR;  //²âÊÔAD³ÌÐòÖ´ÐÐÊ±¼ä
float32	Test_End_of_ADISR;    
float32	Test_End_of_Cotrol;   //Ö÷¿ØÖÆ»ØÂ·Ö´ÐÐÊ±¼ä

float32 OmeGa_L = 2 * 157.07963268f * 0.000170f;  // dqÖá½âñîÓÃ

float32 Volt_Err_R = 0.0f;	   //new added 2013.1.22 for Grid connect£¬for Compare Vinv and Vgrid
float32 Volt_Err_S = 0.0f;
float32 Volt_Err_T = 0.0f;

Uint16	PWMTIME = 0;   //pwm¹¤×÷ÑÓÊ±1£¬PWM ¿ªÆôÇ°1s¸øPWM¸³ºã¶¨Õ¼¿Õ±È
int16	Connect_Time = 0;      //Vinv ÓëVgrid ±È½ÏÑÓÊ±¼ÆÊýÆ÷
int16	Connect_Time1 = 0;
int16  	STATUS_AC_RELAY1_ON = 0; //Connecting
float32 PWM_Gain = 0;


/*=============================================================================*
 * FUNCTION: fSat()
 * PURPOSE : use U/I ADC result to decide Ta,Tb,Tc by SVPWM_Model, and
 *			 dqPLL and PID Current Controller included.
 * INPUT: 
 *     xinput, maxsaturation,minsaturation
 *
 * RETURN: 
 *     float
 *
 * CALLS: 
 *     float
 * 
 *============================================================================*/
float fSat(float fxinput,float fMax,float fMin)
{
	float fxoutput = fxinput;

	if(fxinput >= fMax)
		fxoutput = fMax;
	else if(fxinput <= fMin)
		fxoutput = fMin;

	return(fxoutput);				
}  
/*=============================================================================*
 * FUNCTION: ADC_INT_Control()
 * PURPOSE : use U/I ADC result to decide Ta,Tb,Tc by SVPWM_Model, and
 *			 dqPLL and PID Current Controller included.
 * INPUT: 
 *     Va, Vb, Vc, Ia, Ib, and Ic
 *
 * RETURN: 
 *     void
 *
 * CALLS: 
 *     void
 *
 * CALLED BY: 
 *     void SVPWMADCswi(void); 
 * 
 *============================================================================*/
void ADC_INT_Control(void)
{ // start of SVPWM_Model

  Calc_Curr_PI_Para();
// Test_Start_of_ADISR = EPwm1Regs.TBCTR;
  if ( (GridTiedState == g_Sys_Current_State) || (PreRunState == g_Sys_Current_State ))
  {
//	VoltagePIcontroller();	//µçÑ¹»·±Õ»·¿ØÖÆº¯Êý
    Calc_Id_reference();      //µçÁ÷»ºÆô¶¯º¯Êý 
    //Check_If_dqPLL_LockedIn();  //¼ì²âËøÏàÊÇ·ñÕýÈ·º¯Êý
  }

/*  if ( (GridTiedState == g_Sys_Current_State) && (BusCon_Reg.f32IGain == 1))
  {
  	OutputWattCtrl();
  	OutputVarCtrl();
  }
*/

  Get_ADC_Result1();		//¶ÁÈ¡²ÉÑùµçÍøµçÑ¹Óëµç¸ÐµçÁ÷£¬µçÍøµçÁ÷Öµ


  CurrentPIDcontroller();	//µçÁ÷»·±Õ»·¿ØÖÆº¯Êý

// Test_End_of_Cotrol = EPwm1Regs.TBCTR;
  	dqPLLcontroller();          //PLL º¯Êý

  	Get_ADC_Result2();          //¶ÁÈ¡ÆäËü²ÉÑùÖµº¯Êý


//	Test_End_of_Cotrol = EPwm1Regs.TBCTR;
	HwInvOCPDetection();
	HwPVOVPDetection();

	ADAccCalc();              //¼ÆËãÀÛ¼Óº¯Êý£¬ÎªÁË¼ÆËã¸÷¸ö²ÉÑùµÄÆ½¾ùÖµºÍÓÐÐ§Öµ
//************************Ë²Ê±²ÉÑù±£»¤***********************
   //*******Ë²Ê±µçÍøµçÁ÷¹ýÁ÷±£»¤********
	if((abs(GetRealValue.f32IGrid_R) > 500)||(abs(GetRealValue.f32IGrid_S) > 500)||(abs(GetRealValue.f32IGrid_T) > 500)) //39
	{
			if((GridTiedState == g_Sys_Current_State)||( CheckState == g_Sys_Current_State))
		    	g_SysFaultMessage.bit.HW_OCP_AC_Fault = 1;			//disabled by XXXXX 20111004

			PWMOutputsDisable();
		    AC_RELAY1_OFF;
	}

	//*******Ë²Ê±µçÍøµçÑ¹¹ýÑ¹±£»¤********
	if((abs(GetRealValue.f32VGrid_R) > 300)||(abs(GetRealValue.f32VGrid_S) > 300)||(abs(GetRealValue.f32VGrid_T) > 300)) //39
	{
		if((GridTiedState == g_Sys_Current_State)||( CheckState == g_Sys_Current_State))
	    	g_SysFaultMessage.bit.VGridTOverRating = 1;			//disabled by XXXXX 20111004

	    PWMOutputsDisable();
	    AC_RELAY1_OFF;
	} 

	//*******Ë²Ê±ÊäÈëµçÑ¹¹ýÑ¹±£»¤********
	if((abs(GetRealValue.f32VPV) > 740)) //39
	{
		if((GridTiedState == g_Sys_Current_State)||( CheckState == g_Sys_Current_State))
			g_SysFaultMessage.bit.HW_BusOVP_Fault = 1;			//disabled by XXXXX 20111004

		PWMOutputsDisable();
		AC_RELAY1_OFF;
		AC_RELAY1_OFF;
	} 
  
    //*******Ë²Ê±µç¸ÐµçÁ÷¹ýÁ÷±£»¤********
	if((abs(GetRealValue.f32IL_R) > 450)||(abs(GetRealValue.f32IL_S) > 450)||(abs(GetRealValue.f32IL_T) > 450)) //39
	{
     	if((GridTiedState == g_Sys_Current_State)||( CheckState == g_Sys_Current_State))
	     	g_SysFaultMessage.bit.HW_OCP_AC_Fault = 1;			//disabled by  20111004

	    PWMOutputsDisable();
	    AC_RELAY1_OFF;
	} 	    
} // end of SVPWM_Model 

/*=============================================================================*
 * FUNCTION: VoltagePIcontroller()
 * PURPOSE : PI controller for Voltage control
 * INPUT: 
 *     SVPWMADbufferTemp.i16I_Induction_a and SVPWMADbufferTemp.i16I_Induction_b
 *
 * RETURN: 
 *     void
 *
 * CALLS: 
 *     void
 *
 * CALLED BY: 
 *     void SVPWM_Model(void); 
 * 
 *============================================================================*/
void VoltagePIcontroller(void)	
{ 	
//	g_Ctrl_Para.Volt_Kp = 2.35f; 
//	g_Ctrl_Para.Volt_Ki = 0.001f;			// 0.001667
	static float32 BusFilter = 0;

	BusFilter = 0.1f * GetRealValue.f32VPV + 0.9f * BusFilter;
	BusCon_Reg.f32BusRef = 550.0f;
//	BusCon_Reg.f32BusRef = g_f32Mppt_Autobus_Volt;
	BusCon_Reg.f32BusVolt_Fdb = BusFilter;//GetRealValue.f32VPV
	BusCon_Reg.f32BusErr_Old = BusCon_Reg.f32BusErr_New;
	BusCon_Reg.f32BusErr_New = BusCon_Reg.f32BusVolt_Fdb - BusCon_Reg.f32BusRef;
	
	if(BusCon_Reg.f32BusErr_New > 10)
		BusCon_Reg.f32BusErr_New = 10;
	else if(BusCon_Reg.f32BusErr_New < -10)
		BusCon_Reg.f32BusErr_New = -10;

	BusCon_Reg.f32Id_Ref = BusCon_Reg.f32Id_Ref + (g_Ctrl_Para.Volt_Kp + g_Ctrl_Para.Volt_Ki) * BusCon_Reg.f32BusErr_New - g_Ctrl_Para.Volt_Kp * BusCon_Reg.f32BusErr_Old; 
//	BusCon_Reg.f32Id_Ref = BusCon_Reg.f32Id_Ref + (2.35f + 0.001f) * BusCon_Reg.f32BusErr_New - 2.35f * BusCon_Reg.f32BusErr_Old; 
	if((PreRunState == g_Sys_Current_State) && (BusCon_Reg.f32Id_Ref >= 50))
		BusCon_Reg.f32Id_Ref = 50;

	if(BusCon_Reg.f32Id_Ref >= 750)
		BusCon_Reg.f32Id_Ref = 750;
	if(BusCon_Reg.f32Id_Ref <= 0)
		BusCon_Reg.f32Id_Ref = 0;
}

/*=============================================================================*
 * FUNCTION: Calc_Curr_PI_Para()
 * PURPOSE : 
 * INPUT: 
 *     
 *
 * RETURN: 
 *     void
 *
 * CALLS: 
 *     void
 *
 * CALLED BY: 
 *     void
 * 
 *============================================================================*/
void Calc_Curr_PI_Para(void)	
{
	static float f32temp1;

	f32temp1 = 0.1f * Calc_Result.f32VPV + 0.9f * f32temp1;

	if((f32temp1 >= 425.0f) && (f32temp1 < 475.0f))
	{
		g_Ctrl_Para.Curr_Kp = 0.0008f;
		g_Ctrl_Para.Curr_Ki = 0.00003f;
	}
	if((f32temp1 >= 475.0f) && (f32temp1 < 525.0f))
	{
		g_Ctrl_Para.Curr_Kp = 0.0007f;
		g_Ctrl_Para.Curr_Ki = 0.000025f;
	}
	if((f32temp1 >= 525.0f) && (f32temp1 < 575.0f))
	{
		g_Ctrl_Para.Curr_Kp = 0.0007f;
		g_Ctrl_Para.Curr_Ki = 0.000025f;
	}
	if((f32temp1 >= 575.0f) && (f32temp1 < 625.0f))
	{
		g_Ctrl_Para.Curr_Kp = 0.0006f;
		g_Ctrl_Para.Curr_Ki = 0.00002f;
	}
	if((f32temp1 >= 625.0f) && (f32temp1 < 675.0f))
	{
		g_Ctrl_Para.Curr_Kp = 0.0006f;
		g_Ctrl_Para.Curr_Ki = 0.00002f;
	}
	if((f32temp1 >= 675.0f) && (f32temp1 < 725.0f))
	{
		g_Ctrl_Para.Curr_Kp = 0.0005f;
		g_Ctrl_Para.Curr_Ki = 0.000015f;
	}
	if((f32temp1 >= 725.0f) && (f32temp1 < 775.0f))
	{
		g_Ctrl_Para.Curr_Kp = 0.0005f;
		g_Ctrl_Para.Curr_Ki = 0.000015f;
	}
	if((f32temp1 >= 775.0f) && (f32temp1 < 825.0f))
	{
		g_Ctrl_Para.Curr_Kp = 0.0004f;
		g_Ctrl_Para.Curr_Ki = 0.00001f;
	}
	if((f32temp1 >= 100.0f) && (f32temp1 < 240.0f))		//for test Vdc=160V
	{
		g_Ctrl_Para.Curr_Kp = 0.0022f;
		g_Ctrl_Para.Curr_Ki = 0.000068f;
	}
	if((f32temp1 >= 240.0f) && (f32temp1 < 425.0f))		//for test Vdc=320V
	{
		g_Ctrl_Para.Curr_Kp = 0.0011f;
		g_Ctrl_Para.Curr_Ki = 0.000034f;
	}
}

/*=============================================================================*
 * FUNCTION: CurrentPIDcontroller()
 * PURPOSE : PID controller for current control in dq frame
 * INPUT: 
 *     SVPWMADbufferTemp.i16I_Induction_a and SVPWMADbufferTemp.i16I_Induction_b
 *
 * RETURN: 
 *     void
 *
 * CALLS: 
 *     void
 *
 * CALLED BY: 
 *     void SVPWM_Model(void); 
 * 
 *============================================================================*/
void CurrentPIDcontroller(void)	
{ 	
	float fCoffVgridPhR = 1.0f;  	//Addition coefficient for low power module
	float fCoffVgridMax = 1.0f; 	//saturation for addition coefficient for low power module
	float fCoffVgridMin = 0; 		//saturation for addition coefficient for low power module
	float fCoffVgridPre = 0.0014f;  //±äÇ°À¡¿ØÖÆ  = 1/700

/************** For 0.74mH L **************/
//	g_Ctrl_Para.Curr_Kp = 0.0005f;		//0.006477f;   //0.01018f; 
//	g_Ctrl_Para.Curr_Ki = 0.000015f;		//0.001042f;
/*************** For 0.74mH L ***************/
//	float Har_Kp_5th = 0.3f;
//	float Har_Ki_5th = 0.02f;

//	float Har_Kp_11th = 0.02f;
//	float Har_Ki_11th = 0.002f;

/************** For 1.2mH L GridConnected ***********/
/*************** For 0.74mH L ***************
	float Har_Kp_5th = 0.35f;
	float Har_Ki_5th = 0.025f;

	float Har_Kp_11th = 0.025f;
	float Har_Ki_11th = 0.003f;
************** For 1.2mH L GridConnected ***********/
 //	float Curr_Kp = 0.0126f;	//0.0312f;	//0.0312f;	//0.0262f;	//0.0112f;	//0.0212f;	    //0.0106f;
//	float Curr_Ki = 0.004268f;	//0.004268f;	//0.003268;	//0.004268f;    //0.002134f;
/*************** For 1.2mH L GridConnected ***********/

/************** For AC Source ***********/	
//	float Curr_Kp = 0.0212f;   //0.0106f;
//	float Curr_Ki = 0.004268f;    //0.002134f;
//	float Curr_Kp = 0.006477f;   //0.01018f;
//	float Curr_Ki = 0.001042f;
/*************** For AC Source ***********/
			
//	float Curr_Kd = 0.0001f;  ±äÇ°À¡¿ØÖÆ
	fCoffVgridPhR = BusCon_Reg.f32Idref_cmd  * fCoffVgridPre;
	fCoffVgridPhR = fSat(fCoffVgridPhR, fCoffVgridMax, fCoffVgridMin);

//-----------------GridCurr Clarke Transform-------------
	IGrid_Clarke.f32Rs = GetRealValue.f32IL_R;
	IGrid_Clarke.f32Ss = GetRealValue.f32IL_S;
	IGrid_Clarke.f32Ts = GetRealValue.f32IL_T;
	IGrid_Clarke.f32FreGrid1 = Calc_Result.f32GridFreqClarke1;
	IGrid_Clarke.f32FreGrid2 = Calc_Result.f32GridFreqClarke2;
	IGrid_Clarke.calc(&IGrid_Clarke);

//------------------GridCurr Park Transform--------------
	IGrid_Park.f32AlphaP = IGrid_Clarke.f32Alpha;	// - 0.3 * PLLConReg.Cos_5Theta;
	IGrid_Park.f32BetaP = IGrid_Clarke.f32Beta;		// - 0.3 * PLLConReg.Sin_5Theta;
	IGrid_Park.f32Sin_Theta = PLLConReg.Sin_Theta;
	IGrid_Park.f32Cos_Theta = PLLConReg.Cos_Theta;
	IGrid_Park.calc(&IGrid_Park);


//-----------------GridCurr DQ Filter--------------------	
//	CurrConReg.f32Id_Filter_k = 0.0305f * IGrid_Park.f32DPs + 0.9695 * CurrConReg.f32Id_Filter_k_1;		// Cutoff Frequecny 100Hz
//	CurrConReg.f32Iq_Filter_k = 0.0305f * IGrid_Park.f32QPs + 0.9695 * CurrConReg.f32Iq_Filter_k_1;		// Cutoff Frequency 100Hz

	CurrConReg.f32Id_Filter = 0.00314f * IGrid_Park.f32DPs + 0.99686f * CurrConReg.f32Id_Filter;		// Cutoff Frequecny 10Hz
	CurrConReg.f32Iq_Filter = 0.00314f * IGrid_Park.f32QPs + 0.99686f * CurrConReg.f32Iq_Filter;		// Cutoff Frequency 10Hz

	CurrConReg.f32Id_Harmonic = (CurrConReg.f32Id_Filter - IGrid_Park.f32DPs) * CurrConReg.f32HarmCoff; //CurrConReg.f32HarmCoff,Ð³²¨Ç°À¡ÏµÊý
	CurrConReg.f32Iq_Harmonic = (CurrConReg.f32Iq_Filter - IGrid_Park.f32QPs) * CurrConReg.f32HarmCoff;

//--------------------------------------------------------

 //	CurrConReg.f32Id_5thHar_k = 1.992f * CurrConReg.f32Id_5thHar_k_1 - 0.996f * CurrConReg.f32Id_5thHar_k_2 + 0.0000498f * (IGrid_Park.f32DPs - CurrConReg.f32Id_k);
 //	CurrConReg.f32Iq_5thHar_k = 1.992f * CurrConReg.f32Iq_5thHar_k_1 - 0.996f * CurrConReg.f32Iq_5thHar_k_2 + 0.0000498f * (IGrid_Park.f32QPs - CurrConReg.f32Iq_k);
 /*	CurrConReg.f32Id_5thHar_k = 1.981f * CurrConReg.f32Id_5thHar_k_1 - 0.985f * CurrConReg.f32Id_5thHar_k_2 + 0.011f * (IGrid_Park.f32DPs - CurrConReg.f32Id_k);
 	CurrConReg.f32Iq_5thHar_k = 1.981f * CurrConReg.f32Iq_5thHar_k_1 - 0.985f * CurrConReg.f32Iq_5thHar_k_2 + 0.011f * (IGrid_Park.f32QPs - CurrConReg.f32Iq_k);


	CurrConReg.f32Id_5thHar_k_2 = CurrConReg.f32Id_5thHar_k_1;
	CurrConReg.f32Iq_5thHar_k_2 = CurrConReg.f32Iq_5thHar_k_1;
	CurrConReg.f32Id_5thHar_k_1 = CurrConReg.f32Id_5thHar_k;
	CurrConReg.f32Iq_5thHar_k_1 = CurrConReg.f32Iq_5thHar_k;

	CurrConReg.f32Id_11thHar_k = 1.937f * CurrConReg.f32Id_11thHar_k_1 - 0.961f * CurrConReg.f32Id_11thHar_k_2 + 0.015f * (IGrid_Park.f32DPs - CurrConReg.f32Id_k);
	CurrConReg.f32Iq_11thHar_k = 1.937f * CurrConReg.f32Iq_11thHar_k_1 - 0.961f * CurrConReg.f32Iq_11thHar_k_2 + 0.015f * (IGrid_Park.f32QPs - CurrConReg.f32Iq_k);

	CurrConReg.f32Id_11thHar_k_2 = CurrConReg.f32Id_11thHar_k_1;
	CurrConReg.f32Iq_11thHar_k_2 = CurrConReg.f32Iq_11thHar_k_1;
	CurrConReg.f32Id_11thHar_k_1 = CurrConReg.f32Id_11thHar_k;
	CurrConReg.f32Iq_11thHar_k_1 = CurrConReg.f32Iq_11thHar_k;

	CurrConReg.f32Id_k = IGrid_Park.f32DPs;
	CurrConReg.f32Iq_k = IGrid_Park.f32QPs;

	CurrConReg.f32Id_5thHar_Out = CurrConReg.f32Id_5thHar_Out + (Har_Kp_5th + Har_Ki_5th) * (-CurrConReg.f32Id_5thHar_k) - Har_Kp_5th * (-CurrConReg.f32Id_5thHar_k_2); 
	CurrConReg.f32Iq_5thHar_Out = CurrConReg.f32Iq_5thHar_Out + (Har_Kp_5th + Har_Ki_5th) * (-CurrConReg.f32Iq_5thHar_k) - Har_Kp_5th * (-CurrConReg.f32Iq_5thHar_k_2); 

	CurrConReg.f32Id_11thHar_Out = CurrConReg.f32Id_11thHar_Out + (Har_Kp_11th + Har_Ki_11th) * (-CurrConReg.f32Id_11thHar_k) - Har_Kp_11th * (-CurrConReg.f32Id_11thHar_k_2); 
	CurrConReg.f32Iq_11thHar_Out = CurrConReg.f32Iq_11thHar_Out + (Har_Kp_11th + Har_Ki_11th) * (-CurrConReg.f32Iq_11thHar_k) - Har_Kp_11th * (-CurrConReg.f32Iq_11thHar_k_2); 
*/
//----------------------GridCurr DQ CloseLoop Regulation----------------
/******************************* 6~7kW ****************************/
    CurrConReg.f32Id_Error_k_1 = CurrConReg.f32Id_Error_k;
    CurrConReg.f32Id_Error_k = BusCon_Reg.f32Idref_cmd - IGrid_Park.f32DPs;// + CurrConReg.f32Id_Harmonic; //µ±ËøÏàÕý³£Ê±£¬PLLConReg.f32Fre_Delta_k=0£¬¹ÂµºÊ±£¬ÔòÎªÒ»Êý£¬ÇÒÕã±ä´ó¡£

    CurrConReg.f32Iq_Error_k_1 = CurrConReg.f32Iq_Error_k;
    CurrConReg.f32Iq_Error_k = 0 - IGrid_Park.f32QPs;// + CurrConReg.f32Iq_Harmonic; //µ±ËøÏàÕý³£Ê±¬PLLConReg.f32Fre_Delta_k=0£¬¹ÂµºÊ±£¬ÔòÎªÒ»Êý£¬ÇÒÕã±ä´ó¡£
//    CurrConReg.f32Iq_Error_k = BusCon_Reg.f32Iqref_cmd - IGrid_Park.f32QPs;// + CurrConReg.f32Iq_Harmonic; //µ±ËøÏàÕý³£Ê±£¬PLLConReg.f32Fre_Delta_k=0£¬¹ÂµºÊ±£¬ÔòÎªÒ»Êý£¬ÇÒÕã±ä´ó¡£
//    CurrConReg.f32Iq_Error_k = 5 * BusCon_Reg.f32IGain - IGrid_Park.f32QPs;// + CurrConReg.f32Iq_Harmonic; //µ±ËøÏàÕý³£Ê±£¬PLLConReg.f32Fre_Delta_k=0£¬¹ÂµºÊ±£¬ÔòÎªÒ»Êý£¬ÇÒÕã±ä´ó¡£
/*********************************************************************/

/****************************** 8~10kW ****************************
    CurrConReg.f32Id_Error_k_1 = CurrConReg.f32Id_Error_k;
    CurrConReg.f32Id_Error_k = BusCon_Reg.f32Idref_cmd - IGrid_Park.f32DPs + 0.2 * CurrConReg.f32Id_Harmonic; //µ±ËøÏàÕý³£Ê±£¬PLLConReg.f32Fre_Delta_k=0£¬¹ÂµºÊ±£¬ÔòÎªÒ»Êý£¬ÇÒÕã±ä´ó¡£

    CurrConReg.f32Iq_Error_k_1 = CurrConReg.f32Iq_Error_k;
    CurrConReg.f32Iq_Error_k = BusCon_Reg.f32Iqref_cmd - IGrid_Park.f32QPs + 0.2 * CurrConReg.f32Iq_Harmonic; //µ±ËøÏàÕý³£Ê±£¬PLLConReg.f32Fre_Delta_k=0£¬¹ÂµºÊ±£¬ÔòÎªÒ»Êý£¬ÇÒÕã±ä´ó¡£
*****************************************************************/
/****************************** 11~15kW ****************************
    CurrConReg.f32Id_Error_k_1 = CurrConReg.f32Id_Error_k;
    CurrConReg.f32Id_Error_k = BusCon_Reg.f32Idref_cmd - IGrid_Park.f32DPs + 0.1 * CurrConReg.f32Id_Harmonic; //µ±ËøÏàÕý³£Ê±£¬PLLConReg.f32Fre_Delta_k=0£¬¹ÂµºÊ±£¬ÔòÎªÒ»Êý£¬ÇÒÕã±ä´ó¡£

    CurrConReg.f32Iq_Error_k_1 = CurrConReg.f32Iq_Error_k;
    CurrConReg.f32Iq_Error_k = BusCon_Reg.f32Iqref_cmd - IGrid_Park.f32QPs + 0.1 * CurrConReg.f32Iq_Harmonic; //µ±ËøÏàÕý³£Ê±£¬PLLConReg.f32Fre_Delta_k=0£¬¹ÂµºÊ±£¬ÔòÎªÒ»Êý£¬ÇÒÕã±ä´ó¡£
*****************************************************************/

/****************************** 16~17kW ****************************
  CurrConReg.f32Id_Error_k_1 = CurrConReg.f32Id_Error_k;     CurrConReg.f32Id_Error_k = BusCon_Reg.f32Idref_cmd - IGrid_Park.f32DPs; //µ±ËøÏàÕý³£Ê±£¬PLLConReg.f32Fre_Delta_k=0£¬¹ÂµºÊ±ªÒ»Êý£¬ÇÒÕã±ä´ó¡£

    CurrConReg.f32Iq_Error_k_1 = CurrConReg.f32Iq_Error_k;    CurrConReg.f32Iq_Error_k = BusCon_Reg.f32Iqref_cmd - IGrid_Park.f32QPs; //µ±ËøÏàÕý³£Ê±£¬PLLConReg.f32Fre_Delta_k=0£¬¹ÂµºÊ±£¬ÔòÎªÒ»ÊýÒ´ó¡
*****************************************************************/

// closed current loop
	CurrConReg.f32DutyD_Con = CurrConReg.f32DutyD_Con + (g_Ctrl_Para.Curr_Kp + g_Ctrl_Para.Curr_Ki) * CurrConReg.f32Id_Error_k - g_Ctrl_Para.Curr_Kp * CurrConReg.f32Id_Error_k_1;// + Curr_Kd * (CurrConReg.f32Id_Error_k-2* CurrConReg.f32Id_Error_k_1 + CurrConReg.f32Id_Error_k_2); 
	CurrConReg.f32DutyQ_Con = CurrConReg.f32DutyQ_Con + (g_Ctrl_Para.Curr_Kp + g_Ctrl_Para.Curr_Ki) * CurrConReg.f32Iq_Error_k - g_Ctrl_Para.Curr_Kp * CurrConReg.f32Iq_Error_k_1;// + Curr_Kd * (CurrConReg.f32Iq_Error_k-2* CurrConReg.f32Iq_Error_k_1 + CurrConReg.f32Iq_Error_k_2);

	if((BusCon_Reg.f32Idref_cmd >= 20) && (CurrConReg.u16Flg_DriveOpen == 0))
		CurrConReg.u16Flg_DriveOpen = 1;

//	CurrConReg.f32DutyD = CurrConReg.f32DutyD_Con;// +  CurrConReg.f32DutyD_ff;// + CurrConReg.f32DutyD_ff * fCoffVgridPhR;    ///  <<<----curr .Id
//	CurrConReg.f32DutyD_Dec =  0.01f * (- OmeGa_L * IGrid_Park.f32QPs * 0.022222f) + 0.99f * CurrConReg.f32DutyD_Dec;   		// + VGrid_Park.f32Ds/350; // + OmeGa_L * IGrid_Park.f32Qs/50;
	CurrConReg.f32DutyD_Dec = - OmeGa_L * CurrConReg.f32Iq_Filter * 0.022222f;
	CurrConReg.f32DutyDP = CurrConReg.f32DutyD_Con + CurrConReg.f32DutyDP_ff * fCoffVgridPhR;// + CurrConReg.f32DutyD_Dec;// + CurrConReg.f32Id_5thHar_Out + CurrConReg.f32Id_11thHar_Out;

//	CurrConReg.f32DutyQ = CurrConReg.f32DutyQ_Con;// + CurrConReg.f32DutyQ_ff; //* fCoffVgridPhR;    ///  <<<----curr .Id
//    CurrConReg.f32DutyQ_Dec = 0.01f * (OmeGa_L * IGrid_Park.f32DPs * 0.022222f) + 0.99f * CurrConReg.f32DutyQ_Dec;			///45;
	CurrConReg.f32DutyQ_Dec = OmeGa_L * CurrConReg.f32Id_Filter * 0.022222f;
    CurrConReg.f32DutyQP = CurrConReg.f32DutyQ_Con + CurrConReg.f32DutyQP_ff * fCoffVgridPhR;// + CurrConReg.f32DutyQ_Dec;// + CurrConReg.f32Iq_5thHar_Out + CurrConReg.f32Iq_11thHar_Out;
 
// closed current loop
  
// ***********open current loop***************
//	CurrConReg.f32DutyD_Con = 0.7; 
//    CurrConReg.f32DutyQ_Con = 0;  
// ***********open current loop***************

	if (CurrConReg.f32DutyDP  > 0.985f)
	{
		CurrConReg.f32DutyDP = 0.985f;
//		CurrConReg.f32DutyD_Con = 0.866f;
		CurrConReg.f32DutyD_Con = CurrConReg.f32DutyDP - CurrConReg.f32DutyDP_ff * fCoffVgridPhR;// * fCoffVgridPhR;
//		CurrConReg.f32DutyD_Con = CurrConReg.f32DutyDP - CurrConReg.f32DutyDP_ff - CurrConReg.f32DutyD_Dec -  CurrConReg.f32Id_5thHar_Out - CurrConReg.f32Id_11thHar_Out;
	}
	else if(CurrConReg.f32DutyDP  < -0.985f)
	{
		CurrConReg.f32DutyDP = - 0.985f;
//		CurrConReg.f32DutyD_Con = - 0.866f;
	CurrConReg.f32DutyD_Con = CurrConReg.f32DutyDP - CurrConReg.f32DutyDP_ff * fCoffVgridPhR;// * fCoffVgridPhR;
//		CurrConReg.f32DutyD_Con = CurrConReg.f32DutyDP - CurrConReg.f32DutyDP_ff - CurrConReg.f32DutyD_Dec - CurrConReg.f32Id_5thHar_Out - CurrConReg.f32Id_11thHar_Out;
	}

	if (CurrConReg.f32DutyQP  > 0.985f)
	{
		CurrConReg.f32DutyQP = 0.985f;
//		CurrConReg.f32DutyQ_Con = 0.866f;
		CurrConReg.f32DutyQ_Con = CurrConReg.f32DutyQP - CurrConReg.f32DutyQP_ff * fCoffVgridPhR;// * fCoffVgridPhR;
//		CurrConReg.f32DutyQ_Con = CurrConReg.f32DutyQP - CurrConReg.f32DutyQP_ff - CurrConReg.f32DutyQ_Dec - CurrConReg.f32Iq_5thHar_Out - CurrConReg.f32Iq_11thHar_Out;
	}
	else if(CurrConReg.f32DutyQP  < (-0.985f))
	{
		CurrConReg.f32DutyQP = - 0.985f;
//		CurrConReg.f32DutyQ_Con = - 0.866f;
		CurrConReg.f32DutyQ_Con = CurrConReg.f32DutyQP - CurrConReg.f32DutyQP_ff * fCoffVgridPhR;// * fCoffVgridPhR;
//		CurrConReg.f32DutyQ_Con = CurrConReg.f32DutyQP - CurrConReg.f32DutyQP_ff - CurrConReg.f32DutyQ_Dec - CurrConReg.f32Iq_5thHar_Out - CurrConReg.f32Iq_11thHar_Out;
	}


// ***********open current loop***************

    IGrid_IPark.f32DPs =  CurrConReg.f32DutyDP; 
  	IGrid_IPark.f32QPs =  CurrConReg.f32DutyQP;  
  	IGrid_IPark.f32DNs =  PLLConReg.f32VdN; 
  	IGrid_IPark.f32QNs =  PLLConReg.f32VqN;

	Volt_Err_R = Calc_Result.f32VGrid_Rrms - Calc_Result.f32VInv_Rrms;		//new added 2013.1.22
	Volt_Err_S = Calc_Result.f32VGrid_Rrms - Calc_Result.f32VInv_Rrms;
	Volt_Err_T = Calc_Result.f32VGrid_Rrms - Calc_Result.f32VInv_Rrms;

//	Volt_Err_R = 0;		//new added 2013.1.22
//	Volt_Err_S = 0;
//	Volt_Err_T = 0;
	

	if((fabs(Volt_Err_R) < 8) && (fabs(Volt_Err_S) < 8) && (fabs(Volt_Err_T) < 8) && (CurrConReg.u16Flg_DriveOpen == 2))
	{
		Connect_Time++;
		if(Connect_Time >= 1001)
			Connect_Time = 1001;
	}
	else
	{
		if(Connect_Time >=1001)
			Connect_Time = 1001;
		else
			Connect_Time = 0;
	}

	if( (g_SysFaultMessage.bit.PLLFault == 0) && (Connect_Time == 1000) && (CurrConReg.u16Flg_DriveOpen == 2) )
	{
    	AC_RELAY1_ON;
    	STATUS_AC_RELAY1_ON = 1;						//Connecting
    }
		
	if(Connect_Time1 < 480 )	
	{	
		if(CurrConReg.u16Flg_DriveOpen == 2)
			PWM_Gain = PWM_Gain + 0.0001f;
		if(PWM_Gain >= 1.0f)
			PWM_Gain = 1.0f;
    	/*CurrConReg.f32DutyD_Con = CurrConReg.f32DutyD_Con  + 0.002f;
    	if(CurrConReg.f32DutyD_Con >= CurrConReg.f32DutyDP_ff)
    	    CurrConReg.f32DutyD_Con = CurrConReg.f32DutyDP_ff;															
		*/
		IGrid_IPark.f32DPs = 1.155f * CurrConReg.f32DutyDP_ff * PWM_Gain;
		// CurrConReg.f32DutyD_Con = 0.0f;
		CurrConReg.f32DutyQ_Con = 0.0f;
  		IGrid_IPark.f32QPs =  0.001f;  
  		IGrid_IPark.f32DNs =  0.0f; 
  		IGrid_IPark.f32QNs =  0.0f;
	}

	if(STATUS_AC_RELAY1_ON == 1)
	{
		Connect_Time1++;
		if(Connect_Time1 >= 480)
		{
			Connect_Time1 = 480;
			CurrConReg.f32DutyD_Con = CurrConReg.f32DutyDP_ff;
			STATUS_AC_RELAY1_ON = 2;
		}
	}

	if( 1 == CurrConReg.u16Flg_DriveOpen )
	{
		IGrid_IPark.f32DPs = 0.01f;
		IGrid_IPark.f32QPs = 0.0f ;
		CurrConReg.f32DutyD_Con = 0.0f;
		CurrConReg.f32DutyQ_Con = 0.0f;
		PWMTIME++;
		if(PWMTIME >= 6000)
		{
			PWMOutputsEnable();
			CurrConReg.u16Flg_DriveOpen = 2;
			PWMTIME = 0;
		}
	}

	IGrid_IPark.f32Sin_Theta = PLLConReg.Sin_Theta;
	IGrid_IPark.f32Cos_Theta = PLLConReg.Cos_Theta;
    IGrid_IPark.calc(&IGrid_IPark);

   	svgen_dq1.Ualpha = IGrid_IPark.f32AlphaP + VGrid_Park.f32AlphaN * Calc_Result.Coff_Dforward * fCoffVgridPhR;// + PLLConReg.f32Fre_Delta_k * PLLConReg.Sin_Theta;
   	svgen_dq1.Ubeta = IGrid_IPark.f32BetaP + VGrid_Park.f32BetaN * Calc_Result.Coff_Dforward * fCoffVgridPhR;// + PLLConReg.f32Fre_Delta_k * PLLConReg.Cos_Theta;
   	svgen_dq1.calc(&svgen_dq1);

//*********************SVPWM**************
    pwm1.MfuncC1 = (float32)(svgen_dq1.Ta); // MfuncC1 is in Q15
    pwm1.MfuncC2 = (float32)(svgen_dq1.Tb); // MfuncC2 is in Q15 
    pwm1.MfuncC3 = (float32)(svgen_dq1.Tc); // MfuncC3 is in Q15
//    pwm1.MfuncC4 = (float32)(svgen_dq1.Taa); // MfuncC4 is in Q15
//    pwm1.MfuncC5 = (float32)(svgen_dq1.Tba); // MfuncC5 is in Q15
//    pwm1.MfuncC6 = (float32)(svgen_dq1.Tca); // MfuncC6 is in Q15
	pwm1.update(&pwm1);

//	if( 1 == CurrConReg.u16Flg_DriveOpen && GetRealValue.f32VPV > 40)

} // end of CurrentPIDcontroller


/*=============================================================================*
 * FUNCTION: OutputWattCtrl()  Êä³öÓÐ¹¦¹¦ÂÊ¿ØÖÆº¯Êý
 * PURPOSE : 
 * INPUT: 
 *     
 *
 * RETURN: 
 *     void
 *
 * CALLS: 
 *     void
 *
 * CALLED BY: 
 *     
 * 
 *============================================================================*/
 void OutputWattCtrl(void)
{
/*	float Watt_Kp = 0.35f; 
	float Watt_Ki = 0.001f;
	float WattErr_New = 0;
	static float WattErr_Old = 0;
*/
	float32	f32temp1;

	if((g_StateCheck.bit.OutputWattCtrl_flag == 1) && (CurLoop == WorkMode))
	{
/*		WattErr_New = g_Ctrl_Para.Output_Watt_set - Calc_Result.f32OutputWatt;
		
		if(WattErr_New > 1000)
			WattErr_New = 1000;
		else if(WattErr_New < -1000)
			WattErr_New = -1000;

		BusCon_Reg.f32Id_Ref = BusCon_Reg.f32Id_Ref + (Watt_Kp + Watt_Ki) * WattErr_New - Watt_Kp * WattErr_Old; 

		WattErr_Old = WattErr_New;

//		if((PreRunState == g_Sys_Current_State) && (BusCon_Reg.f32Id_Ref >= 50))
//			BusCon_Reg.f32Id_Ref = 50;
*/
		f32temp1 = g_Ctrl_Para.Output_Watt_set / Calc_Result.f32VGrid_Rrms * 0.4714f;	//1.414/3=0.4714

		if((BusCon_Reg.f32Id_Ref + 0.1f) <= f32temp1)
			BusCon_Reg.f32Id_Ref +=	0.1f;
		else if((BusCon_Reg.f32Id_Ref - 0.1f) >= f32temp1)	
			BusCon_Reg.f32Id_Ref -=	0.1f;
		else
			BusCon_Reg.f32Id_Ref = f32temp1;

//		BusCon_Reg.f32Id_Ref = g_Ctrl_Para.Output_Watt_set / Calc_Result.f32VGrid_Rrms * 0.4714f;	//1.414/3=0.4714

		if(BusCon_Reg.f32Id_Ref >= 600)
			BusCon_Reg.f32Id_Ref = 600;
		if(BusCon_Reg.f32Id_Ref <= 0)
			BusCon_Reg.f32Id_Ref = 0;
	}
}

/*=============================================================================*
 * FUNCTION: OutputVarCtrl()  Êä³öÎÞ¹¦¹¦ÂÊ¿ØÖÆº¯Êý
 * PURPOSE : 
 * INPUT: 
 *     
 *
 * RETURN: 
 *     void
 *
 * CALLS: 
 *     void
 *
 * CALLED BY: 
 *     
 * 
 *============================================================================*/
 void OutputVarCtrl(void)
{
/*	float Var_Kp = 0.35f; 
	float Var_Ki = 0.001f;
	float VarErr_New = 0;
	static float VarErr_Old = 0;

	if(g_StateCheck.bit.OutputVarCtrl_flag == 1)
	{
		VarErr_New = g_Ctrl_Para.Output_Var_set - Calc_Result.f32OutputVar;
		
		if(VarErr_New > 400)
			VarErr_New = 400;
		else if(VarErr_New < -400)
			VarErr_New = -400;

		BusCon_Reg.f32Iqref_cmd = BusCon_Reg.f32Iqref_cmd + (Var_Kp + Var_Ki) * VarErr_New - Var_Kp * VarErr_Old; 

		VarErr_Old = VarErr_New;

//		if((PreRunState == g_Sys_Current_State) && (BusCon_Reg.f32Id_Ref >= 50))
//			BusCon_Reg.f32Id_Ref = 50;
*/
	float32	f32temp1;

	if(g_StateCheck.bit.OutputVarCtrl_flag == 1)
	{
		f32temp1 = g_Ctrl_Para.Output_Var_set / Calc_Result.f32VGrid_Rrms * 0.4714f;	//1.414/3=0.4714

		if((BusCon_Reg.f32Iqref_cmd + 0.1f) <= f32temp1)
			BusCon_Reg.f32Iqref_cmd +=	0.1f;
		else if((BusCon_Reg.f32Iqref_cmd - 0.1f) >= f32temp1)	
			BusCon_Reg.f32Iqref_cmd -=	0.1f;
		else
			BusCon_Reg.f32Iqref_cmd = f32temp1;

		if(BusCon_Reg.f32Iqref_cmd >= 40)
			BusCon_Reg.f32Iqref_cmd = 40;
		if(BusCon_Reg.f32Iqref_cmd <= -40)
			BusCon_Reg.f32Iqref_cmd = -40;
	}
}


/*=============================================================================*
 * FUNCTION: OutputPF_Ctrl()  Êä³ö¹¦ÂÊÒòÊý¿ØÖÆº¯Êý
 * PURPOSE : 
 * INPUT: 
 *     
 *
 * RETURN: 
 *     void
 *
 * CALLS: 
 *     void
 *
 * CALLED BY: 
 *     
 * 
 *============================================================================*/
 void OutputPF_Ctrl(void)
{
	if(g_StateCheck.bit.OutputPF_Ctrl_flag == 1)
	{
		g_Ctrl_Para.Output_Var_set = Calc_Result.f32OutputVA * g_Ctrl_Para.Output_PF_set;
		g_StateCheck.bit.OutputVarCtrl_flag = 1;
		OutputVarCtrl();
		g_StateCheck.bit.OutputVarCtrl_flag = 0;
	}
}


/*=============================================================================*
 * FUNCTION: dqPLLcontroller()  ËøÏàº¯Êý
 * PURPOSE : dq Frame PLL, providing Theta and frequency for current control 
 *			 loop and anti-islanding algorithm
 * INPUT: 
 *     SVPWMADbufferTemp.i16Vuv and SVPWMADbufferTemp.i16Vvw
 *
 * RETURN: 
 *     void
 *
 * CALLS: 
 *     void
 *
 * CALLED BY: 
 *     AntiIslandingDetection module and void SVPWM_Model(void).
 * 
 *============================================================================*/
 void dqPLLcontroller(void)
{ 
	// ËøÏà»·PI
	float32 PLL_Kp = 5.571e-3f;	// 5.571e-6f;
	float32 PLL_Ki = 0.017e-3f;	//0.017e-5f;	// 0.017e-6f;
 
	float32 f32temp1 = 0,f32temp2 = 0,f32temp3 = 0,f32temp4 = 0;

// start, dq frame Phase Lock Loop controller

//--------------GridVolt Clarke Transform-----------	
	VGrid_Clarke.f32Rs = GetRealValue.f32VGrid_R;
	VGrid_Clarke.f32Ss = GetRealValue.f32VGrid_S;
	VGrid_Clarke.f32Ts = GetRealValue.f32VGrid_T;
	VGrid_Clarke.f32FreGrid1 = Calc_Result.f32GridFreqClarke1;
	VGrid_Clarke.f32FreGrid2 = Calc_Result.f32GridFreqClarke2;
	VGrid_Clarke.calc(&VGrid_Clarke);

	if(VGrid_Clarke.f32AlphaDelay >= 300)
		VGrid_Clarke.f32AlphaDelay = 300;
	else if(VGrid_Clarke.f32AlphaDelay <= -300)
		VGrid_Clarke.f32AlphaDelay = -300;

	if(VGrid_Clarke.f32BetaDelay >= 300)
		VGrid_Clarke.f32BetaDelay = 300;
	else if(VGrid_Clarke.f32BetaDelay <= -300)
		VGrid_Clarke.f32BetaDelay = -300;


//---------------GridVolt Park Transform------------
	VGrid_Park.f32AlphaP = VGrid_Clarke.f32AlphaP;
 	VGrid_Park.f32BetaP = VGrid_Clarke.f32BetaP;
	VGrid_Park.f32AlphaN = VGrid_Clarke.f32AlphaN;
	VGrid_Park.f32BetaN = VGrid_Clarke.f32BetaN;
  	VGrid_Park.f32Sin_Theta = PLLConReg.Sin_Theta;
	VGrid_Park.f32Cos_Theta = PLLConReg.Cos_Theta;
  	VGrid_Park.calc(&VGrid_Park);

// ¼ÆËãµçÍø½Ç¶ÈµÄ Sin ºÍ Cos  sin = Ubate/Um, cos = Ualpha/Um
/*	PLLConReg.f32VGrid_Amp = 1 / sqrt((VGrid_Clarke.f32Alpha * VGrid_Clarke.f32Alpha) + (VGrid_Clarke.f32Beta * VGrid_Clarke.f32Beta));
	PLLConReg.f32VGrid_Sin_Theta = VGrid_Clarke.f32Beta * PLLConReg.f32VGrid_Amp;		// VGrid.sin_Theta
	PLLConReg.f32VGrid_Cos_Theta = VGrid_Clarke.f32Alpha * PLLConReg.f32VGrid_Amp;		// VGrid.cos_Theta
*/	
	PLLConReg.f32VGrid_Amp = 1 / sqrt((VGrid_Clarke.f32AlphaP * VGrid_Clarke.f32AlphaP) + (VGrid_Clarke.f32BetaP * VGrid_Clarke.f32BetaP));
	PLLConReg.f32VGrid_Sin_Theta = VGrid_Clarke.f32BetaP * PLLConReg.f32VGrid_Amp;		// VGrid.sin_Theta
	PLLConReg.f32VGrid_Cos_Theta = VGrid_Clarke.f32AlphaP * PLLConReg.f32VGrid_Amp;		// VGrid.cos_Theta
	
//*************** Low Pass filter£¬ ¼ÆËãÇ°À¡  NUAA*********************************
	f32temp1 = VGrid_Park.f32DPs;
	f32temp2 = VGrid_Park.f32QPs;
	f32temp3 = VGrid_Park.f32DNs;
	f32temp4 = VGrid_Park.f32QNs;

	f32temp1 = (PLLConReg.f32VdP * 0.996875f) + (f32temp1 * 0.003125f);
	f32temp2 = (PLLConReg.f32VqP * 0.996875f) + (f32temp2 * 0.003125f);
	f32temp3 = (PLLConReg.f32VdN * 0.996875f) + (f32temp3 * 0.003125f);
	f32temp4 = (PLLConReg.f32VqN * 0.996875f) + (f32temp4 * 0.003125f);

	PLLConReg.f32VdP = f32temp1;
	PLLConReg.f32VqP = f32temp2;
	PLLConReg.f32VdN = f32temp3;
	PLLConReg.f32VqN = f32temp4;

	CurrConReg.f32DutyDP_ff = PLLConReg.f32VdP * Calc_Result.Coff_Dforward;
	CurrConReg.f32DutyQP_ff = PLLConReg.f32VqP * Calc_Result.Coff_Dforward;
//	CurrConReg.f32DutyDN_ff = PLLConReg.f32VdN * Calc_Result.Coff_Dforward;
//	CurrConReg.f32DutyQN_ff = PLLConReg.f32VqN * Calc_Result.Coff_Dforward;
		

// ---------------DQ  PLL,  PID Module-------------//
	PLLConReg.f32ThetaErr_Old = PLLConReg.f32ThetaErr_New;
	PLLConReg.f32ThetaErr_New = PLLConReg.f32VGrid_Sin_Theta * PLLConReg.Cos_Theta - PLLConReg.f32VGrid_Cos_Theta * PLLConReg.Sin_Theta;					//PLLConReg.f32Vq;

	if(PLLConReg.f32ThetaErr_New > 0.1f)
		PLLConReg.f32ThetaErr_New = 0.1f;
	else if(PLLConReg.f32ThetaErr_New < -0.1f)
		PLLConReg.f32ThetaErr_New = -0.1f;

	PLLConReg.f32Theta_Step = PLLConReg.f32Theta_Step + ( PLL_Kp + PLL_Ki) * PLLConReg.f32ThetaErr_New - PLL_Kp * PLLConReg.f32ThetaErr_Old;//
	
//	PLLConReg.f32Theta_Step = 0.052359877f;
//--------ÏÞÖÆËøÏà²½³¤µÄ×î´óºÍ×îÐ¡Öµ£¬ ·ÀÖ¹ËøÏàÊä³öÆµÂÊ·¶Î§ºÜ´ó------
	if(PLLConReg.f32Theta_Step > Theta_Step_Hi_Limit)
		PLLConReg.f32Theta_Step = Theta_Step_Hi_Limit;
	if(PLLConReg.f32Theta_Step < Theta_Step_Low_Limit)
		PLLConReg.f32Theta_Step = Theta_Step_Low_Limit;

// --------PLL ÖÐµÄÑ¹¿ØÕñµ´Æ÷£¬»ý·Ö»·½ÚIntegral loop
//---------PLL Êä³öÔÚ0- 2 * pi Ö®¼äÖÜÆÚ±ä»»
    PLLConReg.f32Theta +=  PLLConReg.f32Theta_Step;
	
	if(PLLConReg.f32Theta > Value_2Pi)
	{
		PLLConReg.f32Theta = PLLConReg.f32Theta - Value_2Pi;
		g_StateCheck.bit.Zero_Crossing_Flag = 1;					// disabled by XXXXX 20111011
	}
   PLLConReg.Sin_Theta = sin(PLLConReg.f32Theta);
   PLLConReg.Cos_Theta = cos(PLLConReg.f32Theta);

//sincos(PLLConReg.f32Theta, &(PLLConReg.Sin_Theta), &(PLLConReg.Cos_Theta));
//sincos(5 * PLLConReg.f32Theta, &(PLLConReg.Sin_5Theta), &(PLLConReg.Cos_5Theta));	
} // end, dq frame Phase Lock Loop controller



/*=============================================================================*
 * FUNCTION: Check_If_dqPLL_LockedIn()
             ËøÏà±£»¤³ÌÐò£¬ ËøÏàÕýÈ·Ê±£¬Uq = 0£¬
			 ÅÐ¶Ï|Uq|Öµ£¬´óÓÚÄ³Öµ»á±£»¤
 * PURPOSE : Check if dqPLL has locked in with Line Voltage or not
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
 *     void dqPLLcontroller(void).
 * 
 *============================================================================*/
void Check_If_dqPLL_LockedIn(void)
{ 
	static int16 PLL_TIME = 0;

	PLLConReg.f32DQ_PLL_Lockin = 0.4f * VGrid_Park.f32QPs + 0.6f * PLLConReg.f32DQ_PLL_Lockin;

	if(fabs(PLLConReg.f32DQ_PLL_Lockin) >= 10)
	{
		PLL_TIME++;
		if(PLL_TIME >= 40)
		{
			g_SysFaultMessage.bit.PLLFault = 1;	
			PLL_TIME = 0;
		}
	}
	else
	{
		g_SysFaultMessage.bit.PLLFault = 0;
		PLL_TIME = 0;
	}							 	

} // end of Check_If_dqPLL_LockedIn


/*=============================================================================*
 * FUNCTION: InverterStage_Init() Äæ±äÆ÷ ¿ØÖÆ»·Â·ÖÐµÄÒ»ÏÂ±äÁ¿µÄ³õÊ¼»¯
 * PURPOSE : Parameters initialization for Inverter Stage, especially 
 *			for the control loop
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
 *     MainStatusMachine module.
 * 
 *============================================================================*/
void InverterStage_Init(void)
{ // start of InverterStage_Init
	
	Connect_Time =0;
	Connect_Time1 = 0;
	PWM_Gain = 0;
	PWMTIME = 0;
	STATUS_AC_RELAY1_ON = 0;
	CurrConReg.u16Flg_DriveOpen = 0;

	BusCon_Reg.f32IGain = 0;  
	BusCon_Reg.f32Idref_cmd = 0;		//added 2013.1.19

	CurrConReg.f32Id_Filter = 0;
//	CurrConReg.f32Id_Filter_k_1 = 0;
	CurrConReg.f32Iq_Filter = 0;
//	CurrConReg.f32Iq_Filter_k_1 = 0;
	CurrConReg.f32Id_Harmonic = 0;
	CurrConReg.f32Iq_Harmonic = 0;

	CurrConReg.f32Id_5thHar_k_2 = 0;
	CurrConReg.f32Iq_5thHar_k_2 = 0;
	CurrConReg.f32Id_5thHar_k_1 = 0;
	CurrConReg.f32Iq_5thHar_k_1 = 0;
	CurrConReg.f32Id_5thHar_Out = 0;
	CurrConReg.f32Iq_5thHar_Out = 0;

	CurrConReg.f32Id_11thHar_k_2 = 0;
	CurrConReg.f32Iq_11thHar_k_2 = 0;
	CurrConReg.f32Id_11thHar_k_1 = 0;
	CurrConReg.f32Iq_11thHar_k_1 = 0;
	CurrConReg.f32Id_11thHar_Out = 0;
	CurrConReg.f32Iq_11thHar_Out = 0;


	CurrConReg.f32Id_k = 0;
	CurrConReg.f32Iq_k = 0;

	//CurrConReg.f32RC_Dout = 0;
	//CurrConReg.f32RC_Qout = 0;
//	CurrConReg.DArray_Last = 0;
//	CurrConReg.QArray_Last = 0;
    CurrConReg.f32Id_Error_k_2 = 0;
	CurrConReg.f32Id_Error_k_1 = 0;
	CurrConReg.f32Id_Error_k = 0;
	CurrConReg.f32DutyD_Dec = 0;
	CurrConReg.f32DutyD_Con = 0;	//  cur Iq   PID
    CurrConReg.f32Iq_Error_k_2 = 0;
	CurrConReg.f32Iq_Error_k_1 = 0;
	CurrConReg.f32Iq_Error_k = 0;
	CurrConReg.f32DutyQ_Dec = 0;
	CurrConReg.f32DutyQ_Con = 0;
    CurrConReg.f32DutyDP = 0;
	CurrConReg.f32DutyQP = 0;
	BusCon_Reg.f32BusErr_Old = 0;
	BusCon_Reg.f32BusErr_New = 0;

	CurrConReg.f32DutyDP_ff = 0;
	CurrConReg.f32DutyQP_ff = 0;
	CurrConReg.f32DutyDN_ff = 0;
	CurrConReg.f32DutyQN_ff = 0;
	
} // end of InverterStage_Init



int16 swGetStartADIsrPoint(void)
{
    return(PLLConReg.f32Vbeta);
}

int16 swGetEndCONPoint(void) 
{
    return(Test_End_of_Cotrol);
}

int16 swGetEndADIsrPoint(void)
{
    return(Test_End_of_ADISR);
}



/*
int16 swGetCurra(void)
{
    return(SVM_I_L_ab.f32I_Part1);
} 

int16 swGetCurrb(void)
{
    return(SVM_I_L_ab.f32I_Part2);
} 

int16 swGetIDRef(void) 
{
    return(g_f32SVM_I_d_reference);
} 

int16 swGetIIDFeedback1(void)
{
    return(SVM_I_d_q.f32I_Part1);
} 

int16 swGetIIQFeedback2(void)
{
    return(SVM_I_d_q.f32I_Part2);
} 


int16 swGetDuty_d(void)
{
    return(SVM_Volt_d_q_calc.f32V_Part1);
} 

int16 swGetDuty_q(void)
{
    return(SVM_Volt_d_q_calc.f32V_Part2);
} 
*/

//--- end of file -----------------------------------------------------

