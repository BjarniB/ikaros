//
//	Interpolator.h		This file is a part of the IKAROS project
//
//    Copyright (C) 2014 Asas Innab
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

#ifndef Interpolator_
#define Interpolator_

#include "IKAROS.h"


class Interpolator: public Module
{
public:
    static Module * Create(Parameter * p) { return new Interpolator(p); }

    Interpolator(Parameter * p) : Module(p) {}
    virtual ~Interpolator();

    void 		Init();
    void 		Tick();
    float *     GetInterpolation(int tick);
    float *     CubicInterpolationY ();
    float *     catmullRomSpline();
    int         FindP2(int index_y);


    // pointers to inputs and outputs
    // and integers to represent their sizes


    float**     input_ticks;
    float**     input_values;
    float *     input_size_x;
    float *     input_size_y;

    float *     output_array;
    int         output_array_size;
    float *     output_test;
    float *     output_test2;

    // parameter values
    int         tick;
    
    float       tau;
    float       M11;
    float       M12;
    float       M13;
    float       M14;
    float       M21;
    float       M22;
    float       M23;
    float       M24;
    float       M31;
    float       M32;
    float       M33;
    float       M34;
    float       M41;
    float       M42;
    float       M43;
    float       M44;
      
    enum State {eStart=0, eInterpolating, ePaused};
    State current_state;

    enum Command {eNone=0, eInterpolate=-1, ePause=-2};  
    float *     input_command;
};

#endif
