// ******************** FlopCpp **********************************************
// File: MP_data.cpp
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

DataRef::DataRef(MP_data* d, 
                 const MP_index_exp& i1,
                 const MP_index_exp& i2,
                 const MP_index_exp& i3,
                 const MP_index_exp& i4,
                 const MP_index_exp& i5,
                 int s) : 
D(d),I1(i1),I2(i2),I3(i3),I4(i4),I5(i5),C(0),stochastic(s) {
    //if (!d->myrefs.empty()){ //Myrefs is not empty.. so copy Constant from last DataRef TODO: Think about that if we know why we have a vector here in the first place?!
    //    C = (*d->myrefs.back()).C;
    //}
}

const DataRef& DataRef::operator=(const Constant& c) {
    C = c;
    ((D->S1(I1)*D->S2(I2)*D->S3(I3)*D->S4(I4)*D->S5(I5)).such_that(B)).forall(this); //TODO: What happens if Dimensions are not right?
    return *this;
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

RandomVariable* DataRef::getRandomVariable() const {
    return C->getRandomVariable();
}



double DataRef::evaluate(int scenario) const {
    //Check for sanity of indices
    int i1 = D->S1.check(I1->evaluate());
    int i2 = D->S2.check(I2->evaluate());
    int i3 = D->S3.check(I3->evaluate());
    int i4 = D->S4.check(I4->evaluate());
    int i5 = D->S5.check(I5->evaluate());	
    // Compute index in flattened multidimensional array
    int i = D->f(i1,i2,i3,i4,i5);
    // Return value
    if ( i == outOfBound ) { //This is interesting behaviour. What happens if indexing expressions are so that multiplicative stuff needs to be done? Zero migth be the correct answer?
        return 0;
    } else { //What would happen if we would evaluate C?
         return D->v[i];
        //return C->evaluate(scenario);
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
    //Check sanity of indices
    int i1 = D->S1.check(I1->evaluate());
    int i2 = D->S2.check(I2->evaluate());
    int i3 = D->S3.check(I3->evaluate());
    int i4 = D->S4.check(I4->evaluate());
    int i5 = D->S5.check(I5->evaluate());
    //Compute index in flattened multidimensional array
    int i = D->f(i1,i2,i3,i4,i5);
    // Set Value at computed index to v
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

DataRef& flopc::DataRef::probability( double p )
{
    return *this;
}

DataRef& flopc::MP_data::operator()( const MP_index_exp& lcli1 /*= MP_index_exp::getEmpty()*/, const MP_index_exp& lcli2 /*= MP_index_exp::getEmpty()*/, const MP_index_exp& lcli3 /*= MP_index_exp::getEmpty()*/, const MP_index_exp& lcli4 /*= MP_index_exp::getEmpty()*/, const MP_index_exp& lcli5 /*= MP_index_exp::getEmpty() */ )
{
    // Why do we return more than one DataRefs? Why do we return only a new DataRef?
    //Answer: Depends on the calling context, depending on the index expressions. These can be different from one call to another and we need different DataRef to obey the indexation.
    myrefs.push_back(new DataRef(this, lcli1, lcli2, lcli3, lcli4, lcli5));
    return *myrefs.back();

}

    void DataRef::propagateIndexExpression(const MP_index_exp& i1,const MP_index_exp& i2,const MP_index_exp& i3,const MP_index_exp& i4,const MP_index_exp& i5) const{
        I1 = i1;
        I2 = i2;
        I3 = i3;
        I4 = i4;
        I5 = i5;
    }

 void DataRef::insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const {
    }  

