// ******************** flopc++ **********************************************
// File: MP_variable.hpp
// $Id$
// Author: Tim Helge Hultberg (thh@mat.ua.pt)
// Copyright (C) 2003 Tim Helge Hultberg
// All Rights Reserved.
//****************************************************************************

#ifndef _MP_variable_hpp_
#define _MP_variable_hpp_

#include "MP_set.hpp"
#include "MP_index.hpp"
#include "MP_expression.hpp"
#include "MP_domain.hpp"
#include "MP_data.hpp"

namespace flopc {

    enum variableType {continuous, discrete};

    class MP_model;
    class MP_variable;

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
	void generate(const MP_domain& domain,
		      vector<Constant> multiplicators,
		      GenerateFunctor& f,
		      double m) const;
	MP_variable* V;
	int offset;
	const MP_index_exp I1,I2,I3,I4,I5;
    };


    class MP_variable : public RowMajor, public Functor {
	friend class MP_model;
	friend class DisplayVariable;
	friend class VariableRef;
    public:
	MP_variable(const MP_set_base &s1 = MP_set::Empty, 
		    const MP_set_base &s2 = MP_set::Empty, 
		    const MP_set_base &s3 = MP_set::Empty,
		    const MP_set_base &s4 = MP_set::Empty, 
		    const MP_set_base &s5 = MP_set::Empty);

	~MP_variable() {
	}

	double level(int i1=0, int i2=0, int i3=0, int i4=0, int i5=0);

	const VariableRef& operator()(
	    const MP_index_exp& d1 = MP_index_exp::Empty, 
	    const MP_index_exp& d2 = MP_index_exp::Empty, 
	    const MP_index_exp& d3 = MP_index_exp::Empty,
	    const MP_index_exp& d4 = MP_index_exp::Empty, 
	    const MP_index_exp& d5 = MP_index_exp::Empty
	    ) {
	    return *new VariableRef(this, d1, d2, d3, d4, d5);
	}
    
	void display(string s = "");  

	void binary() { 
	    upperLimit.initialize(1);
	    type = discrete; 
	}

	void integer() { 
	    type = discrete; 
	}
 
	MP_data upperLimit;
	MP_data lowerLimit;
    private:
	void operator()() const;
	const MP_set_base *S1, *S2, *S3, *S4, *S5;
	MP_index i1,i2,i3,i4,i5;

	MP_model *M;
	variableType type;
	int offset;
    };

    class MP_binary_variable : public MP_variable {
    public:
	MP_binary_variable(const MP_set_base &s1 = MP_set::Empty, 
			   const MP_set_base &s2 = MP_set::Empty, 
			   const MP_set_base &s3 = MP_set::Empty,
			   const MP_set_base &s4 = MP_set::Empty, 
			   const MP_set_base &s5 = MP_set::Empty) :
	    MP_variable(s1,s2,s3,s4,s5) {
	    binary();
	}
    };

} // End of namespace flopc
#endif 
