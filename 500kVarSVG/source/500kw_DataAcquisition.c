/*=============================================================================*
 *       
 *  ALL RIGHTS RESERVED
 *
 *  FILENAME : 500kw_DataAcquisition.c 
 *
 *  PURPOSE  : Data acquisition file of 28335 for 5KW PV Inverter.
 *  
 *  HISTORY  :
 *    DATE:2012.12.21       VERSION:V1.0        AUTHOR:WangWanBao            NOTE
 *                	    Created
 *
 *----------------------------------------------------------------------------
 *  GLOBAL VARIABLES
 *    NAME                                    DESCRIPTION
 *      
 *----------------------------------------------------------------------------
 *  GLOBAL FUNCTIONS
 *    NAME                                    DESCRIPTION
 *    
 *============================================================================*/


#include "DSP2833x_Device.h"			// Peripheral address definitions
#include "500kw_MAIN_HEADER.h"					// Main include file


struct	AD_Sample_Reg1	GeneralADbuffer, GetRealValue, ADGain,ADChannelOffset, ADOffset;
struct	AD_ACC_Reg1	AD_Acc, AD_Sum, Calc_Result;
//struct	Data_Saving	Data_Saving1;

//int32   g_i32GridFrequencyPriodTemp; 
float32	f32SumCounterReci;   //һ�����ڲ��������ĵ�������������ƽ��ֵ����Чֵ
int16   i16Cnt_SysParaTemp; // ������ʱ��ֹͣ��ֹ����ƫ��

//----����Ƶ�ʶ���
Uint32   VgridRFreqCnt = 0;
Uint32   VgridSFreqCnt = 0;
Uint32   VgridTFreqCnt = 0;
Uint16   VgridRZero_Cnt = 0;
Uint16   VgridSZero_Cnt = 0;
Uint16   VgridTZero_Cnt = 0;
float32 IGBTR_TempValue = 0;  
float32 IGBTS_TempValue = 0;  
float32 IGBTT_TempValue = 0;  


//----����ѹ�������ѹ��Чֵ�Ĳ�ֵ�� �����ж�����ѹ�Ƿ���
extern float32 	Volt_Err_R;	//new added 2013.1.22 for Grid connect 
extern float32 	Volt_Err_S;
extern float32 	Volt_Err_T;
//extern int16	Connect_Time;   //�ж���ʱ


//float32 GFCIWatch;            //test for gfci


void Get_ADC_Result1();     // �������������ѹ����е��� �������������
void Get_ADC_Result2();     // �������������ѹ���������¶ȵ������ǿ������ĺ���
void ADAccCalc();           // ����ֵ�ۼӼ��㺯��
void TSK_GridPeriod();      //����20ms����������������ӳ���

//------TSK_GridPeriod �������ӳ���---------//
void LCurrentsAveCalc();       //��е��������ƽ��ֵ����
void GridCurrentsAveCalc();	   //�������������ƽ��ֵ����
void GridVoltsAveCalc();       //������ѹ�����ƽ��ֵ����
void InvVoltsAveCalc();   	   //����ѹ�����ƽ��ֵ����	
void LCurrentsRMSCalc();       //��е�������Чֵ����	
void GridCurrentsRMSCalc();    //������������Чֵ����
void GridVoltsRMSCalc();       //������ѹ����Чֵ����
void InvVoltsRMSCalc();		   //����ѹ����Чֵ����		
void PVVoltsCalc();            //�����ѹƽ��ֵ����
void PVCurrentsCalc();         //�������ƽ��ֵ����
void DCICalc();                //��е�����ֱ����Ӳ������������
void PVInsulationResCalc();    //��Ե��⣬ƽ��ֵֵ����
void PowerCalc();              //���빦�ʣ��������  ������ڹ���
void KwHourCalc();
void GridFrequencyCalc();      //����Ƶ�ʼ�⺯����R��S��T ���� PLL���Ƶ��
void GFCIAveCalc();            //©�������ƽ��ֵ����
void GFCIRMSCalc();            //©���������Чֵ����
//void StandbusVoltCal();      //MPPT module will use it
void GridLineVoltsRMSCalc();   //�����ߵ�ѹ��Чֵ����
void InvLineVoltsRMSCalc();    //����ߵ�ѹ��Чֵ����
void Vgrid_D_DffCalc();        //����ǰ��ϵ������
void IGBT_Temp();
void TemperatureCheck();       //�¶ȱ���


extern PARK VGrid_Park; //��������������ڼ���ǰ��
extern PARK IGrid_Park;

const float32  IGBTTempValue[71] = {11.053,10.833,10.313,10.10,9.932,9.485,9.231,8.750,8.644,8.363,\
                                    8.182,7.857,7.500,7.187,6.847,6.713,6.379,6.176,5.854,5.683,\
                                    5.566,5.250,5.00,4.726,4.437,4.362,4.209,4.051,3.970,3.772,\
                                    3.636,3.461,3.281,3.092,2.901,2.805,2.705,2.458,2.320,2.245,\
                                    2.155,2.070,1.950,1.840,1.745,1.665,1.605,1.545,1.485,1.425,\
                                    1.365,1.315,1.260,1.210,1.135,1.110,1.060,1.005,0.955,0.9,\
                                    0.85,0.820,0.795,0.770,0.740,0.715,0.685,0.660,0.630,0.59,0.575};

//int i = 0; 



/*=============================================================================*
 * FUNCTION: Get_ADC_Result1()   Get_ADC_Result2()
 * PURPOSE : get  AD results from the AD sample port 
 * CALLS: 
 *============================================================================*/
void Get_ADC_Result1(void)
{   
	//����������Ҫ��ȥ1.5Vƫ��
	GeneralADbuffer.f32IGrid_R = (float32)AdcMirror.ADCRESULT10 - ADDefaultACOffset;								// A0 result		
	GeneralADbuffer.f32VGrid_R = (float32)AdcMirror.ADCRESULT4 - ADDefaultACOffset;  
	GeneralADbuffer.f32IL_R = (float32)AdcMirror.ADCRESULT5 - ADDefaultACOffset;      					 	// B4 result
	GetRealValue.f32IGrid_R = GeneralADbuffer.f32IGrid_R * ADGain.f32IGrid_R - ADChannelOffset.f32IGrid_R;		// 
	GetRealValue.f32VGrid_R = GeneralADbuffer.f32VGrid_R * ADGain.f32VGrid_R - ADChannelOffset.f32VGrid_R;		// 
    GetRealValue.f32IL_R = GeneralADbuffer.f32IL_R * ADGain.f32IL_R;// - ADChannelOffset.f32IL_R;	             	// 	

	GeneralADbuffer.f32IGrid_S = (float32)AdcMirror.ADCRESULT6 - ADDefaultACOffset;								// B0 result		
	GeneralADbuffer.f32VGrid_S = (float32)AdcMirror.ADCRESULT1 - ADDefaultACOffset;
    GeneralADbuffer.f32IL_S = (float32)AdcMirror.ADCRESULT3 - ADDefaultACOffset;	                            // B3 result
	GetRealValue.f32IGrid_S = GeneralADbuffer.f32IGrid_S * ADGain.f32IGrid_S - ADChannelOffset.f32IGrid_S;		// 
	GetRealValue.f32VGrid_S = GeneralADbuffer.f32VGrid_S * ADGain.f32VGrid_S - ADChannelOffset.f32VGrid_S;		// 
	GetRealValue.f32IL_S = GeneralADbuffer.f32IL_S * ADGain.f32IL_S;// - ADChannelOffset.f32IL_S;	             	

    GeneralADbuffer.f32IGrid_T = (float32)AdcMirror.ADCRESULT8 - ADDefaultACOffset;								// B1 result		
	GeneralADbuffer.f32VGrid_T = (float32)AdcMirror.ADCRESULT0 - ADDefaultACOffset;
	GeneralADbuffer.f32IL_T = (float32)AdcMirror.ADCRESULT7 - ADDefaultACOffset;	                            // B2 result
	GetRealValue.f32IGrid_T = GeneralADbuffer.f32IGrid_T * ADGain.f32IGrid_T - ADChannelOffset.f32IGrid_T;		// 20 * PLLConReg.Sin_AITheta;
	GetRealValue.f32VGrid_T = GeneralADbuffer.f32VGrid_T * ADGain.f32VGrid_T - ADChannelOffset.f32VGrid_T;		// 300 * PLLConReg.Sin_AITheta;
	GetRealValue.f32IL_T = GeneralADbuffer.f32IL_T * ADGain.f32IL_T;// - ADChannelOffset.f32IL_T;	             	// 20 * PLLConReg.Sin_AITheta;
}

/*=============================================================================*
 * FUNCTION: Get_ADC_Result2()   Get_ADC_Result2()
 * PURPOSE : get  AD results from the AD sample port 
 * CALLS: 
 *============================================================================*/
void Get_ADC_Result2(void)
{  // start of function
	static int16 SELCNT = 0;     //��·ѡ��1 
	static int16 SELCNT1 = 0;    //��·ѡ��2
    CHOSE3_OFF;
    
    GetRealValue.f32VPV = (float32)AdcMirror.ADCRESULT15 * ADGain.f32VPV - ADChannelOffset.f32VPV;			// A1 result		
	GetRealValue.f32IPV = (float32)(AdcMirror.ADCRESULT9 - ADDefaultACOffset) * ADGain.f32IPV - ADChannelOffset.f32IPV;			// A3 result
    GetRealValue.f32GFCI = (float32)AdcMirror.ADCRESULT2 * ADGain.f32GFCI - ADChannelOffset.f32GFCI;    			//
    GetRealValue.f32Viso = 10.3f - (float32)AdcMirror.ADCRESULT12 * ADGain.f32Viso - ADChannelOffset.f32Viso + 0.0358f;    			//

    GeneralADbuffer.f32Mux1 = (float32)AdcMirror.ADCRESULT11;		                     						// A5 result
	GeneralADbuffer.f32Mux2 = (float32)AdcMirror.ADCRESULT13;

	switch(SELCNT)
	{
		case 0:								 	// SEL_B = 0 SEL_A = 0										 
		{
			GetRealValue.f32DCIR = (float32)(GeneralADbuffer.f32Mux1 - ADDefaultACOffset) * ADGain.f32DCIR - ADChannelOffset.f32DCIR;
			GeneralADbuffer.f32VInv_R = (float32)GeneralADbuffer.f32Mux2 - ADDefaultACOffset;       						// B6 result
			GetRealValue.f32VInv_R = GeneralADbuffer.f32VInv_R * ADGain.f32VInv_R - ADChannelOffset.f32VInv_R; 			// 50 * PLLConReg.Sin_AITheta;//
			SEL_A_ON;SELCNT++; break;
		}			
		case 1:								    // SEL_B = 0 SEL_A = 1
		{
			GetRealValue.f32DCIS = (float32)(GeneralADbuffer.f32Mux1 - ADDefaultACOffset) * ADGain.f32DCIS - ADChannelOffset.f32DCIS;
			GeneralADbuffer.f32VInv_S = (float32)GeneralADbuffer.f32Mux2 - ADDefaultACOffset;       						// B7 result
			GetRealValue.f32VInv_S = GeneralADbuffer.f32VInv_S * ADGain.f32VInv_S - ADChannelOffset.f32VInv_S; 			// 50 * PLLConReg.Sin_AITheta;//
			SEL_A_OFF; SEL_B_ON;  SELCNT++; break;
		}
		case 2:									// SEL_B = 1 SEL_A = 0
		{
			GetRealValue.f32DCIT = (float32)(GeneralADbuffer.f32Mux1 - ADDefaultACOffset) * ADGain.f32DCIT - ADChannelOffset.f32DCIT;
			GeneralADbuffer.f32VInv_T = (float32)GeneralADbuffer.f32Mux2 - ADDefaultACOffset;	       						// B5 result
			GetRealValue.f32VInv_T = GeneralADbuffer.f32VInv_T * ADGain.f32VInv_T - ADChannelOffset.f32VInv_T; 			// 50 * PLLConReg.Sin_AITheta;//
			SEL_B_OFF; SELCNT =0; break;
		}
		default: break;
	}

    GeneralADbuffer.f32Mux3 = (float32)AdcMirror.ADCRESULT14;

    switch(SELCNT1)
	{
		case 0:								 	// SEL_B = 0 SEL_A = 0										 
		{
			IGBTR_TempValue = (float32)(GeneralADbuffer.f32Mux3) * ADGain.f32Temp * 0.1f + IGBTR_TempValue * 0.9f;// - ADChannelOffset.f32;
			CHOSE1_ON;SELCNT1++; break;
		}			
		case 1:								    // SEL_B = 0 SEL_A = 1
		{
			IGBTS_TempValue = (float32)(GeneralADbuffer.f32Mux3) * ADGain.f32Temp * 0.1f + IGBTS_TempValue * 0.9f;// - ADChannelOffset.f32;
			CHOSE1_OFF; CHOSE2_ON;  SELCNT1++; break;
		 }
		case 2:									// SEL_B = 1 SEL_A = 0
		{
			IGBTT_TempValue = (float32)(GeneralADbuffer.f32Mux3) * ADGain.f32Temp * 0.1f + IGBTT_TempValue * 0.9f;// - ADChannelOffset.f32;
			CHOSE2_OFF; SELCNT1 =0; break;
		  }
		default: break;
	}
} // end of SVPWM_Model


/*=============================================================================*
 * FUNCTION: ADAccCalc() 
 * PURPOSE : get  AD results from the AD sample port
             ADD for one grid period
 * CALLS: 
 *============================================================================*/
void ADAccCalc(void)
{ 	
//	float32    f32temp;

	AD_Acc.i16Counter++;   //����һ�����ڵĲ��������� ��һ����������

	// ------------------------------------------------
	// --- Accumulate Value Calculation
	// --- ��������һ�������ۼӣ�Ϊ����ƽ��ֵ����Чֵ
	// ------------------------------------------------

	AD_Acc.f32VPV += GetRealValue.f32VPV;
	AD_Acc.f32IPV += GetRealValue.f32IPV;

	AD_Acc.f32DCIR += GetRealValue.f32DCIR;
	AD_Acc.f32DCIS += GetRealValue.f32DCIS;
	AD_Acc.f32DCIT += GetRealValue.f32DCIT;

	AD_Acc.f32Viso += GetRealValue.f32Viso;
	AD_Acc.f32GFCIrms += GetRealValue.f32GFCI * GetRealValue.f32GFCI; 

// ---g_StateCheck.bit.AD_initial ���������Ҫ���ڿ���������ִ�У��������к�������ֽ�����Ҫ
// ---ƽ��ֵ����ֻ�ڲ�����ǰ5s���У����в���ƫ�õ����� 
	if (1 == g_StateCheck.bit.AD_initial)  
	{
		AD_Acc.f32IGrid_Rave += GetRealValue.f32IGrid_R;
		AD_Acc.f32IGrid_Save += GetRealValue.f32IGrid_S;
		AD_Acc.f32IGrid_Tave += GetRealValue.f32IGrid_T;

		AD_Acc.f32IL_Rave += GetRealValue.f32IL_R;
		AD_Acc.f32IL_Save += GetRealValue.f32IL_S;
		AD_Acc.f32IL_Tave += GetRealValue.f32IL_T;

		AD_Acc.f32VGrid_Rave += GetRealValue.f32VGrid_R;
		AD_Acc.f32VGrid_Save += GetRealValue.f32VGrid_S;
		AD_Acc.f32VGrid_Tave += GetRealValue.f32VGrid_T;

		AD_Acc.f32VInv_Rave += GetRealValue.f32VInv_R;
		AD_Acc.f32VInv_Save += GetRealValue.f32VInv_S;
		AD_Acc.f32VInv_Tave += GetRealValue.f32VInv_T;

		AD_Acc.f32GFCIave += GetRealValue.f32GFCI;
	}

	AD_Acc.f32IGrid_Rrms += GetRealValue.f32IGrid_R * GetRealValue.f32IGrid_R;
	AD_Acc.f32IGrid_Srms += GetRealValue.f32IGrid_S * GetRealValue.f32IGrid_S;
	AD_Acc.f32IGrid_Trms += GetRealValue.f32IGrid_T * GetRealValue.f32IGrid_T;

	AD_Acc.f32IL_Rrms += GetRealValue.f32IL_R * GetRealValue.f32IL_R;
	AD_Acc.f32IL_Srms += GetRealValue.f32IL_S * GetRealValue.f32IL_S;
	AD_Acc.f32IL_Trms += GetRealValue.f32IL_T * GetRealValue.f32IL_T;

	AD_Acc.f32VGrid_Rrms += GetRealValue.f32VGrid_R * GetRealValue.f32VGrid_R;
	AD_Acc.f32VGrid_Srms += GetRealValue.f32VGrid_S * GetRealValue.f32VGrid_S;
	AD_Acc.f32VGrid_Trms += GetRealValue.f32VGrid_T * GetRealValue.f32VGrid_T;

	AD_Acc.f32VGrid_RSrms += (GetRealValue.f32VGrid_R - GetRealValue.f32VGrid_S) * (GetRealValue.f32VGrid_R - GetRealValue.f32VGrid_S);
	AD_Acc.f32VGrid_STrms += (GetRealValue.f32VGrid_S - GetRealValue.f32VGrid_T) * (GetRealValue.f32VGrid_S - GetRealValue.f32VGrid_T);
	AD_Acc.f32VGrid_TRrms += (GetRealValue.f32VGrid_T - GetRealValue.f32VGrid_R) * (GetRealValue.f32VGrid_T - GetRealValue.f32VGrid_R);

	AD_Acc.f32VInv_Rrms += GetRealValue.f32VInv_R * GetRealValue.f32VInv_R;
	AD_Acc.f32VInv_Srms += GetRealValue.f32VInv_S * GetRealValue.f32VInv_S;
	AD_Acc.f32VInv_Trms += GetRealValue.f32VInv_T * GetRealValue.f32VInv_T;

	AD_Acc.f32VInv_RSrms += (GetRealValue.f32VInv_R - GetRealValue.f32VInv_S) * (GetRealValue.f32VInv_R - GetRealValue.f32VInv_S);
	AD_Acc.f32VInv_STrms += (GetRealValue.f32VInv_S - GetRealValue.f32VInv_T) * (GetRealValue.f32VInv_S - GetRealValue.f32VInv_T);
	AD_Acc.f32VInv_TRrms += (GetRealValue.f32VInv_T - GetRealValue.f32VInv_R) * (GetRealValue.f32VInv_T - GetRealValue.f32VInv_R);

    AD_Acc.f32GridFreq += (CoffStepToFre * PLLConReg.f32Theta_Step);//50;//(SYSCLOCK_FREQ /(float32)(g_i32GridFrequencyPriodTemp))*100;

    
	// ------------------------------------------------
	// ---  Accumulate of input power , output active power, and energy calculation
	// --- �������빦�� �� ����й����� �� ���ڹ���
	// ------------------------------------------------
	AD_Acc.f32Input1Watt += GetRealValue.f32VPV * GetRealValue.f32IPV;  // ���빦��

	AD_Acc.f32OutputWatt += 0.001f * (GetRealValue.f32IGrid_R * GetRealValue.f32VGrid_R
                                    + GetRealValue.f32IGrid_S * GetRealValue.f32VGrid_S 
						            + GetRealValue.f32IGrid_T * GetRealValue.f32VGrid_T);

	AD_Acc.f32OutputVar +=0.001f * (fabs(GetRealValue.f32IGrid_R * (GetRealValue.f32VGrid_R - GetRealValue.f32VGrid_T))
						 + fabs(GetRealValue.f32IGrid_S * (GetRealValue.f32VGrid_S - GetRealValue.f32VGrid_T)));

/*	AD_Acc.f32OutputVar += 0.001f * ((fabs(GetRealValue.f32IGrid_R * GetRealValue.f32VGrid_R)
						 + fabs(GetRealValue.f32IGrid_S * GetRealValue.f32VGrid_S);
						 + fabs(GetRealValue.f32IGrid_T * GetRealValue.f32VGrid_T));
*/

	// ------------------------------------------------
	// --- ������ѹǰ������
	// ------------------------------------------------
    AD_Acc.f32DutyD_ff += VGrid_Park.f32DPs;  //������ѹǰ������
	AD_Acc.f32DutyQ_ff += VGrid_Park.f32QPs;  //������ѹǰ������
	
	// ------------------------------------------------
    // -- if����ж�һ�����ڣ� Ȼ���һ���������в������ۼӺ͸�����һ�������ı���
	// ------------------------------------------------
	if ( 1== g_StateCheck.bit.Zero_Crossing_Flag)  //һ���������ڵ�����־
	{
		g_StateCheck.bit.Zero_Crossing_Flag = 0;
		AD_Sum.i16Counter = AD_Acc.i16Counter;  //����һ�����ڲ����ĸ���

		AD_Sum.f32VPV = AD_Acc.f32VPV;
		AD_Sum.f32IPV = AD_Acc.f32IPV;

		AD_Sum.f32DCIR = AD_Acc.f32DCIR;
		AD_Sum.f32DCIS = AD_Acc.f32DCIS;
		AD_Sum.f32DCIT = AD_Acc.f32DCIT;

		AD_Sum.f32Viso = AD_Acc.f32Viso;
		AD_Sum.f32GFCIrms = AD_Acc.f32GFCIrms;

		AD_Sum.f32GridFreq = AD_Acc.f32GridFreq;

		AD_Sum.f32DutyD_ff = AD_Acc.f32DutyD_ff;//by NUAA

		// --- Pre og Grid V/I RMS caculation
		if (1 == g_StateCheck.bit.AD_initial)
		{
			AD_Sum.f32IGrid_Rave = AD_Acc.f32IGrid_Rave;
			AD_Sum.f32IGrid_Save = AD_Acc.f32IGrid_Save;
			AD_Sum.f32IGrid_Tave = AD_Acc.f32IGrid_Tave;

			AD_Sum.f32IL_Rave = AD_Acc.f32IL_Rave;
			AD_Sum.f32IL_Save = AD_Acc.f32IL_Save;
			AD_Sum.f32IL_Tave = AD_Acc.f32IL_Tave;

			AD_Sum.f32VGrid_Rave = AD_Acc.f32VGrid_Rave;
			AD_Sum.f32VGrid_Save = AD_Acc.f32VGrid_Save;
			AD_Sum.f32VGrid_Tave = AD_Acc.f32VGrid_Tave;

			AD_Sum.f32VInv_Rave = AD_Acc.f32VInv_Rave;
			AD_Sum.f32VInv_Save = AD_Acc.f32VInv_Save;
			AD_Sum.f32VInv_Tave = AD_Acc.f32VInv_Tave;

			AD_Sum.f32GFCIave = AD_Acc.f32GFCIave;
		}

		AD_Sum.f32IGrid_Rrms = AD_Acc.f32IGrid_Rrms;
		AD_Sum.f32IGrid_Srms = AD_Acc.f32IGrid_Srms;
		AD_Sum.f32IGrid_Trms = AD_Acc.f32IGrid_Trms;

        AD_Sum.f32IL_Rrms = AD_Acc.f32IL_Rrms;
		AD_Sum.f32IL_Srms = AD_Acc.f32IL_Srms;
		AD_Sum.f32IL_Trms = AD_Acc.f32IL_Trms;

		AD_Sum.f32VGrid_Rrms = AD_Acc.f32VGrid_Rrms;
		AD_Sum.f32VGrid_Srms = AD_Acc.f32VGrid_Srms;
		AD_Sum.f32VGrid_Trms = AD_Acc.f32VGrid_Trms;

		AD_Sum.f32VInv_Rrms = AD_Acc.f32VInv_Rrms;
		AD_Sum.f32VInv_Srms = AD_Acc.f32VInv_Srms;
		AD_Sum.f32VInv_Trms = AD_Acc.f32VInv_Trms;


		AD_Sum.f32VGrid_RSrms = AD_Acc.f32VGrid_RSrms;
		AD_Sum.f32VGrid_STrms = AD_Acc.f32VGrid_STrms;
		AD_Sum.f32VGrid_TRrms = AD_Acc.f32VGrid_TRrms;

		AD_Sum.f32VInv_RSrms = AD_Acc.f32VInv_RSrms;
		AD_Sum.f32VInv_STrms = AD_Acc.f32VInv_STrms;
		AD_Sum.f32VInv_TRrms = AD_Acc.f32VInv_TRrms;
  
		// ------------------------------------------------
		// ---  Accumulate of output active power, and energy calculation
		// ------------------------------------------------
		AD_Sum.f32OutputWatt = AD_Acc.f32OutputWatt;
		AD_Sum.f32OutputVar = AD_Acc.f32OutputVar;
		// ------------------------------------------------
		// ---  Accumulate of Input power calculation for MPPT
		// ------------------------------------------------
		AD_Sum.f32Input1Watt = AD_Acc.f32Input1Watt;

		//����һ�������Ժ���������ۼӱ���
		AD_Acc.i16Counter = 0;	

		AD_Acc.f32VPV = 0;
		AD_Acc.f32IPV = 0;

		AD_Acc.f32DCIR = 0;
		AD_Acc.f32DCIS = 0;
		AD_Acc.f32DCIT = 0;


		AD_Acc.f32Viso = 0;
		AD_Acc.f32GridFreq = 0;

		if (1 == g_StateCheck.bit.AD_initial)
		{
			AD_Acc.f32IGrid_Rave = 0;
			AD_Acc.f32IGrid_Save = 0;
			AD_Acc.f32IGrid_Tave = 0;

			AD_Acc.f32IL_Rave = 0;
			AD_Acc.f32IL_Save = 0;
			AD_Acc.f32IL_Tave = 0;

			AD_Acc.f32VGrid_Rave = 0;
			AD_Acc.f32VGrid_Save = 0;
			AD_Acc.f32VGrid_Tave = 0;

			AD_Acc.f32VInv_Rave = 0;
			AD_Acc.f32VInv_Save = 0;
			AD_Acc.f32VInv_Tave = 0;

			AD_Acc.f32GFCIave = 0;
		}

		AD_Acc.f32IGrid_Rrms = 0;
		AD_Acc.f32IGrid_Srms = 0;
		AD_Acc.f32IGrid_Trms = 0;

		AD_Acc.f32IL_Rrms = 0;
		AD_Acc.f32IL_Srms = 0;
		AD_Acc.f32IL_Trms = 0;

		AD_Acc.f32VGrid_Rrms = 0;
		AD_Acc.f32VGrid_Srms = 0;
		AD_Acc.f32VGrid_Trms = 0;

		AD_Acc.f32VInv_Rrms = 0;
		AD_Acc.f32VInv_Srms = 0;
		AD_Acc.f32VInv_Trms = 0;

		AD_Acc.f32VGrid_RSrms = 0;
		AD_Acc.f32VGrid_STrms = 0;
		AD_Acc.f32VGrid_TRrms = 0;

		AD_Acc.f32VInv_RSrms = 0;
		AD_Acc.f32VInv_STrms = 0;
		AD_Acc.f32VInv_TRrms = 0;

		AD_Acc.f32GFCIrms = 0;

		AD_Acc.f32OutputWatt = 0;
		AD_Acc.f32OutputVar = 0;
		AD_Acc.f32Input1Watt = 0;

		AD_Acc.f32DutyD_ff = 0;

		SEM_post(&SEM_GridPeriod);	//notify TASK EnergyCalcLowPrio to calc
	}// end of if zero crossing flag
} 

/*=============================================================================*
 * FUNCTION: TSK_GridPeriod();һ����������ִ��һ��   20ms
 * PURPOSE : 
 *		Task function. Average and RMS value is calculated in EnergyCalcLowPrio.
 *		The task sem is posted in  function EnergyAccCalc() every grid period.
 * INPUT: 
 *     Varables struct ECalc_Sum
 *
 * RETURN: 
 *     void
 *
 * CALLS: 
 *     Calculate functions and check functions
 *				Caluculate functions:
 *				GridCurrentsRMSCalc(), GridVoltsRMSCalc(); 
 *				InductorCurrentsRMSCalc(); Temperature();
 *				PVPanelInformation(); PowerCalculate();	KwHourCalculate();
 *				Check functions:
 *				StandbusVoltCal(); GridVoltCheck();	GridCurrentCheck();
 *				GridFreqCheck(); PvVoltCheck();	PvIsolationCheck();
 * CALLED BY: 
 *     BIOS 
 * 
 *============================================================================*/

void TSK_GridPeriod(void)//20ms
{

	while(1)
	{ // while loop start
		if (SEM_pend(&SEM_GridPeriod, SYS_FOREVER) == TRUE )
		{	
			if (AD_Sum.i16Counter > 0) 
			{
				f32SumCounterReci = 1 / (float32)AD_Sum.i16Counter;				
				if (1 == g_StateCheck.bit.AD_initial) //�ñ�־λ��ʾֻ�ڳ���ʼ��һ��ʱ��ִ�У�5s��������򽫱�����
				{   
                    LCurrentsAveCalc();		// ��е���ƽ��ֵ
		            GridCurrentsAveCalc();	// ��������ƽ��ֵ
					PVCurrentsCalc();		// ����������ƽ��ֵ
					GridVoltsAveCalc();		// ������ѹƽ��ֵ
					InvVoltsAveCalc();		// �˲�����C��ѹƽ��ֵ
					GFCIAveCalc();			// ©����ƽ��ֵ
					i16Cnt_SysParaTemp++;
					if (i16Cnt_SysParaTemp == 200)  // 4s��ǰ200����������
					{		
						g_StateCheck.bit.AD_initial = 0;
						ADOffsetCheck();     // �����˿�ƫ�õ����ͱ���
						i16Cnt_SysParaTemp = 0;
					}
				}

				LCurrentsRMSCalc();
				GridCurrentsRMSCalc();		// ����������Чֵ
				GridVoltsRMSCalc();			// �������ѹ��Чֵ
				InvVoltsRMSCalc();			// ������ѹ��Чֵ
				GFCIRMSCalc();				// ©������Чֵ
				
				InvLineVoltsRMSCalc();		// ����ߵ�ѹ��Чֵ	
				GridLineVoltsRMSCalc();		// �����ߵ�ѹ��Чֵ

				PVVoltsCalc();				// ��������ѹƽ��ֵ��ǰ��ϵ������
				PVCurrentsCalc();			// ����������ƽ��ֵ

				DCICalc();					// Ӳ��������е���ƽ��ֵ
				PVInsulationResCalc();		// ��Ե����ƽ��ֵ����
				PowerCalc();				// ���빦�ʣ� ����й����޹������ڹ��ʼ���
				KwHourCalc();				// �ܷ���������

				GridFrequencyCalc();		// R,S,T�������Ƶ�ʼ��㣬�������Ƶ�ʼ���
//				StandbusVoltCal();			// MPPT module
				Vgrid_D_DffCalc();			// ������ѹ����ǰ��ƽ��ֵ����

//				GridVoltCheck();			// ������500KW_Protection.c �У�������ѹֵ��������
//				GridCurrentCheck();			// ������500KW_Protection.c �У���������ֵ��������
//				GridFreqCheck();			// ������500KW_Protection.c �У�����Ƶ��ֵ��������
//				PVReversCurrCheck();		// ������500KW_Protection.c �У��������˷���ŵ籣��
//				PVVoltCheck();           	// ������500KW_Protection.c �У������ѹֵ��������
				
				IGBT_Temp();                // �Ѳ������¶�����ת����ʵ�ʵ��¶�
				TemperatureCheck();      	// ������500kw_DataAcquisition.c �У��¶ȱ���
//				GridCurrentDciCheck();   	// ������500KW_Protection.c �У�����(���)����ֱ����������
//				GFCICheck();             	// ������500KW_Protection.c �У�©������Ᵽ��                                  
               
                IGain_Slowup();         	// ������500KW_MPPT.c��, �������������� 
				SEM_post(&MpptReady);
			}     
		}                   
	}//end of while (1) 
}//end of EnergyCalcLowPrio task function



/*=============================================================================*
 * FUNCTION: GridCurrentsRMSCalc()
 * PURPOSE : 
 *		Calculate grid current RMS value,caculated value is ten times of real 
 *		value, for example, 502 means 50.2 Ampere.
 * INPUT: 
 *     ECalc_Sum.i32GridCurrentUSqrt,ECalc_Sum.i32GridCurrentVSqrt,
 *		ECalc_Sum.i32GridCurrentWSqrt
 *
 * RETURN: 
 *     void
 *
 * CALLS: 
 *     void
 *
 * CALLED BY: 
 *     EnergyCalcLowPrio()
 * 
 *============================================================================*/
void GridCurrentsRMSCalc(void)
{
	float32	f32temp1, f32temp2, f32temp3;

	f32temp1 = sqrt(AD_Sum.f32IGrid_Rrms * f32SumCounterReci);	//
	f32temp2 = sqrt(AD_Sum.f32IGrid_Srms * f32SumCounterReci);	//
	f32temp3 = sqrt(AD_Sum.f32IGrid_Trms * f32SumCounterReci);	//

	//Current unit: 0.1A
	//inertia filter corner freq: 12Hz 
	f32temp1 = (Calc_Result.f32IGrid_Rrms * 0.4f) + (f32temp1 * 0.6f);	        
	f32temp2 = (Calc_Result.f32IGrid_Srms * 0.4f) + (f32temp2 * 0.6f);
	f32temp3 = (Calc_Result.f32IGrid_Trms * 0.4f) + (f32temp3 * 0.6f);

	Calc_Result.f32IGrid_Rrms = f32temp1;
	Calc_Result.f32IGrid_Srms = f32temp2;
	Calc_Result.f32IGrid_Trms = f32temp3;
}


/*=============================================================================*
 * FUNCTION: LCurrentsRMSCalc()
 * PURPOSE : 
 *		Calculate grid current RMS value,caculated value is ten times of real 
 *		value, for example, 502 means 50.2 Ampere.
 * INPUT: 
 *     ECalc_Sum.i32GridCurrentUSqrt,ECalc_Sum.i32GridCurrentVSqrt,
 *		ECalc_Sum.i32GridCurrentWSqrt
 *
 * RETURN: 
 *     void
 *
 * CALLS: 
 *     void
 *
 * CALLED BY: 
 *     EnergyCalcLowPrio()
 * 
 *============================================================================*/
void LCurrentsRMSCalc(void)
{
	float32	f32temp1, f32temp2, f32temp3;

	f32temp1 = sqrt(AD_Sum.f32IL_Rrms * f32SumCounterReci);	//
	f32temp2 = sqrt(AD_Sum.f32IL_Srms * f32SumCounterReci);	//
	f32temp3 = sqrt(AD_Sum.f32IL_Trms * f32SumCounterReci);	//

	//Current unit: 0.1A
	//inertia filter corner freq: 12Hz 
	f32temp1 = (Calc_Result.f32IL_Rrms * 0.4f) + (f32temp1 * 0.6f);	        
	f32temp2 = (Calc_Result.f32IL_Srms * 0.4f) + (f32temp2 * 0.6f);
	f32temp3 = (Calc_Result.f32IL_Trms * 0.4f) + (f32temp3 * 0.6f);

	Calc_Result.f32IL_Rrms = f32temp1;
	Calc_Result.f32IL_Srms = f32temp2;
	Calc_Result.f32IL_Trms = f32temp3;
}


void GridVoltsRMSCalc(void)
{
	float32	f32temp1, f32temp2, f32temp3;

	f32temp1 = sqrt(AD_Sum.f32VGrid_Rrms * f32SumCounterReci);	//
	f32temp2 = sqrt(AD_Sum.f32VGrid_Srms * f32SumCounterReci);	//
	f32temp3 = sqrt(AD_Sum.f32VGrid_Trms * f32SumCounterReci);	//

	f32temp1 = (Calc_Result.f32VGrid_Rrms * 0.4f) + (f32temp1 * 0.6f);	        
	f32temp2 = (Calc_Result.f32VGrid_Srms * 0.4f) + (f32temp2 * 0.6f);
	f32temp3 = (Calc_Result.f32VGrid_Trms * 0.4f) + (f32temp3 * 0.6f);

	Calc_Result.f32VGrid_Rrms = f32temp1;
	Calc_Result.f32VGrid_Srms = f32temp2;
	Calc_Result.f32VGrid_Trms = f32temp3;


}
void GridLineVoltsRMSCalc(void)
{
	float32	f32temp1, f32temp2, f32temp3;

	f32temp1 = sqrt(AD_Sum.f32VGrid_RSrms * f32SumCounterReci);			//
	f32temp2 = sqrt(AD_Sum.f32VGrid_STrms * f32SumCounterReci);			//
	f32temp3 = sqrt(AD_Sum.f32VGrid_TRrms * f32SumCounterReci);			//

	f32temp1 = (Calc_Result.f32VGrid_RSrms * 0.4f) + (f32temp1 * 0.6f);	        
	f32temp2 = (Calc_Result.f32VGrid_STrms * 0.4f) + (f32temp2 * 0.6f);
	f32temp3 = (Calc_Result.f32VGrid_TRrms * 0.4f) + (f32temp3 * 0.6f);

	Calc_Result.f32VGrid_RSrms = f32temp1;
	Calc_Result.f32VGrid_STrms = f32temp2;
	Calc_Result.f32VGrid_TRrms = f32temp3;

}
void InvVoltsRMSCalc(void)
{
	float32	f32temp1, f32temp2, f32temp3;

	f32temp1 = sqrt(AD_Sum.f32VInv_Rrms * f32SumCounterReci);	//
	f32temp2 = sqrt(AD_Sum.f32VInv_Srms * f32SumCounterReci);	//
	f32temp3 = sqrt(AD_Sum.f32VInv_Trms * f32SumCounterReci);	//

	f32temp1 = (Calc_Result.f32VInv_Rrms * 0.4f) + (f32temp1 * 0.6f);	        
	f32temp2 = (Calc_Result.f32VInv_Srms * 0.4f) + (f32temp2 * 0.6f);
	f32temp3 = (Calc_Result.f32VInv_Trms * 0.4f) + (f32temp3 * 0.6f);

	Calc_Result.f32VInv_Rrms = f32temp1;
	Calc_Result.f32VInv_Srms = f32temp2;
	Calc_Result.f32VInv_Trms = f32temp3;

}
void InvLineVoltsRMSCalc(void)
{
	float32	f32temp1, f32temp2, f32temp3;

	f32temp1 = sqrt(AD_Sum.f32VInv_RSrms * f32SumCounterReci);	//
	f32temp2 = sqrt(AD_Sum.f32VInv_STrms * f32SumCounterReci);	//
	f32temp3 = sqrt(AD_Sum.f32VInv_TRrms * f32SumCounterReci);	//

	f32temp1 = (Calc_Result.f32VInv_RSrms * 0.4f) + (f32temp1 * 0.6f);	        
	f32temp2 = (Calc_Result.f32VInv_STrms * 0.4f) + (f32temp2 * 0.6f);
	f32temp3 = (Calc_Result.f32VInv_TRrms * 0.4f) + (f32temp3 * 0.6f);

	Calc_Result.f32VInv_RSrms = f32temp1;
	Calc_Result.f32VInv_STrms = f32temp2;
	Calc_Result.f32VInv_TRrms = f32temp3;

}
void GFCIRMSCalc(void)
{
    float32	f32temp1;

	f32temp1 = sqrt(AD_Sum.f32GFCIrms * f32SumCounterReci);	
	Calc_Result.f32GFCIrms = f32temp1;

}

void LCurrentsAveCalc(void)
{
	float32	f32temp1, f32temp2, f32temp3;

	f32temp1 = (AD_Sum.f32IL_Rave  * f32SumCounterReci);	
	f32temp2 = (AD_Sum.f32IL_Save  * f32SumCounterReci);	
	f32temp3 = (AD_Sum.f32IL_Tave  * f32SumCounterReci);	

	f32temp1 = (Calc_Result.f32IL_Rave * 0.4f) + (f32temp1 * 0.6f);	        
	f32temp2 = (Calc_Result.f32IL_Save * 0.4f) + (f32temp2 * 0.6f);
	f32temp3 = (Calc_Result.f32IL_Tave * 0.4f) + (f32temp3 * 0.6f);

	Calc_Result.f32IL_Rave = f32temp1;
	Calc_Result.f32IL_Save = f32temp2;
	Calc_Result.f32IL_Tave = f32temp3;	
}
void GridCurrentsAveCalc(void)
{
    float32	f32temp1, f32temp2, f32temp3;

	f32temp1 = (AD_Sum.f32IGrid_Rave  * f32SumCounterReci);	
	f32temp2 = (AD_Sum.f32IGrid_Save  * f32SumCounterReci);	
	f32temp3 = (AD_Sum.f32IGrid_Tave  * f32SumCounterReci);	

	f32temp1 = (Calc_Result.f32IGrid_Rave * 0.4f) + (f32temp1 * 0.6f);	        
	f32temp2 = (Calc_Result.f32IGrid_Save * 0.4f) + (f32temp2 * 0.6f);
	f32temp3 = (Calc_Result.f32IGrid_Tave * 0.4f) + (f32temp3 * 0.6f);

	Calc_Result.f32IGrid_Rave = f32temp1;
	Calc_Result.f32IGrid_Save = f32temp2;
	Calc_Result.f32IGrid_Tave = f32temp3;		

}
void GridVoltsAveCalc(void)
{
    float32	f32temp1, f32temp2, f32temp3;

	f32temp1 = (AD_Sum.f32VGrid_Rave  * f32SumCounterReci);	
	f32temp2 = (AD_Sum.f32VGrid_Save  * f32SumCounterReci);	
	f32temp3 = (AD_Sum.f32VGrid_Tave  * f32SumCounterReci);	

	f32temp1 = (Calc_Result.f32VGrid_Rave * 0.4f) + (f32temp1 * 0.6f);	        
	f32temp2 = (Calc_Result.f32VGrid_Save * 0.4f) + (f32temp2 * 0.6f);
	f32temp3 = (Calc_Result.f32VGrid_Tave * 0.4f) + (f32temp3 * 0.6f);

	Calc_Result.f32VGrid_Rave = f32temp1;
	Calc_Result.f32VGrid_Save = f32temp2;
	Calc_Result.f32VGrid_Tave = f32temp3;		

}
void InvVoltsAveCalc(void)
{
    float32	f32temp1, f32temp2, f32temp3;

	f32temp1 = (AD_Sum.f32VInv_Rave  * f32SumCounterReci);	
	f32temp2 = (AD_Sum.f32VInv_Save  * f32SumCounterReci);	
	f32temp3 = (AD_Sum.f32VInv_Tave  * f32SumCounterReci);	

	f32temp1 = (Calc_Result.f32VInv_Rave * 0.4f) + (f32temp1 * 0.6f);	        
	f32temp2 = (Calc_Result.f32VInv_Save * 0.4f) + (f32temp2 * 0.6f);
	f32temp3 = (Calc_Result.f32VInv_Tave * 0.4f) + (f32temp3 * 0.6f);

	Calc_Result.f32VInv_Rave = f32temp1;
	Calc_Result.f32VInv_Save = f32temp2;
	Calc_Result.f32VInv_Tave = f32temp3;		

}
void GFCIAveCalc(void)
{
    float32	f32temp1;

	f32temp1 = (AD_Sum.f32GFCIave * f32SumCounterReci);	
	f32temp1 = (Calc_Result.f32GFCIave * 0.2f) + (f32temp1 * 0.8f);	        
	Calc_Result.f32GFCIave = f32temp1;//f32temp1;
}

void PVVoltsCalc(void)
{
    float32	f32temp1;
	f32temp1 = (AD_Sum.f32VPV * f32SumCounterReci);		
	f32temp1 = (Calc_Result.f32VPV * 0.4f) + (f32temp1 * 0.6f);	        
	Calc_Result.f32VPV = f32temp1;

	if(Calc_Result.f32VPV > 30)
	{
		f32temp1 = 1.5 / Calc_Result.f32VPV;
		Calc_Result.Coff_Dforward = f32temp1;
	}
	else
		Calc_Result.Coff_Dforward = 0;
}

void PVCurrentsCalc(void)
{
    float32	f32temp1;

	f32temp1 = (AD_Sum.f32IPV * f32SumCounterReci);		
	f32temp1 = (Calc_Result.f32IPV * 0.4f) + (f32temp1 * 0.6f);	        
	Calc_Result.f32IPV = f32temp1;

}


void DCICalc(void)
{
    float32	f32temp1, f32temp2, f32temp3;

	f32temp1 = (AD_Sum.f32DCIR  * f32SumCounterReci);	
	f32temp2 = (AD_Sum.f32DCIS  * f32SumCounterReci);	
	f32temp3 = (AD_Sum.f32DCIT  * f32SumCounterReci);

	f32temp1 = (Calc_Result.f32DCIR * 0.4f) + (f32temp1 * 0.6f);	        
	f32temp2 = (Calc_Result.f32DCIS * 0.4f) + (f32temp2 * 0.6f);
	f32temp3 = (Calc_Result.f32DCIT * 0.4f) + (f32temp3 * 0.6f);
		
	Calc_Result.f32DCIR = f32temp1;
	Calc_Result.f32DCIS = f32temp2;
	Calc_Result.f32DCIT = f32temp3;		
}

void PVInsulationResCalc(void)
{
    float32	f32temp1;

	f32temp1 = AD_Sum.f32Viso * f32SumCounterReci;		
	f32temp1 = Calc_Result.f32Viso * 0.4f + f32temp1 * 0.6f;	        		
	Calc_Result.f32Viso = f32temp1;
}

void PowerCalc(void)
{
    float32	f32temp1, f32temp3, f32temp4; //f32temp2, f32temp3, f32temp4;

	f32temp1 = AD_Sum.f32Input1Watt  * f32SumCounterReci;
//	f32temp2 = AD_Sum.f32Input2Watt  * f32SumCounterReci;
	f32temp3 = AD_Sum.f32OutputWatt  * f32SumCounterReci;
	f32temp4 = AD_Sum.f32OutputVar  * f32SumCounterReci;

	f32temp1 = (Calc_Result.f32Input1Watt * 0.5f) + (f32temp1 * 0.5f);	        
//	f32temp2 = (Calc_Result.f32Input2Watt * 0.5f) + (f32temp2 * 0.5f);
	f32temp3 = (Calc_Result.f32OutputWatt * 0.5f) + (f32temp3 * 0.5f);
	f32temp4 = (Calc_Result.f32OutputVar * 0.5f) + (f32temp4 * 0.5f);

	Calc_Result.f32Input1Watt = f32temp1;
//	Calc_Result.f32Input2Watt = f32temp2;		
	Calc_Result.f32OutputWatt = f32temp3;
	Calc_Result.f32OutputVar = f32temp4;

//	f32temp1 = sqrt(Calc_Result.f32OutputWatt * Calc_Result.f32OutputWatt + Calc_Result.f32OutputVar * Calc_Result.f32OutputVar);
	Calc_Result.f32OutputVA = Calc_Result.f32OutputVar;

	f32temp1 = sqrt(Calc_Result.f32OutputVar * Calc_Result.f32OutputVar - Calc_Result.f32OutputWatt * Calc_Result.f32OutputWatt);
	Calc_Result.f32OutputQ = f32temp1;

	Calc_Result.f32OutputPF = Calc_Result.f32OutputWatt / Calc_Result.f32OutputVar;
}	

void KwHourCalc(void)
{
	float32	f32temp1;

	f32temp1 = AD_Sum.f32OutputWatt * EnergyMeasureGain;		//�������������ۼӣ��ա��¡��ꡢ�ܣ�				
	Calc_Result.f32HistoryOutputEnergy_Day += f32temp1;			//ÿ���ۼ�ֵ��Ϊ20ms�ڵķ�����
	Calc_Result.f32HistoryOutputEnergy_Month += f32temp1;		//��λkW*h
	Calc_Result.f32HistoryOutputEnergy_Year += f32temp1;
	Calc_Result.f32HistoryOutputEnergy += f32temp1;

	if(g_StateCheck.bit.TimeRead_flag == 1)						//��ʾ�ѴӼ�ʱоƬ��ȡ��ʱ��
	{
		g_StateCheck.bit.TimeRead_flag = 0;
		if(g_Time.year != g_Time.year_old)						//�����������
		{
			Calc_Result.f32HistoryOutputEnergy_Day = 0;
			Calc_Result.f32HistoryOutputEnergy_Month = 0;
			Calc_Result.f32HistoryOutputEnergy_Year = 0;
		}
		else if(g_Time.month != g_Time.month_old)				//�����������
		{
			Calc_Result.f32HistoryOutputEnergy_Day = 0;
			Calc_Result.f32HistoryOutputEnergy_Month = 0;
		}
		else if(g_Time.day != g_Time.day_old)					//�����������
			Calc_Result.f32HistoryOutputEnergy_Day = 0;
	}

	g_Time.day_old = g_Time.day;								//��ֵ��"��һ��"��ʱ�������������һ�ε��ж�
	g_Time.month_old = g_Time.month;
	g_Time.year_old = g_Time.year;

}//end of function 

void GridFrequencyCalc(void)
{
//------Cap
/*	static Uint16 VgridRZero_Cnt_Old = 0;
	static Uint16 VgridSZero_Cnt_Old = 0;
	static Uint16 VgridTZero_Cnt_Old = 0;
*/	static float32 f32VgridRFreq_Old = 0;
	static float32 f32VgridRFreq_Old1 = 0;
	static float32 f32VgridSFreq_Old = 0;
	static float32 f32VgridSFreq_Old1 = 0;
	static float32 f32VgridTFreq_Old = 0;
	static float32 f32VgridTFreq_Old1 = 0;

//PLL frequency calculation
    static float32	f32temp1 = 0;
    static float32	f32GridFreq_k_1 = 50;

	f32temp1 = (AD_Sum.f32GridFreq  * f32SumCounterReci);   // 1/120
	PLLConReg.f32Fre_Delta_k = 0.2 * PLLConReg.f32Fre_Delta_k + 0.8 * (f32temp1 - f32GridFreq_k_1);//���Ӧ����0

	if(PLLConReg.f32Fre_Delta_k > 1)
		PLLConReg.f32Fre_Delta_k = 1;
	else if(PLLConReg.f32Fre_Delta_k < -1)
		PLLConReg.f32Fre_Delta_k = -1;
	
	f32GridFreq_k_1 = f32temp1;
	f32temp1 = (Calc_Result.f32GridFreq * 0.6f) + (f32temp1 * 0.4f);
	Calc_Result.f32GridFreq = f32temp1;
	f32temp1 = 0.000166667f * Calc_Result.f32GridFreq * Value_2Pi;
	Calc_Result.f32GridFreqClarke1 = f32temp1 - 2.0f;
	Calc_Result.f32GridFreqClarke2 = 1.0f / (2.0f + f32temp1);


//Cap frequency calculation
	f32VgridRFreq_Old1 = f32VgridRFreq_Old ;
	f32VgridRFreq_Old = Calc_Result.f32GridRFreq;
	Calc_Result.f32GridRFreq = 150000000.0 / (float32)VgridRFreqCnt;
	Calc_Result.f32GridRFreq = Calc_Result.f32GridRFreq*0.2f+f32VgridRFreq_Old*0.4f+f32VgridRFreq_Old1*0.4f;

	f32VgridSFreq_Old1 = f32VgridSFreq_Old;
	f32VgridSFreq_Old = Calc_Result.f32GridSFreq;
	Calc_Result.f32GridSFreq = 150000000.0 / (float32)VgridSFreqCnt;
	Calc_Result.f32GridSFreq = Calc_Result.f32GridSFreq*0.2f+f32VgridSFreq_Old*0.4f+f32VgridSFreq_Old1*0.4f;

	f32VgridTFreq_Old1 = f32VgridTFreq_Old;
	f32VgridTFreq_Old = Calc_Result.f32GridTFreq;
	Calc_Result.f32GridTFreq = 150000000.0 / (float32)VgridTFreqCnt;
	Calc_Result.f32GridTFreq = Calc_Result.f32GridTFreq*0.2f+f32VgridTFreq_Old*0.4f+f32VgridTFreq_Old1*0.4f;

/*	if(VgridRZero_Cnt == VgridRZero_Cnt_Old)
	{
		Calc_Result.f32GridRFreq = 0;
	}
	VgridRZero_Cnt_Old = VgridRZero_Cnt;
	if(VgridSZero_Cnt == VgridSZero_Cnt_Old)
	{
		Calc_Result.f32GridSFreq = 0;
	}
	VgridSZero_Cnt_Old = VgridSZero_Cnt;
	if(VgridTZero_Cnt == VgridTZero_Cnt_Old)
	{
		Calc_Result.f32GridTFreq = 0;
	}
	VgridTZero_Cnt_Old = VgridTZero_Cnt;
*/
}


//
void IGBT_Temp(void)
{
	static int i = 0;
	static int j = 0;
	static int k = 0;	
	static float32 IGBT_TempValue_R = 0;  
	static float32 IGBT_TempValue_S = 0;  
	static float32 IGBT_TempValue_T = 0; 

	for(i=0;i<71;i++)
	{
		if((IGBT_TempValue_R < IGBTTempValue[i]) && (IGBT_TempValue_R >= IGBTTempValue[i+1]))
		{
			GetRealValue.f32Temp_R = 2 * i;
			IGBT_TempValue_R =  IGBTR_TempValue;
			break;
		}
	}
	for(j=0;j<71;j++)
	{
		if((IGBT_TempValue_S < IGBTTempValue[j]) && (IGBT_TempValue_S >= IGBTTempValue[j+1]))
		{
			GetRealValue.f32Temp_S = 2 * j;
			IGBT_TempValue_S =  IGBTS_TempValue;
			break;
		}
	}
	for(k=0;k<71;k++)
	{
		if((IGBT_TempValue_T < IGBTTempValue[k]) && (IGBT_TempValue_T >= IGBTTempValue[k+1]))
		{
			GetRealValue.f32Temp_T = 2 * k;
			IGBT_TempValue_T =  IGBTT_TempValue;
			break;
		}
	}	
}

//
void TemperatureCheck(void)
{
	if( GetRealValue.f32Temp_R > 105) 
	 	g_SysFaultMessage.bit.IGBT1TempFault = 1;
	else if( GetRealValue.f32Temp_R < 50) 
		g_SysFaultMessage.bit.IGBT1TempFault = 0;
	
	if( GetRealValue.f32Temp_S > 105) 
		g_SysFaultMessage.bit.IGBT2TempFault = 1;
	else if (GetRealValue.f32Temp_S < 50)
		g_SysFaultMessage.bit.IGBT2TempFault = 0;

	if( GetRealValue.f32Temp_T > 105) 
		g_SysFaultMessage.bit.IGBT3TempFault = 1;
	else if (GetRealValue.f32Temp_S < 50)
		g_SysFaultMessage.bit.IGBT3TempFault = 0;

	if((GetRealValue.f32Temp_R < 40)|(GetRealValue.f32Temp_S < 40)|(GetRealValue.f32Temp_T < 40))
	{
		FAN_MIDDLE_OFF();
		FAN_HIGH_OFF();
		FAN_LOW_ON();
	}
	else if((GetRealValue.f32Temp_R < 55)|(GetRealValue.f32Temp_S < 55)|(GetRealValue.f32Temp_T < 55))
	{
		FAN_LOW_OFF();
		FAN_HIGH_OFF();
		FAN_MIDDLE_ON();
	}
	else
	{
		FAN_LOW_OFF();
		FAN_MIDDLE_OFF();
		FAN_HIGH_ON();
	}

}

/************************NUAA***********************************
* FUNCION : void Vgrid_D_DffCalc(void)
* PURPOSE :  caculated  standbus voltage from grid voltage 
*                   
* INPUT :
*        void
* RETURN :
*        void
* CALLS:
*        void
*
* CALLED BY:  void Vgrid_D_DffCalc(void)
* 
**********************************************************************/
void Vgrid_D_DffCalc(void)
{
    float32	f32temp1; 
    float32 f32temp2;

	f32temp1 = (AD_Acc.f32DutyD_ff * f32SumCounterReci);	
	f32temp2 = (AD_Acc.f32DutyQ_ff * f32SumCounterReci);	

	f32temp1 = (AD_Acc.f32DutyD_ff * 0.4f) + (f32temp1 * 0.6f);	        
	f32temp2 = (AD_Acc.f32DutyQ_ff * 0.4f) + (f32temp2 * 0.6f);

	Calc_Result.f32DutyD_ff = f32temp1;
	Calc_Result.f32DutyQ_ff = f32temp2;
}
//************************NUAA***********************************

// over ouput current protection signal, Ӳ����������
void HwInvOCPDetection (void)
{   
  	static Uint16 s_u16Cnt_OCP_AC_Level = 0 ; 
  	static Uint16 s_u16BackCnt_OCP_AC_Level = 0 ; 
  	if(g_IO_State.bit.AC_OCP == 1)
  	{						 
    	s_u16Cnt_OCP_AC_Level ++;
    	s_u16BackCnt_OCP_AC_Level = 0;
    	if(s_u16Cnt_OCP_AC_Level >= 1)	  // 20ms?
    	{
      		s_u16Cnt_OCP_AC_Level = 0;
      		g_StateCheck.bit.AC_OCP = 1;	///  
    	}
  	}
  	else
  	{
    	s_u16BackCnt_OCP_AC_Level++;
    	s_u16Cnt_OCP_AC_Level = 0;
    	if(s_u16BackCnt_OCP_AC_Level > 2000)
    	{
      		s_u16BackCnt_OCP_AC_Level = 0;
      		g_StateCheck.bit.AC_OCP = 0;      
    	}
  	}
}


// PV Over Voltage Detection��Ӳ������
void HwPVOVPDetection (void)
{   
  	static Uint16 s_u16Cnt_Bus_OVP_Level = 0 ; 
  	static Uint16 s_u16Cnt_Bus_OVP_Fault_Back = 0;

  	if(0 == g_StateCheck.bit.Bus_OVP)
  	{
    	if(0 == Bus_OVP_LEVEL)     //Bus_OVP_LEVEL=0 represents OVP error
    	{
      		s_u16Cnt_Bus_OVP_Level ++;
      		if(s_u16Cnt_Bus_OVP_Level > 2)	  // 2*2ms
      		{
        		s_u16Cnt_Bus_OVP_Level = 0;
        		g_StateCheck.bit.Bus_OVP = 1;    // bus_ovp ���û��ͨ��	 
      		}
    	}
    	else
      		s_u16Cnt_Bus_OVP_Level = 0;
  	}
  	else
  	{
    	if(1 ==  Bus_OVP_LEVEL)
    	{
      		s_u16Cnt_Bus_OVP_Fault_Back ++;
      		if(s_u16Cnt_Bus_OVP_Fault_Back > 500)	  // 500*2ms
      		{
        		s_u16Cnt_Bus_OVP_Fault_Back = 0;
        		g_StateCheck.bit.Bus_OVP = 0;	  
      		}
    	}
    	else
      		s_u16Cnt_Bus_OVP_Fault_Back = 0;
  	}
}


//--- end of file -----------------------------------------------------

