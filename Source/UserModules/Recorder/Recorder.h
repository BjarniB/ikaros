//
//	Recorder.h		This file is a part of the IKAROS project
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

#ifndef Recorder_
#define Recorder_

#include "IKAROS.h"

class Recorder: public Module
{
public:
    static Module * Create(Parameter * p) { return new Recorder(p); }

    Recorder(Parameter * p) : Module(p) {}
    virtual ~Recorder();

    void 		Init();
    void 		Tick();

    void Record();
    void Play();
    void Pre_play();
    
    float *     input_array;
    int         input_array_size;
    float *     command;
    float *     sync_in;
    
    float *     output_array;
    float *     sync_out;
    
    // internal data storage

    
    float **    recording;

    // parameter values

    float       maxlength;
    bool        repeat;
    bool        debug;

    int         tick;
    int         endtick;

    enum State {eStart=0, eRecording, ePlaying, eRecord_Paused, ePlay_Paused, eReady_To_Play};
    enum Command {eNone=0, eRecord, ePlay, ePause};  
    State       current_state;
    
};

#endif
