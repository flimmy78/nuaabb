/* =================================================================================
File name:       PARK.H                     
                    
Originator:	Digital Control Systems Group
			Texas Instruments

Description: 
Header file containing constants, data type definitions, and 
function prototypes for the PARK.

=====================================================================================
 *  
 *  HISTORY  :
 *    DATE £º2012.12.20           VERSION£ºV1.0        AUTHOR£ºWangWanBao           NOTE
 *                                                
------------------------------------------------------------------------------*/

typedef struct {  float32  f32AlphaP;  	  // Input: stationary AlphaP-axis stator variable 
				  float32  f32BetaP;	  // Input: stationary BetaP-axis stator variable
				  float32  f32AlphaN;	  // Input: stationary AlphaN-axis stator variable
				  float32  f32BetaN;  	  // Input: stationary BetaN-axis stator variable
				  float32  f32Angle;	  // Input: rotating angle (rad) Unused
				  float32  f32Sin_Theta;  // Input: PARK Theta sin
                  float32  f32Cos_Theta;  // Input: PARK Theta sin
				  float32  f32DPs;		  // Output: rotating dP-axis stator variable 
				  float32  f32QPs;		  // Output: rotating qP-axis stator variable
				  float32  f32DNs;		  // Output: rotating dN-axis stator variable
				  float32  f32QNs;		  // Output: rotating qN-axis stator variable 
		 	 	  void  (*calc)();	      // Pointer to calculation function 
				        } PARK;	            

extern PARK IGrid_Park,VGrid_Park;

typedef PARK *PARK_handle;

/*-----------------------------------------------------------------------------
Default initalizer for the PARK object.
-----------------------------------------------------------------------------*/                     
#define PARK_DEFAULTS {   0, \
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
              			  (void (*)(Uint32))park_calc }
              			  
/*------------------------------------------------------------------------------
Prototypes for the functions in PARK.C
------------------------------------------------------------------------------*/
void park_calc(PARK_handle);


