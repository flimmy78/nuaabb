/*=============================================================================*
 *         Copyright(c) 2009-2011,    ALL RIGHTS RESERVED
 *
 *  FILENAME : Adc.c 
 *
 *  PURPOSE  : ADC Module initialization configuration and basic data process
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

#include "DSP2833x_Device.h"			     // Peripheral address definitions
#include "500kVar_SVG.h"					// Main include file

/*=============================================================================*
 * FUNCTION: InitSPI()
 * PURPOSE : ADC hardware module initialization.
 * INPUT: 
 *     void
 *
 * RETURN: 
 *     void
 *
 * CALLS: 
 *     void
 *
 * CALLED BY: 
 *     Main.c  
 * 
 *============================================================================*/
void InitSpi(void)
{ // start of InitSpi()
	
	SpiaRegs.SPICCR.all = 0x000F; //�����ʼ״̬��������������������Բ��ֹ��8λ
	SpiaRegs.SPICTL.all = 0x0002; //������SPIʱ�ӷ�ʽ������ģʽ��ʹ�ܷ��ͣ���ֹ�ж� 
	SpiaRegs.SPIBRR = 0x007F; //������ 1Mhz
	SpiaRegs.SPICCR.all = 0x008F; //�˳���ʼ״̬ 
	SpiaRegs.SPIPRI.bit.FREE = 1;


} // end of InitAdc()

void Spi_fifo_init()
{
	SpiaRegs.SPIFFTX.all = 0xE040;
	SpiaRegs.SPIFFRX.all = 0x204f;
	SpiaRegs.SPIFFCT.all = 0x0;

}
//--- end of file -----------------------------------------------------
