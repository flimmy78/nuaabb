/* =================================================================================
File name:       500Kw_CLARKE.H                     
                    
Originator:	Digital Control Systems Group
			      Texas Instruments

Description: 
Header file containing constants, data type definitions, and 
function prototypes for the CLARKE.
=====================================================================================
 *  HISTORY  :
 *    DATE:2012.12.20           VERSION:V1.0        AUTHOR:WangWanBao            NOTE
------------------------------------------------------------------------------*/

typedef struct {		
                float32  f32Rs;  	    	// Input: phase-R stator variable  
				float32  f32Ss;		    	// Input: phase-S stator variable
                float32  f32Ts;    	  		// Input: phase-T stator variable
				float32  f32Alpha;	  		// Middle para: stationary Alpha-axis stator variable 
				float32  f32Beta;	    	// Middle para: stationary Beta-axis stator variable 
		 	 	float32	 f32AlphaOld;       // Middle para: used for P-N dispart 
			    float32	 f32BetaOld;		// Middle para: used for P-N dispart 
		 	 	float32	 f32AlphaDelay;		// Middle para: used for P-N dispart 
				float32	 f32BetaDelay;		// Middle para: used for P-N dispart 
				float32	 f32AlphaP; 		// Output:  stationary AlphaP-axis stator variable
				float32  f32BetaP;			// Output:  stationary BetaP-axis stator variable
				float32  f32AlphaN;			// Output:  stationary AlphaN-axis stator variable
				float32	 f32BetaN;			// Output:  stationary BetaN-axis stator variable
				float32	 f32FreGrid1;		// Middle para£º APF parameter
				float32	 f32FreGrid2;		// Middle para£º APF parameter
		 	 	void  (*calc)();      // Pointer to calculation function
				        } CLARKE;	            

extern CLARKE IGrid_Clarke,VGrid_Clarke;

typedef CLARKE *CLARKE_handle;

/*-----------------------------------------------------------------------------
Default initalizer for the CLARKE object.
-----------------------------------------------------------------------------*/                     
#define CLARKE_DEFAULTS { 0, \
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
                          0, \
                          0, \
                          0, \
						  0, \
                          (void (*)(Uint32))clarke_calc }

/*------------------------------------------------------------------------------
Prototypes for the functions in CLARKE.C
------------------------------------------------------------------------------*/
void clarke_calc(CLARKE_handle);
