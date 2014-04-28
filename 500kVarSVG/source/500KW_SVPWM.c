/***************************************************************
File name:        SVPWM_DQ.C(Fix point version)

Designer:    Huhaibing
             NUAA University
Description:
             Space-vector PWM generation based on Ualpha and Ubelta.The 
             results can be directly written to CPMR1,CMPR2,CMPR3 registers.
             
Version:
             09-21-2005   Ver 1.0
***************************************************************/

#include "DSP2833x_Device.h"			// Peripheral address definitions
#include "500KW_MAIN_HEADER.h"				// Main include file

void svgendq_calc(SVGENDQ *v)
 {
 
   float32 Va,Vb,Vc,t1,t2,temp,ta,tb,tc;
   int sector = 0;

// inverse Clarke transformation
   Va 	=  	v->Ubeta;                                  //X
   temp	=	(v->Ualpha * 1.732051f * 0.5f );     
   Vb	=	(-0.5f * v->Ubeta)+ temp;                  //-Z
   Vc	=	(-0.5f * v->Ubeta)- temp;                  //-Y
   
//sector determination
   if(Va>=0)
   sector	=	1;
   if(Vb>=0)
   sector	=	sector+2;
   if(Vc>=0)
   sector	=	sector+4;

   
	if(sector==1)
    {
   	  	t1	= -Vb;
   	  	t2 	= -Vc;
   	}
   	else if(sector==2)
   	{
   	  	t1	=  -Vc;
   	  	t2 	=  -Va;
   	}
   	else if(sector==3)
   	{
   	  	t1 	=   Vb;
   	  	t2 	=   Va;
   	}
   	else if(sector==4)
   	{
   	  	t1 	=   -Va;
   	  	t2 	=   -Vb;
   	}
   	else if(sector==5)
   	{
   	  	t1 	=    Va;
   	  	t2  =    Vc;
   	}
   	else if(sector==6)
   	{
   	  	t1 	=    Vc;
   	  	t2 	=    Vb;
   	}
	else
   	{
   	  	t1 	=   Vb;
   	  	t2 	=   Va;
   	}

// saturations
  temp = t1+t2;   	 
  if(temp > 1)
   {
     temp =(1 / temp);     
     t1 = t1 * temp;
     t2 = t2 * temp;
   }  
// duty cycle computation   
   ta=(1-t1-t2) * 0.5f;
   tb=ta+t1;
   tc=tb+t2;


// PWM dispatcher     
  	if(sector==1)
   	{
     	v->Ta = tb;
     	v->Tb = ta;
     	v->Tc = tc;
   	}
    else if(sector==2)
    {
     	v->Ta = ta;
     	v->Tb = tc;
     	v->Tc = tb;
    }
    else if(sector==3)
    {
     	v->Ta = ta;
     	v->Tb = tb;
     	v->Tc = tc;
    }    
    else if(sector==4)
    {
     	v->Ta = tc;
     	v->Tb = tb;
     	v->Tc = ta;	
    }      
    else if(sector==5)
    {
     	v->Ta = tc;
     	v->Tb = ta;
     	v->Tc = tb;
    } 
   	else if(sector==6)
    {
     	v->Ta = tb;
     	v->Tb = tc;
     	v->Tc = ta;
    }
    else
   	{
     	v->Ta = ta;
     	v->Tb = tb;
     	v->Tc = tc;
    }    
   
// denormalization

  }  
   

