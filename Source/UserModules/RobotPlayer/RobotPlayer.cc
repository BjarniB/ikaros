#include "RobotPlayer.h"
#include<iostream>
#include <math.h>

using namespace ikaros;
using namespace std;


void
RobotPlayer::Init()
{

    Bind(maxTicks, "maxTicks");
    maxTicks = 10000;
    
    Bind(state, "state");
    state = 0;
    
     ticks = 0;
     

     stopTick = 0;
    

    prevState = state;
    
    
    
    internal_array_serv1 = create_array(maxTicks);
     internal_array_serv2 = create_array(maxTicks);
      internal_array_serv3 = create_array(maxTicks);
      
       internal_load_serv1 = create_array(maxTicks);
     internal_load_serv2 = create_array(maxTicks);
      internal_load_serv3 = create_array(maxTicks);
    
    
    input_array = GetInputArray("INPUT");
    input_array_size = GetInputSize("INPUT");
    
    input_load_array = GetInputArray("INPUT_LOAD");
     input_load_array_size = GetInputSize("INPUT_LOAD");
    
    input_state = GetInputArray("INPUT_STATE");
    input_state_size = GetInputSize("INPUT_STATE");
    
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
	
    state = input_state[0];
    if (prevState != state)  {
	    stopTick = ticks;
	    ticks = 0;
    }
  
	if (state == 0 && ticks < maxTicks){
	  internal_array_serv1[ticks] = input_array[0];
	  internal_array_serv2[ticks] = input_array[1];
	  internal_array_serv3[ticks] = input_array[2];
	  //printf("INPUT1: %f\n",input_array[0]);
	  //printf("INPUT2: %f\n",input_array[1]);
	  //printf("INPUT3: %f\n",input_array[2]);
	  

	  internal_load_serv1[ticks] = input_load_array[0];
	  internal_load_serv2[ticks] = input_load_array[1];
	  internal_load_serv3[ticks] = input_load_array[2];
	  
	  
	  ticks++;
	  
} else if (state == 1 &&  ticks < maxTicks) {

	  if (ticks >= stopTick) {
	 
	    ticks = 0;
	    
	  }
	  
	  printf("output1: %f\n",internal_array_serv1[stopTick]);
	    
	    output_array[0] = 60;
	    output_array[1] = 300;
	    output_array[2] = 250;
	    
	    output_load_array[0] = abs(internal_load_serv1[ticks]);
	    output_load_array[1] = abs(internal_load_serv2[ticks]);
	    output_load_array[2] = abs(internal_load_serv3[ticks]);
	     
	    float diff1 = internal_array_serv1[ticks] - input_array[0];
	    float diff2 = internal_array_serv2[ticks] - input_array[1];
	    float diff3 = internal_array_serv3[ticks] - input_array[2];
	    
	   
	    output_diff_array[0] = diff1;
	    output_diff_array[1] = diff2;
	    output_diff_array[2] = diff3;
	    
	   
	   
	 
	
	   ticks++;
	
}

prevState = state;
  
}

// Install the module. This code is executed during start-up.

static InitClass init("RobotPlayer", &RobotPlayer::Create, "Source/UserModules/RobotPlayer/");


