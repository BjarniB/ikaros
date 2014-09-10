
#ifndef RobotRecorder_
#define RobotRecorder_

#include "IKAROS.h"

class RobotRecorder: public Module
{
public:
    static Module * Create(Parameter * p) { return new RobotRecorder(p); }

    RobotRecorder(Parameter * p) : Module(p) {}
    virtual ~RobotRecorder();

    void 		Init();
    void 		Tick();

    // pointers to inputs and outputs
    // and integers to represent their sizes

    float *     input_array;
    int         input_array_size;
    
    //float *     input_state;
    //int         input_state_size;

    float *     output_array;
    int         output_array_size;
    
    


    // internal data storage

    float *     internal_array;
    int    internal_array_size;

    // parameter values

  int 	ticks;
  int maxTicks;
  int state; // 0 - record, 1 - replay
  int prevState; // state for previous tick
  int stopTick; // tick number where we stop recording
  
  

};

#endif
