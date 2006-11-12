// ******************** FlopCpp **********************************************
// File: MP_constraint.cpp
// $Id$
// Author: Tim Helge Hultberg (thh@mat.ua.pt)
// Copyright (C) 2003 Tim Helge Hultberg
// All Rights Reserved.
//****************************************************************************

#include <iostream>
#include <sstream>
using std::cout;
using std::endl;

#include <OsiSolverInterface.hpp>
#include "MP_constraint.hpp"
#include "MP_expression.hpp"
#include "MP_model.hpp"
#include "MP_constant.hpp"
#include "MP_data.hpp"

using namespace flopc;


void MP_constraint::operator=(const Constraint &v) {
    left = v.left;
    right = v.right;
    sense = v.sense;
}

std::ostream &flopc::operator<<(std::ostream &os, const Sense_enum &se)
{
    switch(se)
    {
    case LE: os<<"<=";
        break;
    case GE: os<<">=";
        break;
    case EQ: os<<"==";
        break;
    default:
        os<<"OPERATORUKNOWN"<<std::endl;
    };
    return os;
}


int MP_constraint::row_number() const {
    int i1 = S1.check(I1->evaluate());
    int i2 = S2.check(I2->evaluate());
    int i3 = S3.check(I3->evaluate());
    int i4 = S4.check(I4->evaluate());
    int i5 = S5.check(I5->evaluate());
    
    if (i1==outOfBound || i2==outOfBound || i3==outOfBound ||
	i4==outOfBound || i5==outOfBound) {
	return outOfBound;
    } else {
	return offset + f(I1->evaluate(),I2->evaluate(),I3->evaluate(),
			  I4->evaluate(),I5->evaluate()); 
    }
}

double MP_constraint::price(int i1, int i2, int i3, int i4, int i5) const {
    return  M->rowPrice[offset + f(i1,i2,i3,i4,i5)];
}
 
MP_constraint::MP_constraint(
    const MP_set_base &s1, 
    const MP_set_base &s2, 
    const MP_set_base &s3,
    const MP_set_base &s4, 
    const MP_set_base &s5) :
    RowMajor(s1.size(),s2.size(),s3.size(),s4.size(),s5.size()),
    M(MP_model::current_model),
    offset(-1),
    S1(s1),S2(s2),S3(s3),S4(s4),S5(s5),
    I1(0),I2(0),I3(0),I4(0),I5(0)
{
    MP_model::current_model->add(*this);
}

void MP_constraint::coefficients(GenerateFunctor& f) {
    f.setConstraint(this);
    
    vector<Constant> v;
    
    if (I1.operator->()!=0) {
	left->generate(S1(I1)*S2(I2)*S3(I3)*S4(I4)*S5(I5).such_that(B),v,f,1.0);
 	right->generate(S1(I1)*S2(I2)*S3(I3)*S4(I4)*S5(I5).such_that(B),v,f,-1.0);
    } else {
        cout<<"FlopCpp Warning: Constraint declared but not defined."<<endl;
    }
}

void MP_constraint::insertVariables(set<MP_variable*>& v) {
    if (left.operator->()!=0) {
	left->insertVariables(v);
    }
    if (right.operator->()!=0) {
	right->insertVariables(v);
    }
}

void MP_constraint::display(string s) const {
    cout<<s<<endl;
    if (offset >=0) {
      for (int i=offset; i<offset+size(); i++) {
	cout<<i<<"  "<<M->Solver->getRowLower()[i]<<"  "<<M->rowActivity[i]<<"  "<<M->Solver->getRowUpper()[i]<<"  "<<M->rowPrice[i]<<endl;
      }
    } else {
      cout<<"No solution available!"<<endl;
    }
}
    std::string MP_constraint::toString()const
    {
        std::stringstream ss;
        ss<<getName()<<':';
        ss<<left->toString();
        ss<<" "<<sense;
        ss<<" "<<right->toString();
        ss<<" for each ";
        if(&S1 != &MP_set::getEmpty())
        {
            ss<<S1.getName();
        }
        if(&S2 != &MP_set::getEmpty())
        {
            ss<<','<<S2.getName();
        }
        if(&S3 != &MP_set::getEmpty())
        {
            ss<<','<<S3.getName();
        }
        if(&S4 != &MP_set::getEmpty())
        {
            ss<<','<<S4.getName();
        }
        if(&S5 != &MP_set::getEmpty())
        {
            ss<<','<<S5.getName();
        }
        ss<<" in {";

        const MP_index_base *pExpIdx = MP_index_exp::getEmpty().operator->();
        if(I1.operator->()!=pExpIdx)
            ss<<I1->toString();
        if(I2.operator->()!=pExpIdx)
            ss<<','<<I2->toString();
        if(I3.operator ->()!=pExpIdx)
            ss<<','<<I3->toString();
        if(I4.operator->()!=pExpIdx)
            ss<<','<<I4->toString();
        if(I5.operator->()!=pExpIdx)
            ss<<','<<I5->toString();
        ss<<"}";
        /*
        if(offset>=0)
        {
            for(int i=offset;i<offset+size();i++)
            {
                ss<<i<<' '<<M->Solver->getRowLower()[i]<<' '<<M->rowActivity[i]<<' '<<M->Solver->getRowUpper()[i]<<" "<<M->rowPrice[i];
            }
        }
        else
        {
            ss<<"No solution available";
        }
        */
        return ss.str();
    }
