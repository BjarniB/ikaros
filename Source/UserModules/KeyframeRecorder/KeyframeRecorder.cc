//
//	KeyframeRecorder.cc		This file is a part of the IKAROS project
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

#include "KeyframeRecorder.h"

// use the ikaros namespace to access the math library
// this is preferred to using math.h

using namespace ikaros;
const float cRecording = 0.f;
const float cPlayback = 1.f;

void
KeyframeRecorder::Init()
{
 
    Bind(repeat, "repeat");
    Bind(equality_tolerance, "equality_tolerance");
    Bind(debug, "debug");

    input_array = GetInputArray("INPUT");
    input_array_size = GetInputSize("INPUT");
    derivative = GetInputArray("DERIVATIVE");
    command = GetInputArray("COMMAND");
    sync_input = GetInputArray("SYNC_INPUT");
    // Do the same for the outputs

    output = GetOutputArray("OUTPUT");
    sync = GetOutputArray("SYNC");

    recording = new std::vector<Keyframe>[input_array_size];
    keyframe_iterator = new std::vector<Keyframe>::iterator[input_array_size];

    for (int i = 0; i < input_array_size; ++i)
    {
        recording[i].reserve(10);
        //keyframe_iterator[i] = recording[i].begin();
    }
    prevsign = new float[input_array_size];
    reset_array(prevsign, input_array_size);
}



KeyframeRecorder::~KeyframeRecorder()
{
    // Destroy data structures that you allocated in Init.
    delete[] recording;
    delete[] keyframe_iterator;
    delete[] prevsign;
    // Do NOT destroy data structures that you got from the
    // kernel with GetInputArray, GetInputMatrix etc.
}



void
KeyframeRecorder::Tick()
{
    Command cmd = (Command)command[0];
    if(debug){
        printf("kfr command=%i state before=%i\n", cmd, current_state);
    }

    switch(current_state)
    {

      case eStart:
        if(cmd==eRecord)
        {
          current_state=eRecording;
          tick=0;  
        }    
        break;
      case eRecording:
        if(cmd==ePause)
          current_state=eRecord_Paused;
        else if(cmd==ePlay)
        {
            if(sync_input[0]==1.f)
                current_state = ePlaying;
            else 
                current_state = eReady_To_Play;
            tick=0;
            for (int i = 0; i < input_array_size; ++i)
                keyframe_iterator[i] = recording[i].begin();
        }
        else 
            record();
        break;
      case ePlaying:
        if(cmd==ePause)
          current_state=ePlay_Paused;
        else
          play();
        break;
      case eRecord_Paused:
        if(cmd==ePause || cmd==eRecord)
          current_state=eRecording;
        break;
      case ePlay_Paused:
        if(cmd==ePause || cmd==ePlay)
          current_state=ePlaying;
        break;
      case eReady_To_Play:
        if(sync_input[0]==1.f)
          current_state=ePlaying;
      default:
        break;
    }
    if(debug){
        printf("kfr state after=%i\n", current_state);
    }
}
/*
void
KeyframeRecorder::Tick()
{
    // check for state change
    if(state[0] != prevstate)
    {
        tick = 0;
        prevstate = state[0];
        if(state[0] == cPlayback)
        {
            for (int i = 0; i < input_array_size; ++i)
                keyframe_iterator[i] = recording[i].begin();
        }
    }

    if(state[0] == cRecording)
    {
        for (int i = 0; i < input_array_size; ++i)
        {
            // if there is difference in sign, record
            float currentsign = derivative[i] < 0?-1:1;

            if(currentsign != prevsign[i])
            {
                if(prevsign[i]!=0)
                {
                    (recording[i]).push_back(Keyframe(tick, input_array[i]));
                    printf("stored: %i, %i, %f currentsign=%f, prevsign=%f\n",i, tick, input_array[i], currentsign, prevsign[i] );
                }
                prevsign[i] = currentsign;
            }
        }
        tick++;
    }
    else // playback
    {
        for (int i = 0; i < input_array_size; ++i)
        {
            // if have reach current keyframe, increment to next 
            // keyframe and output it
            sync[i] = (float)keyframe_iterator[i]->first;
            output[i] = keyframe_iterator[i]->second;
            printf("kfr output %i, %i, %f\n", i, keyframe_iterator[i]->first, keyframe_iterator[i]->second);    
            if(equal(input_array[i], keyframe_iterator[i]->second, equality_tolerance) 
                && keyframe_iterator[i] != recording[i].end())
            {
                keyframe_iterator[i]++;
                
            }
            if(repeat && (keyframe_iterator[i])==recording[i].end())
                keyframe_iterator[i] = recording[i].begin();
        }
    }

}
*/

void
KeyframeRecorder::record()
{
    for (int i = 0; i < input_array_size; ++i)
    {
        // if there is difference in sign, record
        float currentsign = derivative[i] < 0?-1:1;

        if(currentsign != prevsign[i])
        {
            if(prevsign[i]!=0)
            {
                (recording[i]).push_back(Keyframe(tick, input_array[i]));
                if(debug)
                    printf("stored: %i, %i, %f currentsign=%f, prevsign=%f\n",i, tick, input_array[i], currentsign, prevsign[i] );
            }
            prevsign[i] = currentsign;
        }
    }
    tick++;
}

void 
KeyframeRecorder::play()
{
    for (int i = 0; i < input_array_size; ++i)
    {
        // if have reached current keyframe, increment to next 
        // keyframe and output it
        sync[i] = (float)keyframe_iterator[i]->first;
        output[i] = keyframe_iterator[i]->second;
        if(debug)
            printf("kfr output i=%i, tick=%i, pos=%f, input=%f\n", i, keyframe_iterator[i]->first, keyframe_iterator[i]->second, input_array[i]);    
        if(equal(input_array[i], keyframe_iterator[i]->second, equality_tolerance) 
            && keyframe_iterator[i] != recording[i].end())
        {
            keyframe_iterator[i]++;
            
        }
        if(repeat && (keyframe_iterator[i])==recording[i].end())
            keyframe_iterator[i] = recording[i].begin();
    }
}

// Install the module. This code is executed during start-up.

static InitClass init("KeyframeRecorder", &KeyframeRecorder::Create, "Source/UserModules/KeyframeRecorder/");


