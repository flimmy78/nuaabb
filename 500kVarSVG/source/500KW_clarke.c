/*=============================================================================*
 *  Copyright(c) 20012-2013,    ALL RIGHTS RESERVED
 *
 *  FILENAME : CLARKE.C 
 *
 *  PURPOSE  : The Clarke Transformation 
 *  
 *  HISTORY  :
 *  DATE            VERSION        AUTHOR            NOTE
 *    
 *
 *----------------------------------------------------------------------------
 *  GLOBAL VARIABLES
 *    NAME                                    DESCRIPTION
 *      
 *============================================================================*/

#include "DSP2833x_Device.h"			    // Peripheral address definitions
#include "500KW_MAIN_HEADER.h"				  // Main include file

void clarke_calc(CLARKE *v)
{	

	v->f32Alpha = (v->f32Rs) * 0.66666666666667f - (v->f32Ss + v->f32Ts) * 0.33333333333334f;
	v->f32Beta = (v->f32Ss - v->f32Ts)*0.57735026918963f;          // 1/sqrt(3) = 0.57735026918963

	v->f32AlphaDelay = v->f32AlphaOld + (v->f32Alpha - v->f32AlphaDelay) * v->f32FreGrid1 * v->f32FreGrid2;
	v->f32BetaDelay = v->f32BetaOld +  (v->f32Beta - v->f32BetaDelay) * v->f32FreGrid1 * v->f32FreGrid2;

	v->f32AlphaOld = v->f32Alpha;
	v->f32BetaOld = v->f32Beta;

	v->f32AlphaP = (v->f32Alpha - v->f32BetaDelay) * 0.5;
	v->f32AlphaN = (v->f32Alpha + v->f32BetaDelay) * 0.5;

	v->f32BetaP = (v->f32AlphaDelay + v->f32Beta) * 0.5;
	v->f32BetaN = (v->f32Beta - v->f32AlphaDelay) * 0.5;
}


