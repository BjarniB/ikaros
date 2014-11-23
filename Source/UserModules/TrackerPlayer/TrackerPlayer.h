
#ifndef TrackerPlayer_
#define TrackerPlayer_

#include "IKAROS.h"

class TrackerPlayer: public Module
{
public:
  static Module * Create(Parameter * p) { return new TrackerPlayer(p); }

  TrackerPlayer(Parameter * p) : Module(p) {}
  virtual ~TrackerPlayer();

  void 		Init();
  void 		Tick();

    // pointers to inputs and outputs
    // and integers to represent their sizes

  float *     input_file_array;
  int         input_file_array_size;

  float *    input_pos_array;
  int         input_pos_array_size;

  float *     output_pos_array;
  int         output_pos_array_size;

  float *     output_torque_array;
  int         output_torque_array_size;


    // internal data storage

  float tickPos[20][10000];

    //offset to map tracker data to robot-arm start position
  float startPos[20];
  float startOffset[20];


    // parameter values

  int ticks;
  int maxTicks;
  int playOnRead; // 0: wait for raw data to be loaded, 1: play as we read raw data

  int prevState; // state for previous tick
  int stopTick; // tick number where we stop recording
  bool readingFile;

  int amtServo;

  float lineSum;
  bool reachedFileEnd;

  bool switcher;

  //constants
  int RECORD = 0;
  int PLAY = 1;
  
  

};

#endif
