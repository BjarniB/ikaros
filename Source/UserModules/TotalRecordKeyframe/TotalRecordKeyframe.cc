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

void
TotalRecordKeyframe::Init()
{
 
    Bind(debugmode, "debug");

    input_array = GetInputArray("INPUT");
    input_speed = GetInputArray("SPEED");
    input_array_size = GetInputSize("INPUT");
    command = GetInputArray("COMMAND");
    sync_in = GetInputArray("SYNC_IN");
    
    printf("Init inputs set\n");


    // Do the same for the outputs
    torque = GetOutputArray("TORQUE");
    sync_out = GetOutputArray("SYNC_OUT");

    output_values = GetOutputMatrix("OUT_VALUE");
    output_ticks = GetOutputMatrix("OUT_TICK");
    output_size_x = GetOutputArray("SIZE_X");
    output_size_y = GetOutputArray("SIZE_Y");

    printf("Init outputs set\n");

    keyframes = new std::vector<Keyframe>[input_array_size];
  
    frames = new std::vector<Frame>[input_array_size];
  
    printf("Init vectors set\n");

    for (int i = 0; i < input_array_size; ++i)
    {
        keyframes[i].reserve(10);
    }

    prevsign = new float[input_array_size];
    reset_array(prevsign, input_array_size);

    // TODO cleanup remnants of pause frames
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
    delete[] prevsign;
    delete[] tickCounter;
    delete[] frames;
    // Do NOT destroy data structures that you got from the
    // kernel with GetInputArray, GetInputMatrix etc.
    printf("Frames Stored: %i \n", tick);
}


// TODO change states from to do eStart, eRecording, eProcessing, eSendData
void
TotalRecordKeyframe::Tick()
{
   
    Command cmd = (Command)command[0];

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
            ExportOutputs();
            sync_out[0] = 1.0f;
            break;
        case eRecord_Paused:
            if(cmd == eProcess){
                current_state = eProcessing;
            }else if (cmd == eRecord){
                current_state = eRecording;
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
        if(debugmode)
            printf("%i : %i : %f : %f \n",i, tick, input_array[i], input_speed[i]);
        // Simply outputs the input
        //output[i] = input_array[i];
    }
    tick++;
}

// TODO cleanup of pause frames to be done
void
TotalRecordKeyframe::process()
{
    for(int i = 0; i < input_array_size; ++i){
        if(debugmode)
            printf("Enter process: %i \n", i);
        
        bool pauseFrame = false;
        int type;
        int index = 0;

        // First keyframe
        Keyframe prev = Keyframe(0, (frames[i]).at(index).tick, frames[i].at(index).val);
        (keyframes[i]).push_back(prev);
        
        if(debugmode)
            printf("%i : %i : %i : %f \n",i, 0, (frames[i]).at(index).tick, (frames[i]).at(index).val);
        
        for(index = 1; index < frames[i].size(); index++){

            int key = checkFrame(index,i);

            // TODO Some cleanup here
            if(key == 1){
                if(!pauseFrame){
                    pauseFrame = true;
                    tickCounter[i] = 1;
                }else{
                    tickCounter[i]++;
                }
            }else if (key == 0){
                if(pauseFrame){
                    keyframes[i].push_back(Keyframe(0, frames[i].at(index).tick, frames[i].at(index).val));
                    printf("%i : %i : %i : %f \n",i, 0, (frames[i]).at(index).tick, frames[i].at(index).val);
                    pauseFrame = false;
                }else{
                    keyframes[i].push_back(Keyframe(0, frames[i].at(index).tick, frames[i].at(index).val));
                    printf("%i : %i : %i : %f \n",i, 0, (frames[i]).at(index).tick, (frames[i]).at(index).val);
                }
            }
        }
        
        // Last keyframe
        (keyframes[i]).push_back(Keyframe(0, (frames[i]).at(index-1).tick, (frames[i]).at(index-1).val));
        if(debugmode){
            printf("%i : %i : %i : %f \n",i, 0, (frames[i]).at(index-1).tick, (frames[i]).at(index-1).val);
            printf("End of process %i: last frame: %i, %f index: %i \n", i, (frames[i]).at(index-1).tick, (frames[i]).at(index-1).val,index);
        }
        //set torque for playback
    }
        set_array(torque,0.9f,input_array_size);
}

/**
 *  Method for checking if the frame at index is a keyframe
 */
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

/**
 *  Moving average method for removing recorded noise
 */
void
TotalRecordKeyframe::movingAverage(){

    if(frames[0].size() < 25){
        if(debugmode)
            printf("Size: %i\n", (int)frames[0].size());
        return;
    }

    std::vector<Frame> * raw = frames;
    frames = new std::vector<Frame>[input_array_size];

    for(int i = 0; i < input_array_size; ++i){

        if(debugmode)
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
            
            if(debugmode)
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
            if(debugmode)
                printf("ma stored servo: %i, index: %i tick: %i, sum: %f speed: %f \n", i, index, raw[i].at(index).tick, sum,raw[i].at(index).moving_speed);
            index++;
        }

        //last value and set every value after it as same
        for(int j = index; j < raw[i].size()-1; j++){
            frames[i].push_back(Frame(raw[i].at(j).tick, sum, raw[i].at(j).moving_speed));
            if(debugmode)
                printf("ma stored servo: %i, tick: %i, sum: %f speed: %f \n", i, raw[i].at(j).tick, sum, raw[i].at(j).moving_speed);
        }
    }
    delete[] raw;
}

// Setups for output matrices
// test with socket module and print the matrices out in the socket module
void
TotalRecordKeyframe::ExportOutputs(){

    int biggest = 0;
    for(int i = 0; i < input_array_size; ++i){
        if(keyframes[i].size() > keyframes[biggest].size())
            biggest = i;
    }

    output_size_y[0] = input_array_size;
    output_size_x[0] = keyframes[biggest].size()+1;

    for(int i = 0; i < input_array_size; i++){

        for(int j = 0; j <= keyframes[i].size(); j++){
            if(j == keyframes[i].size()){
                output_values[i][j] = -1;
                output_ticks[i][j] = -1;
            }else{
                output_values[i][j] = keyframes[i].at(j).val;
                output_ticks[i][j] = keyframes[i].at(j).tick;
            }
        }
    }

    if(debugmode){
        print_matrix("ticks", output_ticks, (int)output_size_x[0], (int)output_size_y[0]);
        print_matrix("values", output_values, (int)output_size_x[0], (int)output_size_y[0]);
    
        printf("Keyframe recorder print output matrix done\n");
    }
}

// Install the module. This code is executed during start-up.

static InitClass init("TotalRecordKeyframe", &TotalRecordKeyframe::Create, "Source/UserModules/TotalRecordKeyframe/");


