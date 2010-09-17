// ******************** FlopCpp **********************************************
// File: MP_variable.cpp
//****************************************************************************

#include <iostream>
#include <sstream>
using std::cout;
using std::endl;

#include <OsiSolverInterface.hpp>
#include "MP_variable.hpp"
#include "MP_domain.hpp" 
#include "MP_constant.hpp" 
#include "MP_model.hpp"
#include "SmiScnModel.hpp"
#include "MP_index.hpp"
using namespace flopc;


MP_variable::MP_variable(const MP_set_base &s1, 
                         const MP_set_base &s2, 
                         const MP_set_base &s3,
                         const MP_set_base &s4, 
                         const MP_set_base &s5) :
RowMajor(s1.size(),s2.size(),s3.size(),s4.size(),s5.size()),
upperLimit(s1,s2,s3,s4,s5),
lowerLimit(s1,s2,s3,s4,s5),
S1(&s1),S2(&s2),S3(&s3),S4(&s4),S5(&s5),
offset(-1)
{
    type = type::continuous;
    lowerLimit.initialize(0);
    upperLimit.initialize(MP_model::getCurrentModel()->getInfinity());
    setStageSet();
} 

MP_variable::~MP_variable() { }

double MP_variable::level(int lcl_i1, int lcl_i2, int lcl_i3, int lcl_i4, int lcl_i5) { 
     assert(M != 0);
    assert(M->Solver != 0);
    if (M->stage.size()) //TODO: This gives solution values for the first stage only..
        throw invalid_argument_exception();
    else
        return M->Solver->getColSolution()[offset +  f(lcl_i1,lcl_i2,lcl_i3,lcl_i4,lcl_i5)];

} 

double MP_variable::levelScenario(int scenario, int lcl_i1, int lcl_i2, int lcl_i3, int lcl_i4, int lcl_i5) { 
    assert(M != 0);
    assert(M->Solver != 0);
    if (M->colIndirection[offset+f(lcl_i1,lcl_i2,lcl_i3,lcl_i4,lcl_i5)] == outOfBound)
        throw invalid_argument_exception();
    if (M->stage.size()) //TODO: This gives solution values for the first stage only..
        return M->smiModel->getColSolution(scenario,getCurrentStage(lcl_i1,lcl_i2,lcl_i3,lcl_i4,lcl_i5),M->colIndirection[offset+f(lcl_i1,lcl_i2,lcl_i3,lcl_i4,lcl_i5)]);
    else
        throw invalid_argument_exception();

}  

void MP_variable::setStageSet(){
    if (S1->isStage())
        stageSet = 0;
    else 
        if (S2->isStage())
            stageSet = 1;
        else 
            if (S3->isStage())
                stageSet = 2;
            else 
                if (S4->isStage())
                    stageSet = 3;
                else 
                    if (S5->isStage())
                        stageSet = 4;
                    else
                        stageSet = outOfBound;
}

int MP_variable::getCurrentStage(int i1, int i2, int i3, int i4, int i5){
    switch (stageSet)
    {
    case 0: return i1;
    case 1: return i2;
    case 2: return i3;
    case 3: return i4;
    case 4: return i5;
    case outOfBound : return outOfBound;
    default : return outOfBound;
    }
}

void MP_variable::operator()() const {
    if (S1!=&MP_set::getEmpty()) cout << i1.evaluate() << " ";
    if (S2!=&MP_set::getEmpty()) cout << i2.evaluate() << " ";
    if (S3!=&MP_set::getEmpty()) cout << i3.evaluate() << " ";
    if (S4!=&MP_set::getEmpty()) cout << i4.evaluate() << " ";
    if (S5!=&MP_set::getEmpty()) cout << i5.evaluate() << " ";
    if (M->stage.size()){ //TODO: Terribly inefficient..
        int length = 0;
        double* values = 0;
        for (int i = 0; i < M->scenSet.size(); i++){
            values = M->smiModel->getColSolution(i,&length);
            //TODO: Check length //Check colIndirection for outOfBound..
            cout<< " Sc. " << i << ": " << values[M->colIndirection[offset +
            f(i1.evaluate(),
            i2.evaluate(),
            i3.evaluate(),
            i4.evaluate(),
            i5.evaluate())]];
            ::free(values); //we need to use free, as smi callocs the values..
        }
        cout << endl;
    }
    else
        cout<<"  "<< M->Solver->getColSolution()[offset +
        f(i1.evaluate(),
        i2.evaluate(),
        i3.evaluate(),
        i4.evaluate(),
        i5.evaluate())]<<endl;

}

void MP_variable::display(const std::string &s) {
    cout<<s<<endl;
    if (offset >= 0) {
        ((*S1)(i1)*(*S2)(i2)*(*S3)(i3)*(*S4)(i4)*(*S5)(i5)).forall(this);
    } else {
        cout<<"No solution available!"<<endl;
    }
}

void MP_variable::bounds(std::vector<std::vector<boost::shared_ptr<MP::Coef> > >& cfs) {

    //Store Constant index expressions of upper and lower bound and reset them afterwards?! Just for the moment..
    // Create new MP_index? 
    //MP_index in1,in2,in3,in4,in5;
    //const VariableRef* var = new VariableRef(this,in1,in2,in3,in4,in5);
    //delete var;
    const VariableRef& var = this->operator()(i1,i2,i3,i4,i5); //This is incorrect: We put MP_index in but we want a MP_index_exp. What happens to the indices of the limit Constants?!
    MP::VariableBoundsFunctor f(&var, cfs);
    ((*S1)(i1)*(*S2)(i2)*(*S3)(i3)*(*S4)(i4)*(*S5)(i5)).forall(&f);
    myrefs.pop_back();
}

void MP_variable::integer()
{
    type = type::discrete;
}

void MP_variable::binary()
{
    //upperLimit.initialize(1);
    lowerLimit.initialize(0);
    upperLimit.initialize(1);
    type = type::binary;
}

void MP_variable::free()
{
    lowerLimit.initialize(-MP_model::getCurrentModel()->getInfinity());
    upperLimit.initialize(MP_model::getCurrentModel()->getInfinity());
    if (type == type::binary)
        type = type::discrete;
}


const VariableRef& flopc::MP_variable::operator()( const MP_index_exp& d1 /*= MP_index_exp::getEmpty()*/, const MP_index_exp& d2 /*= MP_index_exp::getEmpty()*/, const MP_index_exp& d3 /*= MP_index_exp::getEmpty()*/, const MP_index_exp& d4 /*= MP_index_exp::getEmpty()*/, const MP_index_exp& d5 /*= MP_index_exp::getEmpty() */ )
{
    if (MP_model::getCurrentModel()->checkSemantic()){
        LOG_IF(WARNING,S1->getIndex() != &MP_set::getEmpty() && d1->getIndex() != S1->getIndex() && d1->getDomain(&MP_set::getEmpty()) != MP_domain::getEmpty()  ) << "First index given to MP_variable with name " << this->getName() << " does not correspond to the defined index. This may lead to subtle errors. Continue only if you know what you are doing.";
        LOG_IF(WARNING,S2->getIndex() != &MP_set::getEmpty() && d2->getIndex() != S2->getIndex() && d2->getDomain(&MP_set::getEmpty()) != MP_domain::getEmpty()) << "Second index given to MP_variable with name " << this->getName() << " does not correspond to the defined index. This may lead to subtle errors. Continue only if you know what you are doing.";
        LOG_IF(WARNING,S3->getIndex() != &MP_set::getEmpty() && d3->getIndex() != S3->getIndex() && d3->getDomain(&MP_set::getEmpty()) != MP_domain::getEmpty()) << "Third index given to MP_variable with name " << this->getName() << " does not correspond to the defined index. This may lead to subtle errors. Continue only if you know what you are doing.";
        LOG_IF(WARNING,S4->getIndex() != &MP_set::getEmpty() && d4->getIndex() != S4->getIndex() && d4->getDomain(&MP_set::getEmpty()) != MP_domain::getEmpty()) << "Fourth index given to MP_variable with name " << this->getName() << " does not correspond to the defined index. This may lead to subtle errors. Continue only if you know what you are doing.";
        LOG_IF(WARNING,S5->getIndex() != &MP_set::getEmpty() && d5->getIndex() != S5->getIndex() && d5->getDomain(&MP_set::getEmpty()) != MP_domain::getEmpty()) << "Fifth index given to MP_variable with name " << this->getName() << " does not correspond to the defined index. This may lead to subtle errors. Continue only if you know what you are doing.";
    }
    //TODO: Look at lower/upperLimits again
    //lowerLimit->propagateIndexExpressions(d1,d2,d3,d4,d5);
    //upperLimit->propagateIndexExpressions(d1,d2,d3,d4,d5);
    VariableRef* vPtr = new VariableRef(this, d1, d2, d3, d4, d5);
    myrefs.push_back(MP_expression(vPtr));
    return *vPtr;
}