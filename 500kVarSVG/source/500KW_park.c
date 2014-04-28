 /*
 *  FILENAME : PARK.C
 *
 *  PURPOSE  : Park Transformation
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

#include "DSP2833x_Device.h"			  // Peripheral address definitions
#include "500KW_MAIN_HEADER.h"				// Main include file


void park_calc(PARK *v)
{	
     // ÕýÐò
     v->f32DPs = v->f32AlphaP * v->f32Cos_Theta + v->f32BetaP * v->f32Sin_Theta;
     v->f32QPs = v->f32BetaP * v->f32Cos_Theta - v->f32AlphaP * v->f32Sin_Theta;
	
	 // ¸ºÐò
	 v->f32DNs = v->f32AlphaN * v->f32Cos_Theta - v->f32BetaN * v->f32Sin_Theta;
     v->f32QNs = v->f32BetaN * v->f32Cos_Theta + v->f32AlphaN * v->f32Sin_Theta; 

}


