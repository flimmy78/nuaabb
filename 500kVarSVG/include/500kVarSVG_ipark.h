/* =================================================================================
File name:       IPARK.H                     
                    
Originator:	Digital Control Systems Group
			Texas Instruments

Description: 
Header file containing constants, data type definitions, and 
function prototypes for the IPARK.

=====================================================================================
 *  HISTORY  :
 *    DATE£º2012.12.20            VERSION:V1.0        AUTHOR£ºWangWanBao            NOTE                                              
------------------------------------------------------------------------------*/

typedef struct { float32  f32AlphaP;  	 // Output: stationary AlphaP-axis stator variable 
				 float32  f32BetaP;	     // Output: stationary BetaP-axis stator variable
				 float32  f32AlphaN;  	 // Output: stationary AlphaN-axis stator variable 
				 float32  f32BetaN;	     // Output: stationary BetaN-axis stator variable 
				 float32  f32Angle;	     // Input: rotating angle (rad)  unused
				 float32  f32Sin_Theta;  // Input: Anti-PARK Theta sin
                 float32  f32Cos_Theta;  // Input: Anti-PARK Theta cos
				 float32  f32DPs;		 // Input: rotating dP-axis stator variable 
				 float32  f32QPs;		 // Input: rotating qP-axis stator variable
				 float32  f32DNs;		 // Input: rotating dN-axis stator variable 
				 float32  f32QNs;		 // Input: rotating qN-axis stator variable 
		 	 	 void  (*calc)();	     // Pointer to calculation function 
				        } IPARK;	            

extern IPARK IGrid_IPark;

typedef IPARK *IPARK_handle;

/*-----------------------------------------------------------------------------
Default initalizer for the IPARK object.
-----------------------------------------------------------------------------*/                     
#define IPARK_DEFAULTS {  0, \
                          0, \
                          0, \
                          0, \
                          0, \
						  0, \
						  0, \
						  0, \
                          0, \
						  0, \
						  0, \
              			  (void (*)(Uint32))ipark_calc }

/*------------------------------------------------------------------------------
Prototypes for the functions in IPARK.C
------------------------------------------------------------------------------*/
extern void ipark_calc(IPARK_handle);

