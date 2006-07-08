// ******************** flopc++ **********************************************
// File: MP_domain.hpp
// $Id$
// Author: Tim Helge Hultberg (thh@mat.ua.pt)
// Copyright (C) 2003 Tim Helge Hultberg
// All Rights Reserved.
//****************************************************************************
#ifndef _MP_domain_hpp_
#define _MP_domain_hpp_

#include <vector>
#include <map>
#include "MP_utilities.hpp"
#include "MP_boolean.hpp" 
#include "MP_index.hpp" 

namespace flopc {

class MP_set_base;
class MP_set;
class MP_index;

template<int nbr> class MP_subset;

    /** @brief Reference to a set of index values.
        @ingroup INTERNAL_USE
        @note FOR INTERNAL USE: This is not normally used directly by the
    calling code.
     */
class MP_domain_base : public Functor, public MP_index_base {
    friend class MP_domain;
    friend class Handle<MP_domain_base*>;
    friend MP_domain operator*(const MP_domain& a, const MP_domain& b);
private:
    int count;
public:
    MP_domain_base();
	virtual ~MP_domain_base();

    virtual Functor* makeInsertFunctor() const;
    virtual MP_index* getIndex() const = 0;
    virtual const MP_set_base* getSet() const = 0;
    void display()const;
    virtual size_t size() const ;
    const Functor* donext;
};

class MP_domain : public Handle<MP_domain_base*> {
    friend MP_domain flopc::operator*(const MP_domain& a, const MP_domain& b);
    
    friend class MP_constraint;
    friend class MP_index_exp;
public:
    MP_domain();
	~MP_domain();
    MP_domain(MP_domain_base* r);

    MP_domain such_that(const MP_boolean& b);

    void Forall(const Functor* op) const;
    size_t size() const;
    static const MP_domain& getEmpty();
private:
	std::vector<MP_boolean> condition;
    Handle<MP_domain_base*> last;
private:
    static const MP_domain* Empty;
};

class MP_domain_set : public MP_domain_base {
public:
    MP_domain_set(const MP_set* s, MP_index* i);
    void operator()() const;
    int evaluate() const;
    const MP_set_base* getSet() const;
    MP_index* getIndex() const;
    MP_domain getDomain(MP_set* s) const ;
    ~MP_domain_set();
private:
    const MP_set* S;
     MP_index* I;
};

template<int nbr> class MP_domain_subset;

template<int nbr> class insertFunctor : public Functor {
public:
    insertFunctor(MP_domain_subset<nbr>* d) : D(d) {}
    void operator()() const { 
	std::vector<int> elm(nbr);
	for (int i=0; i<nbr; i++) {
	    elm[i] = D->I[i]->evaluate();
	}
	D->S->insert(elm);
    }
private:
    MP_domain_subset<nbr>* D;
};

template<int nbr> class MP_domain_subset : public MP_domain_base {
    friend class insertFunctor<nbr>;
public:
    MP_domain_subset<nbr> (MP_subset<nbr>* s, 
			   const std::vector<MP_index*> &i) : S(s), I(i){}

    int evaluate() const {
	return S->evaluate(I);
    }
    MP_set_base* getSet() const {
	return S;
    }
    MP_index* getIndex() const {
	return S;
    }
    MP_domain getDomain(MP_set* s) const {
	return MP_domain(const_cast<MP_domain_subset<nbr>*>(this));
    }
    void operator()() const {
	bool isBound[nbr];
	bool allBound = true;
	for (int j=0; j<nbr; j++) {
	    if (I[j]->isInstantiated() == true) {
		isBound[j] = true;
	    } else {
		isBound[j] = false;
		allBound = false;
		if (I[j]!=&MP_index::getEmpty()) { 
		    I[j]->instantiate();
		}
	    }
	}
	if (allBound == true) {
	    (*donext)(); 
	} else {
	    std::map<std::vector<int>, int>::const_iterator i;
	    int counter = 0;
	    for (i = S->elements.begin(); i != S->elements.end(); i++) {
		S->assign(counter);
		counter++;
		bool goOn = true;
		for (int j=0; j<nbr; j++) {
		    if (isBound[j] == true) {
			if (I[j]->evaluate() != i->first[j]) {
			    goOn = false;
			    break;
			}
		    } else {
			I[j]->assign(i->first[j]);
		    }
		}
		if (goOn == true) {
		    (*donext)();
		}
	    }
	}
	for (int j=0; j<nbr; j++) {
	    if (isBound[j] == false) {
		I[j]->assign(0);
		I[j]->unInstantiate();
	    }
	}
    }    

    Functor* makeInsertFunctor() const {
	return new insertFunctor<nbr>(
	    const_cast<MP_domain_subset<nbr>*>(this));
    }
private:
    MP_subset<nbr>* S;
    std::vector<MP_index*> I;
};

MP_domain operator*(const MP_domain& a, const MP_domain& b);
  
}  // End of namespace flopc
#endif
