#include "RobotRecorder.h"
#include<iostream>
#include <math.h>

using namespace ikaros;
using namespace std;


void
RobotRecorder::Init()
{

    Bind(maxTicks, "maxTicks");
    maxTicks = 10000;
    
    Bind(state, "state");
    state = 0;
    
     ticks = 0;
     

     stopTick = 0;
    

    prevState = state;
    
    
    
    internal_array = create_array(maxTicks);
    
    
    input_array = GetInputArray("INPUT");
    input_array_size = GetInputSize("INPUT");
    
    //input_state = GetInputArray("INPUT_STATE");
    //input_state_size = GetInputSize("INPUT_STATE");

    output_array = GetOutputArray("OUTPUT");
    output_array_size = GetOutputSize("OUTPUT");


  
}



RobotRecorder::~RobotRecorder()
{
  destroy_array(internal_array);
}




void
RobotRecorder::Tick()
{
    //state = input_state[0];
    if (prevState != state)  {
	    stopTick = ticks;
	    ticks = 0;
    }
  
	if (state == 0 && ticks < maxTicks){
	  internal_array[ticks++] = input_array[0];
	  printf("INPUT: %f\n",input_array[0]);
	  
} else if (state == 1 &&  ticks < maxTicks) {
	  if (ticks == stopTick)
	    ticks = 0;
	  
	   printf("OUTPUT: %i %f\n",ticks, internal_array[ticks]);
	
	    output_array[0] = internal_array[ticks++];
	  
	 
}

prevState = state;
}

// Install the module. This code is executed during start-up.

static InitClass init("RobotRecorder", &RobotRecorder::Create, "Source/UserModules/RobotRecorder/");


