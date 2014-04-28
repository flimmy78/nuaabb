/*=============================================================================*
 *  Copyright(c) 2012-2013, ALL RIGHTS RESERVED
 *
 *  FILENAME : 500kw_DataAcquisition.h
 *  PURPOSE  : header files 500KW_DataAcquisition.c 
 *			       define constant, struct declaration, extern varibles
 *				 
 *  HISTORY  :
 *    DATE 2012.12.18           VERSION V1.0       AUTHOR:WangWanBao            NOTE
 *        
 ******************************************************************************/

#ifndef DATA_ACQUISITION_H
#define DATA_ACQUISITION_H

//================= Global constants============================================
// -----------------------------------------------
// --- Grid Frequency Limits Specifications
// -----------------------------------------------
#define Value_2Pi	6.283185307   	// PI * 2 value
#define Value_Pi 	3.141592654     // PI value

//#define	Stabilized_BusVoltage     600   
#define	Rated50HzGirdFrequecy		50	// Unit, Hz
#define	RatedDeviationofGridFreq	1	// Unit, Hz
#define MaxGridFrequecy				(Rated50HzGirdFrequecy + RatedDeviationofGridFreq)	
#define MinGridFrequecy				(Rated50HzGirdFrequecy - RatedDeviationofGridFreq)

//   PLL protection limits
#define DetectMaxGridFrequency		Rated50HzGirdFrequecy * 1.1	
#define DetectMinGridFrequency		Rated50HzGirdFrequecy * 0.9
#define ThetaStepRated				Rated50HzGirdFrequecy * Value_2Pi/PWM_FREQ/2
#define Theta_Step_Hi_Limit			DetectMaxGridFrequency * Value_2Pi/PWM_FREQ/2
#define Theta_Step_Low_Limit		DetectMinGridFrequency * Value_2Pi/PWM_FREQ/2

#define CoffStepToFre				(2 * PWM_FREQ / Value_2Pi)  // for test PLL output frequency

// -----------------------------------------------
// define measure gain, the caculated values are ten times of real values 
// except for insulation resistor (1 time) and inductor DCoffset(100 times) 
// -----定义了一些采样的比例
#define IGridMeasureGain		0.689338235f   	// 
#define ILMeasureGain		    0.48828125f		// 

#define VGridMeasureGain		0.168579826f	//  
#define VInvMeasureGain			0.168579826f	// 

#define IPVMeasureGain			0.646254595f	//  
//#define VPVMeasureGain			0.264102051f	//  
#define VPVMeasureGain			0.253778843f	//  
#define InsulationMeasureGain	0.003111492f		//

#define ILDCIMeasureGain		0.008121744f	// 
#define IGFCIMeasureGain		0.002501897f    //

#define EnergyMeasureGain		4.6296e-8f 		//1/120 * 0.02 / 3600	kW * s / s
									
#define TempGain                0.00366211f;

#define ADDefaultACOffset			  2048.0		//define AD samples' DC offset 



//-----温度多路选择采样用
#define CHOSE1_ON		    GpioDataRegs.GPASET.bit.GPIO22 = 1
#define CHOSE1_OFF			GpioDataRegs.GPACLEAR.bit.GPIO22 = 1
#define CHOSE2_ON		    GpioDataRegs.GPASET.bit.GPIO23 = 1
#define CHOSE2_OFF			GpioDataRegs.GPACLEAR.bit.GPIO23 = 1
#define CHOSE3_ON			GpioDataRegs.GPASET.bit.GPIO24 = 1
#define CHOSE3_OFF			GpioDataRegs.GPACLEAR.bit.GPIO24 = 1


//Insulation Resistor Relay control pin 绝缘检测继电器控制
#define PV1_ISO_GROUND_RLY_OFF         GpioDataRegs.GPACLEAR.bit.GPIO18 = 1;
#define PV1_ISO_GROUND_RLY_ON          GpioDataRegs.GPASET.bit.GPIO18 = 1;

#define PV1_ISO_TEST_RLY_OFF           GpioDataRegs.GPACLEAR.bit.GPIO19 = 1;//5kw ISO test I/O
#define PV1_ISO_TEST_RLY_ON            GpioDataRegs.GPASET.bit.GPIO19 = 1;


/*=================struct define====================*/
struct	AD_Sample_Reg1 
{
	float32	f32IGrid_R;    //grid current R phase
	float32	f32IGrid_S;    //grid current S phase
	float32	f32IGrid_T;	   //grid current T phase

	float32	f32IL_R;       //Inductor current R phase
	float32	f32IL_S;	   //Inductor current S phase
	float32	f32IL_T;       //Inductor current T phase

	float32	f32VGrid_R;    //GRID voltage R phase
	float32	f32VGrid_S;	   //GRID voltage S phase
	float32	f32VGrid_T;    //GRID voltage T phase

	float32	f32VInv_R;     //LC Filter C voltage R phase
	float32	f32VInv_S;	   //LC Filter C voltage S phase
	float32	f32VInv_T;     //LC Filter C voltage T phase

	float32	f32VPV;        //Input PV voltage 
	float32	f32IPV;        //Input PV current 

	float32	f32Mux1;       //多路选择,L current DCI
	float32	f32Mux2;	   //多路选择,VINV voltage
	float32	f32Mux3;	   //多路选择,temp sample

//	float32	f32VbusP;
//	float32	f32VbusN;
	float32	f32GFCI;       //漏电流采样

	float32	f32DCIR;       //L current DCI R Phase
	float32	f32DCIS;	   //L current DCI S Phase
	float32	f32DCIT;       //L current DCI T Phase

	float32	f32Viso;       //ISO test 
	float32 f32Temp;       //Temp test
	float32 f32Temp_R;     // R phase IGBT temp 
	float32 f32Temp_S;	   // S phase IGBT temp 
	float32 f32Temp_T;     // T phase IGBT temp 

};

struct	AD_ACC_Reg1
{
	int16	i16Counter;    //一个电网周期计数

	float32	f32IGrid_Rave;
	float32	f32IGrid_Save;
	float32	f32IGrid_Tave;

	float32	f32IL_Rave;
	float32	f32IL_Save;
	float32	f32IL_Tave;

	float32	f32VGrid_Rave;
	float32	f32VGrid_Save;
	float32	f32VGrid_Tave;

	float32	f32VInv_Rave;
	float32	f32VInv_Save;
	float32	f32VInv_Tave;

	float32	f32GFCIave;


	float32	f32IGrid_Rrms;
	float32	f32IGrid_Srms;
	float32	f32IGrid_Trms;

	float32	f32IL_Rrms;
	float32	f32IL_Srms;
	float32	f32IL_Trms;

	float32	f32VGrid_RSrms;
	float32	f32VGrid_STrms;
	float32	f32VGrid_TRrms;

	float32 f32VGrid_Rrms1Cycle;
	float32 f32VGrid_Srms1Cycle;
	float32 f32VGrid_Trms1Cycle;


	float32	f32VInv_RSrms;
	float32	f32VInv_STrms;
	float32	f32VInv_TRrms;

	float32	f32VGrid_Rrms;
	float32	f32VGrid_Srms;
	float32	f32VGrid_Trms;

	float32	f32VInv_Rrms;
	float32	f32VInv_Srms;
	float32	f32VInv_Trms;

	float32	f32GFCIrms;
	float32 f32GFCIrmsSave;

	float32	f32VPV;
	float32	f32IPV;

//	float32	f32VbusP;
//	float32	f32VbusN;
	float32	f32Vbus;
	float32	Coff_Dforward;   //Grid 前馈系数

	float32	f32DCIR;        
	float32	f32DCIS;
	float32	f32DCIT;

	float32	f32Viso;        
	float32	f32Viso2;

	float32	f32GridFreq;
	float32	f32GridRFreq;
	float32	f32GridSFreq;
	float32	f32GridTFreq;

	float32	f32Input1Watt;
	float32	f32Input2Watt;
	float32	f32OutputWatt;
	float32	f32OutputVar;
	float32	f32OutputVA;
	float32 f32OutputPF;
	float32	f32OutputQ;
	float32	f32OutputEnergy;
	float32	f32HistoryOutputEnergy;

	float32 f32DutyD_ff;           // D轴前馈
    float32 f32DutyQ_ff;           // Q轴前馈
	float32 f32GridFreqClarke1;    // APF 离散化系数1
	float32 f32GridFreqClarke2;	   // APF 离散化系数2
	float32 f32HistoryOutputEnergy_Day;
	float32 f32HistoryOutputEnergy_Month;
	float32 f32HistoryOutputEnergy_Year;
};


struct Data_Saving   //Save Sample diagram
{
	float32 Data1[400];
};


/*=================struct define====================*/

/*=================gloable varibles declaration====================*/
extern	struct	AD_Sample_Reg1	GeneralADbuffer, GetRealValue, ADGain, ADChannelOffset, ADOffset;
extern	struct	AD_ACC_Reg1	 AD_Acc, AD_Sum, Calc_Result;
extern	float32	f32SumCounterReci;
extern	int16 i16Cnt_SysParaTemp;
extern  Uint32   VgridSFreqCnt, VgridTFreqCnt,VgridRFreqCnt;
extern  Uint16   VgridRZero_Cnt,VgridSZero_Cnt,VgridTZero_Cnt;
//extern	struct	Data_Saving	Data_Saving1;
/*=================end of gloable varibles declaration====================*/

/*=================gloable function declaration====================*/
extern void Get_ADC_Result1(void);// functions called in ADC.c
extern void Get_ADC_Result2(void);// functions called in ADC.c
extern void ADAccCalc(void);

extern void	DcFanSpeedSense(void);
extern void	HwInvOCPDetection(void);
//extern void	HwBoost1OCPDetection(void);
//extern void	HwBoost2OCPDetection(void);
extern void	HwPVOVPDetection(void);
//extern void	AuxPowerDetection(void);
extern void	RChipWDDetection(void);
/*=================end of gloable function declaration====================*/

	 // extern float32  s_f32InductionCurrentDCA_Sum , s_f32InductionCurrentDCB_Sum, s_f32InductionCurrentDCC_Sum;
	 // extern float32	f32temp11, f32temp22, f32temp33; 


#endif	//end of ENERGY_CALC_H defination


//--- end of file -----------------------------------------------------


