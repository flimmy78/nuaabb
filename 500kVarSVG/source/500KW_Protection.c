/*=============================================================================*
 *         Copyright(c)
 *  ALL RIGHTS RESERVED
 *
 *  FILENAME : 500KW_ProtectionLogic.c 
 *
 *  PURPOSE  : Calculate electrical energy related measurments values, 
 *			       and do some checks according to calculated values. 
 *  
 *  HISTORY  :
 *    DATE            VERSION        AUTHOR            NOTE
 *   2012             v1.0         wangwanbao         
 *
 *----------------------------------------------------------------------------
 *  GLOBAL VARIABLES
 *    NAME                                    DESCRIPTION
 *   g_ECalc_Result     					struct, calculation results 

 *   NAME								 	 DESCRIPTION
 *	g_i16ECalc_Grid_Zero_Crossing_Flag	 flag of every grid period*      
 *----------------------------------------------------------------------------
 *  GLOBAL FUNCTIONS
 *    NAME                                    DESCRIPTION
 *  EnergyAccCalc    				swi function.This function only performes 
 *									the basic data accumulate preprocess	*    
 *============================================================================*/


#include "DSP2833x_Device.h"			// Peripheral address definitions
#include "500KW_MAIN_HEADER.h"			// Main include file
//#include "EnergyCalc.h"

SAFETY_PARAMETER_REG	SafetyReg;      // 保护限幅值结构体


//----所有函数申明----
void GridVoltCheck(void);		// 电网电压保护
void ADOffsetCheck(void);		// 采样软件计算平均值保护
void GridCurrentCheck(void);	// 电网电流保护
void GridFreqCheck(void);		// 频率保护


void GridCurrentDciCheck(void);	// 电感电流直流分量硬件采样保护
void GFCICheck(void);			// 漏电流保护
void PVVoltCheck(void);			// 输入电压保护
void PVReversCurrCheck(void);   // 输入电流防反保护
void IslandingCheck(void);

//void IslandingCheck(void);      // 孤岛保护函数
//--------函数申明结束----------


float32 Rx1 = 0, Rx2 = 0;
Uint16  s_u16Cnt_Insulation_Check_Counter = 0;
//Uint8   u8Cnt1 = 0;
//Uint8   u8Cnt2 = 0;

//float32 PViso1,PViso2,PViso3,PViso4;                    	//xlx test
//float32 wGFCIIniValueSum,wGFCIIniValue;                   //xlx GFCI

/**********************************************************************
* FUNCION :  Grid Volt Check
* PURPOSE :  Three phase grid voltage range Check  for  safety rugulation
*                   
* INPUT :
*        void
* RETURN :
*        void
* CALLS:
*        void
*
* CALLED BY:  void EnergyCalcLowPrio(void)
* 
**********************************************************************/
void GridVoltCheck(void) //电网电压过压，欠压检测，分几级保护
{ 
    static  Uint16  s_u16Cnt_GridVoltR_High_Fault = 0;
    static  Uint16  s_u16Cnt_GridVoltR_Low_Fault = 0; 
    static  Uint16  s_u16Cnt_GridVoltR_Fault_Back = 0; 

	static  Uint16  s_u16Cnt_GridVoltS_High_Fault = 0;
    static  Uint16  s_u16Cnt_GridVoltS_Low_Fault = 0; 
    static  Uint16  s_u16Cnt_GridVoltS_Fault_Back = 0; 

	static  Uint16  s_u16Cnt_GridVoltT_High_Fault = 0;
    static  Uint16  s_u16Cnt_GridVoltT_Low_Fault = 0; 
    static  Uint16  s_u16Cnt_GridVoltT_Fault_Back = 0; 

//-----------------------------------------R---------------------------------------		//R相电压故障保护
    if(g_SysFaultMessage.bit.VGridROverRating == 0 && g_SysFaultMessage.bit.VGridRUnderRating == 0)  
    {																					//无故障
		s_u16Cnt_GridVoltR_Fault_Back = 0;												//故障恢复计数清零
        if(sqrt(AD_Sum.f32VGrid_Rrms * f32SumCounterReci) > SafetyReg.f32VGrid_Hi2Limit)//过压135%，为提高保护速度，使用周期均方根作判断
    	{																				//保护时间0.05s
			g_SysFaultMessage.bit.VGridROverRating = 1;
			s_u16Cnt_GridVoltR_Fault_Back = 0;		 
		}
		else if (sqrt(AD_Sum.f32VGrid_Rrms * f32SumCounterReci) < SafetyReg.f32VGrid_Low2Limit)//欠压50%，为提高保护速度，使用周期均方根作判断
		{																				//保护时间0.1s
			g_SysFaultMessage.bit.VGridRUnderRating = 1;
			s_u16Cnt_GridVoltR_Fault_Back = 0;				 
		}
		else if(Calc_Result.f32VGrid_Rrms > SafetyReg.f32VGrid_Hi1Limit)				//过压110%
		{																				//保护时间2s
			s_u16Cnt_GridVoltR_High_Fault++;
            if(s_u16Cnt_GridVoltR_High_Fault >= SafetyReg.f32VGrid_ProtectionTime * 5)	//故障持续100ms*5保护
            {
				g_SysFaultMessage.bit.VGridROverRating = 1;
				s_u16Cnt_GridVoltR_High_Fault = 0;
				s_u16Cnt_GridVoltR_Fault_Back = 0;		
            }
        }
		else if(Calc_Result.f32VGrid_Rrms < SafetyReg.f32VGrid_Low1Limit)				//欠压85%
		{																				//保护时间2s
			s_u16Cnt_GridVoltR_Low_Fault++;
            if(s_u16Cnt_GridVoltR_Low_Fault >= SafetyReg.f32VGrid_ProtectionTime * 5)	//故障持续100ms*5保护
            {
				g_SysFaultMessage.bit.VGridRUnderRating = 1;
				s_u16Cnt_GridVoltR_Low_Fault = 0;
				s_u16Cnt_GridVoltR_Fault_Back = 0;		
            }
        }
        else																			//无过欠压故障
        {
            s_u16Cnt_GridVoltR_Low_Fault = 0;
	     	s_u16Cnt_GridVoltR_High_Fault = 0;		
        }	  
	}
    else																				//有故障
    {
		if((Calc_Result.f32VGrid_Rrms > SafetyReg.f32VGrid_LowLimitBack) && (Calc_Result.f32VGrid_Rrms < SafetyReg.f32VGrid_HiLimitBack))
		{																				//电压恢复正常值
			s_u16Cnt_GridVoltR_Fault_Back++;											//等待1min清除故障位
			if(s_u16Cnt_GridVoltR_Fault_Back >= 300)     								//20ms * 3000 = 60s
			{
				g_SysFaultMessage.bit.VGridROverRating = 0;
				g_SysFaultMessage.bit.VGridRUnderRating = 0; 		
				s_u16Cnt_GridVoltR_Fault_Back = 0;
			}
		}
		else																			//电网电压正常
		{
			s_u16Cnt_GridVoltR_Fault_Back = 0;
		}  
    }  

//---------------------------------------S---------------------------------------		//S相电压故障保护
	if(g_SysFaultMessage.bit.VGridSOverRating == 0 && g_SysFaultMessage.bit.VGridSUnderRating == 0)	
    {																					//无故障	
		s_u16Cnt_GridVoltS_Fault_Back = 0;												//故障恢复计数清零
        if(sqrt(AD_Sum.f32VGrid_Srms * f32SumCounterReci) > SafetyReg.f32VGrid_Hi2Limit)//过压135%，为提高保护速度，使用周期均方根作判断
    	{																				//保护时间0.05s
			g_SysFaultMessage.bit.VGridSOverRating = 1;
			s_u16Cnt_GridVoltS_Fault_Back = 0;		 
		}
		else if (sqrt(AD_Sum.f32VGrid_Srms * f32SumCounterReci) < SafetyReg.f32VGrid_Low2Limit)	//欠压50%，为提高保护速度，使用周期均方根作判断
		{																				//保护时间0.1s
			g_SysFaultMessage.bit.VGridSUnderRating = 1;
			s_u16Cnt_GridVoltS_Fault_Back = 0;				 
		}
		else if(Calc_Result.f32VGrid_Srms > SafetyReg.f32VGrid_Hi1Limit)				//过压110%
		{																				//保护时间2s
			s_u16Cnt_GridVoltS_High_Fault++;
            if(s_u16Cnt_GridVoltS_High_Fault >= SafetyReg.f32VGrid_ProtectionTime * 5)	//故障持续100ms*5保护
            {
				g_SysFaultMessage.bit.VGridSOverRating = 1;
				s_u16Cnt_GridVoltS_High_Fault = 0;
				s_u16Cnt_GridVoltS_Fault_Back = 0;		
            }
        }
		else if(Calc_Result.f32VGrid_Srms < SafetyReg.f32VGrid_Low1Limit)				//欠压85%
		{																				//保护时间2s
			s_u16Cnt_GridVoltS_Low_Fault++;
            if(s_u16Cnt_GridVoltS_Low_Fault >= SafetyReg.f32VGrid_ProtectionTime * 5)	//故障持续100ms*5保护
            {
				g_SysFaultMessage.bit.VGridSUnderRating = 1;
				s_u16Cnt_GridVoltS_Low_Fault = 0;
				s_u16Cnt_GridVoltS_Fault_Back = 0;		
            }
        }
        else																			//无过欠压故障
        {
            s_u16Cnt_GridVoltS_Low_Fault = 0;
	     	s_u16Cnt_GridVoltS_High_Fault = 0;		
        }	
	}
    else																				//有故障
    {
		if((Calc_Result.f32VGrid_Srms >SafetyReg.f32VGrid_LowLimitBack) && (Calc_Result.f32VGrid_Srms < SafetyReg.f32VGrid_HiLimitBack))
		{																				//电压恢复正常值
			s_u16Cnt_GridVoltS_Fault_Back++;											//等待1min清除故障位
			if(s_u16Cnt_GridVoltS_Fault_Back >= 300)     								//3000 * 20ms = 60s
			{
				g_SysFaultMessage.bit.VGridSOverRating = 0;
				g_SysFaultMessage.bit.VGridSUnderRating = 0; 		
				s_u16Cnt_GridVoltS_Fault_Back = 0;
			}
		}
		else																			//电压未恢复
		{
			s_u16Cnt_GridVoltS_Fault_Back = 0;
		}  
    }

//---------------------------------------T---------------------------------------		//T相电压故障保护
	if(g_SysFaultMessage.bit.VGridTOverRating == 0 && g_SysFaultMessage.bit.VGridTUnderRating == 0)	    
    {																					//无故障	
		s_u16Cnt_GridVoltT_Fault_Back = 0;												//故障恢复计数清零
        if(sqrt(AD_Sum.f32VGrid_Trms * f32SumCounterReci) > SafetyReg.f32VGrid_Hi2Limit)//过压135%，为提高保护速度，使用周期均方根作判断
    	{																				//保护时间0.05s
			g_SysFaultMessage.bit.VGridTOverRating = 1;
			s_u16Cnt_GridVoltT_Fault_Back = 0;		 
		}
		else if(sqrt(AD_Sum.f32VGrid_Trms * f32SumCounterReci) < SafetyReg.f32VGrid_Low2Limit)//欠压50%，为提高保护速度，使用周期均方根作判断
		{																				//保护时间0.1s
			g_SysFaultMessage.bit.VGridTUnderRating = 1;
			s_u16Cnt_GridVoltT_Fault_Back = 0;				 
		}
		else if(Calc_Result.f32VGrid_Trms > SafetyReg.f32VGrid_Hi1Limit)				//过压110%
		{																				//保护时间2s
			s_u16Cnt_GridVoltT_High_Fault++;
            if(s_u16Cnt_GridVoltT_High_Fault >= SafetyReg.f32VGrid_ProtectionTime * 5)	//故障持续100ms*5保护
            {
				g_SysFaultMessage.bit.VGridTOverRating = 1;
				s_u16Cnt_GridVoltT_High_Fault = 0;
				s_u16Cnt_GridVoltT_Fault_Back = 0;		
            }
        }
		else if(Calc_Result.f32VGrid_Trms < SafetyReg.f32VGrid_Low1Limit)				//欠压85%
		{																				//保护时间2s
			s_u16Cnt_GridVoltT_Low_Fault++;
            if(s_u16Cnt_GridVoltT_Low_Fault >= SafetyReg.f32VGrid_ProtectionTime * 5)	//故障持续100ms*5保护
            {
				g_SysFaultMessage.bit.VGridTUnderRating = 1;
				s_u16Cnt_GridVoltT_Low_Fault = 0;
				s_u16Cnt_GridVoltT_Fault_Back = 0;		
            }
        }
        else																			//电网电压正常
        {
            s_u16Cnt_GridVoltT_Low_Fault = 0;
	     	s_u16Cnt_GridVoltT_High_Fault = 0;		
        }	
	}
    else																				//有故障
    {
		if((Calc_Result.f32VGrid_Trms >SafetyReg.f32VGrid_LowLimitBack) && (Calc_Result.f32VGrid_Trms < SafetyReg.f32VGrid_HiLimitBack))
		{																				//电压恢复正常值
			s_u16Cnt_GridVoltT_Fault_Back++;
			if(s_u16Cnt_GridVoltT_Fault_Back >= 300)     								//等待1min清除故障位
			{																			//20ms * 3000 = 60s
				g_SysFaultMessage.bit.VGridTOverRating = 0;
				g_SysFaultMessage.bit.VGridTUnderRating = 0; 		
				s_u16Cnt_GridVoltT_Fault_Back = 0;
			}
		}
		else																			//电压故障未恢复
		{
			s_u16Cnt_GridVoltT_Fault_Back = 0;
		}  
    }
}
/**********************************************************************
* FUNCION :  Grid  frequency Check
* PURPOSE :  grid frequency range checked  for  safety rugulation
*                   
* INPUT :
*        void
* RETURN :
*        void
* CALLS:
*        void
*
* CALLED BY:  void EnergyCalcLowPrio(void)
* 
**********************************************************************/
void GridFreqCheck(void) 
{ 
/*    static Uint16 s_u16Cnt_GridFreq_High_Fault = 0;
    static Uint16 s_u16Cnt_GridFreq_Low_Fault = 0;
    static Uint16 s_u16Cnt_GridFreq_Fault_Back = 0;
*/
	static Uint16 s_u16Cnt_GridRFreq_High_Fault = 0;
    static Uint16 s_u16Cnt_GridRFreq_Low_Fault = 0;
    static Uint16 s_u16Cnt_GridRFreq_Fault_Back = 0;    

/*	static Uint16 s_u16Cnt_GridSFreq_High_Fault = 0;
    static Uint16 s_u16Cnt_GridSFreq_Low_Fault = 0;
    static Uint16 s_u16Cnt_GridSFreq_Fault_Back = 0;

	static Uint16 s_u16Cnt_GridTFreq_High_Fault = 0;
    static Uint16 s_u16Cnt_GridTFreq_Low_Fault = 0;
    static Uint16 s_u16Cnt_GridTFreq_Fault_Back = 0;

//--------------------------------PLL frequency------------------------------------		//锁相频率故障保护
    if((0 == g_SysFaultMessage.bit.FreGridOverRating) && (0 == g_SysFaultMessage.bit.FreGridUnderRating))
    {																					//无故障
		s_u16Cnt_GridFreq_Fault_Back = 0;												//故障恢复计数清零		
        if(Calc_Result.f32GridFreq >= SafetyReg.f32FreGrid_Hi2Limit) 					//两种上限频率，第二种
    	{																				//保护值50.5Hz，直接保护
			g_SysFaultMessage.bit.FreGridOverRating = 1;
			s_u16Cnt_GridFreq_Fault_Back = 0;		 
		}
		else if (Calc_Result.f32GridFreq <= SafetyReg.f32FreGrid_Low2Limit) 			//两种下限频率，第二种
		{																				//保护值48.0Hz，直接保护
			g_SysFaultMessage.bit.FreGridUnderRating = 1;
			s_u16Cnt_GridFreq_Fault_Back = 0;				 
		}
		else if(Calc_Result.f32GridFreq >= SafetyReg.f32FreGrid_HiLimit) 				//两种上限频率，第一种
        {																				//保护值50.2Hz，等待2min进行保护
            s_u16Cnt_GridFreq_High_Fault++;
            if(s_u16Cnt_GridFreq_High_Fault >= SafetyReg.f32FreGrid_ProtectionTime) 	//2min		
            {
                s_u16Cnt_GridFreq_High_Fault = 0;
                g_SysFaultMessage.bit.FreGridOverRating = 1;
            }
        }
        else if(Calc_Result.f32GridFreq <= SafetyReg.f32FreGrid_LowLimit)   			//两种下限频率，第一种
        {																				//保护值49.5Hz，等待10min进行保护
            s_u16Cnt_GridFreq_Low_Fault++;
            if (s_u16Cnt_GridFreq_Low_Fault >= SafetyReg.f32FreGrid_ProtectionTime * 5)	//2min * 5 = 10min		
            {
                s_u16Cnt_GridFreq_Low_Fault = 0;
                g_SysFaultMessage.bit.FreGridUnderRating = 1;	  
            }
        }
        else																			//电网频率正常
        {
            s_u16Cnt_GridFreq_High_Fault = 0;
            s_u16Cnt_GridFreq_Low_Fault = 0;
        }
    }
    else																				//有故障
    {
        if((Calc_Result.f32GridFreq < SafetyReg.f32FreGrid_HiLimit) && (Calc_Result.f32GridFreq > SafetyReg.f32FreGrid_LowLimit)) //落在安全频率以内
        {																				//电网频率恢复正常
            s_u16Cnt_GridFreq_Fault_Back++;   											//等待1min清除故障位
            if (s_u16Cnt_GridFreq_Fault_Back > 300)      								//20ms * 3000= 60s
            {
                s_u16Cnt_GridFreq_Fault_Back = 0;
                g_SysFaultMessage.bit.FreGridOverRating = 0;
                g_SysFaultMessage.bit.FreGridUnderRating = 0;	  		  
            }
        }
        else																			//电网频率故障未恢复
        {
            s_u16Cnt_GridFreq_Fault_Back = 0;
        }

    }
*/
//----------------------------------------R----------------------------------------		//R相频率故障保护
    if((0 == g_SysFaultMessage.bit.FreGridROverRating) && (0 == g_SysFaultMessage.bit.FreGridRUnderRating))
    {																					//无故障
		s_u16Cnt_GridRFreq_Fault_Back = 0;												//故障恢复计数清零		
        if(Calc_Result.f32GridRFreq >= SafetyReg.f32FreGrid_Hi2Limit) 					//两种上限频率，第二种
    	{																				//保护值50.5Hz，直接保护
			g_SysFaultMessage.bit.FreGridROverRating = 1;
			s_u16Cnt_GridRFreq_Fault_Back = 0;		 
		}
		else if (Calc_Result.f32GridRFreq <= SafetyReg.f32FreGrid_Low2Limit) 			//两种下限频率，第二种
		{																				//保护值48.0Hz，直接保护
			g_SysFaultMessage.bit.FreGridRUnderRating = 1;
			s_u16Cnt_GridRFreq_Fault_Back = 0;				 
		}
		else if(Calc_Result.f32GridRFreq >= SafetyReg.f32FreGrid_HiLimit) 				//两种上限频率，第一种
        {																				//保护值50.2Hz，等待2min进行保护
            s_u16Cnt_GridRFreq_High_Fault++;
            if(s_u16Cnt_GridRFreq_High_Fault >= SafetyReg.f32FreGrid_ProtectionTime) 	//2min		
            {
                s_u16Cnt_GridRFreq_High_Fault = 0;
                g_SysFaultMessage.bit.FreGridROverRating = 1;
            }
        }
        else if(Calc_Result.f32GridRFreq <= SafetyReg.f32FreGrid_LowLimit)   			//两种下限频率，第一种
        {																				//保护值49.5Hz，等待10min进行保护
            s_u16Cnt_GridRFreq_Low_Fault++;
            if (s_u16Cnt_GridRFreq_Low_Fault >= SafetyReg.f32FreGrid_ProtectionTime * 5)//2min * 5 = 10min		
            {
                s_u16Cnt_GridRFreq_Low_Fault = 0;
                g_SysFaultMessage.bit.FreGridRUnderRating = 1;	  
            }
        }
        else																			//电网频率正常
        {
            s_u16Cnt_GridRFreq_High_Fault = 0;
            s_u16Cnt_GridRFreq_Low_Fault = 0;
        }
    }
    else																				//有故障
    {
        if((Calc_Result.f32GridRFreq < SafetyReg.f32FreGrid_HiLimit) && (Calc_Result.f32GridRFreq > SafetyReg.f32FreGrid_LowLimit)) //落在安全频率以内
        {																				//电网频率恢复正常
            s_u16Cnt_GridRFreq_Fault_Back++;   											//等待1min清除故障位
            if (s_u16Cnt_GridRFreq_Fault_Back > 300)      								//20ms * 3000= 60s
            {
                s_u16Cnt_GridRFreq_Fault_Back = 0;
                g_SysFaultMessage.bit.FreGridROverRating = 0;
                g_SysFaultMessage.bit.FreGridRUnderRating = 0;	  		  
            }
        }
        else																			//电网频率故障未恢复
            s_u16Cnt_GridRFreq_Fault_Back = 0;
    }

//----------------------------------------S----------------------------------------		//S相频率故障保护
/*    if((0 == g_SysFaultMessage.bit.FreGridSOverRating) && (0 == g_SysFaultMessage.bit.FreGridSUnderRating))
    {																					//无故障
		s_u16Cnt_GridSFreq_Fault_Back = 0;												//故障恢复计数清零		
        if(Calc_Result.f32GridSFreq >= SafetyReg.f32FreGrid_Hi2Limit) 					//两种上限频率，第二种
    	{																				//保护值50.5Hz，直接保护
			g_SysFaultMessage.bit.FreGridSOverRating = 1;
			s_u16Cnt_GridSFreq_Fault_Back = 0;		 
		}
		else if (Calc_Result.f32GridSFreq <= SafetyReg.f32FreGrid_Low2Limit) 			//两种下限频率，第二种
		{																				//保护值48.0Hz，直接保护
			g_SysFaultMessage.bit.FreGridSUnderRating = 1;
			s_u16Cnt_GridSFreq_Fault_Back = 0;				 
		}
		else if(Calc_Result.f32GridSFreq >= SafetyReg.f32FreGrid_HiLimit) 				//两种上限频率，第一种
        {																				//保护值50.2Hz，等待2min进行保护
            s_u16Cnt_GridSFreq_High_Fault++;
            if(s_u16Cnt_GridSFreq_High_Fault >= SafetyReg.f32FreGrid_ProtectionTime) 	//2min		
            {
                s_u16Cnt_GridSFreq_High_Fault = 0;
                g_SysFaultMessage.bit.FreGridSOverRating = 1;
            }
        }
        else if(Calc_Result.f32GridSFreq <= SafetyReg.f32FreGrid_LowLimit)   			//两种下限频率，第一种
        {																				//保护值49.5Hz，等待10min进行保护
            s_u16Cnt_GridSFreq_Low_Fault++;
            if (s_u16Cnt_GridSFreq_Low_Fault >= SafetyReg.f32FreGrid_ProtectionTime * 5)//2min * 5 = 10min		
            {
                s_u16Cnt_GridSFreq_Low_Fault = 0;
                g_SysFaultMessage.bit.FreGridSUnderRating = 1;	  
            }
        }
        else																			//电网频率正常
        {
            s_u16Cnt_GridSFreq_High_Fault = 0;
            s_u16Cnt_GridSFreq_Low_Fault = 0;
        }
    }
    else																				//有故障
    {
        if((Calc_Result.f32GridSFreq < SafetyReg.f32FreGrid_HiLimit) && (Calc_Result.f32GridSFreq > SafetyReg.f32FreGrid_LowLimit)) //落在安全频率以内
        {																				//电网频率恢复正常
            s_u16Cnt_GridSFreq_Fault_Back++;   											//等待1min清除故障位
            if (s_u16Cnt_GridSFreq_Fault_Back > 300)      								//20ms * 3000= 60s
            {
                s_u16Cnt_GridSFreq_Fault_Back = 0;
                g_SysFaultMessage.bit.FreGridSOverRating = 0;
                g_SysFaultMessage.bit.FreGridSUnderRating = 0;	  		  
            }
        }
        else																			//电网频率故障未恢复
            s_u16Cnt_GridSFreq_Fault_Back = 0;
    }

//----------------------------------------T----------------------------------------		//T相频率故障保护
    if((0 == g_SysFaultMessage.bit.FreGridTOverRating) && (0 == g_SysFaultMessage.bit.FreGridTUnderRating))
    {																					//无故障
		s_u16Cnt_GridTFreq_Fault_Back = 0;												//故障恢复计数清零		
        if(Calc_Result.f32GridTFreq >= SafetyReg.f32FreGrid_Hi2Limit) 					//两种上限频率，第二种
    	{																				//保护值50.5Hz，直接保护
			g_SysFaultMessage.bit.FreGridTOverRating = 1;
			s_u16Cnt_GridTFreq_Fault_Back = 0;		 
		}
		else if (Calc_Result.f32GridTFreq <= SafetyReg.f32FreGrid_Low2Limit) 			//两种下限频率，第二种
		{																				//保护值48.0Hz，直接保护
			g_SysFaultMessage.bit.FreGridTUnderRating = 1;
			s_u16Cnt_GridTFreq_Fault_Back = 0;				 
		}
		else if(Calc_Result.f32GridTFreq >= SafetyReg.f32FreGrid_HiLimit) 				//两种上限频率，第一种
        {																				//保护值50.2Hz，等待2min进行保护
            s_u16Cnt_GridTFreq_High_Fault++;
            if(s_u16Cnt_GridTFreq_High_Fault >= SafetyReg.f32FreGrid_ProtectionTime) 	//2min		
            {
                s_u16Cnt_GridTFreq_High_Fault = 0;
                g_SysFaultMessage.bit.FreGridTOverRating = 1;
            }
        }
        else if(Calc_Result.f32GridTFreq <= SafetyReg.f32FreGrid_LowLimit)   			//两种下限频率，第一种
        {																				//保护值49.5Hz，等待10min进行保护
            s_u16Cnt_GridTFreq_Low_Fault++;
            if (s_u16Cnt_GridTFreq_Low_Fault >= SafetyReg.f32FreGrid_ProtectionTime * 5)//2min * 5 = 10min		
            {
                s_u16Cnt_GridTFreq_Low_Fault = 0;
                g_SysFaultMessage.bit.FreGridTUnderRating = 1;	  
            }
        }
        else																			//电网频率正常
        {
            s_u16Cnt_GridTFreq_High_Fault = 0;
            s_u16Cnt_GridTFreq_Low_Fault = 0;
        }
    }
    else																				//有故障
    {
        if((Calc_Result.f32GridTFreq < SafetyReg.f32FreGrid_HiLimit) && (Calc_Result.f32GridTFreq > SafetyReg.f32FreGrid_LowLimit)) //落在安全频率以内
        {																				//电网频率恢复正常
            s_u16Cnt_GridTFreq_Fault_Back++;   											//等待1min清除故障位
            if (s_u16Cnt_GridTFreq_Fault_Back > 300)      								//20ms * 3000= 60s
            {
                s_u16Cnt_GridTFreq_Fault_Back = 0;
                g_SysFaultMessage.bit.FreGridTOverRating = 0;
                g_SysFaultMessage.bit.FreGridTUnderRating = 0;	  		  
            }
        }
        else																			//电网频率故障未恢复
            s_u16Cnt_GridTFreq_Fault_Back = 0;
    }
*/}

/**********************************************************************
* FUNCION :  Grid Current DC injection Check 
* PURPOSE :  Grid Current DC injection range checked   
*                   
* INPUT :
*        void
* RETURN :
*        void
* CALLS:
*        void
*
* CALLED BY:  void EnergyCalcLowPrio(void)
* 
**********************************************************************/
void GridCurrentDciCheck(void)
{
    static  Uint16  s_u16Cnt_R_GridCurDc_Fault = 0;
	static	Uint16	s_u16Cnt_R_GridCurDc_FaultBack = 0;

	static  Uint16  s_u16Cnt_S_GridCurDc_Fault = 0;
	static	Uint16	s_u16Cnt_S_GridCurDc_FaultBack = 0;

	static  Uint16  s_u16Cnt_T_GridCurDc_Fault = 0;
	static	Uint16	s_u16Cnt_T_GridCurDc_FaultBack = 0;

//----------------------------------------R---------------------------------			//R相电网电流直流分量
	if((GridTiedState == g_Sys_Current_State) && (0 == g_SysFaultMessage.bit.DCIGridROverLimit))   
    {																					//无故障
        if(fabs(Calc_Result.f32DCIR) > SafetyReg.f32DCI_Limit)							//高于限值5A
        {
            s_u16Cnt_R_GridCurDc_Fault++;
            if(s_u16Cnt_R_GridCurDc_Fault >= SafetyReg.f32DCI_ProtectionTime) 			//100ms
            {
                s_u16Cnt_R_GridCurDc_Fault = 0;
                g_SysFaultMessage.bit.DCIGridROverLimit = 1;
            }
        }
		else
			s_u16Cnt_R_GridCurDc_Fault = 0;
	}
	else if(1 == g_SysFaultMessage.bit.DCIGridROverLimit)
	{
        if (abs(Calc_Result.f32DCIR) < SafetyReg.f32DCI_Limit)//DCI_1A5)
        {
            s_u16Cnt_R_GridCurDc_FaultBack++;
            if (s_u16Cnt_R_GridCurDc_FaultBack >= 300) // 6s
            {
                s_u16Cnt_R_GridCurDc_FaultBack = 0;
                g_SysFaultMessage.bit.DCIGridROverLimit = 0;
            }
        }
		else
			s_u16Cnt_R_GridCurDc_FaultBack = 0;
	}        

//----------------------------------------S---------------------------------
	if((GridTiedState == g_Sys_Current_State) && (0 == g_SysFaultMessage.bit.DCIGridSOverLimit))   
    {
        if (abs(Calc_Result.f32DCIS) > SafetyReg.f32DCI_Limit)//DCI_1A5)
        {
            s_u16Cnt_S_GridCurDc_Fault++;
            if (s_u16Cnt_S_GridCurDc_Fault >= SafetyReg.f32DCI_ProtectionTime) // 200ms
            {
                s_u16Cnt_S_GridCurDc_Fault = 0;
                g_SysFaultMessage.bit.DCIGridSOverLimit = 1;
            }
        }
		else
			s_u16Cnt_S_GridCurDc_Fault = 0;
	}
	else if(1 == g_SysFaultMessage.bit.DCIGridSOverLimit)
	{
        if (abs(Calc_Result.f32DCIS) < SafetyReg.f32DCI_Limit)//DCI_1A5)
        {
            s_u16Cnt_S_GridCurDc_FaultBack++;
            if (s_u16Cnt_S_GridCurDc_FaultBack >= 300) // 6s
            {
                s_u16Cnt_S_GridCurDc_FaultBack = 0;
                g_SysFaultMessage.bit.DCIGridSOverLimit = 0;
            }
        }
		else
			s_u16Cnt_S_GridCurDc_FaultBack = 0;
	}

//----------------------------------------T---------------------------------
	if((GridTiedState == g_Sys_Current_State) && (0 == g_SysFaultMessage.bit.DCIGridTOverLimit))   
    {
        if (abs(Calc_Result.f32DCIT) > SafetyReg.f32DCI_Limit)//DCI_1A5)
        {
            s_u16Cnt_T_GridCurDc_Fault++;
            if (s_u16Cnt_T_GridCurDc_Fault >= SafetyReg.f32DCI_ProtectionTime) // 200ms
            {
                s_u16Cnt_T_GridCurDc_Fault = 0;
                g_SysFaultMessage.bit.DCIGridTOverLimit = 1;
            }
        }
		else
			s_u16Cnt_T_GridCurDc_Fault = 0;
	}
	else if(1 == g_SysFaultMessage.bit.DCIGridTOverLimit)
	{
        if (abs(Calc_Result.f32DCIT) < SafetyReg.f32DCI_Limit)//DCI_1A5)
        {
            s_u16Cnt_T_GridCurDc_FaultBack++;
            if (s_u16Cnt_T_GridCurDc_FaultBack >= 300) // 6s
            {
                s_u16Cnt_T_GridCurDc_FaultBack = 0;
                g_SysFaultMessage.bit.DCIGridTOverLimit = 0;
            }
        }
		else
			s_u16Cnt_T_GridCurDc_FaultBack = 0;
	}
}


/**********************************************************************
* FUNCION :  Pv Isolation Check
* PURPOSE :  check if Pv Isolation is less that 2M  for safety rugulation
*                   
* INPUT :
*        void
* RETURN :
*        void
* CALLS:
*        void
*
* CALLED BY:  void EnergyCalcLowPrio(void)
* //if(0==g_StateCheck.bit.IsoCheckOver) 
*	{
		PvInsulationCheck();//put it in check state();
	}
**********************************************************************/

void   PvInsulationCheck(void)
{
	static Uint16 s_u16Cnt_Insulation_Check_Counter = 0;
	static Uint16 s_u16Cnt_PvIso_Fault = 0;
	static float32 InsulationTempa,InsulationTempb,InsulationTempc,InsulationTempd;
//	float32 InsulationTempd,InsulationTempf;
	float32 a = 1980000; 	//a = R1 	= 1980K
	float32 b = 3960000;	//b = 2*R1	= 3960K
	float32 c = 2000000;	//c = R1+R2	= 2000K
	float32 Riso1a = 0;
	float32 Riso1b = 0;
	float32 ftemp1 = 0;
	float32 ftemp2 = 0;

	s_u16Cnt_Insulation_Check_Counter++;

	if(s_u16Cnt_Insulation_Check_Counter < 1000)
		PV1_ISO_TEST_RLY_ON;
    
	// 第一步
	if((s_u16Cnt_Insulation_Check_Counter >= 1000)&&(s_u16Cnt_Insulation_Check_Counter < 2000))    //1500
	{
		if((Calc_Result.f32Viso > 0.2f )||(Calc_Result.f32Viso < (-0.2f) ))	//4.096*15K*2/19.02k
		{
			g_SysFaultMessage.bit.PvIsoFault = 1;              //need to be detailed
			g_StateCheck.bit.IsoCheckOver = 0;
			s_u16Cnt_Insulation_Check_Counter = 0;
			PV1_ISO_TEST_RLY_OFF;
		}
	}

	// 第二步
	if((s_u16Cnt_Insulation_Check_Counter >= 2000)&&(s_u16Cnt_Insulation_Check_Counter < 2990))
	{
		PV1_ISO_TEST_RLY_OFF;
		if(s_u16Cnt_Insulation_Check_Counter >= 2500)
		{
			if((Calc_Result.f32Viso > 1.1 * 0.0033557 * Calc_Result.f32VPV)||(Calc_Result.f32Viso < 0.9 * 0.0033557 * Calc_Result.f32VPV))   //20/(3*6*348+20)
			{

				g_SysFaultMessage.bit.PvIsoFault = 1;
				g_StateCheck.bit.IsoCheckOver = 0;
				s_u16Cnt_Insulation_Check_Counter = 0;
			}
		}
	}

	//第三步
	if((s_u16Cnt_Insulation_Check_Counter >= 3000)&&(s_u16Cnt_Insulation_Check_Counter < 5990))
	{
			PV1_ISO_TEST_RLY_ON;
			PV1_ISO_GROUND_RLY_ON;
			if(s_u16Cnt_Insulation_Check_Counter >= 5000)
			{
				InsulationTempa = 100.0f * Calc_Result.f32Viso;
				InsulationTempb = (Calc_Result.f32VPV - InsulationTempa) / InsulationTempa;
			}
	}
	//第四步
	if(s_u16Cnt_Insulation_Check_Counter >= 6000)
	{
		PV1_ISO_TEST_RLY_OFF;
		if(s_u16Cnt_Insulation_Check_Counter >= 7500)
		{
			InsulationTempc = 100.0f * Calc_Result.f32Viso;
			InsulationTempd = (Calc_Result.f32VPV - InsulationTempc)/InsulationTempc;
			ftemp1 = b * (InsulationTempb - InsulationTempd) / (InsulationTempd + 2);
			Riso1a = sqrt(ftemp1 * ftemp1);
			ftemp2 = a * c * Riso1a/ (a * c * InsulationTempb - (a + c) * Riso1a);
			Riso1b = sqrt(ftemp2 * ftemp2);
			Rx1 = Riso1a;
			Rx2 = Riso1b;
			if((Riso1a >= 27000) && (Riso1b >= 27000))
			{
				g_SysFaultMessage.bit.PvIsoFault = 0;
				g_StateCheck.bit.IsoCheckOver = 1;
				s_u16Cnt_Insulation_Check_Counter = 0;
				PV1_ISO_GROUND_RLY_OFF;
			}
			else
			{
				s_u16Cnt_PvIso_Fault++;
				if(s_u16Cnt_PvIso_Fault >= 1)
				{
					g_SysFaultMessage.bit.PvIsoFault = 1;
					g_StateCheck.bit.IsoCheckOver = 1;
					s_u16Cnt_Insulation_Check_Counter = 0;
					PV1_ISO_GROUND_RLY_OFF;
					s_u16Cnt_PvIso_Fault = 0;
				}
			}
		}
	}
}

/**********************************************************************
* FUNCION :  Grid Current Check
* PURPOSE :  Three phase grid current range limited  for  safeguard
*                   
* INPUT :
*        void
* RETURN :
*        void
* CALLS:
*        void
*
* CALLED BY:  void EnergyCalcLowPrio(void)
* 
**********************************************************************/
//------------------------------------xlg GFCI----------------------------------
void GFCICheck2(void)  
{

    static Uint16  GFCIFaultCnt = 0;

    if((Calc_Result.f32GFCIrms > 50) || (Calc_Result.f32GFCIrms < 5))
    {
        GFCIFaultCnt++;
        if(GFCIFaultCnt > 5)
        {
            g_SysFaultMessage.bit.HWADFault_GFCI = 1;
            //g_SysFaultMessage.bit.HWADFault = 1;
            GFCIFaultCnt = 0;
        }
    }
}

/**************************************************************************
 *                                                                        *
 **************************************************************************/

void GFCICheck1(void)
{
    static unsigned char bGFCIDEVICEfilter;
    if(Calc_Result.f32GFCIrms > 30)   //20
    {
        bGFCIDEVICEfilter++;
        if(bGFCIDEVICEfilter > 5)
        {
            g_SysFaultMessage.bit.HWADFault_GFCI = 1;
            //g_SysFaultMessage.bit.HWADFault = 1;
            bGFCIDEVICEfilter = 0;
        }
    }
    else
    {
       
        bGFCIDEVICEfilter = 0;
    }

}

void GFCICheck0(void)
{
	float32 wDealtaGFI;
	float32 wDifferenceGFI=0;
	static float32 wGFIBuf[3];
	static float32 wDeltaBase;
	static Uint16 ubGFCIcnt;
	static Uint16 wCheckGFCIDelay = 0;

	static Uint16 s_u16Cnt_GFCI_Rms_Fault = 0;
	static Uint16 s_u16Cnt_GFCI_Jump1_Fault = 0;
	static Uint16 s_u16Cnt_GFCI_Jump2_Fault = 0;	  
	static Uint16 wFaultBackFilter = 0;
	static Uint16 ubBreakFlag = 0;
	static Uint16 ubBreakSwFlag = 0;

//	wGFIAvg = (wGFIBuf2[0] + wGFIBuf2[1] + wGFIBuf2[2] + wGFIBuf2[3]) / 4; //4


	wGFIBuf[0] =  wGFIBuf[1]; 
	wGFIBuf[1] =  wGFIBuf[2]; 
	wGFIBuf[2] =  Calc_Result.f32GFCIrms;


	if(g_Sys_Current_State== GridTiedState)
	{
		wCheckGFCIDelay ++;
	}
	else
	{
		wCheckGFCIDelay = 0;
	}

	if(wGFIBuf[2] >= wGFIBuf[0])
	{
		wDealtaGFI = wGFIBuf[2] - wGFIBuf[0]; 
	}
	else
	{
		wDealtaGFI = wGFIBuf[0] - wGFIBuf[2]; 
	}


	if(wCheckGFCIDelay >= 150)  //  3s-->6s-->10s    500    
	{
		if(Calc_Result.f32GFCIrms > SafetyReg.f32GFCI_RmsLimit)     //250mA
		{
			s_u16Cnt_GFCI_Rms_Fault++;
			if(s_u16Cnt_GFCI_Rms_Fault > SafetyReg.f32GFCI_RmsProtectionTime)
			{
				g_SysFaultMessage.bit.GFCIJump3Fault = 1;  
				s_u16Cnt_GFCI_Rms_Fault = 0;
//				wGFCIFaultValue = wGFCurrent;
			}
		}
		else
		{
			s_u16Cnt_GFCI_Rms_Fault = 0;
		}  


		if(ubBreakFlag == 0 && wDealtaGFI>1.5 ) 
		{
			if(wGFIBuf[2] > wGFIBuf[0])
			{
				wDifferenceGFI = sqrt(wGFIBuf[2] * wGFIBuf[2] -wGFIBuf[0] * wGFIBuf[0]);	
			}
			else
			{
				wDifferenceGFI = sqrt(wGFIBuf[0] * wGFIBuf[0] - wGFIBuf[2] * wGFIBuf[2]);	 
			}	
		}
		else if(ubBreakFlag == 1 )
		{
			if(wGFIBuf[2] > wDeltaBase)
			{
				wDifferenceGFI = sqrt(wGFIBuf[2] * wGFIBuf[2] -wDeltaBase * wDeltaBase);	 
			}
			else
			{
				wDifferenceGFI = sqrt(wDeltaBase * wDeltaBase - wGFIBuf[2] * wGFIBuf[2]);	
			}	

		}
		if(wDifferenceGFI >= SafetyReg.f32GFCI_Jump3Limit) 
		{
			s_u16Cnt_GFCI_Jump1_Fault = 0;
			s_u16Cnt_GFCI_Jump2_Fault = 0;		
			ubBreakFlag = 0;
			ubBreakSwFlag= 0;
			wDeltaBase = 0;			
			g_SysFaultMessage.bit.GFCIFault = 1;  
			g_SysFaultMessage.bit.GFCIJump3Fault = 1;	
		}
		else if(wDifferenceGFI >= SafetyReg.f32GFCI_Jump2Limit)          
		{
			s_u16Cnt_GFCI_Jump1_Fault = 0;
			ubBreakFlag = 1;
			s_u16Cnt_GFCI_Jump2_Fault++;	
			if((ubBreakSwFlag&0x02)==0) 	
			{
				wDeltaBase = wGFIBuf[0];  
				ubBreakSwFlag = 2;			  
			}
			if(s_u16Cnt_GFCI_Jump2_Fault >= SafetyReg.f32GFCI_Jump2ProtectionTime)   //150ms(110ms)
			{
				s_u16Cnt_GFCI_Jump2_Fault = 0;
				ubBreakSwFlag= 0;
				ubBreakFlag = 0;
				wDeltaBase = 0;	
				g_SysFaultMessage.bit.GFCIFault = 1; 
				g_SysFaultMessage.bit.GFCIJump2Fault = 1;
			}				

		}
		else if(wDifferenceGFI >= SafetyReg.f32GFCI_Jump1Limit)     //   =  
		{
			s_u16Cnt_GFCI_Jump1_Fault++;
			s_u16Cnt_GFCI_Jump2_Fault = 0;	
			ubBreakFlag = 1;
			if((ubBreakSwFlag&0x01)==0) 	
			{
				wDeltaBase = wGFIBuf[0];  
				ubBreakSwFlag = 1;			  
			}	   
			if(s_u16Cnt_GFCI_Jump1_Fault >= SafetyReg.f32GFCI_Jump1ProtectionTime)          // 300ms(160ms)
			{
				s_u16Cnt_GFCI_Jump1_Fault = 0;
				wDeltaBase = 0;	
				ubBreakSwFlag= 0;	  	  
				g_SysFaultMessage.bit.GFCIFault = 1;   	
				g_SysFaultMessage.bit.GFCIJump1Fault = 1;		
				ubBreakFlag = 0;
			}		  
		}
		else
		{ 
			s_u16Cnt_GFCI_Jump1_Fault = 0;
			s_u16Cnt_GFCI_Jump2_Fault = 0;	
			ubGFCIcnt++;
			if(ubGFCIcnt>25) 
			{
				ubGFCIcnt = 0;
				ubBreakFlag = 0;  
				ubBreakSwFlag = 0;			 
				wDeltaBase = 0;			 
			}  
		}
		wDifferenceGFI = 0;	
		wCheckGFCIDelay = 150;
	 }


	if((1 == g_SysFaultMessage.bit.GFCIJump3Fault)||(g_SysFaultMessage.bit.GFCIJump3Fault==1)||(g_SysFaultMessage.bit.GFCIJump2Fault == 1)||(g_SysFaultMessage.bit.GFCIJump1Fault == 1))
	{
		g_SysFaultMessage.bit.GFCIFault = 1;
	}
	else
	{
		g_SysFaultMessage.bit.GFCIFault = 0;
	}
//		if((wGFCIOffsetValue > 0 && wGFIAvg < (wGFCIIniValue + 10)) || 
//		(wGFCIOffsetValue < 0 && wGFIAvg < (wGFCIIniValue + 5)))
//	
	if(g_SysFaultMessage.bit.GFCIFault == 1)
	{	
		if(Calc_Result.f32GFCIrms < 50)
		{
			wFaultBackFilter++;
			if(wFaultBackFilter > 275)
			{
				g_SysFaultMessage.bit.GFCIJump1Fault=0;
				g_SysFaultMessage.bit.GFCIJump2Fault=0;
				g_SysFaultMessage.bit.GFCIJump3Fault=0;
				g_SysFaultMessage.bit.GFCIJump3Fault = 0;

			}
		} 
	}
	else
	{
		wFaultBackFilter = 0;
	}

}

void GFCICheck(void)
{
	static Uint16 GFCI50mAindex = 0;
	static Uint16 GFCIOFFsetNum = 0;
	

    if(U16CntFirstStartDelay10s > 4000)
    {

        if(U16CntFirstStartDelay10s <= 4825 && U16CntFirstStartDelay10s >= 4800) // 350ms
        {
          //  GFCI_50mA_ON; 
        }
        else if(U16CntFirstStartDelay10s <= 4775 && U16CntFirstStartDelay10s >=4750)
        {
            //GFCI_50mA_OFF;

        }
        else if(U16CntFirstStartDelay10s < 4500)
        {

            GFCI50mAindex++;
            if(GFCI50mAindex <= 30)
            {
//				GFCICheck1();  
				ADChannelOffset.f32GFCI = Calc_Result.f32GFCIave; 
 				if(GFCIOFFsetNum == 0)
				{
//					ADChannelOffset.f32GFCI = Calc_Result.f32GFCIrms; 
					GFCICheck1();
            		GFCIOFFsetNum = 1;
            	}

            }
            else
            {
                if(GFCI50mAindex == 31)
                {
              //      GFCI_50mA_ON ;
                }
                else if(GFCI50mAindex >= 40)//33
                {
					Calc_Result.f32GFCIrmsSave = Calc_Result.f32GFCIrms;
                    GFCICheck2();                                      		//test new output xlx
                }
            }
        }
//		GFCI50mAindex = 0;   
    }
    else
    {
        //GFCI_50mA_OFF;
        GFCICheck0();

    }
}



/////////////////////////////////////////////////////////////////////////////////////////

// AD chanel check 偏置量的重新定义
void ADOffsetCheck(void)
{

	ADChannelOffset.f32IPV = Calc_Result.f32IPV;

	// DCI channel offset
	if ((abs(Calc_Result.f32DCIR) > AD_Channel_Offset_DCI_Limit)||(abs(Calc_Result.f32DCIS) > AD_Channel_Offset_DCI_Limit)||(abs(Calc_Result.f32DCIT) > AD_Channel_Offset_DCI_Limit) )
	{
		g_SysFaultMessage.bit.DCIGridROverLimit = 1;
		//g_SysFaultMessage.bit.HWADFault = 1;
	}
	

	else
	{
		ADChannelOffset.f32DCIR = Calc_Result.f32DCIR;
		ADChannelOffset.f32DCIS = Calc_Result.f32DCIS;
		ADChannelOffset.f32DCIT = Calc_Result.f32DCIT;
	}


  //GFCI channel

	// grid current channel
	if ((abs(Calc_Result.f32IGrid_Rave) > AD_Channel_Offset_IGridLimit)||(abs(Calc_Result.f32IGrid_Save) > AD_Channel_Offset_IGridLimit)||(abs(Calc_Result.f32IGrid_Tave) > AD_Channel_Offset_IGridLimit))
	{
		g_SysFaultMessage.bit.DCIGridROverLimit = 1;
	//	g_SysFaultMessage.bit.HWADFault = 1;
	}
	else
	{
		ADChannelOffset.f32IGrid_R = Calc_Result.f32IGrid_Rave;
		ADChannelOffset.f32IGrid_S = Calc_Result.f32IGrid_Save;
		ADChannelOffset.f32IGrid_T = Calc_Result.f32IGrid_Tave;
	}

	// grid voltage channel
	if ((abs(Calc_Result.f32VGrid_Rave) > AD_Channel_Offset_VGridLimit)||(abs(Calc_Result.f32VGrid_Save) > AD_Channel_Offset_VGridLimit)||(abs(Calc_Result.f32VGrid_Tave) > AD_Channel_Offset_VGridLimit))
	{
		g_SysFaultMessage.bit.Fault_VGrid_ave = 1;
	//	g_SysFaultMessage.bit.HWADFault = 1;
	}
	else
	{
		ADChannelOffset.f32VGrid_R = Calc_Result.f32VGrid_Rave;
		ADChannelOffset.f32VGrid_S = Calc_Result.f32VGrid_Save;
		ADChannelOffset.f32VGrid_T = Calc_Result.f32VGrid_Tave;
	}

	// INV voltage channel, 0511
	if ((abs(Calc_Result.f32VInv_Rave) > AD_Channel_Offset_VGridLimit)||(abs(Calc_Result.f32VInv_Save) > AD_Channel_Offset_VGridLimit)||(abs(Calc_Result.f32VInv_Tave) > AD_Channel_Offset_VGridLimit))
	{
		g_SysFaultMessage.bit.Fault_Vinv_ave = 1;
	}
	else
	{
		ADChannelOffset.f32VInv_R = Calc_Result.f32VInv_Rave;
		ADChannelOffset.f32VInv_S = Calc_Result.f32VInv_Save;
		ADChannelOffset.f32VInv_T = Calc_Result.f32VInv_Tave;
	}

}


/**********************************************************************
* FUNCION :  Grid Current Check
* PURPOSE :  Three phase grid current range limited  for  safeguard
*                   
* INPUT :
*        void
* RETURN :
*        void
* CALLS:
*        void
*
* CALLED BY:  void EnergyCalcLowPrio(void)
* 
**********************************************************************/
void GridCurrentCheck(void)
{
    static  Uint16  s_u16Cnt_Ovr_AcCurrent_Delay= 0;
    static  Uint16  s_u16Cnt_Ovr_AcCurrent_FaultBack= 0;
    
	if(0 == g_SysFaultMessage.bit.OCP_AC_RMS_Fault)  //前一状态没有错误
	{    
		if((Calc_Result.f32IGrid_Rrms > OverRated_OutputCurrent)||(Calc_Result.f32IGrid_Srms > OverRated_OutputCurrent)||(Calc_Result.f32IGrid_Trms > OverRated_OutputCurrent))   
	    {  //现在有错误了，进行延时判断
	        s_u16Cnt_Ovr_AcCurrent_Delay++;
	        if(s_u16Cnt_Ovr_AcCurrent_Delay >= 3) // 1ms     
	        {
	            s_u16Cnt_Ovr_AcCurrent_Delay = 0;
	            g_SysFaultMessage.bit.OCP_AC_RMS_Fault = 1;			// disabled by XXXXX 20111005
	        }
	    }
	    else
	    {
	        s_u16Cnt_Ovr_AcCurrent_Delay = 0;
	    }
	}
	else  //前一状态有错误  
	{
		if(Calc_Result.f32IGrid_Rrms < Rated_OutputCurrentRms)  //现在没有错误，延时判断
		{
	        s_u16Cnt_Ovr_AcCurrent_FaultBack++;
	        if(s_u16Cnt_Ovr_AcCurrent_FaultBack >= 300) // 100ms     
	        {
	            s_u16Cnt_Ovr_AcCurrent_FaultBack = 0;
	            g_SysFaultMessage.bit.OCP_AC_RMS_Fault = 0;
	        }
	    }
	    else
	    {
	        s_u16Cnt_Ovr_AcCurrent_FaultBack = 0;
	    }
	}
}

/**********************************************************************
* FUNCION :  Pv Voltage Check
* PURPOSE :  check if Pv Isolation is less that 2M  for safety rugulation
*                   
* INPUT :
*        void
* RETURN :
*        void
* CALLS:
*        void
*
* CALLED BY:  void EnergyCalcLowPrio(void)
* 
**********************************************************************/
void  PVVoltCheck(void)
{
	static Uint16 s_u16Cnt_PvVolt_Ovr_Fault = 0;
	static Uint16 s_u16Cnt_PvVolt_Ovr_Fault_Back = 0;
//	static Uint16 s_u16Cnt_PvVolt_Low_Fault = 0;
//	static Uint16 s_u16Cnt_PvVolt_Low_Fault_Back = 0;

/*	if(g_SysFaultMessage.bit.PvVoltLowFault == 0)  //前一状态无低压错误
	{
		if(GetRealValue.f32VPV < PV_Under_Volt_Limit) //现在存在低压错误，进行延时判断
		{
			s_u16Cnt_PvVolt_Low_Fault++;
			if(s_u16Cnt_PvVolt_Low_Fault >= 20)      //15*0.02s = 0.3 s (0.2s is enough)
			{ 
				s_u16Cnt_PvVolt_Low_Fault = 0;
				g_SysFaultMessage.bit.PvVoltLowFault = 1;
			}
		}
		else
			s_u16Cnt_PvVolt_Low_Fault = 0;

		s_u16Cnt_PvVolt_Low_Fault_Back = 0;
	}
	else   //前一状态有低压错误  现在没有低压错误  进行延时判断
	{
		if(GetRealValue.f32VPV > PV_Under_Volt_Limit) // f32BusVoltage :average value in 20ms
		{
			s_u16Cnt_PvVolt_Low_Fault_Back++;
			if(s_u16Cnt_PvVolt_Low_Fault_Back >300)//300*0.02s=6s
			{
				s_u16Cnt_PvVolt_Low_Fault_Back = 0;
				g_SysFaultMessage.bit.PvVoltLowFault = 0;  
			}
		}
		else
			s_u16Cnt_PvVolt_Low_Fault_Back = 0;

		s_u16Cnt_PvVolt_Low_Fault = 0;
	}
*/
	if(g_SysFaultMessage.bit.PvVoltOvrFault == 0) //过压保护采样的处理方法
	{
		if((AD_Sum.f32VPV * f32SumCounterReci) > PV_Over_Volt_Limit) 
		{
			s_u16Cnt_PvVolt_Ovr_Fault++;
			if(s_u16Cnt_PvVolt_Ovr_Fault > 1)
			{ 
				s_u16Cnt_PvVolt_Ovr_Fault = 0;
				g_SysFaultMessage.bit.PvVoltOvrFault = 1;
			}  
		}
		else
			s_u16Cnt_PvVolt_Ovr_Fault = 0;

    s_u16Cnt_PvVolt_Ovr_Fault_Back = 0;
	}
	else
	{
		if(Calc_Result.f32VPV < (PV_Over_Volt_Limit - 10) ) // f32BusVoltage :average value in 20ms
		{
			s_u16Cnt_PvVolt_Ovr_Fault_Back++;
			if(s_u16Cnt_PvVolt_Ovr_Fault_Back >300)//5s
			{
				s_u16Cnt_PvVolt_Ovr_Fault_Back = 0;
				g_SysFaultMessage.bit.PvVoltOvrFault = 0;  
			}
		}
		else
			s_u16Cnt_PvVolt_Ovr_Fault_Back = 0;

		s_u16Cnt_PvVolt_Ovr_Fault = 0;
	}
}


//光伏输入端方向放电保护
void PVReversCurrCheck(void)
{
	static Uint16 s_u16Cnt_PvReversCurr_Fault = 0;
	if(g_SysFaultMessage.bit.PvReversCurrFault == 0) 
	{
		if(Calc_Result.f32IPV < -10.0f) 
		{
			s_u16Cnt_PvReversCurr_Fault++;
			if(s_u16Cnt_PvReversCurr_Fault > 1)
			{ 
				s_u16Cnt_PvReversCurr_Fault = 0;
				g_SysFaultMessage.bit.PvReversCurrFault = 1;
			}  
		}
		else
			s_u16Cnt_PvReversCurr_Fault = 0;
	}
}

//inverter over current hardware protection 
void HwInvOCPCheck(void)
{
    static Uint16 s_u16Cnt_InvOCP = 0;
	if(1 == g_StateCheck.bit.AC_OCP)    
	{
		s_u16Cnt_InvOCP++;
		if(s_u16Cnt_InvOCP > 2)
		{
			g_SysFaultMessage.bit.HW_OCP_AC_Fault = 1;
			s_u16Cnt_InvOCP = 0;	
		}
		g_StateCheck.bit.AC_OCP = 0;
	}
	else
	{
		g_SysFaultMessage.bit.HW_OCP_AC_Fault = 0;
	}
}
// Bus over voltage protection
void HwBusOVPCheck(void)
{
	static Uint16 s_u16Cnt_BusOVP = 0;
	if(1 == g_StateCheck.bit.Bus_OVP)  //前一状态没有错 延时检测检测
	{
		s_u16Cnt_BusOVP++;
		if(s_u16Cnt_BusOVP > 2)
		{
			g_SysFaultMessage.bit.HW_BusOVP_Fault = 1;
			s_u16Cnt_BusOVP = 0;	
		}
	}
	else if (0 == g_StateCheck.bit.Bus_OVP)
	{
		g_SysFaultMessage.bit.HW_BusOVP_Fault = 0;	
	}	
}




//  
/*void IslandingCheck(void)
{
static	float32 f32temp1 = 0;
static  Uint8   s_u8Cnt_IslandCheck = 0;
static  Uint8	s_u8Cnt_IslandDetection = 0;
static	Uint8	s_u8Cnt_IslandDisturb = 1;
//static	Uint8	s_u8Cnt_IslandDisturbD	= 0;

float32 f32temp2,f32temp3;


	if(0 == s_u8Cnt_IslandCheck && NormalState == g_Sys_Current_State)	
	{
	f32temp1 = Calc_Result.f32VGrid_Rrms + Calc_Result.f32VGrid_Srms + Calc_Result.f32VGrid_Trms;
	s_u8Cnt_IslandCheck = 1;
	}

	if(NormalState != g_Sys_Current_State)
	{
	s_u8Cnt_IslandCheck = 0;
	}

	if(1 == s_u8Cnt_IslandCheck)
	{

	f32temp2 = Calc_Result.f32VGrid_Rrms + Calc_Result.f32VGrid_Srms + Calc_Result.f32VGrid_Trms;
	f32temp3 = f32temp2 - f32temp1;
	f32temp1 = f32temp2;
		
		if(abs(f32temp3) > f32temp2 * 0.01f)
		{
			s_u8Cnt_IslandDisturb = s_u8Cnt_IslandDisturb * 2;
			if(s_u8Cnt_IslandDisturb > 10)
			{
			s_u8Cnt_IslandDisturb = 10;
			}
			if(f32temp3 > 0)
			{
			BusCon_Reg.f32IdRefDelta = BusCon_Reg.f32Idref_cmd * 0.035f * s_u8Cnt_IslandDisturb;
			BusCon_Reg.f32IqRefDelta = BusCon_Reg.f32IdRefDelta * 0.1f;
			}
			else
			{
			BusCon_Reg.f32IdRefDelta = -BusCon_Reg.f32Idref_cmd * 0.035f * s_u8Cnt_IslandDisturb;
			BusCon_Reg.f32IqRefDelta = BusCon_Reg.f32IdRefDelta * 0.1f;
			}
		}
		else
		{
			s_u8Cnt_IslandDisturb = 1;
			if(49 == s_u8Cnt_IslandDetection)
			{
			BusCon_Reg.f32IdRefDelta = - BusCon_Reg.f32Idref_cmd * 0.035f;
			BusCon_Reg.f32IqRefDelta = BusCon_Reg.f32IdRefDelta * 0.1f;
			s_u8Cnt_IslandDetection = 0;
			}
			else
			{

			BusCon_Reg.f32IdRefDelta = 0;
			BusCon_Reg.f32IqRefDelta = 0;
			s_u8Cnt_IslandDetection++;
			}
		}
	}

}*/


void IslandingCheck(void)
{
	static	float32 f32temp1 = 0;
	static  Uint8   s_u8Cnt_IslandCheck = 0;
	static  Uint8	s_u8Cnt_IslandDetection = 0;
	static	Uint8	s_u8Cnt_IslandDisturb = 1;
	//static	Uint8	s_u8Cnt_IslandDisturbD	= 0;

	float32 f32temp2,f32temp3;


	if((0 == s_u8Cnt_IslandCheck) && (GridTiedState == g_Sys_Current_State))	
	{
		f32temp1 = Calc_Result.f32VGrid_Rrms + Calc_Result.f32VGrid_Srms + Calc_Result.f32VGrid_Trms;
		s_u8Cnt_IslandCheck = 1;
	}

	if(GridTiedState != g_Sys_Current_State)
	{
		s_u8Cnt_IslandCheck = 0;
	}

	if(1 == s_u8Cnt_IslandCheck)
	{

		f32temp2 = Calc_Result.f32VGrid_Rrms + Calc_Result.f32VGrid_Srms + Calc_Result.f32VGrid_Trms;
		f32temp3 = f32temp2 - f32temp1;
		f32temp1 = f32temp2;
		
		if(abs(f32temp3) > f32temp2 * 0.01f)
		{
			s_u8Cnt_IslandDisturb = s_u8Cnt_IslandDisturb * 2;
			if(s_u8Cnt_IslandDisturb > 10)
			{
				s_u8Cnt_IslandDisturb = 10;
			}
			if(f32temp3 > 0)
			{
				BusCon_Reg.f32IdRefDelta = BusCon_Reg.f32Idref_cmd * 0.035f * s_u8Cnt_IslandDisturb;
				BusCon_Reg.f32IqRefDelta = BusCon_Reg.f32IdRefDelta * 0.1f;
			}
			else
			{
				BusCon_Reg.f32IdRefDelta = -BusCon_Reg.f32Idref_cmd * 0.035f * s_u8Cnt_IslandDisturb;
				BusCon_Reg.f32IqRefDelta = BusCon_Reg.f32IdRefDelta * 0.1f;
			}
		}
		else
		{
			s_u8Cnt_IslandDisturb = 1;
			if(49 == s_u8Cnt_IslandDetection)
			{
				BusCon_Reg.f32IdRefDelta = - BusCon_Reg.f32Idref_cmd * 0.035f;
				BusCon_Reg.f32IqRefDelta = BusCon_Reg.f32IdRefDelta * 0.1f;
				s_u8Cnt_IslandDetection = 0;
			}
			else
			{

				BusCon_Reg.f32IdRefDelta = 0;
				BusCon_Reg.f32IqRefDelta = 0;
				s_u8Cnt_IslandDetection++;
			}
		}
	}

}

