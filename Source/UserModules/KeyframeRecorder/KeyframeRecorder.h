//
//	KeyframeRecorder.h		This file is a part of the IKAROS project
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

#ifndef KeyframeRecorder_
#define KeyframeRecorder_

#include "IKAROS.h"
#include <utility>
#include <vector>
#include <cmath>
typedef std::pair<int, float> Keyframe;

class KeyframeRecorder: public Module
{
public:
    static Module * Create(Parameter * p) { return new KeyframeRecorder(p); }

    KeyframeRecorder(Parameter * p) : Module(p) {}
    virtual ~KeyframeRecorder();

    void 		Init();
    void 		Tick();

    //bool equal(float a, float b, float tolerance){ return fabs(a-b) <= tolerance;};
    void record();
    void play();
    // pointers to inputs and outputs
    // and integers to represent their sizes

    float *     input_array;
    int         input_array_size;

    float *     derivative;
    float *     command;
    float *     sync_input;

    
    // outputs
    float *     output;
    float *     sync;


    // internal data storage
    std::vector<Keyframe> *     recording;

    // parameter values
    float equality_tolerance;
    bool        repeat;
    bool        debug;
    
    // internals
    float       prevstate;
    float *     prevsign;
    int         tick;
    std::vector<Keyframe>::iterator * keyframe_iterator;
    enum State {eStart=0, eRecording, ePlaying, eRecord_Paused, ePlay_Paused, eReady_To_Play};
    enum Command {eNone=0, eRecord, ePlay, ePause};  
    State current_state;
};

#endif
