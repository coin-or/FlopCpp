// ******************** FlopCpp **********************************************
// File: MP_constant.cpp
// $Id$
// Author: Tim Helge Hultberg (thh@mat.ua.pt)
// Copyright (C) 2003 Tim Helge Hultberg
// All Rights Reserved.
//****************************************************************************

#include <float.h>
#include <cmath>
#include <sstream>
#include "MP_constant.hpp"
#include "MP_data.hpp"
#include "MP_domain.hpp"
#include "MP_index.hpp"

namespace flopc {

  class Constant_index : public Constant_base {
    friend class Constant;
  private:
    Constant_index(const MP_index_exp& i) : I(i) {}
    double evaluate() const {
      return I->evaluate(); 
    }
    const MP_index_exp I;
  };

  class Constant_double : public Constant_base {
    friend class Constant;
  private:
    Constant_double(double d) : D(d) {}
    double evaluate() const { 
      return D; 
    } 
    double D;
  };

  class Constant_abs : public Constant_base {
    friend Constant abs(const Constant& c);
  private:
    Constant_abs(const Constant& c) : C(c) {}
    double evaluate() const {
      return fabs(C->evaluate());
    }    
    Constant C;
  };
  Constant abs(const Constant& c) {
    return new Constant_abs(c);
  }

  class Constant_pos : public Constant_base {
    friend Constant pos(const Constant& c);
  private:
    Constant_pos(const Constant& c) : C(c) {}
    double evaluate() const {
      double temp = C->evaluate();
      if (temp>0) {
        return temp;
      } else {
        return 0.0;
      }
    }    
    Constant C;
  };
  Constant pos(const Constant& c) {
    return new Constant_pos(c);
  }

  class Constant_ceil : public Constant_base {
    friend Constant ceil(const Constant& c);
  private:
    Constant_ceil(const Constant& c) : C(c) {}
    double evaluate() const {
      return std::ceil(C->evaluate());
    }
    Constant C;
  };
  Constant ceil(const Constant& c) {
    return new Constant_ceil(c);
  }

  class Constant_floor : public Constant_base {
    friend Constant floor(const Constant& c);
  private:
    Constant_floor(const Constant& c) : C(c) {}
    double evaluate() const {
      return std::floor(C->evaluate());
    }
    Constant C;
  };
  Constant floor(const Constant& c) {
    return new Constant_floor(c);
  }

  class Constant_exp : public Constant_base {
  protected:
    Constant_exp(const Constant& i, const Constant& j) : left(i),right(j) {}
    Constant left, right;
  };

  class Constant_min_2 : public Constant_exp {
    friend Constant minimum(const Constant& a, const Constant& b);
  private:
    Constant_min_2(const Constant& i, const Constant& j) : Constant_exp(i,j) {}
    double evaluate() const {
      return std::min(left->evaluate(),right->evaluate());
    }
  };

  Constant minimum(const Constant& a, const Constant& b) {
    return new Constant_min_2(a,b);
  }

  class Constant_max_2 : public Constant_exp {
    friend Constant maximum(const Constant& a, const Constant& b);
  private:
    Constant_max_2(const Constant& i, const Constant& j) : Constant_exp(i,j) {}
    double evaluate() const {
      return std::max(left->evaluate(),right->evaluate());
    }
  };

  Constant maximum(const Constant& a, const Constant& b) {
    return new Constant_max_2(a,b);
  }

  class Constant_plus : public Constant_exp {
    friend Constant operator+(const Constant& a, const Constant& b);
    friend Constant operator+(MP_index& a, MP_index& b);
  private:
    Constant_plus(const Constant& i, const Constant& j) : Constant_exp(i,j) {}
    double evaluate() const {
      return left->evaluate()+right->evaluate();
    }
  };

  Constant operator+(const Constant& a, const Constant& b) {
    return new Constant_plus(a,b);
  }
  Constant operator+(MP_index& a, MP_index& b) {
    return new Constant_plus(Constant(a),Constant(b));
  }

  class Constant_minus : public Constant_exp {
    friend Constant operator-(const Constant& a, const Constant& b);
    friend Constant operator-(MP_index& a, MP_index& b);
  private:
    Constant_minus(const Constant& i, const Constant& j): Constant_exp(i,j) {}
    double evaluate() const {
      return left->evaluate()-right->evaluate(); 
    }
  };

  Constant operator-(const Constant& a, const Constant& b) {
    return new Constant_minus(a,b);
  }

  Constant operator-(MP_index& a, MP_index& b) {
    return new Constant_minus(Constant(a),Constant(b));
  }

  
  class Constant_unary_minus : public Constant_base {
    friend Constant operator-(const Constant& a);
  private:
    Constant_unary_minus(const Constant& i) : left(i) {}
    double evaluate() const {
      return -left->evaluate(); 
    }
    Constant left;
  };
  Constant operator-(const Constant& a) {
    return new Constant_unary_minus(a);
  }
  
  class Constant_mult : public Constant_exp {
    friend Constant operator*(const Constant& a, const Constant& b);
  private:
    Constant_mult(const Constant& i, const Constant& j) : Constant_exp(i,j) {}
    double evaluate() const {
      return left->evaluate()*right->evaluate(); 
    }
  };

  Constant operator*(const Constant& a, const Constant& b) {
    return new Constant_mult(a,b);
  }

  class Constant_div : public Constant_exp {
    friend Constant operator/(const Constant& a, const Constant& b); 
  private:
    Constant_div(const Constant& i, const Constant& j) : Constant_exp(i,j) {}
    double evaluate() const {
      return left->evaluate()/right->evaluate(); 
    }
  };

  Constant operator/(const Constant& a, const Constant& b) {
    return new Constant_div(a,b);
  }
   
  class Constant_if : public Constant_exp {
    friend Constant mpif(const MP_boolean& c, const Constant& a, const Constant& b); 
  private:
    Constant_if(const MP_boolean b, const Constant& i, const Constant& j) : Constant_exp(i,j), B(b) {}
    double evaluate() const {
      if (B->evaluate()==true) {
	return left->evaluate();
      } else {
	return right->evaluate(); 
      }
    }
    
    MP_boolean B;
  };
  
  Constant mpif(const MP_boolean& c, const Constant& a, const Constant& b) {
    return new Constant_if(c,a,b);
  }
   

  class Constant_max : public Constant_base {
    friend Constant maximum(const MP_domain& i, const Constant& e);
  private:
    Constant_max(const MP_domain& i, const Constant& e) : d(i), exp(e) {}
    double evaluate() const {    
      MaxFunctor MF(exp);
      d.forall(MF);
      return MF.the_max;
    }
    class MaxFunctor : public Functor {
    public:
      MaxFunctor(Constant exp) : C(exp), the_max(DBL_MIN) {}
      void operator()() const {
        double temp = C->evaluate();
        if (temp > the_max) {
          the_max = temp;
        }
      }
      Constant C;      
      mutable double the_max;
    };

    MP_domain d;
    Constant exp;
  };

  class Constant_min : public Constant_base, public Functor {
    friend Constant minimum(const MP_domain& i, const Constant& e);
  private:
    Constant_min(const MP_domain& i, const Constant& e) : d(i), exp(e) {}
    void operator()() const {
      double temp = exp->evaluate();
      if (temp < the_min) {
        the_min = temp;
      }
    }
    double evaluate() const {    
      the_min = DBL_MAX;
      d.forall(this);
      return the_min;
    }

    MP_domain d;
    Constant exp;
    mutable double the_min;
  };

  class Constant_sum : public Constant_base, public Functor {
    friend Constant sum(const MP_domain& i, const Constant& e);
  private:
    Constant_sum(const MP_domain& i, const Constant& e) : d(i), exp(e) {}
    void operator()() const {
      the_sum += exp->evaluate();
    }
    double evaluate() const {  
      the_sum = 0;
      d.forall(this);
      return the_sum;
    }

    MP_domain d;
    Constant exp;
    mutable double the_sum;
  };

  class Constant_product : public Constant_base, public Functor {
    friend Constant product(const MP_domain& i, const Constant& e);
  private:
    Constant_product(const MP_domain& i, const Constant& e) : d(i), exp(e) {}
    void operator()() const {
      the_product *= exp->evaluate();
    }
    double evaluate() const {  
      the_product = 1;
      d.forall(this);
      return the_product;
    }

    MP_domain d;
    Constant exp;
    mutable double the_product;
  };

  Constant maximum(const MP_domain& i, const Constant& e) {
    return new Constant_max(i,e);
  }
  Constant minimum(const MP_domain& i, const Constant& e) {
    return new Constant_min(i,e);
  }
  Constant sum(const MP_domain& i, const Constant& e) {
    return new Constant_sum(i,e);
  }
  Constant product(const MP_domain& i, const Constant& e) {
    return new Constant_product(i,e);
  }

  Constant::Constant(const DataRef& d) : 
    Handle<Constant_base*>(const_cast<DataRef*>(&d)) {}

  Constant::Constant(const MP_index_exp& i) :
    Handle<Constant_base*>(new Constant_index(i)){}

  Constant::Constant(double d) :
    Handle<Constant_base*>(new Constant_double(d)) {}

  Constant::Constant(int d) :
    Handle<Constant_base*>(new Constant_double(d)) {}
 
} // End of namespace flopc

