// ******************** FlopCpp **********************************************
// File: MP_boolean.hpp
// $Id$
// Author: Tim Helge Hultberg (thh@mat.ua.pt)
// Copyright (C) 2003 Tim Helge Hultberg
// All Rights Reserved.
//****************************************************************************

#ifndef _MP_boolean_hpp_
#define _MP_boolean_hpp_

#include <vector>
using std::vector;

#include "MP_index.hpp"
#include "MP_constant.hpp"
#include "MP_utilities.hpp"

namespace flopc {

    class Boolean_base {
	friend class Handle<Boolean_base*>;
	friend class MP_boolean;
    public:
	virtual bool evaluate() const = 0;
    protected:
	Boolean_base() : count(0) {}
	virtual ~Boolean_base() {}

	int count;
    };

    class SUBSETREF;

    class MP_boolean : public Handle<Boolean_base*> {
	friend class MP_domain_base_;
    public:
	MP_boolean() : Handle<Boolean_base*>(0) {}
	MP_boolean(bool b);
	MP_boolean(const Constant& c); 
	MP_boolean(SUBSETREF& c); 
	MP_boolean(Boolean_base* r) : Handle<Boolean_base*>(r) {};

    };

    MP_boolean operator&&(const MP_boolean& e1, const MP_boolean& e2);
    MP_boolean operator||(const MP_boolean& e1, const MP_boolean& e2);

    MP_boolean alltrue(const MP_domain& d, const MP_boolean& b);

    MP_boolean operator<=(const MP_index_exp& e1, const MP_index_exp& e2);
    MP_boolean operator<=(const Constant& e1, const Constant& e2);
    MP_boolean operator<(const MP_index_exp& e1, const MP_index_exp& e2);
    MP_boolean operator<(const Constant& e1, const Constant& e2); 
    MP_boolean operator>=(const MP_index_exp& e1, const MP_index_exp& e2);
    MP_boolean operator>=(const Constant& e1, const Constant& e2);
    MP_boolean operator>(const MP_index_exp& e1, const MP_index_exp& e2);
    MP_boolean operator>(const Constant& e1, const Constant& e2);
    MP_boolean operator==(const MP_index_exp& e1, const MP_index_exp& e2);
    MP_boolean operator==(const Constant& e1, const Constant& e2);
    MP_boolean operator!=(const MP_index_exp& e1, const MP_index_exp& e2);
    MP_boolean operator!=(const Constant& e1, const Constant& e2);

} // End of namespace flopc
#endif
