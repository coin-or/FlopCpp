// ******************** FlopCpp **********************************************
// File: MP_expression.cpp
//****************************************************************************

#include <sstream>
#include "MP_expression.hpp"
#include "MP_constant.hpp"
#include "MP_boolean.hpp"
#include "MP_constraint.hpp"
#include "MP_set.hpp"
#include "MP_variable.hpp"
#include "MP_model.hpp"
#include <OsiSolverInterface.hpp>

namespace flopc {

  VariableRef::VariableRef(MP_variable* v, 
                           const MP_index_exp& i1,
                           const MP_index_exp& i2,
                           const MP_index_exp& i3,
                           const MP_index_exp& i4,
                           const MP_index_exp& i5) :
    V(v),offset(0),I1(i1),I2(i2),I3(i3),I4(i4),I5(i5) { 
    assert(v != 0);
    offset = v->offset; 
  }
  
  double VariableRef::level() const {
    return  V->M->Solver->getColSolution()[V->offset +
                                           V->f(V->S1->evaluate(),
                                                V->S2->evaluate(),
                                                V->S3->evaluate(),
                                                V->S4->evaluate(),
                                                V->S5->evaluate())];
  }

  int VariableRef::getColumn() const { 
    int i1 = V->S1->check(I1->evaluate());
    int i2 = V->S2->check(I2->evaluate());
    int i3 = V->S3->check(I3->evaluate());
    int i4 = V->S4->check(I4->evaluate());
    int i5 = V->S5->check(I5->evaluate());
    
    if (i1==outOfBound || i2==outOfBound || i3==outOfBound ||
        i4==outOfBound || i5==outOfBound) {
      return outOfBound;
    } else {
      return V->offset +  V->f(i1,i2,i3,i4,i5);
    }
  }

  void VariableRef::generate(const MP_domain& domain,
                             vector<Constant > multiplicators,
                             MP::GenerateFunctor& f,
                             double m)  const {
    f.setMultiplicator(multiplicators,m);
    f.setTerminalExpression(this);
    domain.forall(&f);
  }

  class Expression_constant : public TerminalExpression, public MP {
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
		  MP::GenerateFunctor& f,
                  double m) const {
      f.setMultiplicator(multiplicators,m);
      f.setTerminalExpression(this);
      domain.forall(&f);
    }
    void insertVariables(set<MP_variable*>& v) const {}

    Constant C;
  };


  class Expression_operator : public MP_expression_base, public MP  {
  protected:
    Expression_operator(const MP_expression& e1, const MP_expression& e2) : 
      left(e1),right(e2) {}

    void insertVariables(set<MP_variable*>& v) const {
      left->insertVariables(v);
      right->insertVariables(v);
    }
 
    MP_expression left,right;
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
		  MP::GenerateFunctor& f,
                  double m) const { 
      left->generate(domain, multiplicators, f, m);
      right->generate(domain, multiplicators, f, m);
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
		  MP::GenerateFunctor& f,
                  double m) const {
      left->generate(domain, multiplicators, f, m);
      right->generate(domain, multiplicators, f, -m);
    }
  };

  class Expression_mult : public MP_expression_base,  MP  {
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
		  MP::GenerateFunctor& f,
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

  class Expression_div : public MP_expression_base, MP  {
    friend MP_expression operator/(const MP_expression& e1, const Constant& e2);
  private:
    Expression_div(const MP_expression& e, const Constant& c) : 
      left(e), right(c) {}
    double level() const { 
      return left->level()/right->evaluate(); 
    }
    void generate(const MP_domain& domain,
		  vector<Constant> multiplicators,
		  MP::GenerateFunctor& f,
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
    
  class Expression_sum : public MP_expression_base, public MP  {
    friend MP_expression sum(const MP_domain& d, const MP_expression& e);
  private:
    Expression_sum(const MP_domain& d, const MP_expression& e) : D(d), exp(e) {}

    double level() const {
      SumFunctor SF(exp);
      D.forall(SF);
      return SF.the_sum;
    } 
    void generate(const MP_domain& domain,
		  vector<Constant> multiplicators,
		  MP::GenerateFunctor& f,
		  double m) const {
      // The order, D*domain (NOT domain*D), is important for efficiency! 
      exp->generate(D*domain, multiplicators, f, m); 
    }
    void insertVariables(set<MP_variable*>& v) const {
      exp->insertVariables(v);
    }
    
    class SumFunctor : public Functor {
    public:
      SumFunctor(MP_expression exp) : E(exp), the_sum(0) {}
      void operator()() const {
        the_sum += E->level();
      }
      MP_expression E;      
      mutable double the_sum;
    };

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

  MP_expression operator-(const MP_expression& e1, const MP_expression& e2) {  
    return new Expression_minus(e1, e2);
  }
  MP_expression operator-(const MP_expression& e1, const Constant& e2) {
    return new Expression_minus(e1, e2);
  }
  MP_expression operator-(const Constant& e1, const MP_expression& e2) {
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


bool MP::CoefLess::operator() (const MP::Coef& a, const MP::Coef& b) const {
  if (a.col < b.col) {
    return true;
  } else if (a.col == b.col && a.row < b.row) {
    return true;
  } else {
    return false;
  }
}

void MP::GenerateFunctor::operator()() const {
  assert(M==-1 or M==1);
  double multiplicator = M;
  int stage = 0;
  for (unsigned int i=0; i<multiplicators.size(); i++) {
    multiplicator *= multiplicators[i]->evaluate();
    if (multiplicators[i]->getStage() > stage) {
      stage = multiplicators[i]->getStage();
    }
  }
  int rowNumber = -1;
  if (R != 0) {
    rowNumber = R->row_number();
  }
  if (rowNumber != outOfBound) {
    assert(C != 0);
    int colNumber = C->getColumn();
    if ( colNumber != outOfBound  ) {
      double val = multiplicator*C->getValue();
      int tstage = C->getStage();
      if (tstage > stage) {
        stage = tstage;
      }
      // if (val != 0) {
      Coefs.push_back(MP::Coef(colNumber, rowNumber, val, stage));
      //}
    }
  }
}
