// ******************** FlopCpp **********************************************
// File: MP_set.cpp
// $Id$
// Author: Tim Helge Hultberg (thh@mat.ua.pt)
// Copyright (C) 2003 Tim Helge Hultberg
// All Rights Reserved.
//****************************************************************************

#include "MP_set.hpp"
#include "MP_model.hpp"
#include <sstream>
using namespace flopc;

namespace flopc {

    MP_set MP_set::Empty = *new MP_set(1);
    MP_set &MP_set::getEmpty() {
	return Empty;
    }
    
    void MP_set_base::display()const { 
	//I don't like this hack.  However, until messenger is segregated from the 
	// model, it's pretty impossible to use it in a shared fashion.
///	MP_model::getCurrentModel()->getMessenger()->logMessage(5,toString().c_str());
    }
    std::string MP_set::toString()const
    {
        if(this!=&Empty)
        {
        std::stringstream ss;
        ss<<getName()<<'|'<<cardinality<<'|';
        return ss.str();
        }
        return "";
    }
    std::string MP_set_base::toString()const
    {
        std::stringstream ss;
        MP_domain d(*this);
        ss<<getName()<<" over ("<<d.toString()<<") size="<<size();
        return ss.str();
    }

}
