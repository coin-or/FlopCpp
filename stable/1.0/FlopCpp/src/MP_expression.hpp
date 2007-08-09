// ******************** FlopCpp **********************************************
// File: MP_expression.hpp
// $Id$
// Author: Tim Helge Hultberg (thh@mat.ua.pt)
// Copyright (C) 2003 Tim Helge Hultberg
// All Rights Reserved.
// ****************************************************************************

#ifndef _MP_expression_hpp_
#define _MP_expression_hpp_

#include <vector>
#include <set>
using std::vector;
using std::set;

#include "MP_domain.hpp"
#include "MP_constant.hpp"
#include "MP_utilities.hpp"

namespace flopc {

  class MP_constraint;
  class TerminalExpression; 
  class MP_variable;
  class VariableRef;

  class MP {
    friend class MP_expression;
    friend class MP_constraint;
    friend class MP_model;
    friend class Messenger;
    friend class VerboseMessenger; 
    friend class CoefLess;
    friend class MP_expression_base;
    friend class VariableRef;

    struct Coef {
      Coef(int c, int r, double v, int s = 0) : 
        col(c), row(r), stage(s), val(v)  {}
      int col, row, stage;
      double val;
    };
    
    struct CoefLess {
      bool operator() (const MP::Coef& a, const MP::Coef& b) const;
    };

  protected:
    class GenerateFunctor : public Functor {
    public:
      GenerateFunctor(MP_constraint* r, vector<Coef>& cfs): R(r), Coefs(cfs) {}
      
      void setMultiplicator(vector<Constant>& mults, double m) {
        multiplicators = mults;
        M = m;
      }
      void setTerminalExpression(const TerminalExpression* c) {
        C = c;
      }
      
      void operator()() const;
    private:
      vector<Constant> multiplicators;
      MP_constraint* R;
      double M; // sign (1=lhs, -1=rhs)
      const TerminalExpression* C;
      vector<MP::Coef>& Coefs;
    };
  };

  
  /** @brief The base class for all expressions.
      @ingroup INTERNAL_USE
      @note FOR INTERNAL USE: This is not normally used directly by the
      calling code.
  */
  class MP_expression_base {
    friend class MP_expression;
    friend class Handle<MP_expression_base*>;
  private:
    int count;
  public:
    MP_expression_base() : count(0) {}

    virtual double level() const = 0;
    virtual void generate(const MP_domain& domain,
                          vector<Constant> multiplicators,
                          MP::GenerateFunctor &f,
                          double m) const = 0;

    virtual void insertVariables(set<MP_variable*>& v) const = 0;

    virtual ~MP_expression_base() {}
  };


  /** @brief Symbolic representation of a linear expression.
      @ingroup PublicInterface
      This is one of the main public interface classes.  It is the basis for   
      all linear expressions, including constraints, objective function,
      and expressions involving indexes.
      <br> Although these can be created directly and independently, it
      is expected these will be created through the use of the operators
      which are later in this file.  (operator+, operator-, etc.)
      @note There are constructors which are (silently) used to convert \
      other componenets into expressions.
  */

  class MP_expression : public Handle<MP_expression_base*> {
  public:
    MP_expression() : Handle<MP_expression_base*>(0) {}
    /// Constructor which (silently) converts a Constant to a MP_expression
    MP_expression(const Constant& c);
    /// Constructor which (silently) converts a VariableRef to a MP_expression
    MP_expression(const VariableRef& v);
    MP_expression(MP_expression_base* r) : Handle<MP_expression_base*>(r) {}
  };


  /** @brief The base class for all expressions.
      @ingroup INTERNAL_USE
      @note FOR INTERNAL USE: This is not normally used directly by the
      calling code.
      @todo can this be moved to the cpp file?
  */
  class TerminalExpression : public MP_expression_base {
  public:
    virtual double getValue() const = 0; 
    virtual int getColumn() const = 0;
    virtual int getStage() const = 0;
  };

  /** Semantic representation of a variable in a Math Program
      @ingroup INTERNAL_USE
      @see MP_variable for a public interface.
  */
  class VariableRef : public TerminalExpression {
    friend class MP_variable;
  public:
    int getColumn() const;
  private:
    VariableRef(MP_variable* v, 
                const MP_index_exp& i1,
                const MP_index_exp& i2,
                const MP_index_exp& i3,
                const MP_index_exp& i4,
                const MP_index_exp& i5);

    double level() const;

    void insertVariables(set<MP_variable*>& v) const {
      v.insert(V);
    }
    double getValue() const { 
      return 1.0;
    }
    int getStage() const { 
      return 0;
    }
    void generate(const MP_domain& domain,
                  vector<Constant> multiplicators,
                  MP::GenerateFunctor& f,
                  double m) const;
    MP_variable* V;
    int offset;
    const MP_index_exp I1,I2,I3,I4,I5;
  };

  /// @ingroup PublicInterface
  MP_expression operator+(const MP_expression& e1, const MP_expression& e2);
  /// @ingroup PublicInterface
  MP_expression operator+(const MP_expression& e1, const Constant& e2);
  /// @ingroup PublicInterface
  MP_expression operator+(const Constant& e1, const MP_expression& e2);
  /// @ingroup PublicInterface
  MP_expression operator-(const MP_expression& e1, const MP_expression& e2);
  /// @ingroup PublicInterface
  MP_expression operator-(const MP_expression& e1, const Constant& e2);
  /// @ingroup PublicInterface
  MP_expression operator-(const Constant& e1, const MP_expression& e2);
  /// @ingroup PublicInterface
  MP_expression operator*(const Constant& e1, const MP_expression& e2); 
  /// @ingroup PublicInterface
  MP_expression operator*(const MP_expression& e1, const Constant& e2);
  /// @ingroup PublicInterface
  MP_expression operator/(const MP_expression& e1, const Constant& e2);
  /// @ingroup PublicInterface
  MP_expression sum(const MP_domain& d, const MP_expression& e);

} // End of namespace flopc
#endif
