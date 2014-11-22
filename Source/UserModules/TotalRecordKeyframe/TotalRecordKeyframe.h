//
//	TotalRecordKeyframe.h		This file is a part of the IKAROS project
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

#ifndef TotalRecordKeyframe_
#define TotalRecordKeyframe_

#include "IKAROS.h"
#include <utility>
#include <vector>
#include <cmath>

struct Keyframe
{
    int type; // 0 = pos, 1 = pause
    int tick;
    float val;

    Keyframe(int ty, int ti, float v){
        type = ty;
        tick = ti;
        val = v;
    }
} ;

struct Frame
{
    int tick;
    float val;
    float moving_speed;

    Frame(int ti, float v, float s){
        tick = ti;
        val = v;
        moving_speed = s;
    }
} ;

class TotalRecordKeyframe: public Module
{
public:
    static Module * Create(Parameter * p) { return new TotalRecordKeyframe(p); }

    TotalRecordKeyframe(Parameter * p) : Module(p) {}
    virtual ~TotalRecordKeyframe();

    void 		Init();
    void 		Tick();

    //bool equal(float a, float b, sfloat tolerance){ return fabs(a-b) <= tolerance;};
    void record();
    void process();
    void play();
    int checkFrame(int index, int i); // -1 not a keyframe, 0 pos frame, 1 pause frame
    void movingAverage();
    void printFrames();
    void pre_play();
    void Reset();
    // pointers to inputs and outputs
    // and integers to represent their sizes
    float *     input_array;
    float *     input_speed;
    int         input_array_size;
    float *     sync_in;
    float *     sync_out;

    // outputs
    float *     output;
    float *     torque;

    // internal data storage
    std::vector<Keyframe> *     keyframes;
    std::vector<Frame> *        frames;

    // parameter values
    float equality_tolerance;
    float angle_tolerance;
    bool        repeat;
    bool        debug;
    bool        reset;
    
    // internals
    float *     prevsign;
    int *       tickCounter;
    int         tick;

    // Frame and Keyframe vectors
    std::vector<Keyframe>::iterator * keyframe_iterator;
    std::vector<Frame>::iterator * frame_iterator;

    // State handling
    enum State {eStart=0, eRecording, eProcessing, ePlaying, ePaused, eRecord_Paused, ePlay_Paused, eReady_To_Play};
    State current_state;
    
    // Command handling
    enum Command {eNone=0, eRecord, ePlay, ePause, eProcess};  
    float *     command;
};

#endif
