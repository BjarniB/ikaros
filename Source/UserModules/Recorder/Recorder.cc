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
    // To get the parameters from the IKC file, use the Bind
    // function for each parameter. The parameters are initialized
    // from the IKC and can optionally be changed from the
    // user interface while Ikaros is running. If the parameter is not
    // set, the default value will be used instead.
    
    Bind(maxlength, "maxlength");
    Bind(repeat, "repeat");
    
    // This is were we get pointers to the inputs and outputs

    // Get a pointer to the input INPUT1 and its size which we set
    // to 10 above
    // It does not matter whether a matrix of array is connected
    // to the inputs. We will treat it an array in this module
    // anyway.

    
    // Get pointer to a matrix and treat it as a matrix. If an array is
    // connected to this input, size_y will be 1.

    input_array = GetInputArray("INPUT");
    input_array_size = GetInputSize("INPUT");
    
    // size of these should be 1
    // TODO error if size != 1
    state = GetInputArray("STATE");
    sync = GetInputArray("SYNC");


    // Do the same for the outputs
    output_array = GetOutputArray("OUTPUT");

    recording = create_matrix(input_array_size, maxlength);
    tick = 0;
    endtick = 0;
    prevstate = 0;
    prevsync = 0;
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

    // handle change of state
    if(state[0] != prevstate)
    {
        if(prevstate == cRecording)
            endtick = tick;
        
        tick = 0; 
        prevstate = state[0];       
    }

    if(tick < maxlength)
    {
        if(state[0] == cRecording)
        {
            // recording
            for (int i = 0; i < input_array_size; ++i)
                recording[tick][i] = input_array[i];
        } 
        else 
        {
            if(repeat && tick > endtick)
                tick = 0;
            // check if sync has changed
            if(sync[0]!=prevsync)
            {
                if(sync[0]<maxlength)
                {
                    tick = (int)sync[0];
                    prevsync = sync[0];
                }
            }
            // playback
            for (int i = 0; i < input_array_size; ++i)
                output_array[i] = recording[tick][i];
        }
        tick++;
    }
}



// Install the module. This code is executed during start-up.

static InitClass init("Recorder", &Recorder::Create, "Source/UserModules/Recorder/");


