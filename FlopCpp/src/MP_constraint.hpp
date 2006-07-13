// ******************** FlopCpp **********************************************
// File: MP_constraint.hpp
// $Id$
// Author: Tim Helge Hultberg (thh@mat.ua.pt)
// Copyright (C) 2003 Tim Helge Hultberg
// All Rights Reserved.
//****************************************************************************

#ifndef _MP_constraint_hpp_
#define _MP_constraint_hpp_

#include <set>
using std::set;

#include <map>
using std::map;

#include "MP_set.hpp"
#include "MP_domain.hpp"
#include "MP_utilities.hpp"
#include "MP_expression.hpp"
#include "MP_boolean.hpp"
#include "MP_data.hpp"

namespace flopc {

    class MP_constraint;
    class Constant;
    class MP_model;
    class MP_variable;

    enum  Sense_enum {LE,GE,EQ};

    class Constraint {
	friend class MP_constraint;
	friend class MP_model;
	friend Constraint operator<=(const MP_expression& l, const MP_expression& r);
	friend Constraint operator<=(const Constant& l, const MP_expression& r); 
	friend Constraint operator<=(const MP_expression& l, const Constant& r); 
	friend Constraint operator<=(const VariableRef& l, const VariableRef& r); 

	friend Constraint operator>=(const MP_expression& l, const MP_expression& r);
	friend Constraint operator>=(const Constant& l, const MP_expression& r); 
	friend Constraint operator>=(const MP_expression& l, const Constant& r); 
	friend Constraint operator>=(const VariableRef& l, const VariableRef& r);

	friend Constraint operator==(const MP_expression& l, const MP_expression& r);
	friend Constraint operator==(const Constant& l, const MP_expression& r); 
	friend Constraint operator==(const MP_expression& l, const Constant& r); 
	friend Constraint operator==(const VariableRef& l, const VariableRef& r); 
    private:
	Constraint(const MP_expression& l, const MP_expression& r, Sense_enum s) : 
	    left(l), right(r), sense(s) {}

	MP_expression left,right;
	Sense_enum sense;
    };

    inline Constraint operator<=(const MP_expression& l, const MP_expression& r) {
	return Constraint(l, r, LE);
    }
    inline Constraint operator<=(const Constant& l, const MP_expression& r) {
	return operator<=(MP_expression(l), r);
    }
    inline Constraint operator<=(const MP_expression& l, const Constant& r){
	return operator<=(l, MP_expression(r));
    }
    inline Constraint operator<=(const VariableRef& l, const VariableRef& r) {
	return *new Constraint(l, r, LE);
    }
    
    inline Constraint operator>=(const MP_expression& l, const MP_expression& r) {
	return *new Constraint(l, r, GE);
    }
    inline Constraint operator>=(const Constant& l, const MP_expression& r){
	return operator>=(MP_expression(l), r);
    }
    inline Constraint operator>=(const MP_expression& l, const Constant& r){
	return operator>=(l, MP_expression(r));
    }
    inline Constraint operator>=(const VariableRef& l, const VariableRef& r) {
	return *new Constraint(l, r, GE);
    }
    
    inline Constraint operator==(const MP_expression& l, const MP_expression& r) {
	return *new Constraint(l, r, EQ);
    }
    inline Constraint operator==(const Constant& l, const MP_expression& r){
	return operator==(MP_expression(l), r);
    }
    inline Constraint operator==(const MP_expression& l, const Constant& r) {
	return operator==(l, MP_expression(r));
    }
    inline Constraint operator==(const VariableRef& l, const VariableRef& r) {
	return *new Constraint(l, r, EQ);
    }


    class GenerateFunctor;

    class MP_constraint : public RowMajor, public Named {
    public: 
	MP_constraint(
	    const MP_set_base &s1 = MP_set::getEmpty(), 
	    const MP_set_base &s2 = MP_set::getEmpty(), 
	    const MP_set_base &s3 = MP_set::getEmpty(),
	    const MP_set_base &s4 = MP_set::getEmpty(), 
	    const MP_set_base &s5 = MP_set::getEmpty()
	    );

	MP_constraint& operator()(
	    const MP_index_exp& i1 = MP_index_exp::getEmpty(), 
	    const MP_index_exp& i2 = MP_index_exp::getEmpty(), 
	    const MP_index_exp& i3 = MP_index_exp::getEmpty(), 
	    const MP_index_exp& i4 = MP_index_exp::getEmpty(), 
	    const MP_index_exp& i5 = MP_index_exp::getEmpty()
	    )  {
	    I1 = i1; I2 = i2; I3 = i3; I4 = i4; I5 = i5;
	    return *this;
	}

	operator int() {
	    return offset + f(I1->evaluate(),I2->evaluate(),I3->evaluate(),
			      I4->evaluate(),I5->evaluate());
	}

	virtual ~MP_constraint() {}

	double price(int i1=0, int i2=0, int i3=0, int i4=0, int i5=0) const;

	void coefficients(GenerateFunctor& f);

	int row_number() const;

	MP_constraint& such_that(const MP_boolean& b) {
	    B = b; 
	    return *this; 
	}

	void insertVariables(set<MP_variable*>& v);

	void operator=(const Constraint& v); 
    
	void display(string s="") const;

	MP_model* M;
	int offset;
        // MP_data pprice;
	MP_expression left,right;
	Sense_enum sense;
    private:
	MP_boolean B;
	const MP_set_base &S1, &S2, &S3, &S4, &S5; 
	MP_index_exp I1, I2, I3, I4, I5;
    };

}  // End of namespace flopc
#endif
