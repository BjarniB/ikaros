//
//	Recorder.cc		This file is a part of the IKAROS project
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

#include "Recorder.h"

// use the ikaros namespace to access the math library
// this is preferred to using math.h

using namespace ikaros;

const float cRecording = 0.f;
const float cPlayback = 1.f;

void
Recorder::Init()
{
    
    Bind(maxlength, "maxlength");
    Bind(repeat, "repeat");
    Bind(debug, "debug");
    
    input_array = GetInputArray("INPUT");
    input_array_size = GetInputSize("INPUT");
    command = GetInputArray("COMMAND");
    sync_in = GetInputArray("SYNC_IN");


    // Do the same for the outputs
    output_array = GetOutputArray("OUTPUT");
    sync_out = GetOutputArray("SYNC_OUT");

    // cols=maxlength, rows=inputarraysize
    recording = create_matrix(maxlength, input_array_size);
    tick = 0;
    endtick = 0;
}



Recorder::~Recorder()
{
    // Destroy data structures that you allocated in Init.

    destroy_matrix(recording);

    // Do NOT destroy data structures that you got from the
    // kernel with GetInputArray, GetInputMatrix etc.
}



void
Recorder::Tick()
{
    Command cmd = (Command)command[0];
    if(debug){
        printf("rec command=%i state before=%i\n", cmd, current_state);
    }
    
    switch(current_state)
    {
        case eStart:
            if(cmd==eRecord)
            {
                current_state = eRecording;
                tick = 0;
            }
            break;
        case eRecording:
            if(cmd==ePause)
                current_state = eRecord_Paused;
            else if(cmd==ePlay)
                Pre_play();
            else
                Record();
            break;
        case ePlaying:
            if(cmd==ePause)
                current_state = ePlay_Paused;
            else
                Play();
            // TODO add parameter to concatenate or overwrite
            // and enable going back to recording when playing
            break;
        case eRecord_Paused:
            if(cmd==eRecord)
                current_state = eRecording;
            else if(cmd==ePlay){
                Pre_play();
            }
            break;
        case ePlay_Paused:
            if(cmd==ePlay)
                current_state=ePlaying;
            break;
        case eReady_To_Play:
            if(sync_in[0]==1.f)
                current_state=ePlaying;
            break;
        default:
            break;
    }
    if(debug){
        printf("rec state after=%i\n", current_state);
    }
}

void
Recorder::Record()
{
    for (int i = 0; i < input_array_size; ++i)
    {
        recording[i][tick] = input_array[i];
        if(debug)
            printf("stored: %i, %i, %f\n", i, tick, input_array[i]);
    }
    sync_out[0] = 0.f;
    tick++;
}

void
Recorder::Play()
{
    for (int i = 0; i < input_array_size; ++i)
    {
        output_array[i] = recording[i][tick];
        if(debug)
            printf("rec output %i, %i, %f\n",i, tick, output_array[i] );
    }
    
    if(tick>=endtick)
    {
        sync_out[0] = 1.f;
        if(repeat){
            tick = 0;
            current_state = eReady_To_Play;
        }
    }
    else{
        sync_out[0] = 0.f;
        tick++;
    }
}

void 
Recorder::Pre_play()
{
    if(sync_in[0] == 1.f)
        current_state = ePlaying;
    else
        current_state = eReady_To_Play;
    endtick = tick;
    tick = 0;    
}

// Install the module. This code is executed during start-up.

static InitClass init("Recorder", &Recorder::Create, "Source/UserModules/Recorder/");


