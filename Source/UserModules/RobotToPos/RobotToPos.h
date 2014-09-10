
#ifndef RobotToPos_
#define RobotToPos_

#include "IKAROS.h"

class RobotToPos: public Module
{
public:
    static Module * Create(Parameter * p) { return new RobotToPos(p); }

    RobotToPos(Parameter * p) : Module(p) {}
    virtual ~RobotToPos();

    void 		Init();
    void 		Tick();

    // pointers to inputs and outputs
    // and integers to represent their sizes

    float *     input_array;
    int         input_array_size;

    float *     output_array;
    int         output_array_size;


    // internal data storage

  //float *     internal_array;
    //float **    internal_matrix;

    // parameter values
  float       angle;

};

#endif
