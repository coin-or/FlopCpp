// ******************** FlopCpp **********************************************
// File: MP_expression.cpp
// $Id$
// Author: Tim Helge Hultberg (thh@mat.ua.pt)
// Copyright (C) 2003 Tim Helge Hultberg
// All Rights Reserved.
//****************************************************************************

#include <sstream>
#include "MP_expression.hpp"
#include "MP_constant.hpp"
#include "MP_boolean.hpp"
#include "MP_constraint.hpp"
#include "MP_set.hpp"
#include "MP_variable.hpp"

namespace flopc {

  class Expression_constant : public TerminalExpression {
    friend class MP_expression;

  private:
    Expression_constant(const Constant& c) : C(c) {}
    double level() const { 
      return C->evaluate(); 
    }
    double getValue() const {
      return C->evaluate();
    }
    int getColumn() const {
      return -1;
    }
    int getStage() const {
      return C->getStage(); //NB to be changed
    }
    void generate(const MP_domain& domain,
		  vector<Constant> multiplicators,
		  GenerateFunctor& f,
		  double m) const {
      f.setMultiplicator(multiplicators,m);
      f.setTerminalExpression(this);
      domain.Forall(&f);
    }
    void insertVariables(set<MP_variable*>& v) const {}

    Constant C;
  };

  class Expression_plus : public Expression_operator {
    friend MP_expression operator+(const MP_expression& e1, const MP_expression& e2);
    friend MP_expression operator+(const MP_expression& e1, const Constant& e2);
    friend MP_expression operator+(const Constant& e1, const MP_expression& e2);	
  private:
    Expression_plus(const MP_expression& e1, const MP_expression& e2) : 
      Expression_operator(e1,e2) {}
    double level() const { 
      return left->level()+right->level(); 
    }
    void generate(const MP_domain& domain,
		  vector<Constant> multiplicators,
		  GenerateFunctor& f,
		  double m) const { 
      left->generate(domain, multiplicators,f,m);
      right->generate(domain, multiplicators,f,m);
    }
  };

  class Expression_minus : public Expression_operator {
    friend MP_expression operator-(const MP_expression& e1, const MP_expression& e2);
    friend MP_expression operator-(const MP_expression& e1, const Constant& e2); 
    friend MP_expression operator-(const Constant& e1, const MP_expression& e2);
  private:
    Expression_minus(const MP_expression& e1, const MP_expression& e2) : 
      Expression_operator(e1,e2) {}
    double level() const { 
      return left->level()-right->level(); 
    }
    void generate(const MP_domain& domain,
		  vector<Constant> multiplicators,
		  GenerateFunctor& f,
		  double m) const {
      left->generate(domain, multiplicators,f,m);
      right->generate(domain, multiplicators,f,-m);
    }
  };

  class Expression_mult : public MP_expression_base {
    friend MP_expression operator*(const Constant& e1, const MP_expression& e2); 
    friend MP_expression operator*(const MP_expression& e1, const Constant& e2);

  private:
    Expression_mult(const Constant& e1, const MP_expression& e2) : 
      left(e1), right(e2) {}
    double level() const { 
      return left->evaluate()*right->level(); 
    }
    void generate(const MP_domain& domain,
		  vector<Constant> multiplicators,
		  GenerateFunctor& f,
		  double m) const {
      multiplicators.push_back(left);
      right->generate(domain, multiplicators, f, m);
    }
    void insertVariables(set<MP_variable*>& v) const {
      right->insertVariables(v);
    }
    Constant left;
    MP_expression right;
  };

  class Expression_div : public MP_expression_base {
    friend MP_expression operator/(const MP_expression& e1, const Constant& e2);
  private:
    Expression_div(const MP_expression& e, const Constant& c) : 
      left(e), right(c) {}
    double level() const { 
      return left->level()/right->evaluate(); 
    }
    void generate(const MP_domain& domain,
		  vector<Constant> multiplicators,
		  GenerateFunctor& f,
		  double m) const {
      multiplicators.push_back(1/right);
      left->generate(domain, multiplicators, f, m);
    }
    void insertVariables(set<MP_variable*>& v) const {
      left->insertVariables(v);
    }
    MP_expression left;
    Constant right;
  };
    
  class Expression_sum : public MP_expression_base, public Functor {
    friend MP_expression sum(const MP_domain& d, const MP_expression& e);
  private:
    Expression_sum(const MP_domain& d, const MP_expression& e) : 
      D(d), exp(e) {}
    void operator()() const {
      the_sum += exp->level();
    }
    double level() const {
      the_sum = 0;
      D.Forall(this);
      return the_sum;
    } 
    void generate(const MP_domain& domain,
		  vector<Constant> multiplicators,
		  GenerateFunctor& f,
		  double m) const {
      // The order, D*domain (NOT domain*D), is important for efficiency! 
      exp->generate(D*domain, multiplicators, f, m); 
    }
    void insertVariables(set<MP_variable*>& v) const {
      exp->insertVariables(v);
    }

    mutable double the_sum;
    MP_domain D;
    MP_expression exp;
  };


  MP_expression operator+(const MP_expression& e1, const MP_expression& e2) {
    return new Expression_plus(e1, e2);
  }
  MP_expression operator+(const MP_expression& e1, const Constant& e2) {
    return new Expression_plus(e1, e2);
  }
  MP_expression operator+(const Constant& e1, const MP_expression& e2) {
    return new Expression_plus(e1, e2);
  }

  MP_expression operator-(const MP_expression& e1, 
                          const MP_expression& e2) {
    return new Expression_minus(e1, e2);
  }
  MP_expression operator-(const MP_expression& e1, 
                          const Constant& e2) {
    return new Expression_minus(e1, e2);
  }
  MP_expression operator-(const Constant& e1, 
                          const MP_expression& e2) {
    return new Expression_minus(e1, e2);
  }

  MP_expression operator*(const Constant& e1, const MP_expression& e2) {
    return new Expression_mult(e1, e2);
  }
  MP_expression operator*(const MP_expression& e1, const Constant& e2) {
    return new Expression_mult(e2, e1);
  }
  MP_expression operator/(const MP_expression& e1, const Constant& e2) {
    return new Expression_div(e1, e2);
  }

  MP_expression sum(const MP_domain& d, const MP_expression& e) {
    return new Expression_sum(d, e);  
  }
    
} // End of namespace flopc

using namespace flopc;


MP_expression::MP_expression(const Constant &c) :
  Handle<MP_expression_base*>(new Expression_constant(c)) {} 

MP_expression::MP_expression(const VariableRef &v) : 
  Handle<MP_expression_base*>(const_cast<VariableRef*>(&v)) {} 

int GenerateFunctor::row_number() const {
  return R->row_number();
}

void GenerateFunctor::operator()() const {
  double multiplicator = m_;
  int stage = 0;
  for (unsigned int i=0; i<multiplicators.size(); i++) {
    multiplicator *= multiplicators[i]->evaluate();
    if (multiplicators[i]->getStage() > stage) {
      stage = multiplicators[i]->getStage();
    }
  }
  int rowNumber = row_number();
  if (rowNumber != outOfBound) {
    int colNumber = C->getColumn();
    if ( colNumber != outOfBound  ) {
      double val = multiplicator*C->getValue();
      int tstage = C->getStage();
      if (tstage > stage) {
        stage = tstage;
      }
      // For the SP core it might be usefull to generate zero coefs
      // if (val != 0) {
      Coefs.push_back(Coef(colNumber, rowNumber, val, stage));
      //}
    }
  }
}
