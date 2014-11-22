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
    Bind(debug, "debug");
    Bind(angle_tolerance, "angle_tolerance");

    input_array = GetInputArray("INPUT");
    input_speed = GetInputArray("SPEED");
    input_array_size = GetInputSize("INPUT");
    command = GetInputArray("COMMAND");
    sync_in = GetInputArray("SYNC_IN");
    
    printf("Init inputs set\n");


    // Do the same for the outputs
    output = GetOutputArray("OUTPUT");
    torque = GetOutputArray("TORQUE");
    sync_out = GetOutputArray("SYNC_OUT");

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

    reset = false;

    equality_tolerance = 0.2;

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

    if(current_state == eStart){
        for(int i = 0; i < input_array_size; ++i){
            output[i] = input_array[i];
            //printf("Set servo %i pos %f \n", i, input_array[i]);
        }
    }

    switch(current_state){
        case eStart:
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
            if(cmd == ePause){
                current_state = eRecord_Paused;
            }else if(cmd == eProcess){
                current_state=eProcessing;
            }else{
                record();
            }
            break;
        case eProcessing:
            movingAverage();
            process();
            current_state = eRecord_Paused;
            command[0] = ePause;
            break;
        case ePlaying:
            if(cmd == ePause){
                current_state=ePlay_Paused;
            }else{
                sync_out[0] = 0.0;
                play();
            }
            break;
        case eRecord_Paused:
            if(cmd == eProcess){
                current_state = eProcessing;
            }else if (cmd == ePlay){
                current_state = eReady_To_Play;
            }else if (cmd == eRecord){
                current_state = eRecording;
            }
            break;
        case ePlay_Paused:
            if (cmd == ePlay){
                current_state = eReady_To_Play;
                sync_out[0] = 0.0f;
            }
            break;
        case eReady_To_Play:
            if(!reset)
                Reset();
            pre_play();
        default:
            break;
    }

/*
    switch(current_state)
    {

      case eStart:
        if(reset){
            for (int i = 0; i < input_array_size; ++i){
                output[i] = input[i];
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
        if(cmd==ePause){
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
            for (int i = 0; i < input_array_size; ++i){
                keyframe_iterator[i] = keyframes[i].begin();
            }
        }
      default:
        break;
    }*/
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

/**
*   Set all start positions and send out sync signal
*/
void
TotalRecordKeyframe::pre_play(){
    bool start = true;
    for(int i = 0; i < input_array_size; ++i){
        if(!equal(input_array[i], keyframe_iterator[i]->val, 1.0)){
            start = false;
            break;
        }
    }
    if(start){
        current_state=ePlaying;
        sync_out[0] = 1.0f;
        reset = false;
    }
}

void
TotalRecordKeyframe::Reset(){
    printf("Keyframes reset\n");
    for (int i = 0; i < input_array_size; ++i){
        keyframe_iterator[i] = keyframes[i].begin();
        output[i] = keyframe_iterator[i]->val;
    }
    reset = true;
}

void
TotalRecordKeyframe::process()
{
    for(int i = 0; i < input_array_size; ++i){
        printf("Enter process: %i \n", i);
        bool pauseFrame = false;
        int type;
        int index = 0;

        // First keyframe
        Keyframe prev = Keyframe(0, (frames[i]).at(index).tick, frames[i].at(index).val);
        (keyframes[i]).push_back(prev);
        printf("%i : %i : %i : %f \n",i, 0, (frames[i]).at(index).tick, (frames[i]).at(index).val);
        
        for(index = 1; index < frames[i].size(); index++){
            
            int key = checkFrame(index,i);

            if(key == 1){
                if(!pauseFrame){
                    pauseFrame = true;
                    tickCounter[i] = 1;
                }else{
                    tickCounter[i]++;
                }
            }else if (key == 0){
                if(pauseFrame){
                    keyframes[i].push_back(Keyframe(1, tickCounter[i], prev.val));
                    printf("%i : %i : %i : %f \n",i, 1, tickCounter[i], prev.val);
                
                    keyframes[i].push_back(Keyframe(0, frames[i].at(index).tick, prev.val));
                    printf("%i : %i : %i : %f \n",i, 0, (frames[i]).at(index).tick, prev.val);
                    pauseFrame = false;
                }else{
                    keyframes[i].push_back(Keyframe(0, frames[i].at(index).tick, frames[i].at(index).val));
                    printf("%i : %i : %i : %f \n",i, 0, (frames[i]).at(index).tick, (frames[i]).at(index).val);
                }
            }
        }
        
        // Last keyframe
        (keyframes[i]).push_back(Keyframe(0, (frames[i]).at(index-1).tick, (frames[i]).at(index-1).val));
        printf("%i : %i : %i : %f \n",i, 0, (frames[i]).at(index-1).tick, (frames[i]).at(index-1).val);
        printf("End of process %i: last frame: %i, %f index: %i \n", i, (frames[i]).at(index-1).tick, (frames[i]).at(index-1).val,index);
        
        //reset counter for further use
        tickCounter[i] = 1;

        //set torque for playback
    }
        set_array(torque,0.9f,input_array_size);
}


int
TotalRecordKeyframe::checkFrame(int index, int i){

    float currentsign = prevsign[i];
    int ret = -1;

    if(index < frames[i].size()){
        if(!equal(frames[i].at(index).moving_speed, 0, equality_tolerance)){
            currentsign = frames[i].at(index).moving_speed < 0?-1:1;
            equality_tolerance = 0;
        }else{
            ret = 1;
            equality_tolerance = 0.2;
        }

        if (currentsign != prevsign[i]){
            ret = 0;
        }
    }

    //printf("Check Frame: %i, %i, currentsign=%f, prevsign=%f\n",i, tick, currentsign, prevsign[i] );                
    prevsign[i] = currentsign;
    return ret;
}


void
TotalRecordKeyframe::movingAverage(){

    std::vector<Frame> * raw = frames;
    frames = new std::vector<Frame>[input_array_size];

    for(int i = 0; i < input_array_size; ++i){
        printf("Moving Average for servo: %i \n", i);
        
        float sum = 0;
        int count = 0;

        //first value and set every value before it as same
        int index = 11;
        for(int j = 1; j < index+10; j++){
            sum = sum + raw[i].at(j).val;
            count++;
        }

        sum = sum / count;

        for(int j = 1; j <= index; j++){
            frames[i].push_back(Frame(raw[i].at(j).tick, sum, raw[i].at(j).moving_speed));
            printf("ma stored servo: %i, tick: %i, sum: %f, speed: %f \n", i, raw[i].at(j).tick, sum,raw[i].at(j).moving_speed);
        }
        index++;

        //loop through all indexes and sum and count
        while(index < raw[i].size()-10){
            sum = 0;
            count = 0;
            for(int j = index-10; j <= index+10; j++){
                sum = sum + raw[i].at(j).val;
                count++;
            }

            sum = sum / count;
            
            frames[i].push_back(Frame(raw[i].at(index).tick, sum, raw[i].at(index).moving_speed));
            printf("ma stored servo: %i, index: %i tick: %i, sum: %f speed: %f \n", i, index, raw[i].at(index).tick, sum,raw[i].at(index).moving_speed);
            index++;
        }

        //last value and set every value after it as same
        for(int j = index; j < raw[i].size()-1; j++){
            frames[i].push_back(Frame(raw[i].at(j).tick, sum, raw[i].at(j).moving_speed));
            printf("ma stored servo: %i, tick: %i, sum: %f speed: %f \n", i, raw[i].at(j).tick, sum, raw[i].at(j).moving_speed);
        }
    }
    delete[] raw;

/*
    for (int i = 0; i < input_array_size; ++i)
    {

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
    */
}


void 
TotalRecordKeyframe::play()
{
    bool end = true;
    for(int i = 0; i < input_array_size; ++i){
        if(keyframe_iterator[i] != keyframes[i].end()){
            end = false;
            break;
        }
    }

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
            if(equal(input_array[i], keyframe_iterator[i]->val, angle_tolerance) 
                && keyframe_iterator[i] != keyframes[i].end())
            {
                keyframe_iterator[i]++;
                
            }
        }
        if((end && keyframe_iterator[i] == keyframes[i].end()) || sync_in[0] == 1.f){
            if(repeat){
                current_state = eReady_To_Play;
                tickCounter[i] = -1;
            }
        }

    }
}

// Install the module. This code is executed during start-up.

static InitClass init("TotalRecordKeyframe", &TotalRecordKeyframe::Create, "Source/UserModules/TotalRecordKeyframe/");


