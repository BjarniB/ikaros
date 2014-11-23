//
//	TriggeredSampler.cc		This file is a part of the IKAROS project
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

#include "TriggeredSampler.h"

// use the ikaros namespace to access the math library
// this is preferred to using math.h

using namespace ikaros;


void
TriggeredSampler::Init()
{
    Bind(repeat, "repeat");
	Bind(debugmode, "debug");    

    input_array = GetInputArray("INPUT");
    input_array_size = GetInputSize("INPUT");
    trigger = GetInputArray("TRIGGER");
    sync_in = GetInputArray("SYNC_IN");
    command = GetInputArray("COMMAND");

    output_array = GetOutputArray("OUTPUT");
    sync_out = GetOutputArray("SYNC_OUT");
    ticks = GetOutputArray("TICKS");  

    recording = new std::vector<Keyframe>[input_array_size];
    keyframe_iterator = new std::vector<Keyframe>::iterator[input_array_size];

    for (int i = 0; i < input_array_size; ++i)
        recording[i].reserve(10);

    prevtrigger = new float[input_array_size];
    prevtick = new float[input_array_size];
    reset_array(prevtrigger, input_array_size);
    current_state = eStart;
}



TriggeredSampler::~TriggeredSampler()
{
    // Destroy data structures that you allocated in Init.
    delete[] recording;
    delete[] keyframe_iterator;
    destroy_array(prevtrigger);
    destroy_array(prevtick);
}



void
TriggeredSampler::Tick()
{
	Command cmd  = (Command)command[0];
    if(debugmode)
	{
		// print out debug info
        // printf("TriggeredSampler command=%i state before=%i\n", cmd, current_state);
	}

    switch(current_state)
    {
        case eStart:
            if(cmd == eRecord)
            {
                current_state = eRecording;
                tick = 0;
                reset_array(prevtrigger, input_array_size);
            }
            break;
        case eRecording:
            if(cmd == ePause)
                current_state = eRecord_Paused;
            else if(cmd == ePlay)
                PrePlay();
            else
                Record();
            break;
        case ePlaying:
            if(cmd == ePause)
                current_state = ePlay_Paused;
            else
                Play();
            break;
        case eRecord_Paused:
            if(cmd == eRecord)
                current_state = eRecording;
            else if (cmd == ePlay)
                PrePlay();
            break;
        case ePlay_Paused:
            if(cmd == ePlay)
                current_state = ePlaying;
            break;
        case eReady_To_Play:
            if(sync_in[0] == 1.f)
                current_state = ePlaying;
            break;
        default:
            break;
    }
    // if(debugmode)
    //     printf("TriggeredSampler state after=%i\n", current_state);

}

void
TriggeredSampler::Record()
{
    for (int i = 0; i < input_array_size; ++i)
    {
        if(trigger[i] != prevtrigger[i])
        {
            recording[i].push_back(Keyframe(tick - prevtick[i], input_array[i]));
            if(debugmode)
                printf("TriggeredSampler stored: %i, %i, %f \n",i, tick, input_array[i]);
            prevtrigger[i] = trigger[i];
            prevtick[i] = tick;
        }
    }
    tick++;
}

void
TriggeredSampler::Play()
{
    int completed_channels=0;
    for (int i = 0; i < input_array_size; ++i)
    {
        output_array[i] = keyframe_iterator[i]->second;
        ticks[i] = keyframe_iterator[i]->first;
    
        if(debugmode)
            printf("TriggeredSampler output i=%i, tick=%i, pos=%f\n", i, keyframe_iterator[i]->first, keyframe_iterator[i]->second);    

        if(trigger[i] !=  prevtrigger[i])
        {
            if(keyframe_iterator[i] != recording[i].end())
            {
                keyframe_iterator[i]++;
            }
            else 
            {
                completed_channels++;
            }
            prevtrigger[i] = trigger[i];
        }
    }
    // if all iterators are at end, can reset to beginning
    if(completed_channels == input_array_size)
    {
        set_array(sync_out, 1.f, input_array_size);
        if(repeat)//equal(sync_out, 1.f, input_array_size, 0.f))
        {
            for (int i = 0; i < input_array_size; ++i)
                keyframe_iterator[i] = recording[i].begin();
            PrePlay();
        }
    }
    else
        set_array(sync_out, 0.f, input_array_size);
}

void
TriggeredSampler::PrePlay()
{
    if(sync_in[0] == 1.f)
    {
        current_state = ePlaying;
        reset_array(prevtrigger, input_array_size);
    }
    else
        current_state = eReady_To_Play;
    tick = 0;
    
    
    

    for (int i = 0; i < input_array_size; ++i)
        keyframe_iterator[i] = recording[i].begin();
}

// Install the module. This code is executed during start-up.

static InitClass init("TriggeredSampler", &TriggeredSampler::Create, "Source/UserModules/TriggeredSampler/");


