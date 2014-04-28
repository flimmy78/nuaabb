/*=============================================================================*
 *         Copyright(c) 
 *                          ALL RIGHTS RESERVED
 *
 *  FILENAME : 500KW_StateUpdate.c 

 *  PURPOSE  : state changed between wait state, check state,
 *             normal state,fault state, 
 * 
 *
 *  HISTORY  :
 *    DATE            VERSION        AUTHOR            NOTE
 *    
 *
 ******************************************************************************/
 
//Main including header files
#include "DSP2833x_Device.h"		// Peripheral address definitions
#include "500KW_MAIN_HEADER.h"		// Main include file


//--- Global variables

/*==========================================
* NAME      :  g_StateCheck
* PURPOSE :  indication signal state that is checked
* RANGE    :  union Uint 16 bits
*
*
* CALLED BY: void StateSwitch(void),void EnergyCalcLowPrio(void)
*==========================================*/ 
union STATE_CHECK  g_StateCheck ={0} ,g_StateCheck_Initial = {0};   //IO检测标志位
union IO_STATE     g_IO_State = {0};    //返回IO口状态结构体

/*==========================================
* NAME      :  g_SysFaultMessage
* PURPOSE :  indication all fault  message   
* RANGE    :  union
*
*
* CALLED BY: void StateSwitch(void),void EnergyCalcLowPrio(void)
*==========================================*/ 
union SYS_FAULT_MESSAGE g_SysFaultMessage={0};
union SECOND_SYS_FAULT_MESSAGE s_SysFaultMessage = {0};

/*==========================================
* NAME      :  g_Sys_Current_State
* PURPOSE :  indication current state for pv inverter 
* RANGE    :  enum
*
*
* CALLED BY: void StateSwitch(void),void EnergyCalcLowPrio(void)
*==========================================*/ 
enum SYS_STATE	g_Sys_Current_State = WaitState;

// 从FPGA读取状态反馈量用

//--- Global functions
void PvPolarRevCheck(void);		//光伏输入端极性反接保护
void SetTimeofINV(void);		//设置逆变器时间
void ProcessWaiting(void);        // wait state program
void ProcessDisCharge(void);      // DisChargeState program
void ProcessCharge(void);         // ChargeState program
void ProcessChecking(void);       // check state program
void ProcessPreRunning(void);     // PreRunState program
void ProcessRunning(void);        // normal state program
void ProcessStop(void);           //Stopstate program
void ProcessPermanentfault(void); // Permanent-Fault state program    
void ProcessFault(void);          // Fault can be resolve and back to wait state program


void DigitalIOCheck(void);      // IO port fault-checking

void FAN_LOW_ON(void);          //fan1-3 LOW on controller
void FAN_LOW_OFF(void);         //fan1-3 LOW off ontroller
void FAN_MIDDLE_ON(void);       //fan1-3 M on controller
void FAN_MIDDLE_OFF(void);      //fan1-3 M off ontroller
void FAN_HIGH_ON(void);         //fan1-3 HIGH on controller
void FAN_HIGH_OFF(void);        //fan1-3 HIGH off ontroller
void DC_Charge_ON(void);
void DC_Charge_OFF(void);

Uint16 PermanentFaultCheck(void);    //Permanent-fault recheck, the detecting carry out in main_control loop and 2ms inter
Uint16 FaultCheck(void);             //fault state check  
Uint16 FaultBackCheck(void);         //fault-back check and wait time check 

Uint16 U16CntFirstStartDelay10s = 0;      // GFCI分步测试用
static Uint16 WaitStateStep = 1;

extern int16	Connect_Time;   //判断延时

extern void CheckSafety_PowerUp(void);
extern void ReadEnergyOutput_PowerUp(void);
/**********************************************************************
* FUNCION :  State Switch
* PURPOSE :  state changed for waiting,checking,running,stop,fault when condition is ready
* INPUT :
*        void
* RETURN :
*        void
* CALLS:
*        void
*
* CALLED BY: DSP/BIOS  kernel task  every 2ms 
* 
**********************************************************************/


void StateSwitch(void)
{
#ifdef DebugVersion
    //LOG_printf(&trace, " Entered StartSwitch Machine ");
#endif

	//EEprom operation when power up, by XXXXX, 2012-02-26
//	CheckSafety_PowerUp();
//	ReadEnergyOutput_PowerUp();

    while (1) // start of task main loop 
    {
      if (TRUE == SEM_pend(&TimeBase2msReady, SYS_FOREVER) )   
      { 
	   static Uint16 timeget_cnt = 0;
       int16  *StateBack1,*StateBack2,*StateBack3,*StateBack4;
       int16  *TimeBack1,*TimeBack2,*TimeBack3,*TimeBack4,*TimeBack5,/*,*TimeBack6*/*TimeBack7;
	  	//从FPGA读写过来
		GpioDataRegs.GPBSET.bit.GPIO39 = 1;
		DelayUs(1);
		GpioDataRegs.GPBCLEAR.bit.GPIO39 = 1;
		StateBack1 = (int16 *)0x00200000;
		StateBack2 = (int16 *)0x00200001;
		StateBack3 = (int16 *)0x00200002;
		StateBack4 = (int16 *)0x00200003;
		g_IO_State.Word.byte0 =  *StateBack1;
		g_IO_State.Word.byte1 =  *StateBack2;
		g_IO_State.Word.byte2 =  *StateBack3;
		g_IO_State.Word.byte3 =  (*StateBack4 & 0xf8) + (g_IO_State.Word.byte3 & 0x07);
		//直接反馈到DSP
		g_IO_State.bit.FPGA_Conf_Done = FPGA_Ready;  
		g_IO_State.bit.DC_OVP = Bus_OVP_LEVEL ^ 1;
		g_IO_State.bit.AC_OCP = Inv_OCP_LEVEL;
		g_IO_State.bit.FAN7 = FAN3_LOW_State;
		g_IO_State.bit.FAN8 = FAN3_MIDDLE_State;
		g_IO_State.bit.FAN9 = FAN3_HIGH_State;

		DC_DISCHARGE_ON;

		if(g_StateCheck.bit.TimeSet_flag == 1)
			SetTimeofINV();
		else if((g_StateCheck.bit.TimeSet_flag == 0) && (timeget_cnt >= 400))
		{
			timeget_cnt = 0;

			GpioDataRegs.GPBSET.bit.GPIO39 = 1;
			DelayUs(1);
			GpioDataRegs.GPBCLEAR.bit.GPIO39 = 1;
			
			TimeBack1 = (int16 *)0x00200010;
			TimeBack2 = (int16 *)0x00200011;
			TimeBack3 = (int16 *)0x00200012;
			TimeBack4 = (int16 *)0x00200013;
			TimeBack5 = (int16 *)0x00200014;
	//		TimeBack6 = (int16 *)0x00200015;
			TimeBack7 = (int16 *)0x00200016;

			g_Time.second =	*TimeBack1;
			g_Time.minute =	*TimeBack2;
			g_Time.hour =	*TimeBack3;
			g_Time.day =	*TimeBack4;
			g_Time.month =	*TimeBack5;
	//		g_Time.week =	*TimeBack6;
			g_Time.year =	*TimeBack7;
		}
		else
			timeget_cnt ++;


		DigitalIOCheck();
		 
        switch (g_Sys_Current_State)
        {
          case WaitState:
            {
              ProcessWaiting();           // waiting  state   
            }            
            break; 
          case DisChargeState:
            {
              ProcessDisCharge();         //  discharge  state
            }
            break; 
          case ChargeState:
            {
              ProcessCharge();            //  charge  state
            }
            break; 
          case CheckState:
            {
              ProcessChecking();          //  checking  state
            }
            break;  
          case PreRunState:
            {
              ProcessPreRunning();        //  PreRunstate
            }
            break;  
          case GridTiedState:   
            {
			   FAN_HIGH_ON();
              ProcessRunning();          // GridTiedState  state
            }        
            break; 
            
          case FaultState:    
            {
              ProcessFault();              // fault state 
            }      
            break;   
          case PermanentFaultState:        
            {
              ProcessPermanentfault();     // forever fault state
            }
            break; 
		  case StopState:        
            { 
              ProcessStop();     // forever fault state
            }
            break;

          default: break;	  
        }				     
      }   
    } // end of task main loop 

} // end of StateSwitch


/**********************************************************************
* FUNCION :  Waiting Process
* PURPOSE :  go into Permanent Fault state when the fault is not come back happen,
*            go into generic  Fault  state when the fault is come back happen, then close contactor,close pwm;
*            go into check state when Bus voltage is larger that Standbus voltage, 
* INPUT :
*        void
* RETURN :
*        void
* CALLS:
*        void
*
* CALLED BY:  void StateSwitch(void)             
* 
**********************************************************************/
void ProcessWaiting(void)
{ // start of ProcessWaiting
			
//	static Uint16 WaitStateStep = 1;
	static Uint16 DisChargeActionTime = 0;
	static Uint16 ChargeActionTime = 0;
	static Uint16 ChargeBackTestTime = 0;
	static Uint16 ChargeBackDelayTime = 0;
	static Uint16 ChargeFlag = 0;

  	if( PermanentFaultCheck() )          //Perment fault 
    {
		PWMOutputsDisable();
		AC_RELAY1_OFF;
		WaitStateStep = 0;
    	g_Sys_Current_State = PermanentFaultState; 
  	}
  	else     
  	{	
		if((g_IO_State.bit.DisCharge_COMMAND == 1) && (GetRealValue.f32VPV > 54) && (WaitStateStep == 1))
		{	//当出现永久故障,DSP 复位后进入wait状态，给直流母线放电
			DisChargeActionTime++;
			if(DisChargeActionTime >= 2)  //连续2次
			{
				g_Sys_Current_State = DisChargeState;
				DisChargeActionTime = 0;
				WaitStateStep = 0;
				g_IO_State.bit.DisCharge_COMMAND = 0;
			}
		}
		else
			DisChargeActionTime = 0;

		if((g_IO_State.bit.RUN_COMMAND == 1) && (ChargeFlag == 0) && (WaitStateStep == 1))
		{
			ChargeActionTime++;
			if(ChargeActionTime >= 2)  //连续2次检测到
			{
				DC_CHARGE1_ON;
				DC_CHARGE2_ON;
				ChargeActionTime = 0;
				ChargeFlag = 1;
				g_IO_State.bit.RUN_COMMAND = 0;
			}
		}
		else
			ChargeActionTime = 0;

		if(ChargeFlag == 1)
		{
			ChargeBackTestTime++;
			if(ChargeBackTestTime > 100)  //延时100ms
			{  
//g_IO_State.bit.DCP_Charge_Contactor = 1;
//g_IO_State.bit.DCN_Charge_Contactor = 1;
				if((g_IO_State.bit.DCP_Charge_Contactor == 1) && (g_IO_State.bit.DCN_Charge_Contactor  == 1))
				{
					g_Sys_Current_State = ChargeState;
					ChargeBackDelayTime = 0;
					ChargeBackTestTime = 0;
					ChargeFlag = 0;
					WaitStateStep = 0;
				}
				else 
				{
					ChargeBackDelayTime++;
					if(ChargeBackDelayTime >= 2) //连续两次
					{
						if(g_IO_State.bit.DCP_Charge_Contactor == 0)
							g_SysFaultMessage.bit.DCP_Charge_Contactor_Fault = 1;
						if(g_IO_State.bit.DCN_Charge_Contactor  == 0)
							g_SysFaultMessage.bit.DCN_Charge_Contactor_Fault = 1;
						DC_CHARGE1_OFF;  //关闭充电继电器
						DC_CHARGE2_OFF;
						ChargeBackTestTime = 0;
						ChargeBackDelayTime = 0;
						ChargeFlag = 0;
						WaitStateStep = 0;
					}
				}
			}
		}
	}

} // end of ProcessWATING


/**********************************************************************
* FUNCION :  DisCharge Process
* PURPOSE :  go into Permanent Fault state when the fault is not come back happen,
*            go into generic  Fault  state when the fault is come back happen, then close contactor,close pwm;
*            go into check state when Bus voltage is larger that Standbus voltage, 
* INPUT :
*        void
* RETURN :
*        void
* CALLS:
*        void
*
* CALLED BY:  void StateSwitch(void)             
* 
**********************************************************************/
void ProcessDisCharge(void)      // DisChargeState program
{ // start of DisCharge program
	static float32 DischargeDealyTime1 = 0;
	static Uint16  DischargeDealyTime2 = 0;
	static Uint16  DischargeDealyTime3 = 0;
	static Uint16  DischargeDealyTime4 = 0;
	static Uint16  DischargeOFFFlag = 0;

	if (PermanentFaultCheck())          //Perment fault 
    {
    	PWMOutputsDisable();
		AC_RELAY1_OFF;
		DC_DISCHARGE_OFF;		
    	g_Sys_Current_State = PermanentFaultState; 
  	}
	else
	{
	  DC_DISCHARGE_ON;
	  DischargeDealyTime1++;
	  if((DischargeDealyTime1 >= 50) && (DischargeDealyTime1 <= 500))   //延时100ms 到 20s 之间检测返回状态
	  {
	   	 if(g_IO_State.bit.DISCharge_Contactor == 0) //连续判断2次
		 {   
			 DischargeDealyTime4++;
			 if(DischargeDealyTime4 >= 2)
			 {
				 g_SysFaultMessage.bit.DISCharge_Contactor_Fault = 1;
			 	 DischargeDealyTime1 = 0;
			 }
		 }
		 else
			DischargeDealyTime4 = 0;
	  }
	  else if(( GetRealValue.f32VPV < 20) && (DischargeDealyTime1 > 30000))  //放电20s，电压小于30v
	  {
	  	 DC_DISCHARGE_OFF;
		 DischargeOFFFlag = 1;
	  }

	  if(DischargeOFFFlag == 1)
	  {
         DischargeDealyTime2++;
		 if(DischargeDealyTime2 >= 100)     //再延时200ms
		 {
		 	if(g_IO_State.bit.DISCharge_Contactor == 0)
			 {
				 DischargeDealyTime1 = 0;
				 DischargeDealyTime2 = 0;
				 DischargeDealyTime3 = 0;
				 DischargeOFFFlag = 0;
				 g_Sys_Current_State = WaitState;  //back to wait state
				 WaitStateStep = 1;
			 }
		 	else 
			 {   
				 DischargeDealyTime3++;
				 if(DischargeDealyTime3 >= 5)  //连续检测5次
				 {
			     	 DischargeDealyTime1 = 0;
					 DischargeDealyTime2 = 0;
					 DischargeDealyTime3 = 0;
					 DischargeOFFFlag = 0;
					 g_SysFaultMessage.bit.DISCharge_Contactor_Fault =	1;
				 }
			 }

		 }

	  }
	}
}
// end of DisCharge program


/**********************************************************************
* FUNCION :  Charge Process
* PURPOSE :  go into Permanent Fault state when the fault is not come back happen,
*            go into generic  Fault  state when the fault is come back happen, then close contactor,close pwm;
*            go into check state when Bus voltage is larger that Standbus voltage, 
* INPUT :
*        void
* RETURN :
*        void
* CALLS:
*        void
*
* CALLED BY:  void StateSwitch(void)             
* 
**********************************************************************/
void ProcessCharge(void)         // ChargeState program
{  // start of ChargeState program
	static float32 ChargeDealyTime1 = 0;
	static Uint16  ChargeDealyTime2 = 0;
	static Uint16  ChargeDealyTime3 = 0;
	static Uint16  ChargeDealyTime4 = 0;
	static Uint16  ChargeDealyTime5 = 0;
	static Uint16  ChargeOFFFlag = 0;

	if (PermanentFaultCheck())          //Perment fault 
    {
    	PWMOutputsDisable();
		AC_RELAY1_OFF;
		DC_CHARGE1_OFF;
		DC_CHARGE2_OFF;		
    	g_Sys_Current_State = PermanentFaultState; 
  	}
	else
	{
		if(ChargeOFFFlag == 0)
		{
			ChargeDealyTime1++;

			if((ChargeDealyTime1 > 4000) && (ChargeDealyTime1 < 10000))
//				PvPolarRevCheck();		//光伏输入端极性反接保护

			if(g_SysFaultMessage.bit.PvPolarRevFault == 1)
				ChargeDealyTime1 = 0;

			if(ChargeDealyTime1 > 10000)
//			if(ChargeDealyTime1 > 10000)
			//if(( GetRealValue.f32VPV >= 0) && (ChargeDealyTime1 > 10000))     //等待差点到510V，延时大于20s
        	{
				DC_CHARGE1_OFF;
				DC_CHARGE2_OFF;
				ChargeOFFFlag = 1;
				ChargeDealyTime1 = 0;
       		}
		}

		if(ChargeOFFFlag == 1)
		{
			ChargeDealyTime2++;
			if(ChargeDealyTime2 >= 50)     //延时100ms,等待继电器的机械动作
			{				
				if(g_IO_State.bit.DCP_Charge_Contactor == 1)  //DC+充电状态返回状态为开启，出错
				{   
					ChargeDealyTime3++;
					if(ChargeDealyTime3 >= 5)  //连续检测5次
					{
						g_SysFaultMessage.bit.DCP_Charge_Contactor_Fault= 1;
					    ChargeDealyTime2 = 0;
						ChargeDealyTime3 = 0;
						ChargeDealyTime4 = 0;
						ChargeDealyTime5 = 0;
						ChargeOFFFlag = 0;
					}
				}
				else
					ChargeDealyTime3 = 0;

				if(g_IO_State.bit.DCN_Charge_Contactor == 1)   //DC-充电状态返回状态为开启，出错
				{  
					ChargeDealyTime4++;
					if(ChargeDealyTime4 >= 5)
					{
						g_SysFaultMessage.bit.DCN_Charge_Contactor_Fault= 1;						
						ChargeDealyTime2 = 0;
						ChargeDealyTime3 = 0;
						ChargeDealyTime4 = 0;
						ChargeDealyTime5 = 0;
						ChargeOFFFlag = 0;
					}
				}
				else
					ChargeDealyTime4 = 0;

				if((g_IO_State.bit.DCP_Charge_Contactor == 0) && (g_IO_State.bit.DCN_Charge_Contactor == 0) && (g_IO_State.bit.AC_Breaker == 1))
				{
				    ChargeDealyTime5++;
					if(ChargeDealyTime5 >= 500)  // 1s
					{
						g_Sys_Current_State = CheckState; //end of charging, goto Checkstate
						ChargeDealyTime2 = 0;
						ChargeDealyTime3 = 0;
						ChargeDealyTime4 = 0;
						ChargeDealyTime5 = 0;
						ChargeOFFFlag = 0;
					}
				}
				else
                	ChargeDealyTime5 = 0;
			}
		}
	}
}
// end of ChargeState program

/**********************************************************************
* FUNCION :  Checking Process
* PURPOSE :  go into Permanent Fault state when the fault is not come back happen,
*            go into generic  Fault  state when the fault is come back happen, then close contactor,close pwm;
*            go into check state when Bus voltage is larger that Standbus voltage, 
* INPUT :
*        void
* RETURN :
*        void
* CALLS:
*        void
*
* CALLED BY:  void StateSwitch(void)             
* 
**********************************************************************/
void ProcessChecking(void)   	
{ // start of ProcessChecking   
    static Uint16  CheckDelayTime = 0;
	if(PermanentFaultCheck())          //Perment fault  
  	{
    	PWMOutputsDisable();
		AC_RELAY1_OFF;
    	g_Sys_Current_State = PermanentFaultState; 
  	}
  	else if(FaultCheck())                  //Fault state
    {
    	PWMOutputsDisable();
		AC_RELAY1_OFF;
    	g_Sys_Current_State = FaultState;
  	}
  	else  
  	{
    	if(0 == g_StateCheck.bit.IsoCheckOver)
    	{
     	 	//PvInsulationCheck();
			g_StateCheck.bit.IsoCheckOver = 1;//启用绝缘阻抗检测时注释掉本行 add 2013.6.8 
   		}
		else
		{
		    CheckDelayTime ++;
			if(CheckDelayTime > 8000) //延时10s，等待锁相
      	//	HWI_disable();
			{
      			InverterStage_Init();    //逆变器控制环路量的初始化                           
      			g_Sys_Current_State = PreRunState;
				CheckDelayTime = 0;
			}
      	//	HWI_enable();
    	}   
  	}
}


/**********************************************************************
* FUNCION :  PreRunning Process
* PURPOSE :  LC 滤波电容上电容电压建立 

**********************************************************************/

void ProcessPreRunning(void)     // PreRunState program
{
	if(PermanentFaultCheck())          //Perment fault    
 	{
		PWMOutputsDisable();
		AC_RELAY1_OFF;
    	g_Sys_Current_State = PermanentFaultState;  
  	}
 	else if(FaultCheck())                  //Fault state
  	{
		PWMOutputsDisable();
		AC_RELAY1_OFF;
    	g_Sys_Current_State = FaultState;
  	}
	else
	{
		if(STATUS_AC_RELAY1_ON == 2)
		{
			g_Sys_Current_State = GridTiedState;
			Connect_Time = 0;
		}
	}
}

/**********************************************************************
* FUNCION :  Running Process
* PURPOSE :  go into Permanent Fault state when the fault is not come back happen, go into generic  Fault  state when the fault is come back happen, then close contactor,close pwm;
*                   go into waiting  state when bus voltage is less that  standbus voltage 
             并网运行状态，可以在此状态下进行关机
**********************************************************************/
void ProcessRunning(void) 
{ // start of ProcessRUNNING
	static Uint16 ACRelayBackDelayTime = 0;
	static Uint16 StopDelayTime = 0;
	static Uint16 ACRelayBackTime = 0;
	static Uint16 ACRelayOFFflag = 0;

	if(PermanentFaultCheck())        //Perment fault  
   	{
		PWMOutputsDisable();
		AC_RELAY1_OFF;
    	g_Sys_Current_State = PermanentFaultState;  
   	}
  	else if(FaultCheck())             // fault state
   	{
		PWMOutputsDisable();
		AC_RELAY1_OFF;
    	g_Sys_Current_State = FaultState;
  	}
  	else   //上位机的控制命令清零 2013.01.29
  	{
//g_IO_State.bit.AC_Contactor = 1;
		if((g_IO_State.bit.AC_Contactor == 0) && (ACRelayOFFflag == 0))
		{
		    ACRelayBackDelayTime++;
		    if(ACRelayBackDelayTime >= 2)
		    {
		    	g_SysFaultMessage.bit.AC_Contactor_Fault = 1;
		    	ACRelayBackDelayTime = 0;
		    }
		 }
		else 
			ACRelayBackDelayTime = 0;			

		if((g_IO_State.bit.STOP_COMMAND == 1) && (ACRelayOFFflag == 0))
		{
			StopDelayTime++;
			if(StopDelayTime >= 2)
			{
				PWMOutputsDisable();
				AC_RELAY1_OFF;
				StopDelayTime = 0;
				ACRelayOFFflag = 1; 
			}
		}
		else
			StopDelayTime = 0;

		if(ACRelayOFFflag == 1)
		{
			ACRelayBackTime++;
			if(ACRelayBackTime > 50)  //延时100ms
			{
				ACRelayBackTime = 0;
				ACRelayOFFflag = 0;
				if(g_IO_State.bit.AC_Contactor == 1)
					g_SysFaultMessage.bit.AC_Contactor_Fault = 1;
				else
					g_Sys_Current_State = StopState;
			}
		}
  	}
} // end of ProcessRUNNING


/**********************************************************************
* FUNCION :  Stop Process
* PURPOSE :  go into Permanent Fault state when the fault is not come back happen, go into generic  Fault  state when the fault is come back happen, then close contactor,close pwm;
*            go into waiting  state when bus voltage is less that  standbus voltage 
             如果直流隔离隔离开关、交流断路器断开，则进入Wait状态
			 如果直流隔离隔离开关、交流断路器没有断开，则可以通过上位机命令进入Check状态
**********************************************************************/
void ProcessStop(void)           //Stopstate program
{
	static Uint16 AC_BreakerDelayTime = 0;
	static Uint16 CheckBackDelayTime = 0;

	g_StateCheck = g_StateCheck_Initial; //待修改
	g_StateCheck.bit.AD_initial = 0;	//待修改
	g_StateCheck.bit.IsoCheckOver = 1;

	if(g_IO_State.bit.AC_Breaker == 0)   //等待断开直流隔离开关，AC断路器开关
	{
		AC_BreakerDelayTime++;
		if(AC_BreakerDelayTime >= 5)
		{
			g_Sys_Current_State = WaitState;
			WaitStateStep = 1;
			AC_BreakerDelayTime = 0;
		}
	}
	else
		AC_BreakerDelayTime = 0;

	if((g_IO_State.bit.RUN_COMMAND == 1) && (g_IO_State.bit.AC_Breaker == 1))
	{
		CheckBackDelayTime++;
		if(CheckBackDelayTime >= 2) //延时80ms
		{
            g_Sys_Current_State = CheckState;
			CheckBackDelayTime = 0;
		}
	}

	if((g_IO_State.bit.DisCharge_COMMAND == 1) && (g_IO_State.bit.AC_Contactor == 0))
	{
		CheckBackDelayTime++;
		if(CheckBackDelayTime >= 2) //延时80ms
		{
            g_Sys_Current_State = DisChargeState;
			CheckBackDelayTime = 0;
		}
	}

}

/**********************************************************************
* FUNCION :  Fault Process
* PURPOSE :  go into Permanent Fault state when the fault is not come back happen,then close contactor,close pwm; go into  checking state if  the fault is  come back 
*            不断检测错误故障位是否全部清零，延时一段时间进入Check状态       
* CALLED BY:  void StateSwitch(void)             
* 
**********************************************************************/
void ProcessFault(void) 
{ // start of ProcessFAULT
 	static int16 FaultBackChangeTemp = 0;	

    if(PermanentFaultCheck())         //Perment fault       
    {
        PWMOutputsDisable();
		AC_RELAY1_OFF;
        g_Sys_Current_State = PermanentFaultState;     
    }
    else 
    {
        if(FaultBackCheck())    //故障态错误返回需要等待一段时间
        {
			FaultBackChangeTemp++;
			if(FaultBackChangeTemp >= 2000)  //4s
			{
				g_StateCheck = g_StateCheck_Initial; //待修改
				g_StateCheck.bit.AD_initial = 1;
				g_StateCheck.bit.IsoCheckOver = 1;
				ADChannelOffset = ADOffset;
            	g_Sys_Current_State = CheckState;
				FaultBackChangeTemp = 0;
			}
        }
		else
			FaultBackChangeTemp = 0;
    }		 
} // end of ProcessFAULT

/**********************************************************************
* FUNCION :  Permanent Process
* PURPOSE :  close contactor,close pwm,Permanent Process is not exit,
*           
* CALLED BY:  void StateSwitch(void)             
* 
**********************************************************************/
void ProcessPermanentfault(void)
{ // start of Process permannetfaultstate
  // 出现永久故障得重新复位程序，并检测机器
	g_Sys_Current_State = PermanentFaultState;
    PWMOutputsDisable();
	AC_RELAY1_OFF; 

} //end of permannetfaultstate


/**********************************************************************
* FUNCION :  Fault Check
* PURPOSE :  check if  generic Fault is happend
* CALLED BY:  void ProcessFault(void) ,void ProcessRunning(void) ,void ProcessChecking(void), void ProcessWaiting(void)                  
* 
**********************************************************************/
Uint16 FaultCheck(void)      
{
	if(MpptLoop == WorkMode) //为什么分两种模式
	{
    	if((0 == g_SysFaultMessage.Word.byte0 ) && ((g_SysFaultMessage.Word.byte1 | 0x01)==0x01) 
		&& (0 == g_SysFaultMessage.Word.byte2) && (g_SysFaultMessage.Word.byte3 ==0)
		&& (s_SysFaultMessage.Word.byte0 == 0) && (s_SysFaultMessage.Word.byte1 == 0)
		&& (s_SysFaultMessage.Word.byte2 == 0))
			return(0);
		else
       	 	return(1);
	}
	else
	{
		if((0 == g_SysFaultMessage.Word.byte0 ) && (0 == g_SysFaultMessage.Word.byte1) 
		&& (0 == g_SysFaultMessage.Word.byte2) && (0 == g_SysFaultMessage.Word.byte3 )
		&& (s_SysFaultMessage.Word.byte0 == 0) && (s_SysFaultMessage.Word.byte1 == 0)
		&& (s_SysFaultMessage.Word.byte2 == 0))
        	return(0);
		else
       	 	return(1);
    } 
}

/**********************************************************************
* FUNCION :  Permanent Fault Check
* PURPOSE :  check if Permanent Fault is happend
* CALLED BY:  void ProcessFault(void) ,void ProcessRunning(void) ,void ProcessChecking(void), void ProcessWaiting(void)        
* 
**********************************************************************/
Uint16 PermanentFaultCheck(void)
{
	if((g_SysFaultMessage.Word.byte4 == 0) && (g_SysFaultMessage.Word.byte5 == 0) && (g_SysFaultMessage.Word.byte6 == 0) && (g_SysFaultMessage.Word.byte7 == 0)		// modified by XXXXX 2011-7-5
	&& (g_SysFaultMessage.bit.PvIsoFault == 0))
		return(0);
	else
		return(1);
}

/**********************************************************************
* FUNCION :  Fault Back Check
* PURPOSE :  check if  generic Fault is happend
* CALLED BY:  void ProcessFault(void) ,void ProcessRunning(void) ,void ProcessChecking(void), void ProcessWaiting(void)                  
* 
**********************************************************************/
Uint16 FaultBackCheck(void)
{
    if(MpptLoop == WorkMode)
	{
    	if ((0 == g_SysFaultMessage.Word.byte0 ) && ((g_SysFaultMessage.Word.byte1 | 0x01) == 0x01) 
    	&&(0 == g_SysFaultMessage.Word.byte2) && (g_SysFaultMessage.Word.byte3 == 0)
		&& (s_SysFaultMessage.Word.byte0 == 0) && (s_SysFaultMessage.Word.byte1 == 0)
	 	&& (s_SysFaultMessage.Word.byte2 == 0))
        	return(1);
    	else
    	    return(0);
    }
    else
    {	
		if ((0 == g_SysFaultMessage.Word.byte0 ) && (0 == g_SysFaultMessage.Word.byte1)
		 &&(0 == g_SysFaultMessage.Word.byte2) && (0 == g_SysFaultMessage.Word.byte3)
		 && (s_SysFaultMessage.Word.byte0 == 0) && (s_SysFaultMessage.Word.byte1 == 0)
	     && (s_SysFaultMessage.Word.byte2 == 0))
        	return(1);
    	else
    	    return(0);
    } 
}

void SetTimeofINV(void)
{
	int16 *timeset;
	static int16 time_temp = 0;
//	static int16 timeset_cnt;
	Uint16 timesettemp;

	if(g_StateCheck.bit.TimeSet_flag == 1)
	{
		GpioDataRegs.GPBSET.bit.GPIO39 = 1;
		DelayUs(1);
		GpioDataRegs.GPBCLEAR.bit.GPIO39 = 1;
		
		switch(time_temp)
		{
		case 0:
			timesettemp = (Uint16)(g_Time.second / 10) * 16 + ((Uint16)g_Time.second % 10);
			timeset = (int16 *)0x00200010;
			*timeset = timesettemp;
			time_temp ++;
		break;

		case 1:
			timesettemp = (Uint16)(g_Time.minute / 10) * 16 + ((Uint16)g_Time.minute % 10);
			timeset = (int16 *)0x00200011;
			*timeset = timesettemp;
			time_temp ++;
		break;

		case 2:
			timesettemp = (Uint16)(g_Time.hour / 10) * 16 + ((Uint16)g_Time.hour % 10);
			timeset = (int16 *)0x00200012;
			*timeset = timesettemp;
			time_temp ++;
		break;

		case 3:
			timesettemp = (Uint16)(g_Time.day / 10) * 16 + ((Uint16)g_Time.day % 10);
			timeset = (int16 *)0x00200013;
			*timeset = timesettemp;
			time_temp ++;
		break;

		case 4:
			timesettemp = (Uint16)(g_Time.month / 10) * 16 + ((Uint16)g_Time.month % 10);
			timeset = (int16 *)0x00200014;
			*timeset = timesettemp;
			time_temp ++;
		break;

		case 5:
			timesettemp = (Uint16)(g_Time.year / 10) * 16 + ((Uint16)g_Time.year % 10);
			timeset = (int16 *)0x00200016;
			*timeset = timesettemp;
			g_StateCheck.bit.TimeSet_flag = 0;
			time_temp = 0;
		break;
		default:
			g_StateCheck.bit.TimeSet_flag = 0;
			time_temp = 0;
		break;
		}
//		timeset_cnt = 0;
	}
//	else
//		timeset_cnt ++;

}

//光伏输入端极性反接保护
void PvPolarRevCheck(void)
{
	static Uint16 s_u16Cnt_PvPolarRev_Fault = 0;
	if(GetRealValue.f32VPV <= 20.0f) 
	{
		s_u16Cnt_PvPolarRev_Fault++;
		if(s_u16Cnt_PvPolarRev_Fault > 2)
		{ 
			s_u16Cnt_PvPolarRev_Fault = 0;
			g_SysFaultMessage.bit.PvPolarRevFault = 1;
		}  
	}
	else
	{
		s_u16Cnt_PvPolarRev_Fault = 0;
		g_SysFaultMessage.bit.PvPolarRevFault = 0;
	}
}


void FAN_LOW_ON(void)
{ 
	FAN1_LOW_Enable;
	FAN2_LOW_Enable;
	FAN3_LOW_Enable;
}

void FAN_LOW_OFF(void)
{ 
	FAN1_LOW_Disable;
	FAN2_LOW_Disable;
	FAN3_LOW_Disable;
}

void FAN_MIDDLE_ON(void)
{ 
	FAN1_MIDDLE_Enable;
	FAN2_MIDDLE_Enable;
	FAN3_MIDDLE_Enable;
}

void FAN_MIDDLE_OFF(void)
{ 
	FAN1_MIDDLE_Disable;
	FAN2_MIDDLE_Disable;
	FAN3_MIDDLE_Disable;
}

void FAN_HIGH_ON(void)
{ 
	FAN1_HIGH_Enable;
	FAN2_HIGH_Enable;
	FAN3_HIGH_Enable;
}

void FAN_HIGH_OFF(void)
{ 
	FAN1_HIGH_Disable;
	FAN2_HIGH_Disable;
	FAN3_HIGH_Disable;
}

void DigitalIOCheck()
{
	HwInvOCPCheck();
	HwBusOVPCheck();
//	DCFuseCheck();
//	ACFuseCheck();
//	FANCheck();
//	EmergencyButtonCheck();
//	L_tempCheck();
//	FPGA_Check();
}

//--- end of file -----------------------------------------------------







