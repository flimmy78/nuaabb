/*=============================================================================*
 *         Copyright(c) 2010-2011, ALL RIGHTS RESERVED
 *                          
 *
 *  FILENAME : 17KW_Sci_Driver.h 

 *  PURPOSE  : header files 17KW_Scib_Interface.c 
 *			       Define base structure and driver fuction for SCI 
 *
 *  HISTORY  :
 *    DATE            VERSION        AUTHOR            NOTE
 *    
 *
 ******************************************************************************/


#ifndef SCI_DRIVER_H
#define SCI_DRIVER_H



struct 	Time
{
	Uint8	year;
	Uint8	month;
	Uint8	day;
	Uint8	hour;
	Uint8	minute;
	Uint8	second;
	Uint8	year_old;
	Uint8	month_old;
	Uint8	day_old;	
};
extern struct Time	g_Time;


struct	Skiip4_CAN
{
	Uint8 Operatng_Time1;  //运行时间参数32位，分成4个8位的(以下均是从低字节---到高字节)
	Uint8 Operatng_Time2;
	Uint8 Operatng_Time3;
	Uint8 Operatng_Time4;

	Uint8 DC_Voltage1;     //直流电压采样16位，分成2个8位的
	Uint8 DC_Voltage2;

	Uint8 IGBT_Temp1;      //IGBT温度采样16位，分成2个8位的
	Uint8 IGBT_Temp2;

	Uint8 Ambient_Temp1;   //环境温度采样16位，分成2个8位的
	Uint8 Ambient_Temp2;

	Uint8 Current_Error1;          //当前次故障保存16位，分成2个8位的
	Uint8 Current_Error2;

	Uint8 Current_Error1_Time1;    //当前次故障发生时间保存32位，分成4个8位的
	Uint8 Current_Error1_Time2;
	Uint8 Current_Error1_Time3;
	Uint8 Current_Error1_Time4;

	Uint8 Last_Error1;             //上一次故障保存16位，分成2个8位的
	Uint8 Last_Error2;

	Uint8 Last_Error1_Time1;       //上一次故障发生时间保存32位，分成4个8位的
	Uint8 Last_Error1_Time2; 
	Uint8 Last_Error1_Time3;
	Uint8 Last_Error1_Time4;

	Uint8 LastTwo_Error1;          //上两次故障保存16位，分成2个8位的
	Uint8 LastTwo_Error2;

	Uint8 LastTwo_Error1_Time1;    //上两次故障发生时间保存32位，分成4个8位的
	Uint8 LastTwo_Error1_Time2;
	Uint8 LastTwo_Error1_Time3;
	Uint8 LastTwo_Error1_Time4;

	Uint8 LastThree_Error1;        //上3次故障保存16位，分成2个8位的
	Uint8 LastThree_Error2;

	Uint8 LastThree_Error1_Time1;  //上3次故障发生时间保存32位，分成4个8位的
	Uint8 LastThree_Error1_Time2;
	Uint8 LastThree_Error1_Time3;
	Uint8 LastThree_Error1_Time4;

	Uint8 LastFour_Error1;         //上4次故障保存16位，分成2个8位的
	Uint8 LastFour_Error2;

	Uint8 LastFour_Error1_Time1;   //上4次故障发生时间保存32位，分成4个8位的
	Uint8 LastFour_Error1_Time2;
	Uint8 LastFour_Error1_Time3;
	Uint8 LastFour_Error1_Time4;

	Uint8 LastVce_Error1;         //当前次Vce电压故障保存16位，分成2个8位的
	Uint8 LastVce_Error2;

	Uint8 LastVce_Error1_Time1;   //当前次Vce电压故障发生时间保存32位，分成4个8位的
	Uint8 LastVce_Error1_Time2;
	Uint8 LastVce_Error1_Time3;
	Uint8 LastVce_Error1_Time4;

};



struct	 Ctrl_Para
{
	float	Curr_Kp;
	float	Curr_Ki;
	float	Freq_Lim_Hi;
	float	Freq_Lim_Lo;
	float	Temp_Lim;
	float	Curr_Lim;
	Uint16	Time_of_Over_Curr;
	float	Volt_Lim_1;
	float	Volt_Lim_2;
	Uint16	Time_of_Over_Volt_1;
	Uint16	Time_of_Over_Volt_2;
	float	Volt_Kp;
	float	Volt_Ki;
	float	Output_Watt_set;
	float	Output_Var_set;
	float	Output_PF_set;
	float	Output_Watt_Delt_set;
	float	Output_Var_Delt_set;
	Uint16	type19;
	Uint16	type20;
};
extern struct Ctrl_Para g_Ctrl_Para;

				
//fuction declaration for IPOMS
extern void Skiip4_CanX_Send(void);  //CAN通讯发送函数
extern void Can_Skiip4_Receive(Uint32 Data_High, Uint32 Data_Low, Uint8 Skiip4_X[]);
extern void Skiip4_CAN1_Receive(Uint32 Data_High, Uint32 Data_Low);
extern void Skiip4_CAN2_Receive(Uint32 Data_High, Uint32 Data_Low);
extern void Skiip4_CAN3_Receive(Uint32 Data_High, Uint32 Data_Low);

extern struct Skiip4_CAN  Skiip4_CAN1,Skiip4_CAN2,Skiip4_CAN3;
#endif

//--- end of file -----------------------------------------------------
