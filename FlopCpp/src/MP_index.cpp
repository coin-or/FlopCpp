// ******************** FlopCpp **********************************************
// File: MP_index.cpp
// $Id$
// Author: Tim Helge Hultberg (thh@mat.ua.pt)
// Copyright (C) 2003 Tim Helge Hultberg
// All Rights Reserved.
//****************************************************************************

#include "MP_index.hpp"
#include "MP_domain.hpp"
#include "MP_set.hpp"
#include "MP_model.hpp"

namespace flopc {
  // Initialization of static member data
  MP_index& MP_index::Empty = *new MP_index();
  MP_index& MP_index::Any_index = *new MP_index();
  MP_index_exp MP_index_exp::Empty =  *new MP_index_exp(Constant(0.0));

  MP_index &MP_index::getEmpty() {
    return Empty;
  }
  MP_index &MP_index::Any() {
    return Any_index;
  }
  const MP_index_exp &MP_index_exp::getEmpty() {
    return Empty;
  }

  class MP_index_constant : public MP_index_base {
    friend class MP_index_exp;
  public:
  private:
    MP_index_constant(const Constant& c) : C(c) {}
    int evaluate() const {
      return int(C->evaluate()); 
    }
    MP_index* getIndex() const {
      return 0;
    }
    virtual MP_domain getDomain(MP_set* s) const{
      return MP_domain::getEmpty();
    }
    Constant C;
  };

  class MP_index_subsetRef : public MP_index_base {
    friend class MP_index_exp;
  private:
    MP_index_subsetRef(const SUBSETREF& s) : S(&s) {}
    int evaluate() const {
      cout<<"eval subsetref "<<S->evaluate()<<endl;
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
  
  MP_index_exp operator+(MP_index& i,const Constant& j) {
    return new MP_index_sum(i,j);
  }

  MP_index_exp operator+(MP_index& i,const int& j) {
    return new MP_index_sum(i,Constant(j));
  }

  MP_index_exp operator-(MP_index& i,const Constant& j) {
    return new MP_index_dif(i,j);
  }
    
  MP_index_exp operator-(MP_index& i,const int& j) {
    return new MP_index_dif(i,Constant(j));
  }

  MP_index_exp operator*(MP_index& i,const Constant& j) {
    return new MP_index_mult(i,j);
  }
  

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
  Handle<MP_index_base*>(&i) { operator->()->count++; }

MP_index_exp::MP_index_exp(const MP_index_exp &other):
  Handle<MP_index_base*>((const Handle<MP_index_base*> &)other) {}

