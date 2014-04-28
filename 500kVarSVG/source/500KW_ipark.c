 /*
 *  FILENAME : IPARK.C
 *
 *  PURPOSE  : Inverse Park Transformation
 *  
 *  HISTORY  :
 *    DATE            VERSION        AUTHOR            NOTE
 *    2012             v1.0        wangwanbao
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

#include "DSP2833x_Device.h"		  // Peripheral address definitions
#include "500KW_MAIN_HEADER.h"				// Main include file



void ipark_calc(IPARK *v)
{	
     // Possitive
	 v->f32AlphaP = v->f32DPs * v->f32Cos_Theta - v->f32QPs * v->f32Sin_Theta;
     v->f32BetaP = v->f32QPs * v->f32Cos_Theta + v->f32DPs * v->f32Sin_Theta;
     
	 //negative
     v->f32AlphaN = v->f32DNs * v->f32Cos_Theta + v->f32QNs * v->f32Sin_Theta;
     v->f32BetaN = v->f32QNs * v->f32Cos_Theta - v->f32DNs * v->f32Sin_Theta;   
}
