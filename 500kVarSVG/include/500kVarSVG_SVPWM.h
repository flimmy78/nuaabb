/* =================================================================================
File name:       SVGEN_DQ.H  (IQ version)                    
                    
Originator:	Digital Control Systems Group
			Texas Instruments

Description: 
Header file containing constants, data type definitions, and 
function prototypes for the SVGEN_DQ.
=====================================================================================
 History:
-------------------------------------------------------------------------------------
 04-15-2005	Version 3.20                                                 
------------------------------------------------------------------------------*/
#ifndef __SVGEN_DQ_H__
#define __SVGEN_DQ_H__

typedef struct 	{ float32  Ualpha; 			// Input: reference alpha-axis phase voltage 
				          float32  Ubeta;	// Input: reference beta-axis phase voltage 
				          float32  Ta;		// Output: reference phase-a switching function		
				          float32  Tb;		// Output: reference phase-b switching function 
				          float32  Tc;		// Output: reference phase-c switching function
				          float32  Taa;     // Output: trest reference phase-A switching 
				          float32  Tba;     // Output: trest reference phase-B switching
				          float32  Tca;     // Output: trest reference phase-C switching
				          void (*calc)();	    // Pointer to calculation function
				        } SVGENDQ;
																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																																				
typedef SVGENDQ *SVGENDQ_handle;
/*-----------------------------------------------------------------------------
Default initalizer for the SVGENDQ object.
-----------------------------------------------------------------------------*/                     
#define SVGENDQ_DEFAULTS { 0,0,0,0,0,0,0,0, \
                       (void (*)(Uint32))svgendq_calc }

/*------------------------------------------------------------------------------
Prototypes for the functions in SVGEN_DQ.C
------------------------------------------------------------------------------*/
void svgendq_calc(SVGENDQ_handle);
#endif // __SVGEN_DQ_H__
