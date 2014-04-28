/*=============================================================================*
 *        
 *                          ALL RIGHTS RESERVED
 *
 *  FILENAME : 17KW_MPPT.h 
 *
 *  PURPOSE  : header file of 17KW_MPPT.c 
 *			       define constant, struct declaration, extern varibles 
 *
 *  HISTORY  :
 *    DATE            VERSION        AUTHOR            NOTE
 *    
 *
 ******************************************************************************/                  

#ifndef MPPT_H
#define MPPT_H

//======================= Global constants============================  
#define	Default_MPPT_Delta_VStep	0.5
#define	Default_MPPT_Rate_Cnt	    25
#define DELT_POWER_DC   0          // W*5/1000 *10

#define PV_POWER_DERATE_HIGH_VOLT  800 
#define PV_POWER_DERATE_LOW_VOLT   430 
//#define BUS_VOLT_REF_LIMIT_430V   4300 
//#define BUS_VOLT_REF_LIMIT_820V   8200 

#define BoostCon_K_alpha	2.86f//0.609f//51.666f				//250*2=5us,375-->7.5us
#define BoostCon_K_beta		2.84f//0.575f						//250*2=5us,375-->7.5us

#define BusCon_K_alpha	0.111f// Ki= alpha-beta  Kp = beta0.0583//0.02415//0.03415//0.0587//0.0587f//51.666f				//250*2=5us,375-->7.5us
#define BusCon_K_beta	 0.11f//0.058//0.02413//0.03412//0.05865//0.058f				//250*2=5us,375-->7.5us

//#define  INITIAL_STEP           7   
//#define  INITIAL_REF_MAX    10  

//================ Global variables =====================================
extern float32 g_f32Mppt_BusVolt;                      
extern float32 g_f32Mppt_Autobus_Volt;
extern float32 g_f32Mppt_BusVolt_Ref_New;
extern float32 g_f32Mppt_Input_Watt_Old;
extern float32 g_f32Mppt_Input_Watt_New;
extern float32 g_f32Mppt_Start_Step;
//extern Uint16  g_u16Mppt_FastSearch_Flag;

extern float32 g_f32Mppt_Id_Ref_Limit;
extern float32 g_f32OutPutLoadPower;

extern Uint16 Id_Ref_MaxRate_Flag;
//extern Uint16 Id1_Ref_MaxRate_Flag;
//extern Uint16 Id2_Ref_MaxRate_Flag;
//extern Uint16 g_u16PVOpen_Control_Flag;
//extern Uint16 g_u16PV1Open_Control_Flag;
//extern Uint16 g_u16PV2Open_Control_Flag;
//extern Uint16 g_u16Mppt_TimeBase_Cnt;

struct MPPT_REG
{
    float32   f32InputWatt_New;    
    float32   f32InputWatt_Old;     
    float32   f32OutputWatt;
    float32   f32InputPowerLimit;
    float32   f32OutputPowerLimit;
    float32   f32PV_Fdb;
    float32   f32PV_Fdb_old;
    float32   f32Id_Ref_Max;      // Id reference max
    float32   f32Id_Ref_Min;       // Id reference min
    float32   f32PV_Volt_Open;      // pv open voltage
    float32   f32PV_Volt_Ref;
    Uint16	  u16BoostOffFlag;
	Uint16	  u16BoostOffFlag_time;			// add by Johnny 2011/6/25
    Uint16	  u16Boosttempprotection;        //modified by microwu
};
extern  struct MPPT_REG  Mppt_Reg1;  //,Mppt_Reg1,Mppt_Reg2;
extern  struct MPPT_REG  Mppt_Reg2;  //,Mppt_Reg1,Mppt_Reg2;
struct BUS_CON_REG
{
  float32   f32BusRef;
  float32   f32BusVolt_Fdb;       // bus voltage
  float32   f32BusErr_New;       // current bus voltage error
  float32   f32BusErr_Old;        // before bus voltage error
  float32   f32Id_Ref;            // final Idref command
  float32   f32Iq_Ref;            // final Idref command
  float32   f32IdRefDelta;
  float32   f32IqRefDelta;
  float32   f32BusReg_Out;
  float32   f32ACPowerP_Ref;
  float32   f32ACPowerQ_Ref;
  float32   f32ACCosFi_Ref;
  float32   f32VGridDQ2;
//  float32   f32Idref_slowup;   // Slowup Id command
  float32   f32Idref_cmd;  //  Idref command
  float32	f32Iqref_cmd;
  float32   f32IGain;      //  IGain for slowup mode     
//    float32   f32Iq_Ref;
};
extern  struct BUS_CON_REG  BusCon_Reg;

struct BOOST_CON_REG
{
    float32   f32PVErr_Old;
    float32   f32PVErr_New;
    float32   f32Delta_PV;
    float32   f32Boost_Duty;
    float32   f32Boost1_Duty;
    float32   f32Boost2_Duty;
    float32	  f32Boost_Duty_temp;
    float32   f32Delta_Vbus;
//	float32	  f32BGain;
//	float32	  f32Boost_start;
};
extern  struct BOOST_CON_REG  BooostCon_Reg; //,BooostCon_Reg1,BooostCon_Reg2;

struct BUS_CON_PARA_REG
{
    float32	  f32Bus_K_alpha;
    float32	  f32Bus_K_beta;
    float32   f32Bus_Ref_Max;    // bus voltage reference max
    float32   f32Bus_Ref_Min;     // bus voltage reference min
};
extern  struct BUS_CON_PARA_REG  BusConPara_Reg;  

struct BOOST_CON_PARA_REG
{
    float32   f32K_delta_P;
    float32   f32Boost_K_alpha;
    float32   f32Boost_K_beta;
    float32   f32Boost_Duty_Max;
    float32   f32Boost_Duty_Min;
	float32   f32Vbus_Boost_MAX;
};
extern  struct BOOST_CON_PARA_REG  BoostConPara_Reg; 
           
struct MPPT_PARA_REG
{
    float32   f32OutputRatedPower;
    float32   f32InputRatedPower;
	float32   f32Input_Current_Limit;
	float32   f32Output_Current_Limit;

    Uint16	  f32Mppt_Rate_Cnt;	
    float32   f32Delta_Volt_Step;
    float32   f32Start_Step;

    float32  f32SWVersion1;
    float32  f32SWVersion2;    
};
extern  struct MPPT_PARA_REG  MpptPara_Reg;
  
struct POWER_DERATE_REG
{
    float32   f32OutputPower_AmbientTLimit;
    float32	  f32OutputPower_HeatsinkTLimit;
    float32	  f32Input1Power_HeatsinkTLimit;
    float32	  f32Input2Power_HeatsinkTLimit;

    float32	  f32InputPowerLimit_MaxVolt;
    float32	  f32InputPowerLimit_MinVolt;
    float32	  f32DCPowerDerating_VRate;
    float32	  f32ACPowerDerating_HTRate;
    float32	  f32DCPowerDerating_HTRate;
    float32	  f32ACPowerDerating_ATRate;

    float32	  f32Inv_OverTemperature_Limit;
    float32	  f32Boost_OverTemperature_Limit;
    float32	  f32Ambient_OverTemperature_Limit;

};

extern  struct POWER_DERATE_REG  PowerDerate_Reg;

union  MPPT_STATE 
{
    Uint16 all:16;
    struct
    {
        Uint16  byte0:8;
        Uint16  byte1:8;
    }Word;

    struct
    {
        Uint16  B1:1;    // B0
        Uint16  Pv1VoltLowFault:1;    // B1
        Uint16  Pv2VoltLowFault:1;    // B2
        Uint16  PV1_ON:1;    // B3
        Uint16  PV2_ON:1;    // B4
        Uint16  Multi_string:1;    // B5
        Uint16  Common_String:1;    // B6
        Uint16  B7:1;    // B7     0: hardware  interrupt   1:softwar force  interrupt   

        Uint16  Initial_Flag1:1;    // B0
        Uint16  Initial_Flag2:1;      //B1
        Uint16  Initial_Flag:1; //B2
        Uint16  B11:1;     //B3
        Uint16  B12:1;     //B4
        Uint16  B13:1;  //B5
        Uint16  B14:1; //B6
        Uint16  B15:1;    //B7
    }bit;
}; 
extern union MPPT_STATE  g_MPPT_State;
 
extern void Calc_Id_reference(void);
extern void IGain_Slowup(void);
extern void MpptInitial(void);
#endif


//--- End of file --------------------------------------------------------------
