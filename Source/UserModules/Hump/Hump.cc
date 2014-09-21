//
//	Hump.cc		This file is a part of the IKAROS project
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

#include "Hump.h"

// use the ikaros namespace to access the math library
// this is preferred to using math.h

using namespace ikaros;


void
Hump::Init()
{
    
    Bind(timescale, "timescale");
    Bind(scale, "scale");
    Bind(shift, "shift");
    Bind(state, "state");
    Bind(offset, "offset");
    size = GetInputSize("CURRENT_POS");
    current_pos = GetInputArray("CURRENT_POS"); 
    target_pos = GetInputArray("TARGET_POS");
    prev_target_pos = create_array(size);
    //input_state_size = GetInputSize("INPUT_STATE");
    //input_state = GetInputArray("INPUT_STATE");
    // Do the same for the outputs

    output_array = GetOutputArray("OUTPUT");

    startpos = create_array(size);
    
    t=0;
    first=true;

    // initialize to -1
    memset(prev_target_pos, -1, size*sizeof(float)); // set each byte to 0
    
    /*
    timescale = 0.1;
    scale = 1.0;
    state=1;
    shift=0.5;
    */
}



Hump::~Hump()
{
    destroy_array(prev_target_pos);
    destroy_array(startpos);
}



void
Hump::Tick()
{
    if(state>=1.0){
        //output_array[0] = target*1.0/(1+exp(-alpha*t-shift));
        for(int i=0; i<size; ++i){
            if(target_pos[i] != prev_target_pos[i]){
                startpos[i] = current_pos[i];
                prev_target_pos[i] = target_pos[i];
            }

            // this only work from small to large 
            // note: fix pos!
            float pos = Map_Range(current_pos[i], startpos[i], target_pos[i], 0.0, 1);
            printf("i=%i, current_pos= %f, targetpos= %f, startpos= %f, pos= %f\n", 
                i, 
                current_pos[i],
                target_pos[i],
                startpos[i],
                pos);

            output_array[i] = 4*scale*exp(timescale*(pos-shift))/pow(1+exp(timescale*(pos-shift)),2) + offset;//pow(2.71828,-t);    
            printf("output= %f\n", output_array[i]);
        }
        
        //printf("%f,%f\n", output_array[0],t);
        t+=1.0;    
    }
    
}

/** Note: this should be made inline for efficency
*/
inline float
Hump::Map_Range(float value, float low1, float high1, float low2, float high2) {
    return low2 + (high2 - low2) * (value - low1) / (high1 - low1);
}



// Install the module. This code is executed during start-up.

static InitClass init("Hump", &Hump::Create, "Source/UserModules/Hump/");


