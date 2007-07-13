// ******************** FlopCpp **********************************************
// File: MP_data.cpp
// $Id$
// Author: Tim Helge Hultberg (thh@mat.ua.pt)
// Copyright (C) 2003 Tim Helge Hultberg
// All Rights Reserved.
//****************************************************************************

#include <iostream>
using std::cout;
using std::endl;

#include "MP_data.hpp"
#include "MP_domain.hpp" 
#include "MP_index.hpp" 
#include "MP_set.hpp" 
#include "MP_constant.hpp" 
#include "MP_expression.hpp" 

using namespace flopc;
double MP_data::outOfBoundData = 0;

const DataRef& DataRef::operator=(const Constant& c) {
  C = c;
  ((D->S1(I1)*D->S2(I2)*D->S3(I3)*D->S4(I4)*D->S5(I5)).such_that(B)).forall(this);
  return *this;;
}

const DataRef& DataRef::operator=(const DataRef& r) { 
  return operator=(Constant(const_cast<DataRef*>(&r)));
}

void DataRef::operator()() const {
  evaluate_lhs(C->evaluate());
}

DataRef& DataRef::such_that(const MP_boolean& b) {
  B = b;
  return *this;
}


double DataRef::evaluate() const {
  int i1 = D->S1.check(I1->evaluate());
  int i2 = D->S2.check(I2->evaluate());
  int i3 = D->S3.check(I3->evaluate());
  int i4 = D->S4.check(I4->evaluate());
  int i5 = D->S5.check(I5->evaluate());

  int i = D->f(i1,i2,i3,i4,i5);
  if ( i ==  outOfBound ) {
    return 0;
  } else {
    return D->v[i];
  }
}



int DataRef::getStage() const {
  int i1 = D->S1.checkStage(I1->evaluate());
  int i2 = D->S2.checkStage(I2->evaluate());
  int i3 = D->S3.checkStage(I3->evaluate());
  int i4 = D->S4.checkStage(I4->evaluate());
  int i5 = D->S5.checkStage(I5->evaluate());

  int stage = 0;
  if (i1>stage) stage = i1;
  if (i2>stage) stage = i2;
  if (i3>stage) stage = i3;
  if (i4>stage) stage = i4;
  if (i5>stage) stage = i5;

  // might need to add outofbound check here
  return stage+stochastic; 
}


void DataRef::evaluate_lhs(double v) const {
  int i1 = D->S1.check(I1->evaluate());
  int i2 = D->S2.check(I2->evaluate());
  int i3 = D->S3.check(I3->evaluate());
  int i4 = D->S4.check(I4->evaluate());
  int i5 = D->S5.check(I5->evaluate());

  int i = D->f(i1,i2,i3,i4,i5);
  if (i != outOfBound) {
    D->v[i] = v;
  }
}

void MP_data::operator()() const {
  if (&S1!=&MP_set::getEmpty()) cout << i1.evaluate() << " ";
  if (&S2!=&MP_set::getEmpty()) cout << i2.evaluate() << " ";
  if (&S3!=&MP_set::getEmpty()) cout << i3.evaluate() << " ";
  if (&S4!=&MP_set::getEmpty()) cout << i4.evaluate() << " ";
  if (&S5!=&MP_set::getEmpty()) cout << i5.evaluate() << " ";
  cout<<"  "<<v[f(i1.evaluate(),i2.evaluate(),i3.evaluate(),
                  i4.evaluate(),i5.evaluate())] << endl;
}

void MP_data::display(string s) {
  cout<<s<<endl;
  ((S1)(i1)*(S2)(i2)*(S3)(i3)*(S4)(i4)*(S5)(i5)).forall(this);
}
