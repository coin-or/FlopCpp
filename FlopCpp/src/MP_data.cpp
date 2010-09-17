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
#include "MP_model.hpp"

using namespace flopc;
double MP_data::outOfBoundData = 0;

DataRef::DataRef(MP_data* d, 
                 const MP_index_exp& i1,
                 const MP_index_exp& i2,
                 const MP_index_exp& i3,
                 const MP_index_exp& i4,
                 const MP_index_exp& i5,
                 int s) : 
D(d),I1(i1),I2(i2),I3(i3),I4(i4),I5(i5),origI1(MP_index_exp::getEmpty()),origI2(MP_index_exp::getEmpty()),origI3(MP_index_exp::getEmpty()),origI4(MP_index_exp::getEmpty()),origI5(MP_index_exp::getEmpty()),C(0),stochastic(s) {
}

const DataRef& DataRef::operator=(const Constant& c) {
    C = c;
    ((D->S1(I1)*D->S2(I2)*D->S3(I3)*D->S4(I4)*D->S5(I5)).such_that(B)).forall(this); //TODO: What happens if Dimensions are not right?
    return *this;
}

const DataRef& DataRef::operator=(const DataRef& r) { 
    if (this == &r)
        return *this;
    return operator=(Constant(r));
}

void DataRef::operator()() const {
    evaluate_lhs(C->evaluate()); 
}

DataRef& DataRef::such_that(const MP_boolean& b) {
    B = b;
    return *this;
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
    else {
        DLOG(ERROR) << "An index not suitable for the given set appears. Please recheck your model formulation.";
        throw invalid_argument_exception(); //This should not happen!
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
    if (MP_model::getCurrentModel()->checkSemantic()){
        LOG_IF(WARNING,S1.getIndex() != &MP_set::getEmpty() && lcli1->getIndex() != S1.getIndex() && lcli1->getDomain(&MP_set::getEmpty()) != MP_domain::getEmpty()) << "First index given to MP_data with name " << this->getName() << " does not correspond to the defined index. This may lead to subtle errors. Continue only if you know what you are doing.";
        LOG_IF(WARNING,S2.getIndex() != &MP_set::getEmpty() && lcli2->getIndex() != S2.getIndex() && lcli2->getDomain(&MP_set::getEmpty()) != MP_domain::getEmpty()) << "Second index given to MP_data with name " << this->getName() << " does not correspond to the defined index. This may lead to subtle errors. Continue only if you know what you are doing.";
        LOG_IF(WARNING,S3.getIndex() != &MP_set::getEmpty() && lcli3->getIndex() != S3.getIndex() && lcli3->getDomain(&MP_set::getEmpty()) != MP_domain::getEmpty()) << "Third index given to MP_data with name " << this->getName() << " does not correspond to the defined index. This may lead to subtle errors. Continue only if you know what you are doing.";
        LOG_IF(WARNING,S4.getIndex() != &MP_set::getEmpty() && lcli4->getIndex() != S4.getIndex() && lcli4->getDomain(&MP_set::getEmpty()) != MP_domain::getEmpty()) << "Fourth index given to MP_data with name " << this->getName() << " does not correspond to the defined index. This may lead to subtle errors. Continue only if you know what you are doing.";
        LOG_IF(WARNING,S5.getIndex() != &MP_set::getEmpty() && lcli5->getIndex() != S5.getIndex() && lcli5->getDomain(&MP_set::getEmpty()) != MP_domain::getEmpty()) << "Fifth index given to MP_data with name " << this->getName() << " does not correspond to the defined index. This may lead to subtle errors. Continue only if you know what you are doing.";
    }

    DataRef* dPtr = new DataRef(this, lcli1, lcli2, lcli3, lcli4, lcli5);
    myrefs.push_back(Constant(dPtr));
    return *dPtr;

}

void DataRef::propagateIndexExpressions(const MP_index_exp& i1,const MP_index_exp& i2,const MP_index_exp& i3,const MP_index_exp& i4,const MP_index_exp& i5) {
    if (&i1 != &MP_index_exp::getEmpty()){ //We have no empty indices, so we can update our values
        // Store old values
        const MP_index_exp& empty(MP_index_exp::getEmpty());
        if (origI1.operator!=(MP_index_exp::getEmpty()) ){ // We already have values stored, restore old values
            I1 = origI1;
            I2 = origI2;
            I3 = origI3;
            I4 = origI4;
            I5 = origI5;

        }
        else { // We have not yet stored values
            origI1 = I1.deepCopyIndexExpression();
            origI2 = I2.deepCopyIndexExpression();
            origI3 = I3.deepCopyIndexExpression();
            origI4 = I4.deepCopyIndexExpression();
            origI5 = I5.deepCopyIndexExpression();
        }
        I1 = I1->insertIndexExpr(i1);
        I2 = I2->insertIndexExpr(i2);
        I3 = I3->insertIndexExpr(i3);
        I4 = I4->insertIndexExpr(i4);
        I5 = I5->insertIndexExpr(i5);

    }
}



