//###########################################################################
//
// FILE:	DSP2833x_ECan.c
//
// TITLE:	DSP28133 Enhanced CAN Initialization & Support Functions.
//
//###########################################################################
//
//  Ver | dd mmm yyyy | Who  | Description of changes
// =====|=============|======|===============================================
//  1.00| 11 Sep 2003 | H.J. | Changes since previous version (v.58 Alpha)
//      |             |      | Added several initialization routines
// =====|=============|======|===============================================
// 1.00a| 21 Jul 2004 | H.J. | Used shadow register for all CANMC accesses
//###########################################################################
#include "DSP2833x_Device.h"		// Peripheral address definitions
#include "500kVar_SVG.h"				// Main include file

//---------------------------------------------------------------------------
// InitECan:
 
//---------------------------------------------------------------------------
// This function initializes the eCAN module to a known state.
//
void InitECan(void)
{
/* Create a shadow register structure for the CAN control registers. This is
 needed, since only 32-bit access is allowed to these registers. 16-bit access
 to these registers could potentially corrupt the register contents or return
 false data. */

struct ECAN_REGS ECanaShadow;

    EALLOW;     // EALLOW enables access to protected bits

/* Configure eCAN RX and TX pins for CAN operation using eCAN regs*/
    ECanaShadow.CANTIOC.all = ECanaRegs.CANTIOC.all;
    ECanaShadow.CANTIOC.bit.TXFUNC = 1;
    ECanaRegs.CANTIOC.all = ECanaShadow.CANTIOC.all;

    ECanaShadow.CANRIOC.all = ECanaRegs.CANRIOC.all;
    ECanaShadow.CANRIOC.bit.RXFUNC = 1;
    ECanaRegs.CANRIOC.all = ECanaShadow.CANRIOC.all;

/* Configure eCAN for SCC mode - (ready to access mailboxes 0 to 15) */
    ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.STM = 0;
    ECanaShadow.CANMC.bit.SCB = 0;
    ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;

/* Initialize all bits of 'Message Control Register' to zero */
// Some bits of MSGCTRL register come up in an unknown state. For proper operation,
// all bits (including reserved bits) of MSGCTRL must be initialized to zero

    ECanaMboxes.MBOX0.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX1.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX2.MSGCTRL.all = 0x00000000;
    ECanaMboxes.MBOX3.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX4.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX5.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX6.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX7.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX8.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX9.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX10.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX11.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX12.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX13.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX14.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX15.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX16.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX17.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX18.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX19.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX20.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX21.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX22.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX23.MSGCTRL.all = 0x00000000;
//   ECanaMboxes.MBOX24.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX25.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX26.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX27.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX28.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX29.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX30.MSGCTRL.all = 0x00000000;
//    ECanaMboxes.MBOX31.MSGCTRL.all = 0x00000000;

// TAn, RMPn, GIFn bits are all zero upon reset and are cleared again
//  as a matter of precaution.

// 清除所有的TA位
    ECanaRegs.CANTA.all = 0xFFFFFFFF;   /* Clear all TAn bits */
//清除所有的RMP位
    ECanaRegs.CANRMP.all = 0xFFFFFFFF;  /* Clear all RMPn bits */
//清除所有中断标志位
    ECanaRegs.CANGIF0.all = 0xFFFFFFFF; /* Clear all interrupt flag bits */
    ECanaRegs.CANGIF1.all = 0xFFFFFFFF;

/* Configure bit timing parameters for eCANA*/

    ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
    ECanaShadow.CANMC.bit.CCR = 1 ;            // Set CCR = 1
    ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
    // Wait until the CPU has been granted permission to change the configuration registers
    do
    {
      ECanaShadow.CANES.all = ECanaRegs.CANES.all;
    } while(ECanaShadow.CANES.bit.CCE != 1 );       // Wait for CCE bit to be set..

  /* The following block is only for 60 MHz SYSCLKOUT. (30 MHz CAN module clock Bit rate = 1 Mbps
       See Note at end of file. */
    ECanaShadow.CANBTC.bit.BRPREG = 39;
    ECanaShadow.CANBTC.bit.TSEG2REG = 2;
    ECanaShadow.CANBTC.bit.TSEG1REG = 10;

    ECanaShadow.CANBTC.bit.SAM = 1;
    ECanaRegs.CANBTC.all = ECanaShadow.CANBTC.all;

    ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
    ECanaShadow.CANMC.bit.CCR = 0 ;            // Set CCR = 0
    ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;

    // Wait until the CPU no longer has permission to change the configuration registers
    do
    {
      ECanaShadow.CANES.all = ECanaRegs.CANES.all;
    } while(ECanaShadow.CANES.bit.CCE != 0 );       // Wait for CCE bit to be  cleared..

/* Disable all Mailboxes  */
    ECanaRegs.CANME.all = 0;        // Required before writing the MSGIDs
    EDIS;
}	

void InitECan1(void)
{
	struct ECAN_REGS ECanaShadow;
	
	//********************************************
	//*************PART Initialization*************
	//the following part focus on initializationtion
	EALLOW;
	ECanaShadow.CANMC.all=ECanaRegs.CANMC.all;
	//to work in normal mode
	ECanaShadow.CANMC.bit.STM=0;
	//to work in eCAN mode
	//***ECanaShadow.CANMC.bit.STM=1; //it was SCM, but not found then changed to STM
	ECanaRegs.CANMC.all=ECanaShadow.CANMC.all;
	EDIS;

	
	//Step 1, set CANME to 0 to disable the mailboxes
	ECanaRegs.CANME.all=0;
	//Step 2, apply to change data area through CANMC
	EALLOW;
	ECanaShadow.CANMC.all = ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.CDR = 1;
	ECanaRegs.CANMC.all = ECanaShadow.CANMC.all;
	EDIS;
	//Step 3, config the ID,control, data, direction of the mailboxes
	//mailbox 0 config as  acceptting box
	EALLOW;
	ECanaShadow.CANMD.all = ECanaRegs.CANMD.all;
	ECanaShadow.CANMD.all = 0x00000007;       //0-2号邮箱为接收邮箱，3号邮箱为发送邮箱
	ECanaRegs.CANMD.all= ECanaShadow.CANMD.all;
	EDIS;
	//config the acceptting box's ID
	ECanaMboxes.MBOX0.MSGID.all = 0x0C040000; //Skiip4_1 接收邮箱
	ECanaMboxes.MBOX1.MSGID.all = 0x0C080000; //Skiip4_2 接收邮箱
	ECanaMboxes.MBOX2.MSGID.all = 0x0C100000; //Skiip4_3 接收邮箱
	ECanaMboxes.MBOX3.MSGID.all = 0x0C040000; //Skiip4_1 发送邮箱

	//config data length as 8-bit
	ECanaMboxes.MBOX0.MSGCTRL.bit.DLC = 8;
	ECanaMboxes.MBOX1.MSGCTRL.bit.DLC = 8;
	ECanaMboxes.MBOX2.MSGCTRL.bit.DLC = 8;
	ECanaMboxes.MBOX3.MSGCTRL.bit.DLC = 8;
	//config transmitting prior
	ECanaMboxes.MBOX3.MSGCTRL.bit.TPL=0;
	//no long-distance answer frame was applied
	ECanaMboxes.MBOX3.MSGCTRL.bit.RTR=0;
	//write data to boxes' RAM
	ECanaMboxes.MBOX3.MDL.all=0x0;
	ECanaMboxes.MBOX3.MDH.all=0x0;
	//Step 4, apply for normal operations
	EALLOW;
	ECanaShadow.CANMC.all=ECanaRegs.CANMC.all;
	ECanaShadow.CANMC.bit.CDR=0;
	ECanaRegs.CANMC.all=ECanaShadow.CANMC.all;
	//Step 5,set CANME to 1 to enable mailboxes
	ECanaShadow.CANME.all=ECanaRegs.CANME.all;
	ECanaShadow.CANME.all= 0x0000000F;
	ECanaRegs.CANME.all=ECanaShadow.CANME.all;
	EDIS;

	EALLOW; //enable mailbox interrupts
	ECanaRegs.CANMIM.all = 0x00000007;
	ECanaRegs.CANMIL.all = 0x00000000; //interrupt will be generated at ECAN0INT
    ECanaRegs.CANGIF0.all=0xFFFFFFFF;
	ECanaRegs.CANGIM.bit.I0EN= 1;      //ECAN0INT interrupt application permitted
	EDIS;

	EALLOW;
	ECanaShadow.CANTA.all = ECanaRegs.CANTA.all;
	ECanaShadow.CANTRS.bit.TRS3 = 1;
	ECanaRegs.CANTRS.all = ECanaShadow.CANTRS.all;
	EDIS;

    PieCtrlRegs.PIEIER9.bit.INTx5 = 1;
		
	//*************PART Initialization END*********
	//*********************************************
}



