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
    //input_moving_speed = GetInputArray("MOVING_SPEED");
    input_array_size = GetInputSize("INPUT");
    command = GetInputArray("COMMAND");
    
    printf("Init inputs set\n");


    // Do the same for the outputs
    output = GetOutputArray("OUTPUT");

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
            // reset all servos to 180 degrees
            /*for(int i = 0; i < input_array_size; i++){
                output[i] = 180; 
            }*/
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
        }else if (cmd==ePlay) {
                current_state = ePlaying;
            tick=0;
            for (int i = 0; i < input_array_size; ++i)
                keyframe_iterator[i] = keyframes[i].begin();
        }
      default:
        break;
    }
    if(debug){
        printf("kfr state after=%i\n", current_state);
    }
}


/**
*    Record all positions recieved during keyframes phase
*/
void
TotalRecordKeyframe::record()
{
    // TODO frames to struct with moving speed aswell
    for (int i = 0; i < input_array_size; ++i)
    {
        (frames[i]).push_back(Frame(tick, input_array[i], 0/*input_moving_speed[i]*/));
        if(debug)
            printf("stored: %i, %i, %f \n",i, tick, input_array[i]);
        // Simply outputs the input
        output[i] = input_array[i];
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
            if(frames[i].at(index).moving_speed == 0 && !pauseFrame){
                pauseFrame = true;
                tickCounter[i] = 1;
            }

            if (pauseFrame){
                if(frames[i].at(index).moving_speed != 0){
                    (keyframes[i]).push_back(Keyframe(1, tickCounter[i], (frames[i]).at(index-1).val));
                    printf("stored pause: %i, %i, %f \n",tickCounter[i], (frames[i]).at(index).tick, (frames[i]).at(index).val);
                    pauseFrame = false;
                }
            }

            if (!pauseFrame){
                bool key = checkFrame(index, i);
                if(key){
                    (keyframes[i]).push_back(Keyframe(0, (frames[i]).at(index).tick, (frames[i]).at(index).val));
                    printf("stored pos: %i, %i, %f \n",index, (frames[i]).at(index).tick, (frames[i]).at(index).val);
                }
            }

            tickCounter[i]++;
        }
        printf("End of process %i: last frame: %i, %f index: %i \n", i, (frames[i]).at(index-1).tick, (frames[i]).at(index-1).val,index);
        //reset counter for further use
        tickCounter[i] = 0;
    }
}

bool
TotalRecordKeyframe::checkFrame(int index, int i){

//    printf("Check frame index: %i i: %i \n", index, i);

    float currentsign = prevsign[i];

    if(index < frames[i].size()-1){
        // derivative
        float diff = frames[i].at(index+1).val - frames[i].at(index).val; 
        currentsign = diff < 0?-1:1;
//        printf("index: % i ,derivatives: %f , diff: %f , prevsign: %f \n", index, currentsign, diff, prevsign[i]);
    }

        // first frame is a keyframe
    if(index == 1){
    //    printf("first frame \n");
        prevsign[i] = currentsign;
        return true;
    }

    // last frame is a keyframe
    if(index == (frames[i].size()-1)){
      //  printf("last frame \n");
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
    //printf("End check frame. \n");
    return false;
}

void
TotalRecordKeyframe::movingAverage(){
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

/*
        //TODO change to index stepping, take moving average of index-1 index index+1
        frame_iterator[i] = frames[i].begin();
        frame_iterator[i]++;
        prev = &*frame_iterator[i];
        frame_iterator[i]++;
        while(frame_iterator[i] != frames[i].end()){
            if(debug)
                printf("Average: %i %f \n", prev->tick, prev->val);
            frame_iterator[i]->val= (prev->val + frame_iterator[i]->val) / 2;
            prev = &*frame_iterator[i];
            frame_iterator[i]++;
        }
        printf("End of moving average %i: last frame: %i, %f \n", i, prev->tick, prev->val);
*/
    }
}
/*
    vid record/process

    prevsign != currsign
    speed == 0
        tickcounter++
    else
        keyframes.pushback(Keyframe(1,tickcounter,currentpos));

    keyframe till struct
        type 0 = pos 1=pause
        tick
        pos

    vid playback
    for alla keyframes
        switch(type)
            case ePause:
                ticks--;
                check ticks 0, change keyframe
            case ePos:
                output = pos;
                check currentpos, change keyframe

speed curvor * 0.1
    speed < 0
        target = maxpos;
    else speed > 0 
        target = minpos
    else speed = 0
*/


void 
TotalRecordKeyframe::play()
{
    // TODO playback with pause keyframes, use tickCounter to determine if next keyframe or not
    for (int i = 0; i < input_array_size; ++i)
    {
        if(keyframe_iterator[i]->type == 1 && tickCounter[i] == 0){
            tickCounter[i] = keyframe_iterator[i]->tick;
        }

        // if have reached current keyframe, increment to next 
        // keyframe and output it
        //sync[i] = (float)keyframe_iterator[i]->first;
        output[i] = keyframe_iterator[i]->val;
        if(tickCounter[i] == 0){
            if(debug)
                printf("kfr output i=%i, tick=%i, pos=%f, input=%f\n", i, keyframe_iterator[i]->tick, keyframe_iterator[i]->val, input_array[i]);    
            if(equal(input_array[i], keyframe_iterator[i]->val, equality_tolerance) 
                && keyframe_iterator[i] != keyframes[i].end())
            {
                keyframe_iterator[i]++;
                
            }
            if(repeat && (keyframe_iterator[i])==keyframes[i].end())
                keyframe_iterator[i] = keyframes[i].begin();
        }else{
            tickCounter--;
        }
    }
}


// Install the module. This code is executed during start-up.

static InitClass init("TotalRecordKeyframe", &TotalRecordKeyframe::Create, "Source/UserModules/TotalRecordKeyframe/");


