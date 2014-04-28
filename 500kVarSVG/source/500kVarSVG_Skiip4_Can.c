
#include "DSP2833x_Device.h"			// Peripheral address definitions
#include "500kVar_SVG.h"					// Main include file

struct Skiip4_CAN  Skiip4_CAN1,Skiip4_CAN2,Skiip4_CAN3;

void Skiip4_CanX_Send (void);
void Can_Skiip4_Receive(Uint32 Data_High, Uint32 Data_Low, Uint8 Skiip4_X[]);
void Skiip4_CAN1_Receive(Uint32 Data_High, Uint32 Data_Low);
void Skiip4_CAN2_Receive(Uint32 Data_High, Uint32 Data_Low);
void Skiip4_CAN3_Receive(Uint32 Data_High, Uint32 Data_Low);

//*******��SKiip4ģ���������ݵ�CAN���ͳ���********//
void Skiip4_CanX_Send (void)
{    	
	struct ECAN_REGS ECanaShadow;
	static Uint16 Skiip4Number_Flag = 0;  //Skiip4ģ��ѡ��,3��Skiip4ģ��ѭ����������
	static Uint16 Index_Flag = 0;         //CAN����Skiip4ģ�鴫�䲻ͬ���ݵı�־
	static Uint32 SubIndex_Flag = 0;      //CAN����Skiip4ģ�鴫�䲻ͬ���ݵ��ӱ�־
	Uint16 Cycle_Flag = 0;

//********�˶�ΪSkiip4-1,2,3ģ���ѭ������*********//	
	if(Skiip4Number_Flag == 0)
	{
		ECanaMboxes.MBOX0.MSGID.all = 0x0C040000; //Skiip1 ��������
		Skiip4Number_Flag++;
	}
	else if(Skiip4Number_Flag == 1)
	{
		ECanaMboxes.MBOX1.MSGID.all = 0x0C080000; //Skiip2 ��������
		Skiip4Number_Flag++;
	}
	else if(Skiip4Number_Flag == 2)
	{
		ECanaMboxes.MBOX2.MSGID.all = 0x0C100000; //Skiip3 ��������
		Skiip4Number_Flag = 0;
		Cycle_Flag++;             //ѭ����ʶ��λ1������3��ģ��ѭ��һ�ν���
	}
//************************************************//

//********�˶�Ϊ����ͬ"����֡"�жϱ�־λ�䶯*********//
	if(Cycle_Flag == 1)   //3��Skiip4ģ��ÿѭ��һ�ν�����һ֡����Index_Flag+1
	{	
		if(Index_Flag == 5)    //��"���ϱ���֡"�µȴ�������Ŀ¼ѭ��һ�κ������һ֡
		{
			if(SubIndex_Flag == 0x0A000000)
			{
				SubIndex_Flag = 0;
				Index_Flag++;  //���е���Ŀ¼ѭ�������Ժ󣬽�����һ������"Vce��ѹ�����Ĵ�֡"ģʽ
			}
		}
		else 	Index_Flag++;
	}
//*****************************************************//
	switch(Index_Flag)
	{
		case 1:   //������"����ʱ��֡"
		{
			ECanaMboxes.MBOX0.MDL.all = 0x00200340;  //��4�ֽ�Ϊ�����ţ������4�ֽ����ݺ���
			ECanaMboxes.MBOX0.MDH.all = 0x0;	
		}
		break;
		case 2:    //������"ֱ����ѹ������ֵ֡"
		{
			ECanaMboxes.MBOX0.MDL.all = 0x00200640;  //��4�ֽ�Ϊ�����ţ������4�ֽ����ݺ���
			ECanaMboxes.MBOX0.MDH.all = 0x0;	
		}
		break;
		case 3:    //������"IGBTģ���¶���ֵ֡"
		{
			ECanaMboxes.MBOX0.MDL.all = 0x00200A40;  //��4�ֽ�Ϊ�����ţ������4�ֽ����ݺ���
			ECanaMboxes.MBOX0.MDH.all = 0x0;	
		}
		break;
		case 4:    //������"�����¶Ȳ�����ֵ֡"
		{
			ECanaMboxes.MBOX0.MDL.all = 0x00200B40;  //��4�ֽ�Ϊ�����ţ������4�ֽ����ݺ���
			ECanaMboxes.MBOX0.MDH.all = 0x0;
		}
		break;
		case 5:    //������"���ϱ���֡"
		{	
			if(Cycle_Flag == 1)
			{
				SubIndex_Flag = SubIndex_Flag + 0x01000000;
			}
			ECanaMboxes.MBOX0.MDL.all = 0x00205040 + SubIndex_Flag;  //��4�ֽ�Ϊ�����ţ������4�ֽ����ݺ���
			ECanaMboxes.MBOX0.MDH.all = 0x0;
		}
		break;
		case 6:    //������"IGBTģ���Vce��ѹ�����Ĵ�֡"
		{
			ECanaMboxes.MBOX0.MDL.all = 0x00205140;  //��4�ֽ�Ϊ�����ţ������4�ֽ����ݺ���
			ECanaMboxes.MBOX0.MDH.all = 0x0;
		}
		break;
		case 7:    //������"IGBTģ���Vce��ѹ����ʱ��Ĵ�֡"
		{
			ECanaMboxes.MBOX0.MDL.all = 0x00205240;  //��4�ֽ�Ϊ�����ţ������4�ֽ����ݺ���
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


//***************����skiip4���ݺ���(32λ����)****************//
/*
void Can_Skiip4_Receive(Uint32 Data_High, Uint32 Data_Low, Uint32 Skiip4_X[])
{
	static int i = 0;

	if(Data_Low ==  0x00200340)   //���յ�����"����ʱ��֡"
	{
		Skiip4_X[0] = Data_High;
	}
	if(Data_Low ==  0x00200640)   //���յ�����"ֱ����ѹ������ֵ֡"
	{
		Skiip4_X[1] = Data_High;
	}
	if(Data_Low ==  0x00200A40)   //���յ�����"IGBTģ���¶���ֵ֡"
	{
		Skiip4_X[2] = Data_High;
	}
	if(Data_Low ==  0x00200B40)   //���յ�����"�����¶Ȳ�����ֵ֡"
	{
		Skiip4_X[3] = Data_High;
	}
	for(i=0;i<=10;i++)            //���յ�����"���ϱ���֡(������Ŀ¼)"
	{
		if(Data_Low ==  0x01205040 + 0x01000000 * i)
		{
			Skiip4_X[4 + i] = Data_High;
		}
	}
	if(Data_Low ==  0x00205140)   //���յ�����"IGBTģ���Vce��ѹ�����Ĵ�֡"
	{
		Skiip4_X[5] = Data_High;
	}
	if(Data_Low ==  0x00205240)   //���յ�����"IGBTģ���Vce��ѹ����ʱ��Ĵ�֡"
	{
		Skiip4_X[6] = Data_High;
	}	
}
*/

//***************����skiip4���ݺ���(8λ����)****************//
void Can_Skiip4_Receive(Uint32 Data_High, Uint32 Data_Low, Uint8 Skiip4_X[])
{
	static int i = 0;

	if(Data_Low ==  0x00200340)   //���յ�����"����ʱ��֡"
	{
		Skiip4_X[0] = ((Data_High>>24) & 0x000F);
		Skiip4_X[1] = ((Data_High>>16) & 0x000F);
		Skiip4_X[2] = ((Data_High>>8) & 0x000F);
		Skiip4_X[3] = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00200640)   //���յ�����"ֱ����ѹ������ֵ֡"
	{
		Skiip4_X[4] = ((Data_High>>8) & 0x000F);
		Skiip4_X[5] = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00200A40)   //���յ�����"IGBTģ���¶���ֵ֡"
	{
		Skiip4_X[6] = ((Data_High>>8) & 0x000F);
		Skiip4_X[7] = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00200B40)   //���յ�����"�����¶Ȳ�����ֵ֡"
	{
		Skiip4_X[8] = ((Data_High>>8) & 0x000F);
		Skiip4_X[9] = ((Data_High) & 0x000F);
	}
	for(i=0;i<10;i++)            //���յ�����"���ϱ���֡(������Ŀ¼)"
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
	if(Data_Low ==  0x00205140)   //���յ�����"IGBTģ���Vce��ѹ�����Ĵ�֡"
	{
		Skiip4_X[40] = ((Data_High>>8) & 0x000F);
		Skiip4_X[41] = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00205240)   //���յ�����"IGBTģ���Vce��ѹ����ʱ��Ĵ�֡"
	{
		Skiip4_X[42] = ((Data_High>>24) & 0x000F);
		Skiip4_X[43] = ((Data_High>>16) & 0x000F);
		Skiip4_X[44] = ((Data_High>>8) & 0x000F);
		Skiip4_X[45] = ((Data_High) & 0x000F);
	}	
}
//***************end of Can_Skiip4_Receive()**************//

//**********A��Skiip4��CAN���ճ���************//
void Skiip4_CAN1_Receive(Uint32 Data_High, Uint32 Data_Low)
{
	if(Data_Low ==  0x00200340)   //���յ�����"����ʱ��֡"
	{
		Skiip4_CAN1.Operatng_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN1.Operatng_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN1.Operatng_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN1.Operatng_Time1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00200640)   //���յ�����"ֱ����ѹ������ֵ֡"
	{
		Skiip4_CAN1.DC_Voltage2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN1.DC_Voltage1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00200A40)   //���յ�����"IGBTģ���¶���ֵ֡"
	{
		Skiip4_CAN1.IGBT_Temp2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN1.IGBT_Temp1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00200B40)   //���յ�����"�����¶Ȳ�����ֵ֡"
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
	if(Data_Low ==  0x00205140)   //���յ�����"IGBTģ���Vce��ѹ�����Ĵ�֡"
	{
		Skiip4_CAN1.LastVce_Error2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN1.LastVce_Error1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00205240)   //���յ�����"IGBTģ���Vce��ѹ����ʱ��Ĵ�֡"
	{
		Skiip4_CAN1.LastVce_Error1_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN1.LastVce_Error1_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN1.LastVce_Error1_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN1.LastVce_Error1_Time1 = ((Data_High) & 0x000F);
	}	
}
//**********end of Skiip4_CAN1_Receive()***********//


//**********B��Skiip4��CAN���ճ���************//
void Skiip4_CAN2_Receive(Uint32 Data_High, Uint32 Data_Low)
{
	if(Data_Low ==  0x00200340)   //���յ�����"����ʱ��֡"
	{
		Skiip4_CAN2.Operatng_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN2.Operatng_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN2.Operatng_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN2.Operatng_Time1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00200640)   //���յ�����"ֱ����ѹ������ֵ֡"
	{
		Skiip4_CAN2.DC_Voltage2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN2.DC_Voltage1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00200A40)   //���յ�����"IGBTģ���¶���ֵ֡"
	{
		Skiip4_CAN2.IGBT_Temp2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN2.IGBT_Temp1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00200B40)   //���յ�����"�����¶Ȳ�����ֵ֡"
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
	if(Data_Low ==  0x00205140)   //���յ�����"IGBTģ���Vce��ѹ�����Ĵ�֡"
	{
		Skiip4_CAN2.LastVce_Error2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN2.LastVce_Error1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00205240)   //���յ�����"IGBTģ���Vce��ѹ����ʱ��Ĵ�֡"
	{
		Skiip4_CAN2.LastVce_Error1_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN2.LastVce_Error1_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN2.LastVce_Error1_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN2.LastVce_Error1_Time1 = ((Data_High) & 0x000F);
	}	
}
//**********end of Skiip4_CAN2_Receive()***********//


//**********C��Skiip4��CAN���ճ���************//
void Skiip4_CAN3_Receive(Uint32 Data_High, Uint32 Data_Low)
{
	if(Data_Low ==  0x00200340)   //���յ�����"����ʱ��֡"
	{
		Skiip4_CAN3.Operatng_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN3.Operatng_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN3.Operatng_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN3.Operatng_Time1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00200640)   //���յ�����"ֱ����ѹ������ֵ֡"
	{
		Skiip4_CAN3.DC_Voltage2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN3.DC_Voltage1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00200A40)   //���յ�����"IGBTģ���¶���ֵ֡"
	{
		Skiip4_CAN3.IGBT_Temp2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN3.IGBT_Temp1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00200B40)   //���յ�����"�����¶Ȳ�����ֵ֡"
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
	if(Data_Low ==  0x00205140)   //���յ�����"IGBTģ���Vce��ѹ�����Ĵ�֡"
	{
		Skiip4_CAN3.LastVce_Error2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN3.LastVce_Error1 = ((Data_High) & 0x000F);
	}
	if(Data_Low ==  0x00205240)   //���յ�����"IGBTģ���Vce��ѹ����ʱ��Ĵ�֡"
	{
		Skiip4_CAN3.LastVce_Error1_Time4 = ((Data_High>>24) & 0x000F);
		Skiip4_CAN3.LastVce_Error1_Time3 = ((Data_High>>16) & 0x000F);
		Skiip4_CAN3.LastVce_Error1_Time2 = ((Data_High>>8) & 0x000F);
		Skiip4_CAN3.LastVce_Error1_Time1 = ((Data_High) & 0x000F);
	}	
}
//**********end of Skiip4_CAN3_Receive()***********//
