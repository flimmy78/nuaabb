
#include "DSP2833x_Device.h"			// Peripheral address definitions
#include "500kVar_SVG.h"					// Main include file

struct Skiip4_CAN  Skiip4_CAN1,Skiip4_CAN2,Skiip4_CAN3;

void Skiip4_CanX_Send (void);
void Can_Skiip4_Receive(Uint32 Data_High, Uint32 Data_Low, Uint8 Skiip4_X[]);
void Skiip4_CAN1_Receive(Uint32 Data_High, Uint32 Data_Low);
void Skiip4_CAN2_Receive(Uint32 Data_High, Uint32 Data_Low);
void Skiip4_CAN3_Receive(Uint32 Data_High, Uint32 Data_Low);

//*******向SKiip4模块请求数据的CAN发送程序********//
void Skiip4_CanX_Send (void)
{    	
	struct ECAN_REGS ECanaShadow;
	static Uint16 Skiip4Number_Flag = 0;  //Skiip4模块选择,3个Skiip4模块循环传输数据
	static Uint16 Index_Flag = 0;         //CAN请求Skiip4模块传输不同数据的标志
	static Uint32 SubIndex_Flag = 0;      //CAN请求Skiip4模块传输不同数据的子标志
	Uint16 Cycle_Flag = 0;

//********此段为Skiip4-1,2,3模块的循环函数*********//	
	if(Skiip4Number_Flag == 0)
	{
		ECanaMboxes.MBOX0.MSGID.all = 0x0C040000; //Skiip1 发送邮箱
		Skiip4Number_Flag++;
	}
	else if(Skiip4Number_Flag == 1)
	{
		ECanaMboxes.MBOX1.MSGID.all = 0x0C080000; //Skiip2 发送邮箱
		Skiip4Number_Flag++;
	}
	else if(Skiip4Number_Flag == 2)
	{
		ECanaMboxes.MBOX2.MSGID.all = 0x0C100000; //Skiip3 发送邮箱
		Skiip4Number_Flag = 0;
		Cycle_Flag++;             //循环标识置位1，代表3个模块循环一次结束
	}
//************************************************//

//********此段为请求不同"数据帧"判断标志位变动*********//
	if(Cycle_Flag == 1)   //3个Skiip4模块每循环一次进入下一帧，则Index_Flag+1
	{	
		if(Index_Flag == 5)    //在"故障保存帧"下等待所有子目录循环一次后进入下一帧
		{
			if(SubIndex_Flag == 0x0A000000)
			{
				SubIndex_Flag = 0;
				Index_Flag++;  //所有的子目录循环结束以后，进入下一请求发送"Vce电压保护寄存帧"模式
			}
		}
		else 	Index_Flag++;
	}
//*****************************************************//
	switch(Index_Flag)
	{
		case 1:   //请求发送"运行时间帧"
		{
			ECanaMboxes.MBOX0.MDL.all = 0x00200340;  //低4字节为索引号，代表高4字节数据涵义
			ECanaMboxes.MBOX0.MDH.all = 0x0;	
		}
		break;
		case 2:    //请求发送"直流电压采样数值帧"
		{
			ECanaMboxes.MBOX0.MDL.all = 0x00200640;  //低4字节为索引号，代表高4字节数据涵义
			ECanaMboxes.MBOX0.MDH.all = 0x0;	
		}
		break;
		case 3:    //请求发送"IGBT模块温度数值帧"
		{
			ECanaMboxes.MBOX0.MDL.all = 0x00200A40;  //低4字节为索引号，代表高4字节数据涵义
			ECanaMboxes.MBOX0.MDH.all = 0x0;	
		}
		break;
		case 4:    //请求发送"环境温度采样数值帧"
		{
			ECanaMboxes.MBOX0.MDL.all = 0x00200B40;  //低4字节为索引号，代表高4字节数据涵义
			ECanaMboxes.MBOX0.MDH.all = 0x0;
		}
		break;
		case 5:    //请求发送"故障保存帧"
		{	
			if(Cycle_Flag == 1)
			{
				SubIndex_Flag = SubIndex_Flag + 0x01000000;
			}
			ECanaMboxes.MBOX0.MDL.all = 0x00205040 + SubIndex_Flag;  //低4字节为索引号，代表高4字节数据涵义
			ECanaMboxes.MBOX0.MDH.all = 0x0;
		}
		break;
		case 6:    //请求发送"IGBT模块的Vce电压保护寄存帧"
		{
			ECanaMboxes.MBOX0.MDL.all = 0x00205140;  //低4字节为索引号，代表高4字节数据涵义
			ECanaMboxes.MBOX0.MDH.all = 0x0;
		}
		break;
		case 7:    //请求发送"IGBT模块的Vce电压保护时间寄存帧"
		{
			ECanaMboxes.MBOX0.MDL.all = 0x00205240;  //低4字节为索引号，代表高4字节数据涵义
			ECanaMboxes.MBOX0.MDH.all = 0x0;
			Index_Flag = 0;	
		}
		break;
		default: break;
	}

	
		//*************PART I**************************
		//the next part focus on transmitting messages
		//Step 1, clear register CANTRS
	   //Step 2, initialize mailboxes
		EALLOW;
		ECanaShadow.CANTA.all = ECanaRegs.CANTA.all;
		//Step 3, config TRS to apply for transmitting
		if(ECanaShadow.CANTA.bit.TA3 == 1)
		ECanaShadow.CANTRS.bit.TRS3 = 1;
		ECanaRegs.CANTRS.all = ECanaShadow.CANTRS.all;
		EDIS;
		//Step 4, wait for response to complete transmition
			
	//*************PART I END**********************

} 
//*******end of Skiip4_CanX_Send ()********//


//***************接受skiip4数据函数(32位数组)****************//
/*
void Can_Skiip4_Receive(Uint32 Data_High, Uint32 Data_Low, Uint32 Skiip4_X[])
{
	static int i = 0;

	if(Data_Low ==  0x00200340)   //接收到的是"运行时间帧"
	{
		Skiip4_X[0] = Data_High;
	}
	if(Data_Low ==  0x00200640)   //接收到的是"直流电压采样数值帧"
	{
		Skiip4_X[1] = Data_High;
	}
	if(Data_Low ==  0x00200A40)   //接收到的是"IGBT模块温度数值帧"
	{
		Skiip4_X[2] = Data_High;
	}
	if(Data_Low ==  0x00200B40)   //接收到的是"环境温度采样数值帧"
	{
		Skiip4_X[3] = Data_High;
	}
	for(i=0;i<=10;i++)            //接收到的是"故障保存帧(包含子目录)"
	{
		if(Data_Low ==  0x01205040 + 0x01000000 * i)
		{
			Skiip4_X[4 + i] = Data_High;
		}
	}
	if(Data_Low ==  0x00205140)   //接收到的是"IGBT模块的Vce电压保护寄存帧"
	{
		Skiip4_X[5] = Data_High;
	}
	if(Data_Low ==  0x00205240)   //接收到的是"IGBT模块的Vce电压保护时间寄存帧"
	{
		Skiip4_X[6] = Data_High;
	}	
}
*/

//***************接受skiip4数据函数(8位数组)****************//
void Can_Skiip4_Receive(Uint32 Data_High, Uint32 Data_Low, Uint8 Skiip4_X[])
{
	static int i = 0;

	if(Data_Low ==  0x00200340)   //接收到的是"运行时间帧"
	{
		Skiip4_X[0] = ((Data_High>>24) & 0x000F);
		Skiip4_X[1] = ((Data_High>>16) & 0x000F);
		Skiip4_X[2] = ((Data_High>>8) & 0x000F);
		Skiip4_X[3] = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00200640)   //接收到的是"直流电压采样数值帧"
	{
		Skiip4_X[4] = ((Data_High>>8) & 0x000F);
		Skiip4_X[5] = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00200A40)   //接收到的是"IGBT模块温度数值帧"
	{
		Skiip4_X[6] = ((Data_High>>8) & 0x000F);
		Skiip4_X[7] = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00200B40)   //接收到的是"环境温度采样数值帧"
	{
		Skiip4_X[8] = ((Data_High>>8) & 0x000F);
		Skiip4_X[9] = ((Data_High) & 0x000F);
	}
	for(i=0;i<10;i++)            //接收到的是"故障保存帧(包含子目录)"
	{
		if(Data_Low ==  0x01205040 + 0x01000000 * i)
		{
			if((i%2) == 0)
			{
				Skiip4_X[10 + 3 * i] = ((Data_High>>8) & 0x000F);
				Skiip4_X[11 + 3 * i] = ((Data_High) & 0x000F);

			}
			else
			{
				Skiip4_X[10 + 3 * (i-1) + 2] = ((Data_High>>24) & 0x000F);
				Skiip4_X[11 + 3 * (i-1) + 2] = ((Data_High>>16) & 0x000F);
				Skiip4_X[12 + 3 * (i-1) + 2] = ((Data_High>>8) & 0x000F);
				Skiip4_X[13 + 3 * (i-1) + 2] = ((Data_High) & 0x000F);
			}
		}
	}
	if(Data_Low ==  0x00205140)   //接收到的是"IGBT模块的Vce电压保护寄存帧"
	{
		Skiip4_X[40] = ((Data_High>>8) & 0x000F);
		Skiip4_X[41] = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00205240)   //接收到的是"IGBT模块的Vce电压保护时间寄存帧"
	{
		Skiip4_X[42] = ((Data_High>>24) & 0x000F);
		Skiip4_X[43] = ((Data_High>>16) & 0x000F);
		Skiip4_X[44] = ((Data_High>>8) & 0x000F);
		Skiip4_X[45] = ((Data_High) & 0x000F);
	}	
}
//***************end of Can_Skiip4_Receive()**************//

//**********A相Skiip4的CAN接收程序************//
void Skiip4_CAN1_Receive(Uint32 Data_High, Uint32 Data_Low)
{
	if(Data_Low ==  0x00200340)   //接收到的是"运行时间帧"
	{
		Skiip4_CAN1.Operatng_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN1.Operatng_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN1.Operatng_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN1.Operatng_Time1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00200640)   //接收到的是"直流电压采样数值帧"
	{
		Skiip4_CAN1.DC_Voltage2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN1.DC_Voltage1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00200A40)   //接收到的是"IGBT模块温度数值帧"
	{
		Skiip4_CAN1.IGBT_Temp2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN1.IGBT_Temp1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00200B40)   //接收到的是"环境温度采样数值帧"
	{
		Skiip4_CAN1.Ambient_Temp2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN1.Ambient_Temp1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x01205040)
	{
		Skiip4_CAN1.Current_Error2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN1.Current_Error1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x02205040)
	{
		Skiip4_CAN1.Current_Error1_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN1.Current_Error1_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN1.Current_Error1_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN1.Current_Error1_Time1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x03205040)
	{
		Skiip4_CAN1.Last_Error2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN1.Last_Error1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x04205040)
	{
		Skiip4_CAN1.Last_Error1_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN1.Last_Error1_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN1.Last_Error1_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN1.Last_Error1_Time1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x0505040)
	{
		Skiip4_CAN1.LastTwo_Error2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN1.LastTwo_Error1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x06205040)
	{
		Skiip4_CAN1.LastTwo_Error1_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN1.LastTwo_Error1_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN1.LastTwo_Error1_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN1.LastTwo_Error1_Time1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x0705040)
	{
		Skiip4_CAN1.LastThree_Error2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN1.LastThree_Error1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x08205040)
	{
		Skiip4_CAN1.LastThree_Error1_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN1.LastThree_Error1_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN1.LastThree_Error1_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN1.LastThree_Error1_Time1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x0905040)
	{
		Skiip4_CAN1.LastFour_Error2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN1.LastFour_Error1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x0A205040)
	{
		Skiip4_CAN1.LastFour_Error1_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN1.LastFour_Error1_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN1.LastFour_Error1_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN1.LastFour_Error1_Time1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00205140)   //接收到的是"IGBT模块的Vce电压保护寄存帧"
	{
		Skiip4_CAN1.LastVce_Error2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN1.LastVce_Error1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00205240)   //接收到的是"IGBT模块的Vce电压保护时间寄存帧"
	{
		Skiip4_CAN1.LastVce_Error1_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN1.LastVce_Error1_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN1.LastVce_Error1_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN1.LastVce_Error1_Time1 = ((Data_High) & 0x000F);
	}	
}
//**********end of Skiip4_CAN1_Receive()***********//


//**********B相Skiip4的CAN接收程序************//
void Skiip4_CAN2_Receive(Uint32 Data_High, Uint32 Data_Low)
{
	if(Data_Low ==  0x00200340)   //接收到的是"运行时间帧"
	{
		Skiip4_CAN2.Operatng_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN2.Operatng_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN2.Operatng_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN2.Operatng_Time1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00200640)   //接收到的是"直流电压采样数值帧"
	{
		Skiip4_CAN2.DC_Voltage2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN2.DC_Voltage1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00200A40)   //接收到的是"IGBT模块温度数值帧"
	{
		Skiip4_CAN2.IGBT_Temp2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN2.IGBT_Temp1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00200B40)   //接收到的是"环境温度采样数值帧"
	{
		Skiip4_CAN2.Ambient_Temp2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN2.Ambient_Temp1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x01205040)
	{
		Skiip4_CAN2.Current_Error2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN2.Current_Error1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x02205040)
	{
		Skiip4_CAN2.Current_Error1_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN2.Current_Error1_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN2.Current_Error1_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN2.Current_Error1_Time1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x03205040)
	{
		Skiip4_CAN2.Last_Error2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN2.Last_Error1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x04205040)
	{
		Skiip4_CAN2.Last_Error1_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN2.Last_Error1_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN2.Last_Error1_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN2.Last_Error1_Time1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x0505040)
	{
		Skiip4_CAN2.LastTwo_Error2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN2.LastTwo_Error1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x06205040)
	{
		Skiip4_CAN2.LastTwo_Error1_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN2.LastTwo_Error1_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN2.LastTwo_Error1_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN2.LastTwo_Error1_Time1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x0705040)
	{
		Skiip4_CAN2.LastThree_Error2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN2.LastThree_Error1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x08205040)
	{
		Skiip4_CAN2.LastThree_Error1_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN2.LastThree_Error1_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN2.LastThree_Error1_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN2.LastThree_Error1_Time1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x0905040)
	{
		Skiip4_CAN2.LastFour_Error2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN2.LastFour_Error1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x0A205040)
	{
		Skiip4_CAN2.LastFour_Error1_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN2.LastFour_Error1_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN2.LastFour_Error1_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN2.LastFour_Error1_Time1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00205140)   //接收到的是"IGBT模块的Vce电压保护寄存帧"
	{
		Skiip4_CAN2.LastVce_Error2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN2.LastVce_Error1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00205240)   //接收到的是"IGBT模块的Vce电压保护时间寄存帧"
	{
		Skiip4_CAN2.LastVce_Error1_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN2.LastVce_Error1_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN2.LastVce_Error1_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN2.LastVce_Error1_Time1 = ((Data_High) & 0x000F);
	}	
}
//**********end of Skiip4_CAN2_Receive()***********//


//**********C相Skiip4的CAN接收程序************//
void Skiip4_CAN3_Receive(Uint32 Data_High, Uint32 Data_Low)
{
	if(Data_Low ==  0x00200340)   //接收到的是"运行时间帧"
	{
		Skiip4_CAN3.Operatng_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN3.Operatng_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN3.Operatng_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN3.Operatng_Time1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00200640)   //接收到的是"直流电压采样数值帧"
	{
		Skiip4_CAN3.DC_Voltage2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN3.DC_Voltage1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00200A40)   //接收到的是"IGBT模块温度数值帧"
	{
		Skiip4_CAN3.IGBT_Temp2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN3.IGBT_Temp1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00200B40)   //接收到的是"环境温度采样数值帧"
	{
		Skiip4_CAN3.Ambient_Temp2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN3.Ambient_Temp1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x01205040)
	{
		Skiip4_CAN3.Current_Error2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN3.Current_Error1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x02205040)
	{
		Skiip4_CAN3.Current_Error1_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN3.Current_Error1_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN3.Current_Error1_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN3.Current_Error1_Time1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x03205040)
	{
		Skiip4_CAN3.Last_Error2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN3.Last_Error1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x04205040)
	{
		Skiip4_CAN3.Last_Error1_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN3.Last_Error1_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN3.Last_Error1_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN3.Last_Error1_Time1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x0505040)
	{
		Skiip4_CAN3.LastTwo_Error2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN3.LastTwo_Error1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x06205040)
	{
		Skiip4_CAN3.LastTwo_Error1_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN3.LastTwo_Error1_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN3.LastTwo_Error1_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN3.LastTwo_Error1_Time1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x0705040)
	{
		Skiip4_CAN3.LastThree_Error2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN3.LastThree_Error1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x08205040)
	{
		Skiip4_CAN3.LastThree_Error1_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN3.LastThree_Error1_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN3.LastThree_Error1_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN3.LastThree_Error1_Time1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x0905040)
	{
		Skiip4_CAN3.LastFour_Error2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN3.LastFour_Error1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x0A205040)
	{
		Skiip4_CAN3.LastFour_Error1_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN3.LastFour_Error1_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN3.LastFour_Error1_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN3.LastFour_Error1_Time1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00205140)   //接收到的是"IGBT模块的Vce电压保护寄存帧"
	{
		Skiip4_CAN3.LastVce_Error2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN3.LastVce_Error1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00205240)   //接收到的是"IGBT模块的Vce电压保护时间寄存帧"
	{
		Skiip4_CAN3.LastVce_Error1_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN3.LastVce_Error1_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN3.LastVce_Error1_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN3.LastVce_Error1_Time1 = ((Data_High) & 0x000F);
	}	
}
//**********end of Skiip4_CAN3_Receive()***********//
