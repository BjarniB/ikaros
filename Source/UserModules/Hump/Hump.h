//
//	Hump.h		This file is a part of the IKAROS project
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

#ifndef Hump_
#define Hump_

#include "IKAROS.h"

class Hump: public Module
{
public:
    static Module * Create(Parameter * p) { return new Hump(p); }

    Hump(Parameter * p) : Module(p) {}
    virtual ~Hump();

    void 		Init();
    void 		Tick();
    float       Map_Range(float value, float low1, float high1, float low2, float high2);

    // pointers to inputs and outputs
    // and integers to represent their sizes
    float *     current_pos;
    int         size;
    float *     target_pos;
    float *     prev_target_pos;
    //float *     input_state;
    //int         input_state_size;

    float *     output_array;
    
    // internal data storage

    float *     startpos;
    bool        first;
    //float **    internal_matrix;

    // parameter values
    float       state;
    float       timescale;
    float       scale;
    float       shift;
    float       offset;


    float       t;
};

#endif
