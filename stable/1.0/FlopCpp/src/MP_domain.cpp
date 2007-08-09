// ******************** FlopCpp **********************************************
// File: MP_domain.cpp
// $Id$
// Author: Tim Helge Hultberg (thh@mat.ua.pt)
// Copyright (C) 2003 Tim Helge Hultberg
// All Rights Reserved.
//****************************************************************************

#include "MP_domain.hpp"
#include "MP_set.hpp"
#include "MP_boolean.hpp"
#include "MP_model.hpp"

namespace flopc {
  MP_domain_set::MP_domain_set(const MP_set* s, MP_index* i) 
    : S(s), I(i)  {} 
  MP_domain_set::~MP_domain_set() {}
  MP_domain MP_domain_set::getDomain(MP_set* s) const {
    return MP_domain(const_cast<MP_domain_set*>(this));
  }

  class Functor_conditional : public Functor {
  public:
    Functor_conditional(const Functor* f, const std::vector<MP_boolean> & condition)
      : F(f), Condition(condition) {}
    virtual ~Functor_conditional() {}
    void operator()() const {
      bool goOn = true;
      for (size_t i = 0; i<Condition.size(); i++) {
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
    std::vector<MP_boolean> Condition;
  };	
}

using namespace flopc;

const MP_domain* MP_domain::Empty = 0;

const MP_domain& MP_domain::getEmpty() {
  if(Empty == 0) {
    Empty= new MP_domain(new MP_domain_set(&MP_set::getEmpty(),&MP_set::getEmpty()));
  }
  return *Empty;
}


MP_domain_base::MP_domain_base() : count(0), donext(0) {}
MP_domain_base::~MP_domain_base() {}

Functor* MP_domain_base::makeInsertFunctor() const {
  return 0;
}

size_t MP_domain_base::size() const { 
  return count;
}


void MP_domain_base::display()const { 
  std::stringstream ss;
  ss<<"domain_base::display() size="<<size()<<std::ends;
  MP_model::getCurrentModel()->getMessenger()->logMessage(5,ss.str().c_str());
}

MP_domain::MP_domain() : Handle<MP_domain_base*>(0), last(0) {}
MP_domain::MP_domain(MP_domain_base* r) : Handle<MP_domain_base*>(r), last(r) {}
MP_domain::~MP_domain() {}

MP_domain MP_domain::such_that(const MP_boolean& b) {
  if (b.operator ->() != 0) {
    condition.push_back(b);
  }
  return *this;
}

void MP_domain::forall(const Functor& op) const {
  forall(&op);
}
void MP_domain::forall(const Functor* op) const {
  if (condition.size()>0) {
    last->donext = new Functor_conditional(op,condition);
  } else {
    last->donext = op;
  }
  operator->()->operator()();
}

const MP_set_base* MP_domain_set::getSet() const {
  return S;
}

size_t MP_domain::size() const {
  return operator->()->getSet()->size();
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

MP_index* MP_domain_set::getIndex() const {
  return I;
}


flopc::MP_domain flopc::operator*(const flopc::MP_domain& a, const flopc::MP_domain& b) {
  if (a.operator->() == MP_domain::getEmpty().operator->()) {
    return b;
  } else if (b.operator->() == MP_domain::getEmpty().operator->()) {
    return a;
  } else {
    MP_domain retval = a;
    retval.last->donext = b.operator->();
    const_cast<MP_domain&>(b).increment();
    const_cast<MP_domain&>(a).increment();
    retval.last = b.last;
    retval.condition.insert(retval.condition.end(),b.condition.begin(),
                            b.condition.end());
    return retval;
  }

}
