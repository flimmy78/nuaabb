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

union  IO_STATE   //��������IO���壬4���ֽ�
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
		Uint16  DCP_Fuse:1;     //0 ��ĸ���۶�˿�����ź�
   		Uint16  DCN_Fuse:1;     //1 ��ĸ���۶�˿�����ź�
   		Uint16  ACR_Fuse:1;     //2 ���A���۶�˿�����ź�
   		Uint16  ACS_Fuse:1;     //3 ���B���۶�˿�����ź�
        Uint16  ACT_Fuse:1;     //4 ���C���۶�˿�����ź�
		Uint16  AC_Arrester:1;  //5 AC �����������ź�
		Uint16  DC_Arrester:1;  //6 DC�����������ź�
		Uint16  Rsvd1:1;        //7 ����
		
		//byte1
        Uint16  FAN1:1;    		//0 ���1���ߵ�����״̬
        Uint16  FAN2:1;    		//1 ���1���е�����״̬
        Uint16  FAN3:1;    		//2 ���1���͵�����״̬
		Uint16  FAN4:1;    		//3 ���2���ߵ�����״̬
        Uint16  FAN5:1;    		//4 ���2���е�����״̬
        Uint16  FAN6:1;    		//5 ���2���ֵ�����״̬
	    Uint16  Emergency_Button:1;  //6 Ӧ�����ƿ��ط���״̬
        Uint16  Inductor_Temp:1;     //7 ����¶�״̬����

		//byte2
        Uint16  DCP_Charge_Contactor:1;   //0 ��ĸ��Ԥ���Ӵ�������
        Uint16  DCN_Charge_Contactor:1;   //1 ��ĸ��Ԥ���Ӵ�������
        Uint16  DISCharge_Contactor:1;    //2 ֱ��ĸ��й�ŽӴ�������״̬
        Uint16  AC_Breaker:1;             //3 AC��·��״̬����
        Uint16  AC_Contactor:1;           //4 AC�Ӵ�������״̬
        Uint16  DC_OVP:1;                 //5 Ӳ�������ѹ����
        Uint16  AC_OCP:1;                 //6 Ӳ�������������
		Uint16  Rsvd2:1;                  //7 ����

		//byte3 ��0-2�����IO��״̬����λ����� 
		Uint16  RUN_COMMAND:1;    		//0 �����������λ�����������������IO
        Uint16  STOP_COMMAND:1;   		//1 ֹͣ�������λ�����������ֿ���������IO
        Uint16  DisCharge_COMMAND:1;    //2 й��ֱ������ݵ�ѹ���
        Uint16  FPGA_Conf_Done:1;       //3 FPGA���ý���״̬
		Uint16  FAN7:1;    				//4 ���3���ߵ�����״̬  
        Uint16  FAN8:1;    				//5 ���3���е�����״̬  
        Uint16  FAN9:1;    				//6 ���3���͵�����״̬  
        Uint16  Rsvd3:1;                //7 ����

     } bit;   
};
extern union IO_STATE   g_IO_State;



//�����������õ���һЩ��־λ���ͱ���λ
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
        Uint16  MpptRestart:1;    		 //0  û���õ�
        Uint16  AD_initial:1;   		 //1
        Uint16  EEROM_Data_Load_OVER:1;  //2 EEROM ���������ڴ����
        Uint16  Zero_Crossing_Flag:1;    //3 �����������־        
        Uint16  PLL_LOCKED:1;            //4 ������ɱ�־         
        Uint16  IsoCheckOver:1;          //5 ��Ե�����ɱ�־              
        Uint16  Phase_Sequence_Detect_Over:1;  //�����������û���õ�
        Uint16  Rsvd1:1;                        //����λ
        
        //byet 1       
        //����Ƶ�ʼ��
        Uint16  Freq_Range1:1;           //0 Ƶ�ʼ���ڷ�Χ1����  ���䷶Χ �ο���̫����׼
        Uint16  Freq_Range2:1;           //1 Ƶ�ʼ���ڷ�Χ2���� 
        Uint16  Freq_Range3:1;           //2 Ƶ�ʼ���ڷ�Χ3����
        Uint16  Freq_Range4:1;           //3 Ƶ�ʼ���ڷ�Χ4����
        Uint16  Freq_Range5:1;           //4 Ƶ�ʼ���ڷ�Χ5����        
        //R���ѹ���
        Uint16  VgridR_RMS_Range1:1;     //5 R���ѹ��Чֵ��Χ1   ��Χ�ο���̫����׼
        Uint16  VgridR_RMS_Range2:1;     //6 R���ѹ��Чֵ��Χ2   ��Χ�ο���̫����׼
        Uint16  VgridR_RMS_Range3:1;     //7 R���ѹ��Чֵ��Χ3   ��Χ�ο���̫����׼
        
        //byte2
        Uint16  VgridR_RMS_Range4:1;     //0 R���ѹ��Чֵ��Χ1   ��Χ�ο���̫����׼
        Uint16  VgridR_RMS_Range5:1;     //1 R���ѹ��Чֵ��Χ2   ��Χ�ο���̫����׼          
        //S���ѹ���
        Uint16  VgridS_RMS_Range1:1;     //2 S���ѹ��Чֵ��Χ1   ��Χ�ο���̫����׼
        Uint16  VgridS_RMS_Range2:1;     //3 S���ѹ��Чֵ��Χ2   ��Χ�ο���̫����׼
        Uint16  VgridS_RMS_Range3:1;     //4 S���ѹ��Чֵ��Χ3   ��Χ�ο���̫����׼                
        Uint16  VgridS_RMS_Range4:1;     //5 S���ѹ��Чֵ��Χ1   ��Χ�ο���̫����׼
        Uint16  VgridS_RMS_Range5:1;     //6 S���ѹ��Чֵ��Χ2   ��Χ�ο���̫����׼        
        //T���ѹ��Χ���
        Uint16  VgridT_RMS_Range1:1;     //7 S���ѹ��Чֵ��Χ1   ��Χ�ο���̫����׼
        
        //byte 3
        Uint16  VgridT_RMS_Range2:1;     //0 S���ѹ��Чֵ��Χ2   ��Χ�ο���̫����׼
        Uint16  VgridT_RMS_Range3:1;     //1 S���ѹ��Чֵ��Χ3   ��Χ�ο���̫����׼
        Uint16  VgridT_RMS_Range4:1;     //2 S���ѹ��Чֵ��Χ1   ��Χ�ο���̫����׼
        Uint16  VgridT_RMS_Range5:1;     //3 S���ѹ��Чֵ��Χ2   ��Χ�ο���̫����׼
        //R�������Чֵ��Χ���
        Uint16  IgridR_RMS_Range1:1;     //4 R�ಢ������1��Χ��־  �ɰ�0~110%���ʷ�Χ����
        Uint16  IgridR_RMS_Range2:1;     //5 R�ಢ������2��Χ��־ �ɰ�110��125%��������2s���
        Uint16  IgridR_RMS_Range3:1;     //6 R�ಢ������2��Χ��־ �ɰ�>125%���������ض�        
        //S�������Чֵ��Χ���
        Uint16  IgridS_RMS_Range1:1;     //7 R�ಢ������1��Χ��־  �ɰ�0~110%���ʷ�Χ����
        
        //byte 4
        Uint16  IgridS_RMS_Range2:1;     //0 R�ಢ������2��Χ��־ �ɰ�110��125%����2s���                
        Uint16  IgridS_RMS_Range3:1;     //1 R�ಢ������2��Χ��־ �ɰ�>125%���������ض�        
        //T�������Чֵ��Χ���
        Uint16  IgridT_RMS_Range1:1;     //2 R�ಢ������1��Χ��־  �ɰ�0~110%���ʷ�Χ����
        Uint16  IgridT_RMS_Range2:1;     //3 R�ಢ������2��Χ��־ �ɰ�110��125%��������2s���              
        Uint16  IgridT_RMS_Range3:1;     //4 R�ಢ������2��Χ��־ �ɰ�>125%���������ض�                    
        Uint16  GFCI5ACheck_OVER:1;      //5 ©����������״̬  
        Uint16  AC_OCP: 1;               //6 �����������λ      
        Uint16  Bus_OVP: 1;              //7 �����������λ
        
        //byte 5
        Uint16  RW_EEROM_Enable :1;       //0 ����EEROM��д����
        Uint16  STATE_CHANGING_STATE :1;  //1 ״̬ת����־
        Uint16  STATE_CHANGING_OVER  :1;  //2 ״̬ת������,����״̬ת�������п��ܷ����źŲ��ȶ�        
        //�����ǿ��ư��Լ��־              
        Uint16  SD_Test_OVER:1;           //3 SD�����Խ���                
        Uint16  CAN_TEST_OVER:1;          //4 CANͨ�Ų��Խ���            
        Uint16  EEROM_TEST_OVER:1;        //5 EEROM Read Write Test 
        Uint16  SCI_TEST_OVER:1;          //6 SCI ͨ�Ų��Խ���
        Uint16  FPGA_TEST_OVER:1;         //7 FPGA���Խ���
               
       //byte  6
        Uint16   OutputWattCtrl_flag: 1;  	//����й����ʿ��Ʊ�־          
        Uint16   OutputVarCtrl_flag: 1;  	//����޹������ʿ��Ʊ�־          
        Uint16   OutputPF_Ctrl_flag: 1;    	//��������������Ʊ�־          
        Uint16   TimeSet_flag: 1;           //���������ʱ��          
        Uint16   TimeRead_flag: 1;          //��ȡ�����ʱ���־          
        Uint16   Rsvd2: 3;              	//3λ����λ          

       //byte  7
        Uint16   Rsvd3: 8;              	//8λ����λ          
       
    }bit;
}; 
extern union STATE_CHECK  g_StateCheck, g_StateCheck_Initial;



//��λ����λ���壬��Ϊ���ù��ϺͿɷ��ع���
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
        //�ɻָ�����byte0   
        //byte0
        Uint16  VGridROverRating:1;     //B0   R���ѹ
        Uint16  VGridRUnderRating:1;    //B1   R��Ƿѹ
        Uint16  VGridSOverRating:1;     //B2   S���ѹ         
        Uint16  VGridSUnderRating:1;    //B3   S��Ƿѹ
        Uint16  VGridTOverRating:1;     //B4   T���ѹ
        Uint16  VGridTUnderRating:1;    //B5   T��Ƿѹ
        Uint16  FreGridOverRating:1;    //B6   ����Ƶ��̫��
        Uint16  FreGridUnderRating:1;   //B7   ����Ƶ��̫��

        //byte1
        Uint16  PvVoltLowFault:1;       //B0   PVֱ��ĸ��Ƿѹ
        Uint16  PvVoltOvrFault:1;       //B1   PVֱ��ĸ�߹�ѹ
        Uint16  SCICommLoseFault:1;     //B2   SCIͨ�Ź���
        Uint16  IGBT1TempFault:1;       //B3   IGBT1����
        Uint16  IGBT2TempFault:1;       //B4   IGBT2����
        Uint16  IGBT3TempFault:1;       //B5   IGBT3����
        Uint16  TempAmbient:1;          //B6   �����¶�
        Uint16  OverLoad:1;             //B7   ���ع���added by Ken
        
        //byte 2
        Uint16  DCIGridROverLimit:1;    //B0   R��ֱ��ƫ�ù���        
        Uint16  DCIGridSOverLimit:1;    //B1   S��ֱ��ƫ�ù���
        Uint16  DCIGridTOverLimit:1;    //B2   T��ֱ��ƫ�ù���
        Uint16  FreGridROverRating:1;   //B3   R��Ƶ�ʳ�Խ����
        Uint16  FreGridRUnderRating:1;  //B4   R��Ƶ�ʵ�������
        Uint16  FreGridSOverRating:1;   //B5   S��Ƶ�ʳ�Խ����
        Uint16  FreGridSUnderRating:1;  //B6   S��Ƶ�ʵ�������
        Uint16  FreGridTOverRating:1;   //B7   T��Ƶ�ʳ�Խ����
        
        //byte 3
        Uint16  FreGridTUnderRating:1;  //B0   T��Ƶ�ʵ�������      
        Uint16  HW_BusOVP_Fault:1;      //B1   Ӳ��ֱ��ĸ�߹�ѹ����
        Uint16  HW_OCP_AC_Fault:1;      //B2   Ӳ��������������
        Uint16  OCP_AC_RMS_Fault:1;     //B3   �����������������
        Uint16  Fault_VGrid_ave:1;      //B4   ������ѹƽ��ֵ����
        Uint16  Fault_Vinv_ave:1;       //B5   ����ѹƽ��ֵ����
        Uint16  PLLFault:1;				//B6   ������� 
        Uint16  PvPolarRevFault:1;		//B7   ֱ�����뼫�Է��ӹ���            
        
        //byte 4 һ�����ù��� 
        Uint16  DCP_Fuse_Fault:1;     	//B0 PVĸ�����۶�˿����                  
   		Uint16  DCN_Fuse_Fault:1;     	//B1 PVĸ�߸��۶�˿����
   		Uint16  ACR_Fuse_Fault:1;    	 //B2 R���۶�˿����
   		Uint16  ACS_Fuse_Fault:1;    	 //B3 S���۶�˿����
        Uint16  ACT_Fuse_Fault:1;    	 //B4 T���۶�˿����               
        Uint16  DCP_Charge_Contactor_Fault:1;   //B5 Ԥ����ĸ�ߵ�Ӵ�������
        Uint16  DCN_Charge_Contactor_Fault:1;   //B6 Ԥ��縺ĸ�߽Ӵ�������        
        Uint16  DISCharge_Contactor_Fault:1;    //B7 ֱ��ĸ�߷ŵ�Ӵ�������
        
        //byte 5 ���ù���
        Uint16  Emergency_Fault:1;    	//B0 Ӧ������         
        Uint16  AC_Breaker_Fault:1;   	//B1 AC��·������
        Uint16  AC_Contactor_Fault:1; 	//B2 AC�Ӵ�������                        
        Uint16  CANCommLoseFault:1;   	//B3 CANͨ�Ź���              
        Uint16  InductorTempFault:1;  	//B4 ����¶ȹ���         
        Uint16  FAN1_Fault:1;         	//B5 ����1����
        Uint16  FAN2_Fault:1;         	//B6 ����2����
        Uint16  FAN3_Fault:1;         	//B7 ����2����              
        
        //byte 6  �Լ���� Ҳ�����ù���
        Uint16  SCI_TEST_Fault:1;		//B0 SCI���Թ���                       
        Uint16  CAN_TEST_Fault:1;		//B1 CAN���Թ���
        Uint16  FPGA_TEST_Fault:1;		//B2 FPGA���Թ���
        Uint16  SD_TEST_Fault:1;		//B3 SD���Թ���
        Uint16  FPGA_Conf_Done_Fault:1;	//B4 FPGA���ù���
        Uint16  EEpromRdWrFault:1;		//B5 EEROM��д����     
        Uint16  PvIsoFault:1;			//B6 ��Ե������
        Uint16  PvReversCurrFault:1; 	//B7 ����ŵ����
        
        //byte 7  ©����������            
        Uint16  GFCIFault:1;			//B0  ©����������
        Uint16  GFCIJump1Fault:1;		//B1  ©�����������1����                 
        Uint16  GFCIJump2Fault:1;		//B2  ©�����������2����
        Uint16  GFCIJump3Fault:1;		//B3  ©�����������3����
        Uint16  GFCIOver10AFault:1;		//B4  ©��������10A����
		Uint16  HWADFault_GFCI:1;		//B5  GFCIӲ������
		Uint16  SlaveDSPFault:1;		//B6  ��DSP����
        Uint16  MasterDSPFault:1;		//B7  ��DSP����
      
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
        Uint16  VGridROverRating:1;     //B0   R���ѹ
        Uint16  VGridRUnderRating:1;    //B1   R��Ƿѹ
        Uint16  VGridSOverRating:1;     //B2   S���ѹ         
        Uint16  VGridSUnderRating:1;    //B3   S��Ƿѹ
        Uint16  VGridTOverRating:1;     //B4   T���ѹ
        Uint16  VGridTUnderRating:1;    //B5   T��Ƿѹ
        Uint16  SlaveDSPFault:1;    //B6   ��DSP�Լ����
        Uint16  Rsvd1:1;   //B7   ����Ƶ��̫��

		//byte 1
        Uint16  DCIGridROverLimit:1;    //B0   R��ֱ��ƫ�ù���        
        Uint16  DCIGridSOverLimit:1;    //B1   S��ֱ��ƫ�ù���
        Uint16  DCIGridTOverLimit:1;    //B2   T��ֱ��ƫ�ù���
        Uint16  OCP_AC_RMS_Fault:1;   //B3   �����������
        Uint16  HW_OCP_AC_Fault:1;  //B4     Ӳ�������������
        Uint16  Fault_VGrid_ave:1;   //B5    ������ѹƽ��ֵ����
        Uint16  FreGridUnderRating:1;  //B6   
        Uint16  Rsvd2:1;   //B7   T��Ƶ�ʳ�Խ����

		//byte 2
        Uint16  FreGridROverRating:1;    //B0   R��Ƶ�ʳ�Խ����       
        Uint16  FreGridRUnderRating:1;   //B1   R��Ƶ�ʵ�������
        Uint16  FreGridSOverRating:1;    //B2   S��Ƶ�ʳ�Խ����
        Uint16  FreGridSUnderRating:1;   //B3   S��Ƶ�ʵ�������
        Uint16  FreGridTOverRating:1;    //B4   T��Ƶ�ʳ�Խ����
        Uint16  FreGridTUnderRating:1;   //B5   T��Ƶ�ʵ�������
        Uint16  Rsvd3:1;  //B6   S��Ƶ�ʵ�������
        Uint16  Rsvd4:1;   //B7   T��Ƶ�ʳ�Խ����

		//byte 3
        Uint16  Rsvd5:1;    //B0   ����λ      
        Uint16  Rsvd6:1;    //B1   ����λ 
        Uint16  Rsvd7:1;    //B2   ����λ 
        Uint16  Rsvd8:1;    //B3   ����λ 
        Uint16  Rsvd9:1;    //B4   ��λ 
        Uint16  Rsvd10:1;    //B5  ����λ 
        Uint16  Rsvd11:1;    //B6  ����λ 
        Uint16  Rsvd12:1;    //B7  ����λ 
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


