// ******************** flopc++ **********************************************
// File: MP_index.hpp
// $Id$
// Author: Tim Helge Hultberg (thh@mat.ua.pt)
// Copyright (C) 2003 Tim Helge Hultberg
// All Rights Reserved.
//****************************************************************************

#ifndef _MP_index_hpp_
#define _MP_index_hpp_

#include "MP_utilities.hpp"
#include "MP_constant.hpp"


namespace flopc {

    class MP_index;
    class MP_domain;
    class MP_set;

    class MP_index_base {
	friend class Handle<MP_index_base*>;
	friend class MP_index_exp;
    public:
	virtual int evaluate() const = 0;
	virtual MP_index* getIndex() const = 0;
	virtual MP_domain getDomain(MP_set* s) const = 0;
    protected:
	MP_index_base() : count(0) {}
	virtual ~MP_index_base() {}
    private:
	int count;
    };

    class MP_index : public MP_index_base {
    public:
	MP_index() : index(0), instantiated(false) {}
	int evaluate() const { 
	    return index; 
	}
	bool isInstantiated() const { 
	    return instantiated; 
	}
	void assign(int i) { 
	    index = i;
	}
	void unInstantiate() {
	    instantiated = false; 
	}
	void instantiate() {
	    instantiated = true; 
	}
	MP_index* getIndex() const {
	    return const_cast<MP_index*>(this);
	}
	virtual MP_domain getDomain(MP_set* s) const;
	static MP_index& Any;
	static MP_index& Empty;
    private:
	int index;
	bool instantiated;
    };

    Constant operator+(MP_index& a, MP_index& b);
    Constant operator-(MP_index& a, MP_index& b);

    class SUBSETREF;

    class MP_index_exp : public Handle<MP_index_base*> {
    public:
	MP_index_exp(MP_index_base* r) : Handle<MP_index_base*>(r) {} 
	MP_index_exp(int i=0); 
	MP_index_exp(const Constant& c);
	MP_index_exp(MP_index& i);
	MP_index_exp(const SUBSETREF& d);
	static MP_index_exp Empty;
    };

    class MP_index_mult : public MP_index_base {
	friend MP_index_exp operator*(MP_index& i,const Constant& j) {
	    return new MP_index_mult(i,j);
	}
    private:
	MP_index_mult(MP_index& i, const Constant& j) : left(i), right(j) {}
	int evaluate() const {
	    return left->evaluate()*int(right->evaluate()); 
	}
	MP_index* getIndex() const {
	    return left->getIndex();
	}
	virtual MP_domain getDomain(MP_set* s) const;
	MP_index_exp left;
	Constant right;
    };

    class MP_index_sum : public MP_index_base {
	friend MP_index_exp operator+(MP_index& i,const Constant& j) {
	    return new MP_index_sum(i,j);
	}
    private:
	MP_index_sum(MP_index& i, const Constant& j) : left(i), right(j) {}
	int evaluate() const {
	    return left->evaluate()+int(right->evaluate()); 
	}
	MP_index* getIndex() const {
	    return left->getIndex();
	}
	virtual MP_domain getDomain(MP_set* s) const;
	MP_index_exp left;
	Constant right;
    };

    class MP_index_dif : public MP_index_base {
	friend MP_index_exp operator-(MP_index& i,const Constant& j) {
	    return new MP_index_dif(i,j);
	}
    private:
	MP_index_dif(MP_index& i, const Constant& j) : left(i), right(j) {}
	int evaluate() const {
	    return left->evaluate()-int(right->evaluate()); 
	}
	MP_index* getIndex() const {
	    return left->getIndex();
	}
	virtual MP_domain getDomain(MP_set* s) const;
	MP_index_exp left;
	Constant right;
    };



} // End of namespace flopc
#endif
