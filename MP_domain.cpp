// ******************** flopc++ **********************************************
// File: MP_domain.cpp
// $Id$
// Author: Tim Helge Hultberg (thh@mat.ua.pt)
// Copyright (C) 2003 Tim Helge Hultberg
// All Rights Reserved.
//****************************************************************************

#include "MP_domain.hpp"
#include "MP_set.hpp"
#include "MP_boolean.hpp"

namespace flopc {

  class Functor_conditional : public Functor {
  public:
    Functor_conditional(const Functor* f, const vector<MP_boolean> & condition)
      : F(f), Condition(condition) {}
    virtual ~Functor_conditional() {}
    void operator()() const {
      bool goOn = true;
      for (unsigned int i = 0; i<Condition.size(); i++) {
	if (Condition[i]->evaluate()==false) {
	  goOn = false;
	  break;
	}
      }
      if (goOn == true) {
	F->operator()();
      }
    }
    const Functor* F;
    vector<MP_boolean> Condition;
  };	

  MP_domain operator*(const MP_domain& a, const MP_domain& b) {
    if (a.root == MP_domain::Empty.root) {
      return b;
    } else if (b.root == MP_domain::Empty.root) {
      return a;
    } else {
      MP_domain retval = a;
      retval.last->donext = b.root;
      b.root->count+=1;
      a.root->count+=1;
      retval.last = b.last;
      retval.condition.insert(retval.condition.end(),b.condition.begin(),
			      b.condition.end());
      return retval;
    }
  }
  


}

using namespace flopc;

MP_domain_base::MP_domain_base() : count(0), donext(0) {}

void MP_domain::Forall(const Functor* op) const {
    if (condition.size()>0) {
	last->donext = new Functor_conditional(op,condition);
    } else {
	last->donext = op;
    }
    root->operator()();
}

const MP_set_base* MP_domain_set::getSet() const {
    return S;
}

int MP_domain::size() const {
    return root->getSet()->size();
}

int MP_domain_set::evaluate() const {
    return I->evaluate();
}

void MP_domain_set::operator()() const {
    if (I->isInstantiated() == true) {
	(*donext)(); 
    } else {
	I->instantiate();
	for (int k=0; k<S->size(); k++) {
	    I->assign(k);
	    (*donext)();
	}
	I->assign(0);
	I->unInstantiate();
    }
}
