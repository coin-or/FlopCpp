// ******************** flopc++ **********************************************
// File: MP_index.cpp
// $Id$
// Author: Tim Helge Hultberg (thh@mat.ua.pt)
// Copyright (C) 2003 Tim Helge Hultberg
// All Rights Reserved.
//****************************************************************************

#include "MP_index.hpp"
#include "MP_domain.hpp"
#include "MP_set.hpp"

namespace flopc {

    class MP_index_constant : public MP_index_base {
	friend class MP_index_exp;
    private:
	MP_index_constant(const Constant& c) : C(c) {}
	int evaluate() const {
	    return int(C->evaluate()); 
	}
	MP_index* getIndex() const {
	    return 0;
	}
	virtual MP_domain getDomain(MP_set* s) const{
	    return MP_domain::Empty;
	}
	Constant C;
    };

    class MP_index_subsetRef : public MP_index_base {
	friend class MP_index_exp;
    private:
	MP_index_subsetRef(const SUBSETREF& s) : S(&s) {}
	int evaluate() const {
	    return int(S->evaluate()); 
	}
	MP_index* getIndex() const {
	    return S->getIndex();
	}
	virtual MP_domain getDomain(MP_set* s) const{
	    return MP_domain(S->getDomain(s));
	}
	const SUBSETREF* S;
    };

} // End of namespace flopc

using namespace flopc;


MP_domain MP_index::getDomain(MP_set* s) const{
    return new MP_domain_set(s,const_cast<MP_index*>(this)) ;
}

MP_domain MP_index_mult::getDomain(MP_set* s) const{
    return left->getDomain(s); 
}

MP_domain MP_index_sum::getDomain(MP_set* s) const{
    return left->getDomain(s); 
}

MP_domain MP_index_dif::getDomain(MP_set* s) const{
    return left->getDomain(s);
}

MP_index_exp::MP_index_exp(int i) : 
    Handle<MP_index_base*>(new MP_index_constant(Constant(i))) {} 

MP_index_exp::MP_index_exp(const SUBSETREF& s) : 
    Handle<MP_index_base*>(new MP_index_subsetRef(s)) {}

MP_index_exp::MP_index_exp(const Constant& c) : 
    Handle<MP_index_base*>(new MP_index_constant(c)) {}

MP_index_exp::MP_index_exp(MP_index& i) : 
    Handle<MP_index_base*>(&i) { root->count++; }

