//
//	Reset.h		This file is a part of the IKAROS project
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

#ifndef Reset_
#define Reset_

#include "IKAROS.h"

class Reset: public Module
{
public:
    static Module * Create(Parameter * p) { return new Reset(p); }

    Reset(Parameter * p) : Module(p) {}
    virtual ~Reset();

    void 		Init();
    void 		Tick();

    // pointers to inputs and outputs
    // and integers to represent their sizes

    float *     input_array;
    int         input_array_size;
    float *     command;
    
    float *     output_array;
    float *     sync;

    // internal data storage
    float *     internal_array;

    // parameter values
	bool       	debugmode;
    int         delay;
    float       equaltolerance;

    // internals
    enum State {eStart=0, eLoaded, eResetting, eAt_Reset};
    enum Command {eNone=0, eStore, eReset, eStop};
    State current_state;
};

#endif
