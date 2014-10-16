
#ifndef RobotPlayer_
#define RobotPlayer_

#include "IKAROS.h"

class RobotPlayer: public Module
{
public:
    static Module * Create(Parameter * p) { return new RobotPlayer(p); }

    RobotPlayer(Parameter * p) : Module(p) {}
    virtual ~RobotPlayer();

    void 		Init();
    void 		Tick();

    // pointers to inputs and outputs
    // and integers to represent their sizes

    float *     input_array;
    int         input_array_size;
    
    float *     input_state;
    int         input_state_size;
    
    float * 	input_load_array;
    int 	input_load_array_size;


    float *     output_array;
    int         output_array_size;
    
     float *    output_diff_array;
    int         output_diff_array_size;
    
    float *	output_load_array;
    int 	output_load_array_size;
    
    


    // internal data storage

    float *     internal_array_serv1;
    int    internal_array_size_serv1;
    
    float *     internal_array_serv2;
    int    internal_array_size_serv2;
    
    float *     internal_array_serv3;
    int    internal_array_size_serv3;
    
    
    //load
    float *     internal_load_serv1;
    int    internal_load_size_serv1;
    
    float *     internal_load_serv2;
    int    internal_load_size_serv2;
    
    float *     internal_load_serv3;
    int    internal_load_size_serv3;

    // parameter values
    
    int returnBack;

  int 	ticks;
  int maxTicks;
  int state; // 0 - record, 1 - replay
  int prevState; // state for previous tick
  int stopTick; // tick number where we stop recording
  
  

};

#endif
