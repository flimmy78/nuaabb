/*=============================================================================*
 *         
 *                          ALL RIGHTS RESERVED
 *
 *  FILENAME : 17KW_ProtectionLogic.h 

 *  PURPOSE  : header files 17KW_ProtectionLogic.c 
 *			       define constant, struct declaration, extern varibles 
 *
 *  HISTORY  :
 *
 *
 ******************************************************************************/

#ifndef PROTECTION_LOGIC_H
#define PROTECTION_LOGIC_H

#define Production_250KW

//======================= Global constants============================  

//500KW-��̨250KW
#define Min_OutputPower		150000     //��С�������
#ifdef	Production_250KW
#define Rated_OutputPower		550000 //����������
#define Rated_InputPower		275000 //������빦�ʣ���̨��
#define	Rated_OutputCurrentRms	1100   //����������������Чֵ
#define	Rated_OutputCurrentPeak		    Rated_OutputCurrentRms * 1.414f  //����������������ֵ
#define	Half_Rated_OutputCurrentPeak	Rated_OutputCurrentPeak * 0.5f   //
#define OverRated_OutputCurrent   (Rated_OutputCurrentRms + 20)          //����������������Чֵ����ֵ
#define Rated_InputCurrent	550         //����������ֵ
#define OverRated_InputCurrent	  (Rated_InputCurrent + 30)  //���������󱣻�ֵ
#endif

//-------------------------------------------------------------------------------
//  CHINA_SUN/VDE0126 ��ͬ��֤��׼
//******************************************************************************
#define CHINA_SUN      //�й���̫����֤��׼
//China_Sun=====================================================================
#ifdef CHINA_SUN
  #define VGridHi2Limit	226.0f		    //������ѹΪ167V������ֱ�Ӳ���220V
  #define VGridHi1Limit 184.2f			//264.1 * 0.761f	
  #define VGridHiLimit	240	* 0.761f	 		//264.0 * 0.761f	
  #define VGridHiLimitBack 235.2 * 0.761f	    //235.2 * 0.761f	
  #define VGridLow1Limit	142.3f	    //187 * 0.761f
  #define VGridLow2Limit	83.7f	    //187.1 * 0.761f
  #define VGridLowLimitBack    192 * 0.761f	    //192 * 0.761f	
  #define VGridProtectionTime	5               //����ʱ�� 5 ����������
/*
  #define VGridHi2Limit	240.2f * 0.761f		    //������ѹΪ167V������ֱ�Ӳ���220V
  #define VGridHi1Limit 240.1f * 0.761f			//264.1 * 0.761f	
  #define VGridHiLimit	240	* 0.761f	 		//264.0 * 0.761f	
  #define VGridHiLimitBack 235.2 * 0.761f	    //235.2 * 0.761f	
  #define VGridLow1Limit	187 * 0.761f	    //187 * 0.761f
  #define VGridLow2Limit	187.1 * 0.761f	    //187.1 * 0.761f
  #define VGridLowLimitBack    192 * 0.761f	    //192 * 0.761f	
  #define VGridProtectionTime	5               //����ʱ�� 5 ����������
*/
  #define FreGridHiLimit	50.2	//Ƶ�ʱ���������ֵ1
  #define FreGridHi2Limit	50.5    //Ƶ�ʱ���������ֵ2,��С�ڱ�׼ֵ50.5����߱�����Ӧ�ٶ�
  #define FreGridLow2Limit 	48.0    //Ƶ�ʱ���������ֵ1,�Դ��ڱ�׼ֵ48.0����߱�����Ӧ�ٶ�
  #define FreGridLowLimit 	49.5    //Ƶ�ʱ���������ֵ2
  #define FreGridProtectionTime	6000   //Ƶ�ʱ���ʱ�� 6000 ���������� 2min

  #define SafetyReConnetionTime  10 //��������  �ָ�ʱ��Ϊ 10����������
#endif
//==============================================================================

//TUV��֤��׼===========================================================================
#ifdef VDE0126
  #define VGridHi2Limit	  264.1 //280
  #define VGridHi1Limit   264
  #define VGridHiLimit	  253
  #define VGridLow1Limit	195.1   //170//184//175//175//185
  #define VGridLow2Limit	195  //160//165
  #define VGridLowLimitBack    200.1
  #define VGridProtectionTime	5

  #define FreGridHiLimit		50.15	
  #define FreGridLowLimit 	48.5
  #define FreGridHi2Limit		52.0	
  #define FreGridLow2Limit 	47.5
  #define FreGridProtectionTime	5

  #define SafetyReConnetionTime		10
#endif
//==============================================================================


//��������
#define GFCIRmsLimit		    3      //©�������󱣻�                 
#define GFCIRmsProtectionTime	10     //����ʱ�� 10����������=0.2s
#define GFCIJump1Limit	        0.2    //©����ͻ��1
#define GFCIJump1ProtectionTime	10	   //����1����ʱ�� 10����������=0.2s		
#define GFCIJump2Limit	        0.4	   //©����ͻ��2
#define GFCIJump2ProtectionTime	5      //����2����ʱ�� 5����������=0.1s	
#define GFCIJump3Limit	        0.8    //©����ͻ��3
#define GFCIJump3ProtectionTime	1      //����3����ʱ�� 1����������=0.02s	

#define InsulationRLimit	1200        
#define InsulationRProtectionTime	5

#define DCILimit	5        //Ӳ��������������������ֱ������������ֵ                    
#define DCIProtectionTime 5  //Ӳ��������������������ֱ����������ʱ�� 5����������

#define AD_Channel_Offset_DCI_Limit		20  //�����������е���ֱ�������޷�ֵ20A
#define AD_Channel_Offset_IGridLimit	20  //�����������������������ֱ�������޷�ֵ20A	
#define AD_Channel_Offset_VGridLimit	10  //���������������ѹ������ֱ�������޷�ֵ10V


// PV input defination
#define PV_Under_Volt_Limit	450  //���������ѹ���ֵ450V
#define PV_Over_Volt_Limit	800  //���������ѹ���ֵ800V


// Derating defination  ��ʱ����
#define	ACPowerDerating_AmbientTempRate 	1000
#define	ACPowerDerating_HeathinkTempRate	1000
#define	DCPowerDerating_HeathinkTempRate	500
#define	DCPowerDerating_VoltageRate	50

#define	DeratingAmbientTemperature 60       //modfied by microwu  6.20
#define ShutAmbientTemperature 65 //65      
#define	DeratingBoostTemperatureLimit	85  //85
#define	DeratingInvTemperatureLimit		85 //85
#define	ShutInvTemperatureLimit		90
//******************************************************************************


#define Inv_OCP_LEVEL		GpioDataRegs.GPBDAT.bit.GPIO53  //Ӳ���������������IO��
#define Bus_OVP_LEVEL		GpioDataRegs.GPBDAT.bit.GPIO52  //Ӳ�������ѹ������IO��
#define MasterDSP_LEVEL     GpioDataRegs.GPADAT.bit.GPIO14  //��⸱DSP״̬


//fengshan  Control  pin define
#define FAN1_LOW_Enable		      GpioDataRegs.GPCSET.bit.GPIO70 = 1;  //500kw Fan1_LOW control
#define FAN1_LOW_State			  GpioDataRegs.GPCDAT.bit.GPIO70	
#define FAN1_LOW_Disable		  GpioDataRegs.GPCCLEAR.bit.GPIO70 = 1;

#define FAN1_MIDDLE_Enable		  GpioDataRegs.GPCSET.bit.GPIO67 = 1;  //250kw Fan1_MIDDLE control
#define FAN1_MIDDLE_State		  GpioDataRegs.GPCDAT.bit.GPIO67	
#define FAN1_MIDDLE_Disable		  GpioDataRegs.GPCCLEAR.bit.GPIO67 = 1;

#define FAN1_HIGH_Enable		  GpioDataRegs.GPCSET.bit.GPIO68 = 1;  //250kw Fan1_HIGH control
#define FAN1_HIGH_State			  GpioDataRegs.GPCDAT.bit.GPIO68	
#define FAN1_HIGH_Disable		  GpioDataRegs.GPCCLEAR.bit.GPIO68 = 1;

#define FAN2_LOW_Enable			  GpioDataRegs.GPCSET.bit.GPIO65 = 1;  //250kw Fan2_LOW control
#define FAN2_LOW_State			  GpioDataRegs.GPCDAT.bit.GPIO65	
#define FAN2_LOW_Disable		  GpioDataRegs.GPCCLEAR.bit.GPIO65 = 1;

#define FAN2_MIDDLE_Enable		  GpioDataRegs.GPCSET.bit.GPIO66 = 1;  //250kw Fan2_MIDDLE control
#define FAN2_MIDDLE_State		  GpioDataRegs.GPCDAT.bit.GPIO66	
#define FAN2_MIDDLE_Disable		  GpioDataRegs.GPCCLEAR.bit.GPIO66 = 1;

#define FAN2_HIGH_Enable		  GpioDataRegs.GPBSET.bit.GPIO61 = 1;  //250kw Fan2_HIGH control
#define FAN2_HIGH_State			  GpioDataRegs.GPBDAT.bit.GPIO61	
#define FAN2_HIGH_Disable		  GpioDataRegs.GPBCLEAR.bit.GPIO61 = 1;

#define FAN3_LOW_Enable			  GpioDataRegs.GPCSET.bit.GPIO64 = 1;  //250kw Fan3_LOW control
#define FAN3_LOW_State			  GpioDataRegs.GPCDAT.bit.GPIO64	
#define FAN3_LOW_Disable		  GpioDataRegs.GPCCLEAR.bit.GPIO64 = 1;

#define FAN3_MIDDLE_Enable		  GpioDataRegs.GPBSET.bit.GPIO60 = 1;  //250kw Fan3_MIDDLE control
#define FAN3_MIDDLE_State	      GpioDataRegs.GPBDAT.bit.GPIO60	
#define FAN3_MIDDLE_Disable		  GpioDataRegs.GPBCLEAR.bit.GPIO60 = 1;

#define FAN3_HIGH_Enable		  GpioDataRegs.GPBSET.bit.GPIO59 = 1;  //250kw Fan3_HIGH control
#define FAN3_HIGH_State			  GpioDataRegs.GPBDAT.bit.GPIO59	
#define FAN3_HIGH_Disable		  GpioDataRegs.GPBCLEAR.bit.GPIO59 = 1;

//AC�����Ӵ����Ŀ����ź�
#define AC_RELAY1_ON       	   GpioDataRegs.GPASET.bit.GPIO28 = 1;			// 250kw Relay_R1
#define AC_RELAY1_State		   GpioDataRegs.GPADAT.bit.GPIO13
#define AC_RELAY1_OFF		   GpioDataRegs.GPACLEAR.bit.GPIO28 = 1;

//ֱ�����Ԥ���̵���1����
#define DC_CHARGE1_ON          GpioDataRegs.GPBSET.bit.GPIO34 = 1;			// 250kw Relay_R1
#define DC_CHARGE1_State	   GpioDataRegs.GPBDAT.bit.GPIO39
#define DC_CHARGE1_OFF		   GpioDataRegs.GPBCLEAR.bit.GPIO34 = 1;
//ֱ�����Ԥ���̵���2����
#define DC_CHARGE2_ON          GpioDataRegs.GPBSET.bit.GPIO36 = 1;			// 250kw Relay_R1
#define DC_CHARGE2_State	   GpioDataRegs.GPADAT.bit.GPIO14
#define DC_CHARGE2_OFF		   GpioDataRegs.GPBCLEAR.bit.GPIO36 = 1;
//ֱ����ĵ��ݵ���й�ż̵�������
#define DC_DISCHARGE_ON        GpioDataRegs.GPCSET.bit.GPIO71 = 1;			// 250kw Relay_R1
#define DC_DISCHARGE_State	   GpioDataRegs.GPADAT.bit.GPIO15
#define DC_DISCHARGE_OFF	   GpioDataRegs.GPCCLEAR.bit.GPIO71 = 1;

//�����ź���
#define L_Temp_State	       GpioDataRegs.GPCDAT.bit.GPIO87
#define FPGA_Ready	           GpioDataRegs.GPADAT.bit.GPIO29
#define VgridRZero_State	   GpioDataRegs.GPADAT.bit.GPIO25
  
typedef struct
{
	int16	i16SafeCountry;
	float32	f32VGrid_Hi1Limit;       //������ѹ������ѹ����1
	float32	f32VGrid_Hi2Limit;       //������ѹ������ѹ����2
	float32	f32VGrid_HiLimit;        //������ѹ������ѹ����
	float32	f32VGrid_Low1Limit;      //������ѹ������ѹ����1
	float32	f32VGrid_Low2Limit;      //������ѹ������ѹ����2
    float32 f32VGrid_HiLimitBack;    //������ѹ������ѹ���޿ɷ���ֵ
	float32 f32VGrid_LowLimitBack;   //������ѹ������ѹ���޿ɷ���ֵ
	float32	f32VGrid_ProtectionTime; //������ѹ������ѹʱ��ֵ

	float32	f32FreGrid_HiLimit;      //Ƶ�ʱ�������1
	float32	f32FreGrid_LowLimit;     //Ƶ�ʱ�������1
	float32	f32FreGrid_Hi2Limit;     //Ƶ�ʱ�������2
	float32	f32FreGrid_Low2Limit;    //Ƶ�ʱ�������2
	float32	f32FreGrid_ProtectionTime; //Ƶ�ʱ���ʱ������

	float32	f32ReConnetionTime;        //�ɷ��صı�������ʱʱ��

	float32	f32DCI_Limit;              //��������ֱ����������    
	float32	f32DCI_ProtectionTime;     //��������ֱ����������ʱ��   

	float32	f32GFCI_RmsLimit;            //©���������������
	float32	f32GFCI_RmsProtectionTime;   //©��������������Ƶı���ʱ��
	float32	f32GFCI_Jump1Limit;          //©������������ֵ1
	float32	f32GFCI_Jump1ProtectionTime; //©������������ֵ1����ʱ��
	float32	f32GFCI_Jump2Limit;          //©������������ֵ2
	float32	f32GFCI_Jump2ProtectionTime; //©������������ֵ2����ʱ��
	float32	f32GFCI_Jump3Limit;          //©������������ֵ3
	float32	f32GFCI_Jump3ProtectionTime; //©������������ֵ3����ʱ��

	float32	f32InsulationR_Limit;
	float32	f32InsulationR_ProtectionTime;
}SAFETY_PARAMETER_REG;

extern SAFETY_PARAMETER_REG	SafetyReg;


//===================== Global functions==================================

extern void GridVoltCheck(void);
extern void GridCurrentCheck(void);
extern void GridFreqCheck(void);

extern void GridCurrentDciCheck(void);
extern void GFCICheck(void);
extern void PVVoltCheck(void);
extern void PVReversCurrCheck(void);

//extern void IslandingCheck(void);

extern void ADOffsetCheck(void);
extern void PvInsulationCheck(void);
extern void MatrixCheck(void);
extern void RelayCheck(void);
extern void DCFanCheck(void);
extern void HwInvOCPCheck(void);
//extern void HwBoostOCPCheck(void);
extern void HwBusOVPCheck(void);
extern void AuxPowerCheck(void);
extern void RChipWDCheck(void);



//extern    float32 PViso1,PViso2,PViso3,PViso4;
//extern	float32	Viso1_P;  //added by Ken
//------------------
#endif

//--- end of file -----------------------------------------------------


