// ******************** flopc++ **********************************************
// File: MP_data.hpp
// $Id$
// Author: Tim Helge Hultberg (thh@mat.ua.pt)
// Copyright (C) 2003 Tim Helge Hultberg
// All Rights Reserved.
//****************************************************************************

#ifndef _MP_data_hpp_
#define _MP_data_hpp_

#include <vector>
#include "MP_index.hpp"    
#include "MP_set.hpp"      
#include "MP_constant.hpp" 
#include "MP_boolean.hpp" 

namespace flopc {

    class MP_data;

    class DataRef : public Constant_base, public Functor {
    public:
	DataRef(MP_data* d, 
		const MP_index_exp& i1,
		const MP_index_exp& i2,
		const MP_index_exp& i3,
		const MP_index_exp& i4,
		const MP_index_exp& i5) : 
	    D(d),I1(i1),I2(i2),I3(i3),I4(i4),I5(i5),C(0) {}

	~DataRef() {} 
	DataRef& such_that(const MP_boolean& b);
	double evaluate() const;
	const DataRef& operator=(const DataRef& r); 
	const DataRef& operator=(const Constant& c);
	double& evaluate_lhs() const;
	void operator()() const;
    private:
	MP_data* D;
	MP_index_exp I1,I2,I3,I4,I5;
	Constant C;
	MP_boolean B;
    };

    class MP_data : public RowMajor, public Functor {
	friend class MP_variable;
	friend class DisplayData;
	friend class DataRef;
	friend class MP_model;
    public:
	void operator()() const;
	void initialize(double d) {
	    for (int i=0; i<size(); i++) {
		v[i] = d;
	    }
	}
	MP_data(const MP_set_base &s1 = MP_set::Empty, 
		const MP_set_base &s2 = MP_set::Empty, 
		const MP_set_base &s3 = MP_set::Empty,
		const MP_set_base &s4 = MP_set::Empty, 
		const MP_set_base &s5 = MP_set::Empty) :
	    RowMajor(s1.size(),s2.size(),s3.size(),s4.size(),s5.size()),
	    S1(s1),S2(s2),S3(s3),S4(s4),S5(s5),
	    v(new double[size()]), manageData(true) 
	    {
		initialize(0); 
	    }

	MP_data(double* value,
		const MP_set_base &s1 = MP_set::Empty, 
		const MP_set_base &s2 = MP_set::Empty, 
		const MP_set_base &s3 = MP_set::Empty,
		const MP_set_base &s4 = MP_set::Empty, 
		const MP_set_base &s5 = MP_set::Empty) :
	    RowMajor(s1.size(),s2.size(),s3.size(),s4.size(),s5.size()),
	    S1(s1),S2(s2),S3(s3),S4(s4),S5(s5),
	    v(value), manageData(false) 
	    {
	    }

	~MP_data() {
	    if (manageData == true) delete[] v;
// 	    for (unsigned int i=0; i<myrefs.size(); i++) {
// 		cout<<"# "<<i<<"   "<<myrefs[i]<<endl;
// 		delete myrefs[i]; //Gives segmentation fault. I dont know why!
// 	    }
	}
    
	void value(const double* d) {
	    for (int i=0; i<size(); i++) {
		v[i] = d[i];
	    }
	}

	operator double() {
	    return operator()(0);
	}
    
	double& operator()(int i1, int i2=0, int i3=0, int i4=0, int i5=0) {
	    i1 = S1.check(i1);
	    i2 = S2.check(i2);
	    i3 = S3.check(i3);
	    i4 = S4.check(i4);
	    i5 = S5.check(i5);
	    int i = f(i1,i2,i3,i4,i5);
	    if (i == outOfBound) {
		outOfBoundData = 0;
		return outOfBoundData;
	    } else {
		return v[i];
	    }
	}
    
	DataRef& operator() (
	    const MP_index_exp& i1 = MP_index_exp::Empty,
	    const MP_index_exp& i2 = MP_index_exp::Empty,
	    const MP_index_exp& i3 = MP_index_exp::Empty,
	    const MP_index_exp& i4 = MP_index_exp::Empty,
	    const MP_index_exp& i5 = MP_index_exp::Empty
	    ) {
	    myrefs.push_back(new DataRef(this, i1, i2, i3, i4, i5));
	    return *myrefs.back();
	}
    
	void display(string s = "");
    private:
	MP_data(const MP_data&); // Forbid copy constructor
	MP_data& operator=(const MP_data&); // Forbid assignment

	static double outOfBoundData;

	vector<DataRef*> myrefs;
	MP_index i1,i2,i3,i4,i5;
	const MP_set_base &S1,&S2,&S3,&S4,&S5;
	double* v;
	bool manageData;
    };

} // End of namespace flopc
#endif
