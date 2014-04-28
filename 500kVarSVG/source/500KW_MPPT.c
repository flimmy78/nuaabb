/*=============================================================================*
 *        
 *  ALL RIGHTS RESERVED
 *
 *  FILENAME : 500KW_MPPT.c 
 *
 *  PURPOSE  : In normal state,caculated input pv power,changed bus voltage 
 *             reference and outputed Id reference to SPWM through PID caculation.
 *  
 *  HISTORY  :
 *    DATE            VERSION        AUTHOR            NOTE
 *    2013              v1.0       wangwanbao          
 *
 *----------------------------------------------------------------------------
 *  GLOBAL VARIABLES
 *    NAME                                    DESCRIPTION
 *      
 *============================================================================*/

// Main including header files
#include "DSP2833x_Device.h"		  // Peripheral address definitions
#include "500KW_MAIN_HEADER.h"				// Main include file

/*==========================================
* NAME      :  g_f32Mppt_Standbus_Volt
* PURPOSE :  limited bus voltage, compare with bus voltage
* RANGE    :  float 32 bits,
*
* CALLED BY: void Mppt(void)
*==========================================*/ 
//float32 g_f32Mppt_Autobus_Volt = 620;
//float32 g_f32Vpv_Opendrop_Volt = 800;
/*==========================================
* NAME      :  g_u16Mppt_TimeBase_Cnt
* PURPOSE :  Time base is used for updata input new power
* RANGE    :  Uint 16 bits
*
* CALLED BY: void Mppt(void)
*==========================================*/ 
Uint16  g_u16Mppt_TimeBase_Cnt = 0;

/*==========================================
* NAME      :  g_u16Mppt_Initial_Flag
* PURPOSE :  initial mppt start flage
* RANGE    :  Uint 16 bits
*
* CALLED BY: void Mppt(void)
*==========================================*/ 

/*==========================================
* NAME      :  g_u16Mppt_FastSearch_Flag
* PURPOSE :  start mppt flag
* RANGE    :  Uint 16 bits
*
* CALLED BY: void Mppt(void)
*==========================================*/ 
//Uint16  g_u16Mppt_FastSearch_Flag = 1;


//Uint16 Id_Ref_MaxRate_Flag;
//Uint16 Id1_Ref_MaxRate_Flag;
//Uint16 Id2_Ref_MaxRate_Flag;
//Uint16 g_u16PVOpen_Control_Flag;
//Uint16 g_u16PV1Open_Control_Flag;
//Uint16 g_u16PV2Open_Control_Flag;


struct MPPT_REG  Mppt_Reg1;  //,Mppt_Reg11,Mppt_Reg12;
struct MPPT_REG  Mppt_Reg2;  //,Mppt_Reg11,Mppt_Reg12;
struct BUS_CON_REG  BusCon_Reg;
struct BOOST_CON_REG  BooostCon_Reg;  //,BooostCon_Reg1,BooostCon_Reg2;
struct BUS_CON_PARA_REG  BusConPara_Reg;
struct BOOST_CON_PARA_REG  BoostConPara_Reg;
struct MPPT_PARA_REG  MpptPara_Reg;
struct POWER_DERATE_REG  PowerDerate_Reg; 

union MPPT_STATE  g_MPPT_State;
//--- Global functions

void Calc_Id_reference(void);
//void MppTrack(void);
void MpptInitial(void);   
void IGain_Slowup(void);
void OutPutLoadLimit(void);
void OverTemperatureLimit(void);
void OutputCurrentLimit(void);
void OutputPowerLimit(void);
void InputPowerLimit(void);
//void Calc_Boost_Duty(void);

/*******************************************************************************
* FUNCION :  MpptTask()
* PURPOSE :  initial  MPPT parameter,  caculated input power every 500ms, 
*            caculated Id reference, maximal power point track
*                   
* INPUT :
*        void
* RETURN :
*        void
* CALLS:
*        void
*
* CALLED BY: 	      
* 
*******************************************************************************/
void MpptTask(void)
{
  while(1)
  {
    if (SEM_pend(&MpptReady, SYS_FOREVER) == TRUE )
    {
     /* if(g_Sys_Current_State == NormalState)
      {
      	if(MpptLoop == WorkMode)
      	{
        	Mppt_Reg1.f32OutputWatt = Calc_Result.f32OutputWatt;
        	Mppt_Reg1.f32InputWatt_New = 0.04 * Calc_Result.f32Input1Watt + Mppt_Reg1.f32InputWatt_New;
        	Mppt_Reg1.f32PV_Fdb = 0.04 * (Calc_Result.f32VPV) + Mppt_Reg1.f32PV_Fdb;
        	
		   	if (g_u16Mppt_TimeBase_Cnt == MpptPara_Reg.f32Mppt_Rate_Cnt) // 500ms   
        	{	   
          		MppTrack();           						//　power_test  by XXXXX　　　　　            
          		Mppt_Reg1.f32InputWatt_New = 0;		 
          		Mppt_Reg1.f32PV_Fdb = 0;	
          				

          		g_u16Mppt_TimeBase_Cnt = 0;
          		OverTemperatureLimit();   
          		OutputPowerLimit(); 
          		InputPowerLimit();
          		OutputCurrentLimit(); 

				if(g_f32Vpv_Opendrop_Volt > 320)			//dis_power_test
				{
					g_f32Vpv_Opendrop_Volt = g_f32Vpv_Opendrop_Volt -1;
				}
				else
				{
					g_f32Vpv_Opendrop_Volt = 320;
				}


        	}  
        	if(Mppt_Reg1.f32PV_Volt_Ref > g_f32Mppt_Autobus_Volt)		// power_test  by XXXXX    modified by XXXXX 6.27
    		{
      			BusCon_Reg.f32BusRef = Mppt_Reg1.f32PV_Volt_Ref - 1.5;	
      			Mppt_Reg1.u16BoostOffFlag = 1; 
    		}
    		else
    		{
      			Mppt_Reg1.u16BoostOffFlag = 0;        					//open boost
	  			Mppt_Reg1.u16BoostOffFlag_time++;
			    BusCon_Reg.f32BusRef = g_f32Mppt_Autobus_Volt; 			//modified by XXXXX 6.27
    		}	
      	}
      	if( StabilizedVoltLoop == WorkMode)
      	{

   		if(BusCon_Reg.f32BusRef > g_f32Mppt_Autobus_Volt)			//added by XXXXX  8.8
			{
				BusCon_Reg.f32BusRef = BusCon_Reg.f32BusRef - 0.2;
			}
			else
			{
				BusCon_Reg.f32BusRef = g_f32Mppt_Autobus_Volt;
			}
 
// 			BusCon_Reg.f32BusRef = g_f32Mppt_Autobus_Volt;
        	if (g_u16Mppt_TimeBase_Cnt == MpptPara_Reg.f32Mppt_Rate_Cnt)    // 500ms   
        	{	   
          		g_u16Mppt_TimeBase_Cnt = 0;
          		OverTemperatureLimit();   
          		OutputPowerLimit(); 
//          		InputPowerLimit();
          		OutputCurrentLimit();
        	} 
      	}
		g_u16Mppt_TimeBase_Cnt ++;
	  }
	  else if(g_Sys_Current_State == FaultState)
	  {
			OverTemperatureLimit();
	  }*/
	}
 }	
}

/**********************************************************************
* FUNCION :  OutputCurrentLimit() 
* PURPOSE :  limit output current dynamically for safety
* CALLED BY: void Mppt(void)      
* 
**********************************************************************/
void OutputCurrentLimit(void)             //modified by XXXXX  6.26
{/*
	float32	f32CurrLimitTemp;
	f32CurrLimitTemp = BusCon_Reg.f32Id_Ref + 5;//add 3 Amp
//	BusCon_Reg.f32Id_Ref += 0.5;//testing
//	if(1 == g_MPPT_State.bit.Common_String )
//	{
	if((StabilizedVoltLoop == WorkMode)||(CurLoop == WorkMode))
	{	
		if (f32CurrLimitTemp < Rated_OutputCurrentPeak)
		{
			Mppt_Reg1.f32Id_Ref_Max = f32CurrLimitTemp;
		}
		else
		{
			Mppt_Reg1.f32Id_Ref_Max = Rated_OutputCurrentPeak;
		}
	}
	if(MpptLoop == WorkMode)
	{
		Mppt_Reg1.f32Id_Ref_Max = f32CurrLimitTemp;
	}*/
}

/**********************************************************************
* FUNCION :  MpptInitial() 
* PURPOSE :  initial MPPT parameter
* CALLED BY: void Mppt(void)      
**********************************************************************/


/*******************************************************************************
* FUNCION :  MppTrack() 
* PURPOSE :  maximal power point track
* CALLED BY: void Mppt(void)      
* 
*******************************************************************************/
/* void MppTrack()
{                                                                                                   //modified by XXXXX
	if((Mppt_Reg1.f32InputPowerLimit > Mppt_Reg1.f32InputWatt_New) && (Mppt_Reg1.f32OutputPowerLimit > Mppt_Reg1.f32OutputWatt) && (Mppt_Reg1.u16Boosttempprotection == 0) )
	{
	    if (1 == g_u16PVOpen_Control_Flag)//
	    {
	        if (Mppt_Reg1.f32PV_Volt_Ref > (Mppt_Reg1.f32PV_Volt_Open * 0.8f))    
	        {
	            if (Mppt_Reg1.f32InputWatt_New  + 20 < Mppt_Reg1.f32InputWatt_Old)       
	            {	Mppt_Reg1.f32PV_Volt_Ref = Mppt_Reg1.f32PV_Fdb + MpptPara_Reg.f32Delta_Volt_Step;  
	                g_u16PVOpen_Control_Flag = 0;
	            }
	            else
	            {
	                Mppt_Reg1.f32PV_Volt_Ref = Mppt_Reg1.f32PV_Volt_Ref - (Mppt_Reg1.f32PV_Volt_Open * 0.004);//0.01f);  
	            }
	        }
	        else
	        {
				
	            Mppt_Reg1.f32PV_Volt_Ref =  Mppt_Reg1.f32PV_Fdb + MpptPara_Reg.f32Delta_Volt_Step;
	            g_u16PVOpen_Control_Flag = 0;
	        }
	        Mppt_Reg1.f32InputWatt_Old = Mppt_Reg1.f32InputWatt_New; 
			Mppt_Reg1.f32PV_Fdb_old =   Mppt_Reg1.f32PV_Fdb;
	    }
	    else    // end ubFlagFastSearch = 1
	    {
	        if (Mppt_Reg1.f32InputWatt_New >= Mppt_Reg1.f32InputWatt_Old + DELT_POWER_DC) 
	        {
	            if (Mppt_Reg1.f32PV_Fdb >= Mppt_Reg1.f32PV_Fdb_old)
	            {
	                Mppt_Reg1.f32PV_Volt_Ref =  Mppt_Reg1.f32PV_Volt_Ref + MpptPara_Reg.f32Delta_Volt_Step;
	            }
	            else
	            {
	                Mppt_Reg1.f32PV_Volt_Ref =  Mppt_Reg1.f32PV_Volt_Ref - MpptPara_Reg.f32Delta_Volt_Step;
	            }
	            Mppt_Reg1.f32InputWatt_Old = Mppt_Reg1.f32InputWatt_New;
				Mppt_Reg1.f32PV_Fdb_old = Mppt_Reg1.f32PV_Fdb;
	        }   
	        else if (Mppt_Reg1.f32InputWatt_New + DELT_POWER_DC < Mppt_Reg1.f32InputWatt_Old) //DELT_POWER_DC_7W)     DELT_POWER_DC_5W
	        {
	            if (Mppt_Reg1.f32PV_Fdb >= Mppt_Reg1.f32PV_Fdb_old)
	            {
	                Mppt_Reg1.f32PV_Volt_Ref =  Mppt_Reg1.f32PV_Volt_Ref - MpptPara_Reg.f32Delta_Volt_Step;
	            }
	            else
	            {
	                Mppt_Reg1.f32PV_Volt_Ref =  Mppt_Reg1.f32PV_Volt_Ref + MpptPara_Reg.f32Delta_Volt_Step;
	            }
	            Mppt_Reg1.f32InputWatt_Old = Mppt_Reg1.f32InputWatt_New;
				Mppt_Reg1.f32PV_Fdb_old = Mppt_Reg1.f32PV_Fdb;	   
	        }
	        	//modified by XXXXX
	        else
	        {
			    if (Mppt_Reg1.f32PV_Fdb >= Mppt_Reg1.f32PV_Fdb_old)
	            {
	                Mppt_Reg1.f32PV_Volt_Ref = Mppt_Reg1.f32PV_Volt_Ref - MpptPara_Reg.f32Delta_Volt_Step * 0.05f;
	            }
	            else
	            {
	                Mppt_Reg1.f32PV_Volt_Ref = Mppt_Reg1.f32PV_Volt_Ref + MpptPara_Reg.f32Delta_Volt_Step * 0.05f;
	            } 
			}
			  //modified by XXXXX
		}
	}
//           modified by XXXXX
	else
	{
	  if(Mppt_Reg1.u16Boosttempprotection == 0)
		{
     		 Mppt_Reg1.f32PV_Volt_Ref =  Mppt_Reg1.f32PV_Volt_Ref + MpptPara_Reg.f32Delta_Volt_Step;
    	}
   	 else
    	{
      		if(( 1000  < Mppt_Reg1.f32InputWatt_New) || ( 1000 < Mppt_Reg1.f32OutputWatt))
      		{
        		Mppt_Reg1.f32PV_Volt_Ref =  Mppt_Reg1.f32PV_Volt_Ref + MpptPara_Reg.f32Delta_Volt_Step;
      		}
    	}
	}
//
	if (Mppt_Reg1.f32PV_Volt_Ref > PV_Over_Volt_Limit )
	{
		Mppt_Reg1.f32PV_Volt_Ref =  Mppt_Reg1.f32PV_Volt_Ref - MpptPara_Reg.f32Delta_Volt_Step;
	}
	else if(Mppt_Reg1.f32PV_Volt_Ref < PV_Under_Volt_Limit )
	{
		Mppt_Reg1.f32PV_Volt_Ref =  Mppt_Reg1.f32PV_Volt_Ref + MpptPara_Reg.f32Delta_Volt_Step;
	} 
}
*/

/**********************************************************************
* FUNCION :  OutPut Load Limit
* PURPOSE :  output maximum current Limited
*   if grid voltage is less than 227V,limit Pout at 25A;if grid voltage is larger than 227V,limit Pout at 17kW
*	if input voltage is between                
* INPUT :
*        void
* RETURN :
*        void
* CALLS:
*        void
*
* CALLED BY:  void  MpptTask(void)             
* 
**********************************************************************/

void OutputPowerLimit()   //every 500ms
{/*
    static  Uint16 u16Cnt_MaxPower = 0; 
//    static  Uint16 u16Cnt_MinPower = 0; 
    
    float32 f32OutPutPowerLimit1Temp = 0;    

 	  f32OutPutPowerLimit1Temp = Calc_Result.f32OutputWatt;   

    if ( f32OutPutPowerLimit1Temp > Rated_OutputPower )
    {
      u16Cnt_MaxPower++;
      if (u16Cnt_MaxPower > 10 )  // 10*500ms =5 second
      {
        g_SysFaultMessage.bit.OverLoad = 1 ;
      }
    }
    else
    {
      u16Cnt_MaxPower = 0;
      g_SysFaultMessage.bit.OverLoad = 0 ;
    }

    if ( f32OutPutPowerLimit1Temp < Min_OutputPower )
    {
//    g_SysFaultMessage.bit.StandBy_BusVoltLowFault = 1;
    }
    
	if (f32OutPutPowerLimit1Temp >= MpptPara_Reg.f32OutputRatedPower) 
	{
		Mppt_Reg1.f32OutputPowerLimit = MpptPara_Reg.f32OutputRatedPower;  
	}
	else
	{
		Mppt_Reg1.f32OutputPowerLimit = f32OutPutPowerLimit1Temp;
	}

	if (Mppt_Reg1.f32OutputPowerLimit >= PowerDerate_Reg.f32OutputPower_AmbientTLimit)      
	{
		Mppt_Reg1.f32OutputPowerLimit = PowerDerate_Reg.f32OutputPower_AmbientTLimit;
	}

	if (Mppt_Reg1.f32OutputPowerLimit > PowerDerate_Reg.f32OutputPower_HeatsinkTLimit)
	{
		Mppt_Reg1.f32OutputPowerLimit = PowerDerate_Reg.f32OutputPower_HeatsinkTLimit; 
	}
*/
} 

/**********************************************************************
* FUNCION :  OutPut Load Limit
* PURPOSE :  output maximum current Limited
*                   
* INPUT :
*        void
* RETURN :
*        void
* CALLS:
*        void
*
* CALLED BY:  void  MpptTask(void)             
* 
**********************************************************************/
void InputPowerLimit()
{
/*
    float32 f32VPVPowerLimit1Temp;    

    float32 f32VPVPowerLimit2Temp;

	if (Calc_Result.f32VPV >= PowerDerate_Reg.f32InputPowerLimit_MaxVolt )//800V
	{
		f32VPVPowerLimit1Temp = MpptPara_Reg.f32InputRatedPower * 0.5 - (Calc_Result.f32VPV - PowerDerate_Reg.f32InputPowerLimit_MaxVolt) * PowerDerate_Reg.f32DCPowerDerating_VRate;
	}
	else if (Calc_Result.f32VPV >= PowerDerate_Reg.f32InputPowerLimit_MinVolt )
	{
		f32VPVPowerLimit1Temp = MpptPara_Reg.f32InputRatedPower * 0.5;
	}
	else
	{
		f32VPVPowerLimit1Temp = Calc_Result.f32VPV * MpptPara_Reg.f32Input_Current_Limit;
	}
	if ( Calc_Result.f32VPV2 >= PowerDerate_Reg.f32InputPowerLimit_MaxVolt )//800V
	{
		f32VPVPowerLimit2Temp = MpptPara_Reg.f32InputRatedPower * 0.5 - (Calc_Result.f32VPV2 - PowerDerate_Reg.f32InputPowerLimit_MaxVolt) * PowerDerate_Reg.f32DCPowerDerating_VRate;
	}
	else if (Calc_Result.f32VPV2 >= PowerDerate_Reg.f32InputPowerLimit_MinVolt)
	{
		f32VPVPowerLimit2Temp = MpptPara_Reg.f32InputRatedPower * 0.5;
	}
	else
	{
		f32VPVPowerLimit2Temp = Calc_Result.f32VPV2 * MpptPara_Reg.f32Input_Current_Limit;
	}

	if (f32VPVPowerLimit1Temp >= PowerDerate_Reg.f32Input1Power_HeatsinkTLimit) 
	{
		Mppt_Reg11.f32InputPowerLimit = PowerDerate_Reg.f32Input1Power_HeatsinkTLimit;  
	}
	else
	{
		Mppt_Reg11.f32InputPowerLimit = f32VPVPowerLimit1Temp;
	}

	if (f32VPVPowerLimit2Temp >= PowerDerate_Reg.f32Input2Power_HeatsinkTLimit) 
	{
		Mppt_Reg12.f32InputPowerLimit = PowerDerate_Reg.f32Input2Power_HeatsinkTLimit;  
	}
	else
	{
		Mppt_Reg12.f32InputPowerLimit = f32VPVPowerLimit2Temp;
	}

	Mppt_Reg1.f32InputPowerLimit = Mppt_Reg11.f32InputPowerLimit + Mppt_Reg12.f32InputPowerLimit;
*/
}


/**********************************************************************
* FUNCION :  Over Temperature Limit
* PURPOSE :  Over Temperature load Limited
*                   
* INPUT :
*        void
* RETURN :
*        void
* CALLS:
*        void
*
* CALLED BY:  void  MpptTask(void)             
* 
**********************************************************************/


/**********************************************************************
* FUNCION :  Calc_Id_reference()
* PURPOSE :  calculate Id_reference by filtered MPPT result
*                   
* INPUT :
*        void
* RETURN :
*        void
* CALLS:
*        void
*
* CALLED BY: void Mppt(void)      
* 
**********************************************************************/
void Calc_Id_reference(void)
{
  
  if (CurLoop == WorkMode )  
  {
    BusCon_Reg.f32Idref_cmd = BusCon_Reg.f32IGain * BusCon_Reg.f32Id_Ref;// + BusCon_Reg.f32IdRefDelta); 
//	if(BusCon_Reg.f32IGain == 1)
//	{BusCon_Reg.f32Iqref_cmd =  BusCon_Reg.f32IqRefDelta;}
//	else
//	{
//	BusCon_Reg.f32Iqref_cmd = 0;
//	}
//	BusCon_Reg.f32Idref_cmd = BusCon_Reg.f32IGain * BusCon_Reg.f32Id_Ref;
//	BusCon_Reg.f32Iqref_cmd = 0;
  }
} // end of Calc_Id_reference


void IGain_Slowup(void)
{ 
//**used by XXXXX*************************************************************
	if((GridTiedState == g_Sys_Current_State) || (g_Sys_Current_State == PreRunState))
	{
		BusCon_Reg.f32IGain = BusCon_Reg.f32IGain + 0.0005f;  // 8S 産尼強

//		if((BusCon_Reg.f32IGain >= 0.2) && (g_Sys_Current_State == PreRunState))
//			BusCon_Reg.f32IGain = 0.2;

		if(BusCon_Reg.f32IGain >= 1)
			BusCon_Reg.f32IGain = 1;
	}
	if(BusCon_Reg.f32Id_Ref > 30)
		CurrConReg.f32HarmCoff = 0;
	else
		CurrConReg.f32HarmCoff = (30 - BusCon_Reg.f32Id_Ref) * 0.03125f;
} 


//--- end of file -----------------------------------------------------

