//
//	TotalRecordKeyframe.cc		This file is a part of the IKAROS project
//
//    Copyright (C) 2012 <Author Name>
//
//    This program is free software; you can redistribute it and/or modify
//    it under the terms of the GNU General Public License as published by
//    the Free Software Foundation; either version 2 of the License, or
//    (at your option) any later version.
//
//    This program is distributed in the hope that it will be useful,
//    but WITHOUT ANY WARRANTY; without even the implied warranty of
//    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
//    GNU General Public License for more details.
//
//    You should have received a copy of the GNU General Public License
//    along with this program; if not, write to the Free Software
//    Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
//
//    See http://www.ikaros-project.org/ for more information.
//
//  This example is intended as a starting point for writing new Ikaros modules
//  The example includes most of the calls that you may want to use in a module.
//  If you prefer to start with a clean example, use he module MinimalModule instead.
//

#include "TotalRecordKeyframe.h"

// use the ikaros namespace to access the math library
// this is preferred to using math.h

using namespace ikaros;
const float ckeyframes = 0.f;
const float cPlayback = 1.f;

void
TotalRecordKeyframe::Init()
{
 
    Bind(repeat, "repeat");
    Bind(equality_tolerance, "equality_tolerance");
    Bind(debug, "debug");

    input_array = GetInputArray("INPUT");
    input_speed = GetInputArray("SPEED");
    input_array_size = GetInputSize("INPUT");
    command = GetInputArray("COMMAND");
    
    printf("Init inputs set\n");


    // Do the same for the outputs
    output = GetOutputArray("OUTPUT");
    torque = GetOutputArray("TORQUE");

    printf("Init outputs set\n");

    keyframes = new std::vector<Keyframe>[input_array_size];
    keyframe_iterator = new std::vector<Keyframe>::iterator[input_array_size];

    frames = new std::vector<Frame>[input_array_size];
    frame_iterator = new std::vector<Frame>::iterator[input_array_size];

    printf("Init vectors set\n");

    for (int i = 0; i < input_array_size; ++i)
    {
        keyframes[i].reserve(10);
        //keyframe_iterator[i] = keyframes[i].begin();
    }

    prevsign = new float[input_array_size];
    reset_array(prevsign, input_array_size);

    tickCounter = new int[input_array_size];

    current_state = eStart;

    reset = true;

    printf("Init done \n");
}



TotalRecordKeyframe::~TotalRecordKeyframe()
{
    // Destroy data structures that you allocated in Init.
    delete[] keyframes;
    delete[] keyframe_iterator;
    delete[] prevsign;
    delete[] tickCounter;
    delete[] frames;
    delete[] frame_iterator;
    // Do NOT destroy data structures that you got from the
    // kernel with GetInputArray, GetInputMatrix etc.
    printf("Frames Stored: %i \n", tick);
}



void
TotalRecordKeyframe::Tick()
{
   
    Command cmd = (Command)command[0];
    if(debug){
        printf("kfr command=%i state before=%i\n", cmd, current_state);
    }

    switch(current_state)
    {

      case eStart:
        if(reset){
            for (int i = 0; i < input_array_size; ++i){
                output[i] = 120;
                torque[i] = 0.002f;
            }
            printf("Reset to defaults\n");
            reset = false;
        }
        if(cmd==eRecord)
        {       
            current_state=eRecording;
            delete[] frames;
            delete[] keyframes;
            keyframes = new std::vector<Keyframe>[input_array_size];
            frames = new std::vector<Frame>[input_array_size];
            tick=0;  
            printf("Recording \n");
        }    
        break;
      case eRecording:
        if(cmd==eProcess){
            current_state=eProcessing;
        }else{
            record();  
        }
        break;
      case eProcessing:
        movingAverage();
        process();
        current_state=ePaused;
        break;
      case ePlaying:
        if(cmd==eReset){
            current_state=ePaused;
        }else{
            play();
        }
        break;
      case ePaused:
        if (cmd==eReset)
        {
            current_state=eStart;
            reset = true;
        }
        if (cmd==ePlay) {
            current_state = ePlaying;
            tick=0;
            for (int i = 0; i < input_array_size; ++i)
                keyframe_iterator[i] = keyframes[i].begin();
        }
      default:
        break;
    }
}


/**
*    Record all positions recieved during keyframes phase
*/
void
TotalRecordKeyframe::record()
{
    // TODO maybe add bool variable to determine if recording or not
    // start recording when moving speed != 0 to avoid alot of frames with speed 0 at start
    for (int i = 0; i < input_array_size; ++i)
    {
        (frames[i]).push_back(Frame(tick, input_array[i], input_speed[i]));
        printf("%i : %i : %f : %f \n",i, tick, input_array[i], input_speed[i]);
        // Simply outputs the input
        //output[i] = input_array[i];
    }
    tick++;
}

void
TotalRecordKeyframe::process()
{
    // TODO test pause frames
    for(int i = 0; i < input_array_size; ++i){

        printf("Enter process: %i \n", i);
        bool pauseFrame = false;
        int index = 1;
        for (index = 1; index < frames[i].size(); ++index)
        {
            bool key = checkFrame(index, i);

            if(frames[i].at(index).moving_speed == 0 && !pauseFrame){
                pauseFrame = true;
                tickCounter[i] = 1;
            }

            if (pauseFrame){
                if(frames[i].at(index).moving_speed != 0){
                    (keyframes[i]).push_back(Keyframe(1, tickCounter[i], (frames[i]).at(index-1).val));
                    printf("%i : %i : %i : %f \n",i, 1, tickCounter[i], (frames[i]).at(index).val);
                    pauseFrame = false;
                }
            }

            if (!pauseFrame){
                if(key){
                    (keyframes[i]).push_back(Keyframe(0, (frames[i]).at(index).tick, (frames[i]).at(index).val));
                    printf("%i : %i : %i : %f \n",i, 0, (frames[i]).at(index).tick, (frames[i]).at(index).val);
                }
            }

            tickCounter[i]++;
        }
        torque[i] = 0.6f;
        printf("End of process %i: last frame: %i, %f index: %i \n", i, (frames[i]).at(index-1).tick, (frames[i]).at(index-1).val,index);
        //reset counter for further use
        tickCounter[i] = -1;
    }
}

// TODO might need to split the checks for first and last frame or move it into process
// might be a frame without speed, if recording only startes when movement starts, disregard this
bool
TotalRecordKeyframe::checkFrame(int index, int i){

    float currentsign = prevsign[i];

    if(index < frames[i].size()-1){
        // derivative
        float diff = frames[i].at(index+1).val - frames[i].at(index).val; 
        currentsign = diff < 0?-1:1;
    }

    // first frame is a keyframe
    if(index == 1){
        prevsign[i] = currentsign;
        return true;
    }

    // last frame is a keyframe
    if(index == (frames[i].size()-1)){
        return true;
    }

    if(index < frames[i].size() && index != 1){

        if(currentsign != prevsign[i]) {
            if(prevsign[i]!=0) {
                prevsign[i] = currentsign;
                return true;
                if(debug)
                    printf("stored: %i, %i, %f currentsign=%f, prevsign=%f\n",i, tick, input_array[i], currentsign, prevsign[i] );                
            }
        }
    }
//    printf("Check Frame: %i, %i, currentsign=%f, prevsign=%f\n",i, tick, currentsign, prevsign[i] );                
    return false;
}

void
TotalRecordKeyframe::movingAverage(){
    for (int i = 0; i < input_array_size; ++i)
    {
        printf("Moving Average for servo: %i \n", i);

        float prev = 0;
        float curr = 0;
        float next = 0;
        for(int index = 1; index < frames[i].size(); ++index){
            int count = 0;
            if(index != 1){
                prev = frames[i].at(index-1).val;
                count++;   
            }
            
            if(index < frames[i].size()-1){
                next = frames[i].at(index+1).val;
                count++;
            }
            else
                next = 0;

            curr = frames[i].at(index).val;
            count++;

            frames[i].at(index).val = (prev + curr + next) / count;
        }
    }
}


void 
TotalRecordKeyframe::play()
{

    for (int i = 0; i < input_array_size; ++i){
        if(keyframe_iterator[i]->type == 1){
            if(tickCounter[i] == -1){
                tickCounter[i] = keyframe_iterator[i]->tick;
            }else if(tickCounter[i] == 0){
                tickCounter[i] = -1;
                keyframe_iterator[i]++;
            }else{
                tickCounter[i]--;
            }
        }else{
            output[i] = keyframe_iterator[i]->val;
            if(debug)
                printf("kfr output i=%i, tick=%i, pos=%f, input=%f\n", i, keyframe_iterator[i]->tick, keyframe_iterator[i]->val, input_array[i]);    
            if(equal(input_array[i], keyframe_iterator[i]->val, 1) 
                && keyframe_iterator[i] != keyframes[i].end())
            {
                keyframe_iterator[i]++;
                
            }
        }

        if(repeat && (keyframe_iterator[i])==keyframes[i].end()){
            keyframe_iterator[i] = keyframes[i].begin();
            tickCounter[i] = -1;
        }


    }
}

// Install the module. This code is executed during start-up.

static InitClass init("TotalRecordKeyframe", &TotalRecordKeyframe::Create, "Source/UserModules/TotalRecordKeyframe/");


