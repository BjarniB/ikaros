#include "RobotPlayer.h"
#include<iostream>
#include <math.h>

using namespace ikaros;
using namespace std;


void
RobotPlayer::Init()
{

    Bind(maxTicks, "maxTicks");
    maxTicks = 50;
stopTick = 50;
    
     ticks = 0;
     

     

    returnBack = 1;
    
    
    internal_array_serv1 = create_array(maxTicks);
     internal_array_serv2 = create_array(maxTicks);
      internal_array_serv3 = create_array(maxTicks);
      
       internal_load_serv1 = create_array(maxTicks);
     internal_load_serv2 = create_array(maxTicks);
      internal_load_serv3 = create_array(maxTicks);
      
       
      //create target array
     for (int n=0; n<maxTicks; n++) {
    internal_array_serv1[n] = 180;
    internal_array_serv2[n] = 180;
    internal_array_serv3[n] = 50+245/(1+exp(-0.3*n+10.f));//100+n*n/100;
    
  }
 
  
  //torque curve set to 0.2
 
       for (int n=0; n<maxTicks; n++) {
    internal_load_serv1[n] = 0.2;
    internal_load_serv2[n] = 0.2;
    internal_load_serv3[n] = 0.2;

    
  }
    
    
    input_array = GetInputArray("INPUT");
    input_array_size = GetInputSize("INPUT");
    
    input_load_array = GetInputArray("INPUT_LOAD");
     input_load_array_size = GetInputSize("INPUT_LOAD");
    
    output_array = GetOutputArray("OUTPUT");
    output_array_size = GetOutputSize("OUTPUT");
    
    
    // Outputs the load for each servo
    output_load_array = GetOutputArray("OUTPUT_LOAD");
    output_load_array_size = GetOutputSize("OUTPUT_LOAD");

    // Outputs the difference between target position and feedback position
    output_diff_array = GetOutputArray("OUTPUT_DIFF");
    output_diff_array_size = GetOutputSize("OUTPUT_DIFF");


    
   
  
}



RobotPlayer::~RobotPlayer()
{
  destroy_array(internal_array_serv1);
  destroy_array(internal_array_serv2);
  destroy_array(internal_array_serv3);
  
   destroy_array(internal_load_serv1);
  destroy_array(internal_load_serv2);
  destroy_array(internal_load_serv3);
}



void
RobotPlayer::Tick()
{ 
	
  //returns back to default arm position if 1

   
  
  
	if (returnBack == 1 && ticks < maxTicks){
	 printf("RETURNING BACK tick: %i\n", ticks);
	    
	    output_array[0] = 180;
	    output_array[1] = 180;
	    output_array[2] = 50;
	    
	    output_load_array[0] = 1.0;
	    output_load_array[1] = 1.0;
	    output_load_array[2] = 1.0;
	  
	  
	  ticks++; 
	}else if (returnBack == 1 ) {
	  printf ("FINISHED RETURNING BACK\n");
	  
	
	    ticks = 0;
	    returnBack = 0;
	 
	  
} else if (returnBack == 0 &&  ticks < maxTicks) {
   //sending torque
 
	 output_load_array[0] = internal_load_serv1[ticks];
	 output_load_array[1] = internal_load_serv2[ticks];
	 output_load_array[2] = internal_load_serv3[ticks];
 
	    output_array[0] = 180;
	    output_array[1] = 180;
	    output_array[2] = 300;
	    
	      
	  float pos_current_1 = input_array[0];
	  float pos_target_1 = internal_array_serv1[ticks];
	    
	   float pos_current_2 = input_array[1];
	  float pos_target_2 = internal_array_serv2[ticks];
	    
	  float pos_current_3 = input_array[2];
	  float pos_target_3 = internal_array_serv3[ticks];
	  
	  
	  
	  
	  printf("Current pos: %f \n", pos_current_3);
	  printf("Target pos: %i : %f\n", ticks, pos_target_3);
	  
	 float diff1 = pos_target_1 - pos_current_1;
	 float diff2 = pos_target_2 - pos_current_2;
	 float diff3 = pos_target_3 - pos_current_3;
	 
	
	 
	 //sending difference
	 output_diff_array[0] = diff1;
	 output_diff_array[1] = diff2;
	 output_diff_array[2] = diff3;
	 
	  printf("Difference: %f \n", diff3);
	  printf("Sending torque: %f \n",internal_load_serv3[ticks]);
	  
	  //adjust torque curve for depending on difference
	    int editTick = ticks-1;
	    float adjustment = 0.02;
	    float limit = 10;

	     if (abs(diff3) > limit && (ticks > 0 && ticks < stopTick)) {
	        printf ("Adjusting tick: %i\n", editTick);
	      if (diff3 > 0.0) {
	      
		internal_load_serv3[editTick] += adjustment;
		internal_load_serv3[editTick-1] += 0.5*adjustment;
		
	      } else {
		internal_load_serv3[editTick] -= adjustment;
		internal_load_serv3[editTick-1] -= 0.5*adjustment;
		
		
	      }
	     }
	     
	    
	    
	    
	    ticks++;
	 


	  
	 

	
	  
	    
	    
} else if (returnBack==0 && ticks>=maxTicks) {
	  
	 printf("REACHED END");
	  returnBack = 1;
	  ticks = 0;
}


}

// Install the module. This code is executed during start-up.

static InitClass init("RobotPlayer", &RobotPlayer::Create, "Source/UserModules/RobotPlayer/");


