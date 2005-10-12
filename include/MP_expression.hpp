// ******************** flopc++ **********************************************
// File: MP_expression.hpp
// $Id$
// Author: Tim Helge Hultberg (thh@mat.ua.pt)
// Copyright (C) 2003 Tim Helge Hultberg
// All Rights Reserved.
//****************************************************************************

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

    class Boolean;
    class MP_domain;
    class MP_constraint;
    class Row;
    class MP_variable;
    class VariableRef;
    class CoefLess;

    struct Coef {
	Coef(int c, int r, double v) : col(c), row(r), val(v) {}
	int col, row;
	double val;
    };

    class TerminalExpression;

    class GenerateFunctor : public Functor {
    public:
	GenerateFunctor(vector<Coef>& coefs) : Coefs(coefs) {}

	virtual ~GenerateFunctor(){}

	void setConstraint(MP_constraint* r) {
	    R = r;
	}
	void setMultiplicator(vector<Constant>& mults, double m) {
	    multiplicators = mults;
	    m_ = m;
	}
	void setTerminalExpression(const TerminalExpression* c) {
	    C = c;
	}
	virtual int row_number() const;

	void operator()() const;

	double m_;
	vector<Constant> multiplicators;
	MP_constraint* R;
	const TerminalExpression* C;
	vector<Coef>& Coefs;
    };

    class ObjectiveGenerateFunctor : public GenerateFunctor {
    public:
	ObjectiveGenerateFunctor(vector<Coef>& coefs) : GenerateFunctor(coefs) {}
	virtual int row_number() const {
	    return -1;
	}
    };

    class MP_expression_base {
	friend class MP_expression;
	friend class Handle<MP_expression_base*>;
    private:
	int count;
    public:
	MP_expression_base() : count(1) {}

	virtual double level() const = 0;
	virtual void generate(const MP_domain& domain,
			      vector<Constant> multiplicators,
			      GenerateFunctor& f,
			      double m) const = 0;
	virtual void insertVariables(set<MP_variable*>& v) const = 0;


	virtual ~MP_expression_base() {}
    };

    class MP_expression : public Handle<MP_expression_base*> {
	friend class MP_constraint;
    public:
	MP_expression() : Handle<MP_expression_base*>(0) {}
	MP_expression(MP_expression_base* r) : Handle<MP_expression_base*>(r) {}
	MP_expression(const Constant& c);
	MP_expression(const VariableRef& v);
    };

    class TerminalExpression : public MP_expression_base {
    public:
	virtual double getValue() const = 0; 
	virtual int getColumn() const = 0;
    };

    class Expression_operator : public MP_expression_base {
    public:
	Expression_operator(const MP_expression& e1, const MP_expression& e2) : 
	    left(e1),right(e2) {}
	void insertVariables(set<MP_variable*>& v) const {
	    left->insertVariables(v);
	    right->insertVariables(v);
	}
    protected:
	MP_expression left,right;
    };

    MP_expression operator+(const MP_expression& e1, const MP_expression& e2);
    MP_expression operator+(const MP_expression& e1, const Constant& e2);
    MP_expression operator+(const Constant& e1, const MP_expression& e2);
    MP_expression operator-(const MP_expression& e1, const MP_expression& e2);
    MP_expression operator-(const MP_expression& e1, const Constant& e2);
    MP_expression operator-(const Constant& e1, const MP_expression& e2);
    MP_expression operator*(const Constant& e1, const MP_expression& e2); 
    MP_expression operator*(const MP_expression& e1, const Constant& e2);
    MP_expression sum(const MP_domain& d, const MP_expression& e);

} // End of namespace flopc
#endif
