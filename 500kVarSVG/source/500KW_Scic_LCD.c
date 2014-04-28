/*=============================================================================*
 *         Copyright(c)
 *                          ALL RIGHTS RESERVED
 *
 *  FILENAME : 17KW_SCIa.c 
 *
 *  PURPOSE  : SCIa for LCD.
 *  
 *  HISTORY  :
 *    DATE            VERSION        AUTHOR            NOTE
 *   
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
#include "500KW_MAIN_HEADER.h"

//#pragma CODE_SECTION(Scia_Parsing, "sciaRamFun")

struct Time			 g_Time = {13,3,13,16,39,00};
struct Data_Operating g_Data_Operating = {0};
struct Ctrl_Para 	 g_Ctrl_Para = {0.0005f,0.000015f,50.5f,48.0f,25,300.0,50,184.0,226.0,50,50,2.35f,0.001f,0,0,0};
extern PARK VGrid_Park; 
static void Scic_Parsing(void);


void TSK_SCI_C(void)
{
	while(1)
	{
		if(SEM_pend(&SEM_SCICComm, SYS_FOREVER) == 1)  //SCI通信100ms进一次
		{
			Scic_Parsing();
		}
	}
}


//extern Uint16 CheckSum(Uint8* pBuf, Uint16 len);

Uint8 calcrc_byte(Uint8 abyte)
{
	Uint8 i,crc;
	crc = 0;
	for(i=0;i<8;i++)
	{
		if(((crc^abyte)&0x01))
		{
			crc ^= 0x18;
			crc >>= 1;
			crc |= 0x80;
		}
		else
			crc >>= 1;
		abyte >>= 1;
	}
	return crc;
}

Uint8 CRCNumCalc(Uint8* pBuf, Uint8 len)
{
	Uint8 crc8;
	crc8 = 0;

	while(len--)
	{
		crc8 = calcrc_byte(crc8^*pBuf++);
	}

	return crc8;
}

/*=============================================================================*
 * FUNCTION: GetValue_DefaultRefresh()
 *
 * PURPOSE : Collect refresh data for default, and each data is converted
 *			 into several 8-bit bytes, which are transfered via SCI-A.
 *
 * INPUT:    Uint8* p_buf, the data bytes' buffer for SCI-A transfer.
 *           Note p_buf should be filled from position "p_buf+1".
 *
 *           Globals Calc_Result and g_Sys_Current_State are read.
 *
 * RETURN:   void
 *
 * CALLS:    void
 *
 * CALLED BY: 
 *      Scia_Parsing()
 *============================================================================*/
void GetValue_DefaultRefresh(Uint8* p_buf)
{

}
/*=============================================================================*
 * FUNCTION: GetValue_StandardRefresh()
 *
 * PURPOSE : Collect refresh data for standard screen, and each data is converted
 *			 into several 8-bit bytes, which are transfered via SCI-A.
 *
 * INPUT:    Uint8* p_buf, the data bytes' buffer for SCI-A transfer.
 *           Note p_buf should be filled from position "p_buf+1".
 *
 *           Globals Calc_Result and g_Sys_Current_State are read.
 *
 * RETURN:   void
 *
 * CALLS:    void
 *
 * CALLED BY: 
 *      Scia_Parsing()
 *============================================================================*/
void GetValue_StandardRefresh(Uint8* p_buf)
{

}

/*=============================================================================*
 * FUNCTION: GetValue_InstantRefresh()
 *
 * PURPOSE : Collect refresh data for instanterous screen, and each data is converted
 *			 into several 8-bit bytes, which are transfered via SCI-A.
 *
 * INPUT:    Uint8* p_buf, the data bytes' buffer for SCI-A transfer.
 *           Note p_buf should be filled from position "p_buf+1".
 *
 *           Globals Calc_Result and g_Sys_Current_State are read.
 *
 * RETURN:   void
 *
 * CALLS:    void
 *
 * CALLED BY: 
 *      Scia_Parsing()
 *============================================================================*/
void GetValue_InstantRefresh(Uint8* p_buf)
{

}

Uint16 SetValue_Time(Uint8* p_buf)
{
	if(*(p_buf+2) > 99)							//return error if year >99
		return 0;

	if((*(p_buf+3) > 12) || (*(p_buf+3) < 1))	//return error if month >12 or second <1
		return 0;

	if((*(p_buf+4) > 31) || (*(p_buf+4) < 1))	//return error if day >31 or second <1
		return 0;

	if(*(p_buf+5) > 23)							//return error if hour >23
		return 0;

	if(*(p_buf+6) > 59)							//return error if minute >59
		return 0;

	if(*(p_buf+7) > 59)							//return error if second >59
		return 0;

	g_Time.year 	= *(p_buf+2);
	g_Time.month 	= *(p_buf+3);
	g_Time.day 		= *(p_buf+4);
	g_Time.hour 	= *(p_buf+5);
	g_Time.minute 	= *(p_buf+6);
	g_Time.second 	= *(p_buf+7);
	g_StateCheck.bit.TimeSet_flag = 1;

	return 1;
}

Uint16 get_Hi_8bits(Uint8 u8_buf)
{
	Uint16 u16tmp;
	u16tmp = (Uint16)u8_buf;
	u16tmp <<= 8;
	u16tmp &= 0xFF00;
	return u16tmp;
}

Uint16 get_Lo_8bits(Uint8 u8_buf)
{
	Uint16 u16tmp;
	u16tmp = (Uint16)u8_buf;
	u16tmp &= 0x00FF;
	return u16tmp;
}


Uint16 SetValue_Parameter_set(Uint8* p_buf)
{
	Uint16 u16tmp;
	float f32tmp;

	u16tmp = get_Hi_8bits(*(p_buf+2)) + get_Lo_8bits(*(p_buf+3));
	f32tmp = (float)u16tmp / 1000000;
	if((f32tmp > 0.000001) && (f32tmp < 0.1))
		g_Ctrl_Para.Curr_Kp = f32tmp;
	else
		return 0;

	u16tmp = get_Hi_8bits(*(p_buf+4)) + get_Lo_8bits(*(p_buf+5));
	f32tmp = (float)u16tmp / 1000000;
	if((f32tmp > 0.000001) && (f32tmp < 0.001))
		g_Ctrl_Para.Curr_Ki = f32tmp;
	else
		return 0;

	u16tmp = get_Hi_8bits(*(p_buf+6)) + get_Lo_8bits(*(p_buf+7));
	f32tmp = (float)u16tmp / 1000;
	if((f32tmp > 50.0f) && (f32tmp < 52.0f))
//		g_Ctrl_Para.Freq_Lim_Hi = f32tmp;
		SafetyReg.f32FreGrid_Hi2Limit = f32tmp;

/*
	f32tmp = (float)u16tmp / 10;
	if((f32tmp > 0.0) && (f32tmp < 250.0) && (fabs(f32tmp - Calc_Result.f32OutputWatt) > 0.9))
	{
		g_Ctrl_Para.Output_Watt_set = f32tmp;
		g_StateCheck.bit.OutputWattCtrl_flag = 1;
	}
*/
/*	f32tmp = (float)u16tmp / 10;
	if((f32tmp > 0.0) && (f32tmp < 13.0f) && (fabs(f32tmp - Calc_Result.f32OutputQ) > 0.9f))
	{
		g_Ctrl_Para.Output_Var_set = f32tmp;
		g_StateCheck.bit.OutputVarCtrl_flag = 1;
		g_StateCheck.bit.OutputPF_Ctrl_flag = 0;
	}
*/
/*	f32tmp = (float)u16tmp / 10;
	if((f32tmp > 0.5) && (f32tmp < 1.0) && (fabs(f32tmp - Calc_Result.f32OutputVar) > 0.005))
	{
		g_Ctrl_Para.Output_PF_set = f32tmp;
		g_StateCheck.bit.OutputPF_Ctrl_flag = 1;
		g_StateCheck.bit.OutputVarCtrl_flag = 0;
	}
*/
	else
		return 0;

	u16tmp = get_Hi_8bits(*(p_buf+8)) + get_Lo_8bits(*(p_buf+9));
	f32tmp = (float)u16tmp / 1000;
	if((f32tmp > 45.0) && (f32tmp < 50.0))
//		g_Ctrl_Para.Freq_Lim_Lo = f32tmp;
		SafetyReg.f32FreGrid_Low2Limit = f32tmp;
	else
		return 0;

	u16tmp = get_Hi_8bits(*(p_buf+10)) + get_Lo_8bits(*(p_buf+11));
	f32tmp = (float)u16tmp / 10;
	if((f32tmp > 0.0) && (f32tmp < 1000.0))
		g_Ctrl_Para.Temp_Lim = f32tmp;
	else
		return 0;

	u16tmp = get_Hi_8bits(*(p_buf+12)) + get_Lo_8bits(*(p_buf+13));
	f32tmp = (float)u16tmp / 10;
	if((f32tmp > 10) && (f32tmp < 800.0))
	{
		g_Ctrl_Para.Curr_Lim = f32tmp;
		BusCon_Reg.f32Id_Ref = g_Ctrl_Para.Curr_Lim;
	}
	else
		return 0;

	u16tmp = get_Hi_8bits(*(p_buf+14)) + get_Lo_8bits(*(p_buf+15));
	if((u16tmp > 0) && (u16tmp < 100))
		g_Ctrl_Para.Time_of_Over_Curr = u16tmp;
	else
		return 0;

	u16tmp = get_Hi_8bits(*(p_buf+16)) + get_Lo_8bits(*(p_buf+17));
	f32tmp = (float)u16tmp / 10;
	if((f32tmp > 150.0) && (f32tmp < 200.0))
//		g_Ctrl_Para.Volt_Kp = f32tmp;
//		g_Ctrl_Para.Volt_Lim_1 = f32tmp;
		SafetyReg.f32VGrid_Hi1Limit = f32tmp;
	else
		return 0;

	u16tmp = get_Hi_8bits(*(p_buf+18)) + get_Lo_8bits(*(p_buf+19));
	f32tmp = (float)u16tmp / 10;
	if((f32tmp > 200.0) && (f32tmp < 250.0))
//		g_Ctrl_Para.Volt_Ki = f32tmp;
//		g_Ctrl_Para.Volt_Lim_2 = f32tmp;
		SafetyReg.f32VGrid_Hi2Limit = f32tmp;
	else
		return 0;

	u16tmp = get_Hi_8bits(*(p_buf+20)) + get_Lo_8bits(*(p_buf+21));
	if((u16tmp > 0) && (u16tmp < 100))
		g_Ctrl_Para.Time_of_Over_Volt_1 = u16tmp;
	else
		return 0;

	u16tmp = get_Hi_8bits(*(p_buf+22)) + get_Lo_8bits(*(p_buf+23));
	if((u16tmp > 0) && (u16tmp < 100))
		g_Ctrl_Para.Time_of_Over_Volt_2 = u16tmp;
	else
		return 0;

	return 1;
}

void GetValue_Fault_Message(Uint8* p_buf)
{
	*(p_buf+2) = g_SysFaultMessage.Word.byte0;
	*(p_buf+3) = g_SysFaultMessage.Word.byte1;
	*(p_buf+4) = g_SysFaultMessage.Word.byte2;
	*(p_buf+5) = g_SysFaultMessage.Word.byte3;
	*(p_buf+6) = g_SysFaultMessage.Word.byte4;
	*(p_buf+7) = g_SysFaultMessage.Word.byte5;
	*(p_buf+8) = g_SysFaultMessage.Word.byte6;
	*(p_buf+9) = g_SysFaultMessage.Word.byte7;
}

void GetValue_IO_State(Uint8* p_buf)
{
	*(p_buf+2) = g_IO_State.Word.byte0;
	*(p_buf+3) = g_IO_State.Word.byte1;
	*(p_buf+4) = g_IO_State.Word.byte2;
	*(p_buf+5) = g_IO_State.Word.byte3;
}

void GetValue_Operate_State(Uint8* p_buf)
{
	switch(g_Sys_Current_State)
	{
	case WaitState:
		*(p_buf+2) = 0x11;
	break;
	case ChargeState:
		*(p_buf+2) = 0x33;
	break;
	case DisChargeState:
		*(p_buf+2) = 0x22;
	break;
	case CheckState:
		*(p_buf+2) = 0x44;
	break;
	case PreRunState:
		*(p_buf+2) = 0x55;
	break;
	case GridTiedState:
		*(p_buf+2) = 0x66;
	break;
	case FaultState:
		*(p_buf+2) = 0x88;
	break;
	case PermanentFaultState:
		*(p_buf+2) = 0x99;
	break;
	case StopState:
		*(p_buf+2) = 0x77;
	break;
	default:
		*(p_buf+2) = 0x00;
	break;
	}

}

void GetValue_Operate_Data(Uint8* p_buf)
{
	Uint16 u16tmp;

//	u16tmp = g_Time.second * 10;
	u16tmp = Calc_Result.f32VGrid_Rrms * 10;
//	u16tmp = Calc_Result.f32GridRFreq * 100;
	*(p_buf+2) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+3) = (Uint8) (u16tmp & 0x00FF);

//	u16tmp = g_Time.minute * 10;
	u16tmp = Calc_Result.f32VGrid_Srms * 10;
//	u16tmp = Calc_Result.f32GridSFreq * 100;
	*(p_buf+4) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+5) = (Uint8) (u16tmp & 0x00FF);

//	u16tmp = g_Time.hour * 10;
	u16tmp = Calc_Result.f32VGrid_Trms * 10;
//	u16tmp = Calc_Result.f32GridTFreq * 100;
	*(p_buf+6) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+7) = (Uint8) (u16tmp & 0x00FF);

//	u16tmp = g_Time.day * 10;
	u16tmp = Calc_Result.f32IGrid_Rrms * 10;
	*(p_buf+8) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+9) = (Uint8) (u16tmp & 0x00FF);

//	u16tmp = g_Time.month * 10;
	u16tmp = Calc_Result.f32IGrid_Srms * 10;
	*(p_buf+10) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+11) = (Uint8) (u16tmp & 0x00FF);

//	u16tmp = g_Time.year * 10;
	u16tmp = Calc_Result.f32IGrid_Trms * 10;
	*(p_buf+12) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+13) = (Uint8) (u16tmp & 0x00FF);

	u16tmp = Calc_Result.f32GridFreq * 10;
	*(p_buf+14) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+15) = (Uint8) (u16tmp & 0x00FF);

//	u16tmp = VGrid_Clarke.f32Alpha * 10;
//	u16tmp = PLLConReg.f32Theta_Step * 10000;
	u16tmp = Calc_Result.f32OutputWatt * 10;
	*(p_buf+16) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+17) = (Uint8) (u16tmp & 0x00FF);

//	u16tmp = VGrid_Clarke.f32AlphaOld * 10;
//	u16tmp = PLLConReg.f32VGrid_Amp * 100000;
	u16tmp = Calc_Result.f32OutputQ * 10;
	*(p_buf+18) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+19) = (Uint8) (u16tmp & 0x00FF);

//	u16tmp = VGrid_Clarke.f32AlphaDelay * 10;
//	u16tmp = VGrid_Clarke.f32AlphaP * 10;
//	u16tmp = (- Calc_Result.f32GridFreqClarke1) * 1000;
//	u16tmp = fabs(PLLConReg.f32ThetaErr_Old) * 10000;
//	u16tmp = Calc_Result.f32OutputVar * 10;
	u16tmp = Calc_Result.f32HistoryOutputEnergy_Day * 10;
	*(p_buf+20) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+21) = (Uint8) (u16tmp & 0x00FF);

//	u16tmp =  (- VGrid_Clarke.f32FreGrid1) * 1000;
//	u16tmp =  VGrid_Clarke.f32BetaDelay * 10;
//	u16tmp =  VGrid_Clarke.f32BetaP * 10;
//	u16tmp =  Calc_Result.f32GridFreqClarke2 * 1000;
//	u16tmp =  fabs(PLLConReg.f32ThetaErr_New) * 10000;
//	u16tmp =  Calc_Result.f32OutputPF * 10;
	u16tmp = Calc_Result.f32HistoryOutputEnergy_Month * 10;
	*(p_buf+22) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+23) = (Uint8) (u16tmp & 0x00FF);

//	u16tmp = VGrid_Clarke.f32FreGrid2 * 1000;
//	u16tmp = VGrid_Clarke.f32AlphaOld * 10;
//	u16tmp = PLLConReg.f32VdP * 10;
	u16tmp = Calc_Result.f32HistoryOutputEnergy_Year * 10;
//	u16tmp = 0 * 10;
	*(p_buf+24) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+25) = (Uint8) (u16tmp & 0x00FF);
}

void GetValue_Time(Uint8* p_buf)
{
	*(p_buf+2) = g_Time.year;
	*(p_buf+3) = g_Time.month;
	*(p_buf+4) = g_Time.day;
	*(p_buf+5) = g_Time.hour;
	*(p_buf+6) = g_Time.minute;
	*(p_buf+7) = g_Time.second;

}

void GetValue_Parameter_set(Uint8* p_buf)
{
	Uint16 u16tmp;

	u16tmp = g_Ctrl_Para.Curr_Kp * 1000000;
	*(p_buf+2) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+3) = (Uint8) (u16tmp & 0x00FF);

	u16tmp = g_Ctrl_Para.Curr_Ki * 1000000;
	*(p_buf+4) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+5) = (Uint8) (u16tmp & 0x00FF);

//	u16tmp = Calc_Result.f32OutputWatt * 100;

//	u16tmp = Calc_Result.f32OutputQ * 1000;

//	u16tmp = g_Ctrl_Para.Output_PF_set * 1000;

//	u16tmp = g_Ctrl_Para.Freq_Lim_Hi * 1000;
	u16tmp = SafetyReg.f32FreGrid_Hi2Limit * 1000;
	*(p_buf+6) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+7) = (Uint8) (u16tmp & 0x00FF);

//	u16tmp = g_Ctrl_Para.Freq_Lim_Lo;
	u16tmp = SafetyReg.f32FreGrid_Low2Limit * 1000;
	*(p_buf+8) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+9) = (Uint8) (u16tmp & 0x00FF);

	u16tmp = g_Ctrl_Para.Temp_Lim * 10;
	*(p_buf+10) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+11) = (Uint8) (u16tmp & 0x00FF);

	u16tmp = BusCon_Reg.f32Id_Ref * 10;
//	u16tmp = g_Ctrl_Para.Curr_Lim * 10;
//	u16tmp = fabs(Calc_Result.f32DCIR) * 100;
	*(p_buf+12) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+13) = (Uint8) (u16tmp & 0x00FF);

	u16tmp = g_Ctrl_Para.Time_of_Over_Curr * 10;
//	u16tmp = fabs(Calc_Result.f32DCIS) * 100;
	*(p_buf+14) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+15) = (Uint8) (u16tmp & 0x00FF);

//	u16tmp = g_Ctrl_Para.Volt_Kp * 1000;
//	u16tmp = g_Ctrl_Para.Volt_Lim_1;
	u16tmp = SafetyReg.f32VGrid_Hi1Limit * 10;
//	u16tmp = fabs(Calc_Result.f32DCIT) * 100;
	*(p_buf+16) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+17) = (Uint8) (u16tmp & 0x00FF);

//	u16tmp = g_Ctrl_Para.Volt_Ki * 10000;
//	u16tmp = g_Ctrl_Para.Volt_Lim_2;
	u16tmp = SafetyReg.f32VGrid_Hi2Limit * 10;
//	u16tmp = fabs(Calc_Result.f32IGrid_Rave) * 10;
	*(p_buf+18) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+19) = (Uint8) (u16tmp & 0x00FF);

	u16tmp = g_Ctrl_Para.Time_of_Over_Volt_1 * 10;
//	u16tmp = fabs(Calc_Result.f32IGrid_Save) * 10;
	*(p_buf+20) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+21) = (Uint8) (u16tmp & 0x00FF);

	u16tmp = g_Ctrl_Para.Time_of_Over_Volt_2 * 10;
//	u16tmp = fabs(Calc_Result.f32IGrid_Tave) * 10;
	*(p_buf+22) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+23) = (Uint8) (u16tmp & 0x00FF);

	u16tmp = Calc_Result.f32VInv_Rrms * 10;
//	u16tmp = fabs(Calc_Result.f32IL_Rave) * 10;
	*(p_buf+24) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+25) = (Uint8) (u16tmp & 0x00FF);

	u16tmp = Calc_Result.f32VInv_Srms * 10;
//	u16tmp = fabs(Calc_Result.f32IL_Save) * 10;
	*(p_buf+26) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+27) = (Uint8) (u16tmp & 0x00FF);

	u16tmp = Calc_Result.f32VInv_Trms * 10;
//	u16tmp = fabs(Calc_Result.f32IL_Tave) * 10;
	*(p_buf+28) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+29) = (Uint8) (u16tmp & 0x00FF);

	u16tmp = Calc_Result.f32VPV;
//	u16tmp = VgridRFreqCnt/100;
	*(p_buf+30) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+31) = (Uint8) (u16tmp & 0x00FF);

//	u16tmp = Calc_Result.f32GridRFreq * 100;
	u16tmp = Calc_Result.f32IL_Rrms * 10;
	*(p_buf+32) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+33) = (Uint8) (u16tmp & 0x00FF);

//	u16tmp = Calc_Result.f32GridSFreq * 100;
//	u16tmp = GetRealValue.f32Temp_R * 100;
	u16tmp = Calc_Result.f32IL_Srms * 10;
	*(p_buf+34) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+35) = (Uint8) (u16tmp & 0x00FF);

//	u16tmp = Calc_Result.f32GridTFreq * 100;
	u16tmp = Calc_Result.f32IL_Trms * 10;
//	u16tmp = GetRealValue.f32Temp_S * 100;
	*(p_buf+36) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+37) = (Uint8) (u16tmp & 0x00FF);

//	u16tmp = Calc_Result.f32GridFreq * 1000;
//	u16tmp = Calc_Result.f32Viso * 1000;
	u16tmp = GetRealValue.f32Temp_S * 100;
	*(p_buf+38) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+39) = (Uint8) (u16tmp & 0x00FF);

	u16tmp = VGrid_Park.f32QPs * 1000;
	*(p_buf+40) = (Uint8)((u16tmp & 0xFF00) >> 8);
	*(p_buf+41) = (Uint8) (u16tmp & 0x00FF);

}



//extern volatile Uint16	g_u16EEpromFlag;		
//extern volatile Uint8	g_aux_u8Date;
//extern volatile float32 g_aux_f32OutputEnergy;

static void Scic_Parsing(void)
{//removed by XXXXX  8.17
	static Uint8 s_u8Index=0;   //
	Uint8 u8Tmp;
	Uint8 cnt;           //读写寄存器初始化用
	Uint8 u8Data;
	Uint8 u8RxBuf[45];   //接受数据，最长45字节
	Uint8 u8TxBuf[45];   //发送数据，最长45字节
	Uint8 CRCchecknum;
//	Uint16 u16tmp;
	Uint8 data_type_flag = 2;		    //flag of data type(data、parameter、command)
//	Uint16 dataRXcorrect_flag;			//0,error;	1,data is received correctly
	Uint16 parameterSETcorrect_flag = 2;	//0,error;	1,parameters are set cottectly
	Uint16 COMMANDcorrect_flag = 2;			//0,invalid;1,command is valid

	for(cnt=0; cnt<45; cnt++)
		u8RxBuf[cnt] = 0;      
	for(cnt=0; cnt<45; cnt++)
		u8TxBuf[cnt] = 0;         //接收，发送寄存器初始值均为0

//	CRCchecknum = CRCNumCalc(u8TxBuf, 1);	//calculate the CRC-check nummber


	while(1)
	{
		u8Tmp = SciRead(ID_SCIC, &u8Data);
/*								u8TxBuf[0] = 0x01;	   //first byte: 	address and type
								u8TxBuf[1] = 0x08;	   //second byte:	nummber of data
								GetValue_Fault_Message(u8TxBuf);					//get data
								CRCchecknum = CRCNumCalc(u8TxBuf, u8TxBuf[1]+1);	//calculate the CRC-check nummber
								//u8TxBuf[u8TxBuf[1]+2] = CRCchecknum;				//last byte: CRC-check nummber
								u8TxBuf[u8TxBuf[1]+2] = 0x11;
								SciWrite(ID_SCIC, u8TxBuf, u8TxBuf[1]+3);			//Transmit data
*/
		if(!u8Tmp)
		{
			switch(s_u8Index)
			{
			case 0:			//direction: DSP1 or DSP2
				if(0x01 == ((u8Data>>6) & 0x03) )	//the data is for DSP1
				{									//bit7、bit6 are address bits
					u8RxBuf[0] = u8Data;
					data_type_flag = u8Data & 0x3F;	//bit5-bit0 are data type bits
					switch(data_type_flag)
					{
					case 0x06:						//Set Inverter Time			
						s_u8Index++;	//continue receiving data
					break;
					case 0x07:						//Set Inverter Operate Parameters
						s_u8Index++;
					break;
					case 0x09:						//Commands for Inverter
						s_u8Index++;					
					break;
					default:						//invalid type
										//discard invalid data	
					break;
					}
				}	
			break;
			case 1:			//number of bytes
				if(((data_type_flag == 0x06) && (u8Data == 6))||((data_type_flag == 0x07) && (u8Data == 40))||((data_type_flag == 0x09) && (u8Data == 1)))				
				{
					u8RxBuf[s_u8Index] = u8Data;
					s_u8Index++;
				}
				else
				{
					s_u8Index = 0;
				}
			break;
			default:
				u8RxBuf[s_u8Index] = u8Data;
				s_u8Index++;
				if(s_u8Index == (u8RxBuf[1] + 3))	//'3' is byte0, byte1 and CRCchecknum(1 bytes)
				{
					if( CRCNumCalc(u8RxBuf, u8RxBuf[1] + 2) == u8Data )
//					if(1)
					{
						switch(data_type_flag)
						{
						case 0x06:
							parameterSETcorrect_flag = SetValue_Time(u8RxBuf);
						break;
						case 0x07:
							parameterSETcorrect_flag = SetValue_Parameter_set(u8RxBuf);
						break;
						case 0x09:
							switch(u8RxBuf[2])
							{
							case 0x01:						//Request Inverter fault status
								//COMMANDcorrect_flag = 1;	//Command is valid
								u8TxBuf[0] = 0x41;	//first byte: 	address and type
								u8TxBuf[1] = 8;				//second byte:	nummber of data
								GetValue_Fault_Message(u8TxBuf);					//get data
								CRCchecknum = CRCNumCalc(u8TxBuf, u8TxBuf[1]+2);	//calculate the CRC-check nummber
								u8TxBuf[u8TxBuf[1]+2] = CRCchecknum;				//last byte: CRC-check nummber
								//u8TxBuf[u8TxBuf[1]+2] = 0x11;
								SciWrite(ID_SCIC, u8TxBuf, u8TxBuf[1]+3);			//Transmit data
							break;
							case 0x02:						//Request Inverter IO states
								//COMMANDcorrect_flag = 1;	//Command is valid
								u8TxBuf[0] = 0x42;
								u8TxBuf[1] = 4;
								GetValue_IO_State(u8TxBuf);
								CRCchecknum = CRCNumCalc(u8TxBuf, u8TxBuf[1]+2);
								u8TxBuf[u8TxBuf[1]+2] = CRCchecknum;
								//u8TxBuf[u8TxBuf[1]+2] = 0x11;
								SciWrite(ID_SCIC, u8TxBuf, u8TxBuf[1]+3);
							break;
							case 0x03:						//Request Inverter Operate State
								//COMMANDcorrect_flag = 1;	//Command is valid
								u8TxBuf[0] = 0x43;
								u8TxBuf[1] = 1;
								GetValue_Operate_State(u8TxBuf);
								CRCchecknum = CRCNumCalc(u8TxBuf, u8TxBuf[1]+2);
								u8TxBuf[u8TxBuf[1]+2] = CRCchecknum;
								//u8TxBuf[u8TxBuf[1]+2] = 0x11;
								SciWrite(ID_SCIC, u8TxBuf, u8TxBuf[1]+3);
							break;
							case 0x04:						//Request Inverter Operate Data
								//COMMANDcorrect_flag = 1;	//Command is valid
								u8TxBuf[0] = 0x44;
								u8TxBuf[1] = 24;
								GetValue_Operate_Data(u8TxBuf);
								CRCchecknum = CRCNumCalc(u8TxBuf, u8TxBuf[1]+2);
								u8TxBuf[u8TxBuf[1]+2] = CRCchecknum;
								//u8TxBuf[u8TxBuf[1]+2] = 0x11;
								SciWrite(ID_SCIC, u8TxBuf, u8TxBuf[1]+3);
							break;
							case 0x05:						//Request Inverter Time
								//COMMANDcorrect_flag = 1;	//Command is valid
								u8TxBuf[0] = 0x45;
								u8TxBuf[1] = 6;
								GetValue_Time(u8TxBuf);
								CRCchecknum = CRCNumCalc(u8TxBuf, u8TxBuf[1]+2);
								u8TxBuf[u8TxBuf[1]+2] = CRCchecknum;
								//u8TxBuf[u8TxBuf[1]+2] = 0x11;
								SciWrite(ID_SCIC, u8TxBuf, u8TxBuf[1]+3);
							break;
							case 0x08:						//Request Inverter Operate Parameters
								//COMMANDcorrect_flag = 1;	//Command is valid
								u8TxBuf[0] = 0x48;
								u8TxBuf[1] = 40;
								GetValue_Parameter_set(u8TxBuf);
								CRCchecknum = CRCNumCalc(u8TxBuf, u8TxBuf[1]+2);
								u8TxBuf[u8TxBuf[1]+2] = CRCchecknum;
								//u8TxBuf[u8TxBuf[1]+2] = 0x11;
								SciWrite(ID_SCIC, u8TxBuf, u8TxBuf[1]+3);
							break;
							case 0x09:
								COMMANDcorrect_flag = 1;	//Command is valid
								g_IO_State.bit.RUN_COMMAND = 1;
								g_IO_State.bit.STOP_COMMAND = 0;
								g_IO_State.bit.DisCharge_COMMAND = 0;
							break;
							case 0x10:
								COMMANDcorrect_flag = 1;	//Command is valid
								g_IO_State.bit.STOP_COMMAND = 1;
								g_IO_State.bit.RUN_COMMAND = 0;
								g_IO_State.bit.DisCharge_COMMAND = 0;
							break;
							case 0x11:
								COMMANDcorrect_flag = 1;	//Command is valid
								g_IO_State.bit.DisCharge_COMMAND = 1;
								g_IO_State.bit.RUN_COMMAND = 0;
								g_IO_State.bit.STOP_COMMAND = 0;
							break;
							default:
								s_u8Index = 0;
								COMMANDcorrect_flag = 0;
								//返回无效命令信号
								u8TxBuf[0] = u8RxBuf[0];
								u8TxBuf[1] = 1;
								u8TxBuf[2] = 0x88;
								CRCchecknum = CRCNumCalc(u8TxBuf, 3);
								u8TxBuf[3] = CRCchecknum;
								//u8TxBuf[3] = 0x11;
								SciWrite(ID_SCIC, u8TxBuf, 4);
							break;
							}
							if(COMMANDcorrect_flag == 1)
							{
								//返回命令正确信号
								u8TxBuf[0] = u8RxBuf[0];
								u8TxBuf[1] = 1;
								u8TxBuf[2] = 0x77;
								CRCchecknum = CRCNumCalc(u8TxBuf, 3);
								u8TxBuf[3] = CRCchecknum;
								//u8TxBuf[3] = 0x11;
								SciWrite(ID_SCIC, u8TxBuf, 4);
							}
						break;
						default:
						break;
						}
						if(parameterSETcorrect_flag == 1)
						{
							//返回参数设置正确信号
							u8TxBuf[0] = 0x49;
							u8TxBuf[1] = 1;
							u8TxBuf[2] = 0x66;
							CRCchecknum = CRCNumCalc(u8TxBuf, 3);
							u8TxBuf[3] = CRCchecknum;
							//u8TxBuf[3] = 0x11;
							SciWrite(ID_SCIC, u8TxBuf, 4);
						}
						else if(parameterSETcorrect_flag == 0)
						{
							//返回参数设置错误信号
							u8TxBuf[0] = 0x49;
							u8TxBuf[1] = 1;
							u8TxBuf[2] = 0x33;
							CRCchecknum = CRCNumCalc(u8TxBuf, 3);
							u8TxBuf[3] = CRCchecknum;
							//u8TxBuf[3] = 0x11;
							SciWrite(ID_SCIC, u8TxBuf, 4);
						}
						//返回数据接收正确信号
/*						u8TxBuf[0] = u8RxBuf[0];
						u8TxBuf[1] = 1;
						u8TxBuf[2] = 0xAA;
						CRCchecknum = CRCNumCalc(u8TxBuf, 2);
						u8TxBuf[3] = CRCchecknum;
						//u8TxBuf[3] = 0x11;
						SciWrite(ID_SCIC, u8TxBuf, 4);
*/					}
					else
					{
						//返回数据接收错误信号（CRC校验）
						u8TxBuf[0] = 0x49;
						u8TxBuf[1] = 1;
						u8TxBuf[2] = 0x55;
						CRCchecknum = CRCNumCalc(u8TxBuf, 3);
						u8TxBuf[3] = CRCchecknum;
						//u8TxBuf[3] = 0x11;
						SciWrite(ID_SCIC, u8TxBuf, 4);
					}
					s_u8Index = 0;	
				}
				if(s_u8Index >= 45)
				{
					s_u8Index = 0;
				}
			break;
			}
		}
		else
		{
			break;
		}
	}	
}
