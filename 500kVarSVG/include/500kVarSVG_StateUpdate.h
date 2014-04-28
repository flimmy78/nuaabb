/***********************************************************************
 *    Copyright(c) 2010-2011,    

 *    FILENAME  : 17KW_StateUpdate.h
 *    PURPOSE  :  define constant, struct declaration
 *                        
 *    HISTORY  :
 *    DATE:2012.12.20            VERSION:V1.0        AUTHOR:WangWanBao            NOTE
 *     
 *
 *		
 * 	Software Development Evironment: 
 *	-- CCS 3.3.81.6
 * 	-- BIOS 5.33.06
 *	-- Code Generation Tool v5.2.4 

 ************************************************************************/

#ifndef STATE_UPDATE  //MAINSTATUSMACHINE_H
#define STATE_UPDATE  //MAINSTATUSMACHINE_H

//#define SW_Version1 2012
//#define SW_Version2 0101
//#define SW_Version3 V1.2.1
#define CurLoop 1       //
#define MpptLoop 2      
#define WorkMode CurLoop 
//#define WorkMode   MpptLoop

//safety define
/*
#define  CHINA_SUN  1
#define  VDE0126    2
#define  RD1663     3   //Spanish
#define  DK5940     4
#define FunctionSafety CHINA_SUN
*/
//================ Global variables =====================================

//System Status definition for StateMachine
enum   SYS_STATE                             
{
 	WaitState,ChargeState,DisChargeState, CheckState,PreRunState,GridTiedState,FaultState,PermanentFaultState,StopState
} ; 

extern enum	SYS_STATE	g_Sys_Current_State;

union  IO_STATE   //反馈量的IO定义，4个字节
{
	struct
    {
        Uint16  byte0:8;
        Uint16  byte1:8;
        Uint16  byte2:8;
        Uint16  byte3:8;
	}Word;
 
	
	struct
   	{
		//byte0
		Uint16  DCP_Fuse:1;     //0 正母线熔断丝返回信号
   		Uint16  DCN_Fuse:1;     //1 负母线熔断丝返回信号
   		Uint16  ACR_Fuse:1;     //2 输出A相熔断丝返回信号
   		Uint16  ACS_Fuse:1;     //3 输出B相熔断丝返回信号
        Uint16  ACT_Fuse:1;     //4 输出C相熔断丝返回信号
		Uint16  AC_Arrester:1;  //5 AC 防雷器返回信号
		Uint16  DC_Arrester:1;  //6 DC防雷器返回信号
		Uint16  Rsvd1:1;        //7 保留
		
		//byte1
        Uint16  FAN1:1;    		//0 风机1，高档返回状态
        Uint16  FAN2:1;    		//1 风机1，中档返回状态
        Uint16  FAN3:1;    		//2 风机1，低挡返回状态
		Uint16  FAN4:1;    		//3 风机2，高档返回状态
        Uint16  FAN5:1;    		//4 风机2，中档返回状态
        Uint16  FAN6:1;    		//5 风机2，抵挡返回状态
	    Uint16  Emergency_Button:1;  //6 应急控制开关返回状态
        Uint16  Inductor_Temp:1;     //7 电感温度状态返回

		//byte2
        Uint16  DCP_Charge_Contactor:1;   //0 正母线预充电接触器返回
        Uint16  DCN_Charge_Contactor:1;   //1 负母线预充电接触器返回
        Uint16  DISCharge_Contactor:1;    //2 直流母线泄放接触器返回状态
        Uint16  AC_Breaker:1;             //3 AC断路器状态反馈
        Uint16  AC_Contactor:1;           //4 AC接触器反馈状态
        Uint16  DC_OVP:1;                 //5 硬件输出过压保护
        Uint16  AC_OCP:1;                 //6 硬件输出过流保护
		Uint16  Rsvd2:1;                  //7 保留

		//byte3 （0-2虚拟的IO口状态，上位机命令） 
		Uint16  RUN_COMMAND:1;    		//0 运行命令，把上位发过来命令看作是虚拟IO
        Uint16  STOP_COMMAND:1;   		//1 停止命令，把上位机发过来明林看作是虚拟IO
        Uint16  DisCharge_COMMAND:1;    //2 泄放直流侧电容电压命令，
        Uint16  FPGA_Conf_Done:1;       //3 FPGA配置结束状态
		Uint16  FAN7:1;    				//4 风机3，高档返回状态  
        Uint16  FAN8:1;    				//5 风机3，中档返回状态  
        Uint16  FAN9:1;    				//6 风机3，低档返回状态  
        Uint16  Rsvd3:1;                //7 保留

     } bit;   
};
extern union IO_STATE   g_IO_State;



//程序运行中用到的一些标志位，和保护位
union  STATE_CHECK 
{
    struct
    {
    	Uint16 byte0:8;
    	Uint16 byte1:8;
    	Uint16 byte2:8;
    	Uint16 byte3:8;
    	Uint16 byte4:8;
    	Uint16 byte5:8;
    	Uint16 byte6:8;
    	Uint16 byte7:8;   	
    } Word;
    	   	
    struct
    {
		//byte 0
        Uint16  MpptRestart:1;    		 //0  没有用到
        Uint16  AD_initial:1;   		 //1
        Uint16  EEROM_Data_Load_OVER:1;  //2 EEROM 数据载入内存结束
        Uint16  Zero_Crossing_Flag:1;    //3 电网过零检测标志        
        Uint16  PLL_LOCKED:1;            //4 锁相完成标志         
        Uint16  IsoCheckOver:1;          //5 绝缘检测完成标志              
        Uint16  Phase_Sequence_Detect_Over:1;  //相序检测结束，没有用到
        Uint16  Rsvd1:1;                        //保留位
        
        //byet 1       
        //电网频率检测
        Uint16  Freq_Range1:1;           //0 频率检测在范围1区间  区间范围 参考金太阳标准
        Uint16  Freq_Range2:1;           //1 频率检测在范围2区间 
        Uint16  Freq_Range3:1;           //2 频率检测在范围3区间
        Uint16  Freq_Range4:1;           //3 频率检测在范围4区间
        Uint16  Freq_Range5:1;           //4 频率检测在范围5区间        
        //R相电压检测
        Uint16  VgridR_RMS_Range1:1;     //5 R相电压有效值范围1   范围参考金太阳标准
        Uint16  VgridR_RMS_Range2:1;     //6 R相电压有效值范围2   范围参考金太阳标准
        Uint16  VgridR_RMS_Range3:1;     //7 R相电压有效值范围3   范围参考金太阳标准
        
        //byte2
        Uint16  VgridR_RMS_Range4:1;     //0 R相电压有效值范围1   范围参考金太阳标准
        Uint16  VgridR_RMS_Range5:1;     //1 R相电压有效值范围2   范围参考金太阳标准          
        //S相电压检测
        Uint16  VgridS_RMS_Range1:1;     //2 S相电压有效值范围1   范围参考金太阳标准
        Uint16  VgridS_RMS_Range2:1;     //3 S相电压有效值范围2   范围参考金太阳标准
        Uint16  VgridS_RMS_Range3:1;     //4 S相电压有效值范围3   范围参考金太阳标准                
        Uint16  VgridS_RMS_Range4:1;     //5 S相电压有效值范围1   范围参考金太阳标准
        Uint16  VgridS_RMS_Range5:1;     //6 S相电压有效值范围2   范围参考金太阳标准        
        //T相电压范围检测
        Uint16  VgridT_RMS_Range1:1;     //7 S相电压有效值范围1   范围参考金太阳标准
        
        //byte 3
        Uint16  VgridT_RMS_Range2:1;     //0 S相电压有效值范围2   范围参考金太阳标准
        Uint16  VgridT_RMS_Range3:1;     //1 S相电压有效值范围3   范围参考金太阳标准
        Uint16  VgridT_RMS_Range4:1;     //2 S相电压有效值范围1   范围参考金太阳标准
        Uint16  VgridT_RMS_Range5:1;     //3 S相电压有效值范围2   范围参考金太阳标准
        //R相电流有效值范围检测
        Uint16  IgridR_RMS_Range1:1;     //4 R相并网电流1范围标志  可按0~110%功率范围进行
        Uint16  IgridR_RMS_Range2:1;     //5 R相并网电流2范围标志 可按110～125%功率运行2s设计
        Uint16  IgridR_RMS_Range3:1;     //6 R相并网电流2范围标志 可按>125%功率立即关断        
        //S相电流有效值范围检测
        Uint16  IgridS_RMS_Range1:1;     //7 R相并网电流1范围标志  可按0~110%功率范围进行
        
        //byte 4
        Uint16  IgridS_RMS_Range2:1;     //0 R相并网电流2范围标志 可按110～125%功率2s设计                
        Uint16  IgridS_RMS_Range3:1;     //1 R相并网电流2范围标志 可按>125%功率立即关断        
        //T相电流有效值范围检测
        Uint16  IgridT_RMS_Range1:1;     //2 R相并网电流1范围标志  可按0~110%功率范围进行
        Uint16  IgridT_RMS_Range2:1;     //3 R相并网电流2范围标志 可按110～125%功率运行2s设计              
        Uint16  IgridT_RMS_Range3:1;     //4 R相并网电流2范围标志 可按>125%功率立即关断                    
        Uint16  GFCI5ACheck_OVER:1;      //5 漏电流检测结束状态  
        Uint16  AC_OCP: 1;               //6 软件过流保护位      
        Uint16  Bus_OVP: 1;              //7 软件过流保护位
        
        //byte 5
        Uint16  RW_EEROM_Enable :1;       //0 允许EEROM读写操作
        Uint16  STATE_CHANGING_STATE :1;  //1 状态转换标志
        Uint16  STATE_CHANGING_OVER  :1;  //2 状态转换结束,避免状态转换过程中可能返回信号不稳定        
        //以下是控制板自检标志              
        Uint16  SD_Test_OVER:1;           //3 SD卡测试结束                
        Uint16  CAN_TEST_OVER:1;          //4 CAN通信测试结束            
        Uint16  EEROM_TEST_OVER:1;        //5 EEROM Read Write Test 
        Uint16  SCI_TEST_OVER:1;          //6 SCI 通信测试结束
        Uint16  FPGA_TEST_OVER:1;         //7 FPGA测试结束
               
       //byte  6
        Uint16   OutputWattCtrl_flag: 1;  	//输出有功功率控制标志          
        Uint16   OutputVarCtrl_flag: 1;  	//输出无功功功率控制标志          
        Uint16   OutputPF_Ctrl_flag: 1;    	//输出功率因数控制标志          
        Uint16   TimeSet_flag: 1;           //设置逆变器时间          
        Uint16   TimeRead_flag: 1;          //读取逆变器时间标志          
        Uint16   Rsvd2: 3;              	//3位保留位          

       //byte  7
        Uint16   Rsvd3: 8;              	//8位保留位          
       
    }bit;
}; 
extern union STATE_CHECK  g_StateCheck, g_StateCheck_Initial;



//错位故障位定义，分为永久故障和可返回故障
union  SYS_FAULT_MESSAGE   
{
    struct
    {
        Uint16  byte0:8;
        Uint16  byte1:8;
        Uint16  byte2:8;
        Uint16  byte3:8;
        Uint16  byte4:8;	
        Uint16  byte5:8;
        Uint16  byte6:8; 
        Uint16  byte7:8;                	
    }Word;  

    struct
    {
        //可恢复故障byte0   
        //byte0
        Uint16  VGridROverRating:1;     //B0   R相过压
        Uint16  VGridRUnderRating:1;    //B1   R相欠压
        Uint16  VGridSOverRating:1;     //B2   S相过压         
        Uint16  VGridSUnderRating:1;    //B3   S相欠压
        Uint16  VGridTOverRating:1;     //B4   T相过压
        Uint16  VGridTUnderRating:1;    //B5   T相欠压
        Uint16  FreGridOverRating:1;    //B6   电网频率太高
        Uint16  FreGridUnderRating:1;   //B7   电网频率太低

        //byte1
        Uint16  PvVoltLowFault:1;       //B0   PV直流母线欠压
        Uint16  PvVoltOvrFault:1;       //B1   PV直流母线过压
        Uint16  SCICommLoseFault:1;     //B2   SCI通信故障
        Uint16  IGBT1TempFault:1;       //B3   IGBT1过温
        Uint16  IGBT2TempFault:1;       //B4   IGBT2过温
        Uint16  IGBT3TempFault:1;       //B5   IGBT3过温
        Uint16  TempAmbient:1;          //B6   环境温度
        Uint16  OverLoad:1;             //B7   过载故障added by Ken
        
        //byte 2
        Uint16  DCIGridROverLimit:1;    //B0   R相直流偏置故障        
        Uint16  DCIGridSOverLimit:1;    //B1   S相直流偏置故障
        Uint16  DCIGridTOverLimit:1;    //B2   T相直流偏置故障
        Uint16  FreGridROverRating:1;   //B3   R相频率超越上限
        Uint16  FreGridRUnderRating:1;  //B4   R相频率低于下限
        Uint16  FreGridSOverRating:1;   //B5   S相频率超越上限
        Uint16  FreGridSUnderRating:1;  //B6   S相频率低于下限
        Uint16  FreGridTOverRating:1;   //B7   T相频率超越上限
        
        //byte 3
        Uint16  FreGridTUnderRating:1;  //B0   T相频率低于下限      
        Uint16  HW_BusOVP_Fault:1;      //B1   硬件直流母线过压故障
        Uint16  HW_OCP_AC_Fault:1;      //B2   硬件交流电流故障
        Uint16  OCP_AC_RMS_Fault:1;     //B3   软件检测输出过流故障
        Uint16  Fault_VGrid_ave:1;      //B4   电网电压平均值故障
        Uint16  Fault_Vinv_ave:1;       //B5   逆变电压平均值故障
        Uint16  PLLFault:1;				//B6   锁相故障 
        Uint16  PvPolarRevFault:1;		//B7   直流输入极性反接故障            
        
        //byte 4 一般永久故障 
        Uint16  DCP_Fuse_Fault:1;     	//B0 PV母线正熔断丝故障                  
   		Uint16  DCN_Fuse_Fault:1;     	//B1 PV母线负熔断丝故障
   		Uint16  ACR_Fuse_Fault:1;    	 //B2 R相熔断丝故障
   		Uint16  ACS_Fuse_Fault:1;    	 //B3 S相熔断丝故障
        Uint16  ACT_Fuse_Fault:1;    	 //B4 T相熔断丝故障               
        Uint16  DCP_Charge_Contactor_Fault:1;   //B5 预充正母线电接触器故障
        Uint16  DCN_Charge_Contactor_Fault:1;   //B6 预充电负母线接触器故障        
        Uint16  DISCharge_Contactor_Fault:1;    //B7 直流母线放电接触器故障
        
        //byte 5 永久故障
        Uint16  Emergency_Fault:1;    	//B0 应急故障         
        Uint16  AC_Breaker_Fault:1;   	//B1 AC断路器故障
        Uint16  AC_Contactor_Fault:1; 	//B2 AC接触器故障                        
        Uint16  CANCommLoseFault:1;   	//B3 CAN通信故障              
        Uint16  InductorTempFault:1;  	//B4 电感温度过温         
        Uint16  FAN1_Fault:1;         	//B5 风扇1故障
        Uint16  FAN2_Fault:1;         	//B6 风扇2故障
        Uint16  FAN3_Fault:1;         	//B7 风扇2故障              
        
        //byte 6  自检故障 也是永久故障
        Uint16  SCI_TEST_Fault:1;		//B0 SCI测试故障                       
        Uint16  CAN_TEST_Fault:1;		//B1 CAN测试故障
        Uint16  FPGA_TEST_Fault:1;		//B2 FPGA测试故障
        Uint16  SD_TEST_Fault:1;		//B3 SD测试故障
        Uint16  FPGA_Conf_Done_Fault:1;	//B4 FPGA配置故障
        Uint16  EEpromRdWrFault:1;		//B5 EEROM读写错误     
        Uint16  PvIsoFault:1;			//B6 绝缘检测故障
        Uint16  PvReversCurrFault:1; 	//B7 反向放电故障
        
        //byte 7  漏电流检测故障            
        Uint16  GFCIFault:1;			//B0  漏电流检测故障
        Uint16  GFCIJump1Fault:1;		//B1  漏电流检测跳变1故障                 
        Uint16  GFCIJump2Fault:1;		//B2  漏电流检测跳变2故障
        Uint16  GFCIJump3Fault:1;		//B3  漏电流检测跳变3故障
        Uint16  GFCIOver10AFault:1;		//B4  漏电流超过10A故障
		Uint16  HWADFault_GFCI:1;		//B5  GFCI硬件故障
		Uint16  SlaveDSPFault:1;		//B6  副DSP故障
        Uint16  MasterDSPFault:1;		//B7  主DSP故障
      
      }bit;
};

extern union SYS_FAULT_MESSAGE g_SysFaultMessage;



union  SECOND_SYS_FAULT_MESSAGE   
{
       struct
    {
        Uint16  byte0:8;
        Uint16  byte1:8;
        Uint16  byte2:8;
        Uint16  byte3:8;               	
    }Word;  

    struct
    { 
        //byte0
        Uint16  VGridROverRating:1;     //B0   R相过压
        Uint16  VGridRUnderRating:1;    //B1   R相欠压
        Uint16  VGridSOverRating:1;     //B2   S相过压         
        Uint16  VGridSUnderRating:1;    //B3   S相欠压
        Uint16  VGridTOverRating:1;     //B4   T相过压
        Uint16  VGridTUnderRating:1;    //B5   T相欠压
        Uint16  SlaveDSPFault:1;    //B6   副DSP自检错误
        Uint16  Rsvd1:1;   //B7   电网频率太低

		//byte 1
        Uint16  DCIGridROverLimit:1;    //B0   R相直流偏置故障        
        Uint16  DCIGridSOverLimit:1;    //B1   S相直流偏置故障
        Uint16  DCIGridTOverLimit:1;    //B2   T相直流偏置故障
        Uint16  OCP_AC_RMS_Fault:1;   //B3   输出过流保护
        Uint16  HW_OCP_AC_Fault:1;  //B4     硬件输出过流保护
        Uint16  Fault_VGrid_ave:1;   //B5    电网电压平均值保护
        Uint16  FreGridUnderRating:1;  //B6   
        Uint16  Rsvd2:1;   //B7   T相频率超越上限

		//byte 2
        Uint16  FreGridROverRating:1;    //B0   R相频率超越上限       
        Uint16  FreGridRUnderRating:1;   //B1   R相频率低于下限
        Uint16  FreGridSOverRating:1;    //B2   S相频率超越上限
        Uint16  FreGridSUnderRating:1;   //B3   S相频率低于下限
        Uint16  FreGridTOverRating:1;    //B4   T相频率超越上限
        Uint16  FreGridTUnderRating:1;   //B5   T相频率低于上限
        Uint16  Rsvd3:1;  //B6   S相频率低于下限
        Uint16  Rsvd4:1;   //B7   T相频率超越上限

		//byte 3
        Uint16  Rsvd5:1;    //B0   保留位      
        Uint16  Rsvd6:1;    //B1   保留位 
        Uint16  Rsvd7:1;    //B2   保留位 
        Uint16  Rsvd8:1;    //B3   保留位 
        Uint16  Rsvd9:1;    //B4   留位 
        Uint16  Rsvd10:1;    //B5  保留位 
        Uint16  Rsvd11:1;    //B6  保留位 
        Uint16  Rsvd12:1;    //B7  保留位 
      }bit;
};
extern union SECOND_SYS_FAULT_MESSAGE s_SysFaultMessage;



//===================== Global functions==================================
//extern void RelaysOFF(void);
extern void FAN_ON(void);
extern void FAN_OFF(void);
extern void FAN_MIDDLE_ON(void);
extern void FAN_MIDDLE_OFF(void);
extern void FAN_LOW_ON(void);
extern void FAN_LOW_OFF(void);
extern void FAN_HIGH_ON(void);
extern void FAN_HIGH_OFF(void);
//extern void	DCFuseCheck();
//extern void	ACFuseCheck();
//extern void   FANCheck();
//extern void	EmergencyButtonCheck();
//extern void	L_tempCheck();
//extern void	FPGA_Check();
extern void HwInvOCPCheck(void);
extern void HwBusOVPCheck(void);
//main statemachine
//extern void SysParamDefault(); 
//extern void ProcessWaiting(void);
//extern void ProcessChecking(void);
//extern void ProcessRunning(void);
//extern void ProcessPermanent(void);        
//extern void ProcessFaultfault(void);           

extern Uint16 U16CntFirstStartDelay10s;        

#endif

//--- end of file -----------------------------------------------------


