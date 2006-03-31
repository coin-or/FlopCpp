// ******************** flopc++ **********************************************
// File: MP_constant.hpp
// $Id$
// Author: Tim Helge Hultberg (thh@mat.ua.pt)
// Copyright (C) 2003 Tim Helge Hultberg
// All Rights Reserved.
//****************************************************************************

#ifndef _MP_constant_hpp_
#define _MP_constant_hpp_

#include "MP_utilities.hpp"

namespace flopc {
    class Constant_base {
	friend class Constant;
	friend class Handle<Constant_base*>;
    public:
	virtual double evaluate() const = 0;
    protected:
	Constant_base() : count(0) {}
	virtual ~Constant_base() {}
	int count;
    };
    
    class MP_index_exp;
    class DataRef;
    class MP_domain;

    class Constant : public Handle<Constant_base*> {
    public:
	Constant(Constant_base* r) : Handle<Constant_base*>(r) {}
	Constant(const MP_index_exp& i);
	Constant(const DataRef& d);
	Constant(double d);
	Constant(int d);
    };

    Constant abs(const Constant& c);
    Constant pos(const Constant& c);
    Constant ceil(const Constant& c);
    Constant floor(const Constant& c);
    Constant min(const Constant& a, const Constant& b);
    Constant max(const Constant& a, const Constant& b);
    Constant operator+(const Constant& a, const Constant& b);
    Constant operator-(const Constant& a, const Constant& b);
    Constant operator*(const Constant& a, const Constant& b);
    Constant operator/(const Constant& a, const Constant& b);

    Constant max(const MP_domain& i, const Constant& e);
    Constant min(const MP_domain& i, const Constant& e);
    Constant sum(const MP_domain& i, const Constant& e);
    Constant product(const MP_domain& i, const Constant& e);
}  // End of namespace flopc
#endif
