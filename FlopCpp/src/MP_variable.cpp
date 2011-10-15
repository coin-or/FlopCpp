// ******************** FlopCpp **********************************************
// File: MP_variable.cpp
// $Id$
// Author: Tim Helge Hultberg (thh@mat.ua.pt)
// Copyright (C) 2003 Tim Helge Hultberg
// All Rights Reserved.
//****************************************************************************

#include <iostream>
#include <sstream>

#include <OsiSolverInterface.hpp>
#include "MP_variable.hpp"
#include "MP_domain.hpp" 
#include "MP_constant.hpp" 
#include "MP_model.hpp"

using namespace flopc;
using namespace std;

VariableRef::VariableRef(MP_variable* v, 
			 const MP_index_exp& i1,
			 const MP_index_exp& i2,
			 const MP_index_exp& i3,
			 const MP_index_exp& i4,
			 const MP_index_exp& i5) :
    V(v),I1(i1),I2(i2),I3(i3),I4(i4),I5(i5) { 
    offset = v->offset; 
}

double VariableRef::level() const {
    return  V->M->solution[V->offset +
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
			   GenerateFunctor& f,
			   double m)  const {
    f.setMultiplicator(multiplicators,m);
    f.setTerminalExpression(this);
    domain.Forall(&f);
}
 
MP_variable::MP_variable(const MP_set_base &s1, 
			 const MP_set_base &s2, 
			 const MP_set_base &s3,
			 const MP_set_base &s4, 
			 const MP_set_base &s5) :
    RowMajor(s1.size(),s2.size(),s3.size(),s4.size(),s5.size()),
    upperLimit(MP_data(s1,s2,s3,s4,s5)),
    lowerLimit(MP_data(s1,s2,s3,s4,s5)),
    S1(&s1),S2(&s2),S3(&s3),S4(&s4),S5(&s5),
    offset(-1)
{
    lowerLimit.initialize(0.0);
    upperLimit.initialize(MP_model::getDefaultModel().getInfinity());
    type = continuous;
}    

double MP_variable::level(int lcl_i1, int lcl_i2, int lcl_i3, int lcl_i4, int lcl_i5) {
    return M->solution[offset +  f(lcl_i1,lcl_i2,lcl_i3,lcl_i4,lcl_i5)];
}

void MP_variable::operator()() const {
    if (S1!=&MP_set::getEmpty()) cout << i1.evaluate() << " ";
    if (S2!=&MP_set::getEmpty()) cout << i2.evaluate() << " ";
    if (S3!=&MP_set::getEmpty()) cout << i3.evaluate() << " ";
    if (S4!=&MP_set::getEmpty()) cout << i4.evaluate() << " ";
    if (S5!=&MP_set::getEmpty()) cout << i5.evaluate() << " ";
    cout<<"  "<< M->solution[offset +
			     f(i1.evaluate(),
			       i2.evaluate(),
			       i3.evaluate(),
			       i4.evaluate(),
			       i5.evaluate())]<<endl;
}

void MP_variable::display(const std::string &s) {
  cout<<s<<endl;
  if (offset >= 0) {
    ((*S1)(i1)*(*S2)(i2)*(*S3)(i3)*(*S4)(i4)*(*S5)(i5)).Forall(this);
  } else {
    cout<<"No solution available!"<<endl;
  }
}
