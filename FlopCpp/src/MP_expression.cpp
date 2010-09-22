// ******************** FlopCpp **********************************************
// File: MP_expression.cpp
//****************************************************************************



#include <sstream>
#include <algorithm>

#include <OsiSolverInterface.hpp>

#include "MP_index.hpp"
#include "MP_expression.hpp"
#include "MP_constant.hpp"
#include "MP_boolean.hpp"
#include "MP_constraint.hpp"
#include "MP_set.hpp"
#include "MP_variable.hpp"
#include "MP_model.hpp"
#include "MP_random_constant.hpp"

using std::max;

namespace flopc {

    VariableRef::VariableRef(MP_variable* v, 
        const MP_index_exp& i1,
        const MP_index_exp& i2,
        const MP_index_exp& i3,
        const MP_index_exp& i4,
        const MP_index_exp& i5) :
    V(v),offset(0),I1(i1),I2(i2),I3(i3),I4(i4),I5(i5) { 
        assert(v != 0);
        offset = v->offset; 
    }

    VariableRef::~VariableRef() { }

    double VariableRef::level() const {
        return  V->M->Solver->getColSolution()[V->offset +
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
        vector<TerminalExpression* > multiplicators,
        MP::GenerateFunctor& f,
        double m)  const {
            //Set temporary values of Functor to previously computed ones and apply functor with these
            // values for every element in the domain
            //Set vector of constants of f to current values and set current TerminalExpression
            f.setMultiplicator(multiplicators,m);
            f.setTerminalExpression(this);
            //for current domain, apply functor
            domain.forall(&f);
    }

    int VariableRef::getStage() const
    {
        //We do not know which of these sets is the stage set, so we have to check all (but could abort after we found one)
        //TODO: We can shorten the check if isStage is checked in advance?!
        int i1 = V->S1->checkStage(I1->evaluate());
        int i2 = V->S2->checkStage(I2->evaluate());
        int i3 = V->S3->checkStage(I3->evaluate());
        int i4 = V->S4->checkStage(I4->evaluate());
        int i5 = V->S5->checkStage(I5->evaluate());

        int stage = 0; //Set stage to the correct value. Only one int should be larger than 0.
        if (i1>stage) stage = i1;
        else if (i2>stage) stage = i2;
        else if (i3>stage) stage = i3;
        else if (i4>stage) stage = i4;
        else if (i5>stage) stage = i5;
        return stage;
    }

    void VariableRef::insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const {
        //Look inside bounds for random data
        //TODO: We need to look at the bounds..
        //V->lowerLimit->insertRandomVariables(v);
        //V->upperLimit->insertRandomVariables(v);
    }

    double VariableRef::getValue( int scenario ) const
    {
        return 1.0;
    }


    class Expression_constant : public TerminalExpression, public MP {
        friend class MP_expression;
        friend class Expression_mult;
        friend class Expression_div;
    private:
        Expression_constant(const Constant& c) : C(c) {}
        double level() const { 
            return C->evaluate(); 
        }
        double getValue(int scenario) const {
            return C->evaluate(scenario);
        }
        virtual int getColumn() const { //All Values that get transformed to Expression_constant have no
            //associated variable. Therefore: RHS.
            return -1; //indicates right hand side column //Transform to enum
        }
        virtual int getStage() const {
            return C->getStage(); //NB to be changed
        }
        void generate(const MP_domain& domain,
            vector<TerminalExpression*> multiplicators,
            MP::GenerateFunctor& f,
            double m) const {
                f.setMultiplicator(multiplicators,m);
                f.setTerminalExpression(this);
                domain.forall(&f);
        }
        void insertVariables(set<MP_variable*>& v) const {}

        void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const {
         
        }
        Constant C;
    };

    class Expression_random_constant : public TerminalExpression, public MP {
        friend class MP_expression;
        friend class Expression_mult;
        friend class Expression_div;
        friend class MP;
    private:
        Expression_random_constant(const RandomConstant& c) : C(c) {}
        double level() const { 
            return C->evaluate(); 
        }
        double getValue(int scenario) const {
            return C->evaluate(scenario);
        }
        virtual int getColumn() const { 
            return -1; //RHS if coefficient is without variable
        }
        virtual int getStage() const {
            return C->getStage(); //NB to be changed
        }
        //TODO: Generate function, take a look at it
        void generate(const MP_domain& domain,
            vector<TerminalExpression*> multiplicators,
            MP::GenerateFunctor& f,
            double m) const {
                // If we generate coefficients we need correct indices, i.e. the ones stored in the RandomDataRef. Therefore empty indices should be propagated, as the function can then decide to either propagate the given indices or use the already stored indices.
                C->propagateIndexExpressions(MP_index_exp::getEmpty(),MP_index_exp::getEmpty(),MP_index_exp::getEmpty(),MP_index_exp::getEmpty(),MP_index_exp::getEmpty());
                f.setMultiplicator(multiplicators,m);
                f.setTerminalExpression(this);
                domain.forall(&f);
                //We somehow have to evaluate everything for every scenario.. or at least we should do so
        }
        void insertVariables(set<MP_variable*>& v) const {}

        void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const {
            C->insertRandomVariables(v);

        }
        RandomConstant C;
    };


    class Expression_operator : public MP_expression_base, public MP  {
    protected:
        Expression_operator(const MP_expression& e1, const MP_expression& e2) : 
             left(e1),right(e2) {}

             void insertVariables(set<MP_variable*>& v) const {
                 left->insertVariables(v);
                 right->insertVariables(v);
             }

            void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const {
                 left->insertRandomVariables(v);
                 right->insertRandomVariables(v);
             }

             MP_expression left,right;
    };

    class Expression_unary_operator : public MP_expression_base, public MP  {
    protected:
        Expression_unary_operator(const MP_expression& e1) : 
             left(e1) {}

             void insertVariables(set<MP_variable*>& v) const {
                 left->insertVariables(v);

             }

             void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const {
                 left->insertRandomVariables(v);

             }

             MP_expression left;
    };

    class Expression_plus : public Expression_operator {
        friend MP_expression operator+(const MP_expression& e1, const MP_expression& e2);
        friend MP_expression operator+(const MP_expression& e1, const Constant& e2);
        friend MP_expression operator+(const Constant& e1, const MP_expression& e2);
        friend MP_expression operator+(const MP_expression& e1, const RandomConstant& e2);
        friend MP_expression operator+(const RandomConstant& e1, const MP_expression& e2);
    private:
        Expression_plus(const MP_expression& e1, const MP_expression& e2) : 
           Expression_operator(e1,e2) {}
           double level() const { 
               return left->level()+right->level(); 
           }
           void generate(const MP_domain& domain,
               vector<TerminalExpression *> multiplicators,
               MP::GenerateFunctor& f,
               double m) const { 
                   left->generate(domain, multiplicators, f, m);
                   right->generate(domain, multiplicators, f, m);
           }
    };

    class Expression_minus : public Expression_operator {
        friend MP_expression operator-(const MP_expression& e1, const MP_expression& e2);
        friend MP_expression operator-(const MP_expression& e1, const Constant& e2); 
        friend MP_expression operator-(const Constant& e1, const MP_expression& e2);
        friend MP_expression operator-(const MP_expression& e1, const RandomConstant& e2); 
        friend MP_expression operator-(const RandomConstant& e1, const MP_expression& e2);
    private:
        Expression_minus(const MP_expression& e1, const MP_expression& e2) : 
           Expression_operator(e1,e2) {}
           double level() const { 
               return left->level()-right->level(); 
           }
           void generate(const MP_domain& domain,
               vector<TerminalExpression *> multiplicators,
               MP::GenerateFunctor& f,
               double m) const {
                   left->generate(domain, multiplicators, f, m);
                   right->generate(domain, multiplicators, f, -m);
           }
    };

    class Expression_unary_minus : public Expression_unary_operator {
        friend MP_expression operator-(const MP_expression& e1);
    private:
        Expression_unary_minus(const MP_expression& e1) : 
           Expression_unary_operator(e1) {}
           double level() const { 
               return -left->level();
           }
           void generate(const MP_domain& domain,
               vector<TerminalExpression *> multiplicators,
               MP::GenerateFunctor& f,
               double m) const {
                   left->generate(domain, multiplicators, f, -m);
           }
    };

    class Expression_mult : public MP_expression_base,  MP  {
        friend MP_expression operator*(const Constant& e1, const MP_expression& e2); 
        friend MP_expression operator*(const MP_expression& e1, const Constant& e2);
        friend MP_expression operator*(const RandomConstant& e1, const MP_expression& e2);
        friend MP_expression operator*(const MP_expression& e1, const RandomConstant& e2);


    private:
        Expression_mult(const RandomConstant& e1, const MP_expression& e2) : 
           left(e1), right(e2) {}
           double level() const { 
               return left->evaluate()*right->level(); 
           }
           void generate(const MP_domain& domain,
               vector<TerminalExpression *> multiplicators,
               MP::GenerateFunctor& f,
               double m) const {
                   multiplicators.push_back(new Expression_random_constant(left));
                   right->generate(domain, multiplicators, f, m);
                   delete multiplicators.back();
                   multiplicators.pop_back();
   
           }
           void insertVariables(set<MP_variable*>& v) const {
               right->insertVariables(v);
           }
           void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const {
               left->insertRandomVariables(v);
           }
          RandomConstant left;
          MP_expression right;
    };

    class Expression_div : public MP_expression_base, MP  {
        friend MP_expression operator/(const MP_expression& e1, const Constant& e2);
                friend MP_expression operator/(const MP_expression& e1, const RandomConstant& e2);
        //friend MP_expression operator/(const RandomDataRef& e1, const MP_expression& e2);
        //friend MP_expression operator/(const MP_expression& e1, const RandomDataRef& e2);
    private:
        Expression_div(const MP_expression& e, const Constant& c) : 
           left(e), right(c) {}
           Expression_div(const MP_expression& e, const RandomConstant& c) : 
           left(e), right(c) {}
           double level() const { 
               return left->level()/right->evaluate(); 
           }
           void generate(const MP_domain& domain,
               vector<TerminalExpression *> multiplicators,
               MP::GenerateFunctor& f,
               double m) const {
                   multiplicators.push_back(new Expression_random_constant(1/right));
                   left->generate(domain, multiplicators, f, m);
                   delete multiplicators.back();
                   multiplicators.pop_back();
           }
           void insertVariables(set<MP_variable*>& v) const {
               left->insertVariables(v);
           }
           void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const {
               right->insertRandomVariables(v);
           }
           MP_expression left;
           RandomConstant right;
    };

    class Expression_sum : public MP_expression_base, public MP  {
        friend MP_expression sum(const MP_domain& d, const MP_expression& e);
    private:
        Expression_sum(const MP_domain& d, const MP_expression& e) : D(d), exp(e) {}

        double level() const {
            SumFunctor SF(exp);
            D.forall(SF);
            return SF.the_sum;
        } 
        void generate(const MP_domain& domain,
            vector<TerminalExpression *> multiplicators,
            MP::GenerateFunctor& f,
            double m) const {
                // The order, D*domain (NOT domain*D), is important for efficiency! 
                exp->generate(D*domain, multiplicators, f, m); 
        }
        void insertVariables(set<MP_variable*>& v) const {
            exp->insertVariables(v);
        }
        void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const {
            exp->insertRandomVariables(v);
        }

        class SumFunctor : public Functor {
        public:
            SumFunctor(MP_expression exp) : E(exp), the_sum(0) {}
            void operator()() const {
                the_sum += E->level();
            }
            MP_expression E;      
            mutable double the_sum;
        };

        MP_domain D;
        MP_expression exp;
    };


    MP_expression operator+(const MP_expression& e1, const MP_expression& e2) {
        return new Expression_plus(e1, e2);
    }
    MP_expression operator+(const MP_expression& e1, const Constant& e2) {
        return new Expression_plus(e1, e2);
    }
    MP_expression operator+(const Constant& e1, const MP_expression& e2) {
        return new Expression_plus(e1, e2);
    }
    MP_expression operator+(const MP_expression& e1, const RandomConstant& e2) {
        return new Expression_plus(e1, e2);
    }
    MP_expression operator+(const RandomConstant& e1, const MP_expression& e2) {
        return new Expression_plus(e1, e2);
    }

    MP_expression operator-(const MP_expression& e1, const MP_expression& e2) {  
        return new Expression_minus(e1, e2);
    }
    MP_expression operator-(const MP_expression& e1, const Constant& e2) {
        return new Expression_minus(e1, e2);
    }
    MP_expression operator-(const Constant& e1, const MP_expression& e2) {
        return new Expression_minus(e1, e2);
    }
    MP_expression operator-(const MP_expression& e1, const RandomConstant& e2) {
        return new Expression_minus(e1, e2);
    }
    MP_expression operator-(const RandomConstant& e1, const MP_expression& e2) {
        return new Expression_minus(e1, e2);
    }
    MP_expression operator-(const MP_expression& e1){
        return new Expression_unary_minus(e1);
    }

    MP_expression operator*(const Constant& e1, const MP_expression& e2) {
        return new Expression_mult(e1, e2);
    }
    MP_expression operator*(const MP_expression& e1, const Constant& e2) {
        return new Expression_mult(e2, e1);
    }

    MP_expression operator*( const MP_expression& e1, const RandomConstant& e2 )
    {
        return new Expression_mult(e2,e1);
    }

    MP_expression operator*( const RandomConstant& e1, const MP_expression& e2 )
    {
        return new Expression_mult(e1,e2);
    }
    MP_expression operator/(const MP_expression& e1, const Constant& e2) {
        return new Expression_div(e1, e2);
    }

    //flopc::MP_expression operator/( const MP_expression& e1, const RandomDataRef& e2 )
    //{
    //    return new Expression_div(e1,e2);
    //}

    //flopc::MP_expression operator/( const RandomDataRef& e1, const MP_expression& e2 )
    //{
    //    return new Expression_div(e1,e2);
    //}
    MP_expression sum(const MP_domain& d, const MP_expression& e) {
        return new Expression_sum(d, e);  
    }



} // End of namespace flopc

using namespace flopc;

//Allow transformation of Constant, VariableRef and RandomDataRef to TerminalExpression..
MP_expression::MP_expression(const Constant &c) :
Handle<MP_expression_base*>(new Expression_constant(c)) {} 

MP_expression::MP_expression(const VariableRef &v) : 
Handle<MP_expression_base*>(const_cast<VariableRef*>(&v)) { } // We delete the VariableRef from the corresponding MP_variable

MP_expression::MP_expression(const RandomConstant &r) :
Handle<MP_expression_base*>(new Expression_random_constant(r)) {}

MP_expression::MP_expression( MP_expression_base* r ) : 
Handle<MP_expression_base*>(r) {}



//bool MP::CoefLess::operator() (const MP::Coef& a, const MP::Coef& b) const {
bool MP::CoefLess (const MP::Coef& a, const MP::Coef& b) {
    if (a.col < b.col) {
        return true;
    } else if (a.col == b.col && a.row < b.row) {
        return true;
    } else {
        return false;
    }
}

bool MP::CoefLessShared (const boost::shared_ptr<MP::Coef>& a, const boost::shared_ptr<MP::Coef>& b) {
    if (a->col < b->col) {
        return true;
    } else if (a->col == b->col && a->row < b->row) {
        return true;
    } else {
        return false;
    }
}


bool MP::CoefLessWithStageShared( const boost::shared_ptr<MP::Coef>& a, const boost::shared_ptr<MP::Coef>& b ) {
    if (a->varStage < b->varStage){
        return true;
    }
    else if (a->varStage == b->varStage && a->col < b->col){
        return MP::CoefLessShared(a,b);
    }
    else {
        return false;
    }
}

void MP::VariableBoundsFunctor::operator()() const {
 //Generate Coefficient for random bounds only.. we have to take a look at the variables bounds
    //TODO: Evaluate lower limit and upper limit for every scenario, IF they contain a RandomVariable (that is a stage dependent random variable..)
    //if (var->V->lowerLimit)
    //std::vector<double> scenarioValues;
    //if (var->V->lowerLimit->getStage()){ //
    //    scenarioValues.clear();
    //    for (int i = 0; i < MP_model::getCurrentModel()->getScenSet().size(); i++) {
    //        scenarioValues.push_back(var->V->lowerLimit.evaluate(i));
    //    }
    //    boost::shared_ptr<MP::Coef> temp(new MP::Coef(var->getColumn(),lowerBound,var->V->lowerLimit.evaluate(outOfBound),var->getStage(),var->V->lowerLimit.getStage(),scenarioValues));
    //    Coefs[temp->randomStage].push_back(temp);
    //}
    //if (var->V->upperLimit->getStage()){ //
    //    scenarioValues.clear();
    //    for (int i = 0; i < MP_model::getCurrentModel()->getScenSet().size(); i++) {
    //        scenarioValues.push_back(var->V->upperLimit.evaluate(i));
    //    }
    //    boost::shared_ptr<MP::Coef> temp(new MP::Coef(var->getColumn(),upperBound,var->V->upperLimit.evaluate(outOfBound),var->getStage(),var->V->upperLimit.getStage(),scenarioValues));
    //    Coefs[temp->randomStage].push_back(temp);
    //}
}

//This Function adds a coefficient to the Coefs Array
void MP::GenerateFunctor::operator()() const {
    assert(M==-1 || M==1);
    double sideOfConstraint = M; //LHS (=1) or RHS (=-1)?
    int stage = 0;
    std::vector<double> scenarioValues;
    RandomVariable* ptrRV = 0;
    for (unsigned int i=0; i<multiplicators.size(); i++) {
        if (multiplicators[i]->getStage() > stage) {
            // If we have a stage, we have a Expression_random_constant
            // At this moment we have to ensure that correct indexation is used in the subexpressions
            Expression_random_constant* ePtr = dynamic_cast<Expression_random_constant*>(multiplicators[i]);
            if (ePtr == 0)
                throw invalid_argument_exception();
            // Propagate correct indices, i.e. use the ones defined during assignment phase for MP_random_data. Therefore we have to call the function with empty indices to signal this.
            ePtr->C->propagateIndexExpressions(MP_index_exp::getEmpty(),MP_index_exp::getEmpty(),MP_index_exp::getEmpty(),MP_index_exp::getEmpty(),MP_index_exp::getEmpty());
            stage = max(stage,multiplicators[i]->getStage());
            // Copy values for every scenario to coefficient
            for (int j = 0; j < MP_model::getCurrentModel()->getScenSet().size();j++){
                double temp = sideOfConstraint*(multiplicators[i]->getValue(j));
                scenarioValues.push_back(temp);
            }
            //Compute mean via computation of scenario values
            double meanValue = 0;
            std::vector<double> probs = MP_model::getCurrentModel()->getProbabilities();
            for (int j = 0; j < scenarioValues.size(); j++)
                meanValue += scenarioValues[j]*probs[j];
            // Store mean value
            sideOfConstraint *= meanValue;
        }
        else {
            sideOfConstraint *= multiplicators[i]->getValue();
        }
    }
    // If rowNumber is -1 after call to row_number we have an objective function coefficient
    int rowNumber = -1;
    if (R != 0) { //If we are at a normal constraint get correct rowNumber
        rowNumber = R->row_number();
    }
    if (rowNumber != outOfBound) { //C 
        assert(C != 0);
        int colNumber = C->getColumn(); //Get Column number. -1 for RHS
        if ( colNumber != outOfBound  ) {
            double val = sideOfConstraint*C->getValue(outOfBound); //Get Value of Coefficient. In case of RV get mean value.
            // Set variable stage.
            int varStage = C->getStage(); //equal to 0 for constant value
            //Now comes the fun part: If varStage > stage this means one of two things:
            //1: VariableRef with fixed Coefficient
            //2: We added a RandomVariable without a variable
            //as a terminalExpression: => No Multiplicative constant whatsoever..
            // What is not allowed? varStage > stage && stage > 0 . 
            if (stage == 0 && varStage > 0) {//Assume the second case: This can be specified to varStage > 1 and stage == 0
                stage = varStage;
                //We need to check if we have a lonesome random variable here. This means a dynamic_cast to MP_expression instead of VariableRef..
                const Expression_random_constant* ePtr = dynamic_cast<const Expression_random_constant*> (C);
                if (ePtr != 0){
                    for (int i = 0; i < MP_model::getCurrentModel()->getScenSet().size(); i++ ){
                        scenarioValues.push_back(sideOfConstraint*C->getValue(i));
                    }

                    // If we sample we may want to replace the mean value, otherwise we probably don't
                    if (MP_model::getCurrentModel()->getSample() ){
                        double meanValue = 0;
                        std::vector<double> probs = MP_model::getCurrentModel()->getProbabilities();
                        for (int j = 0; j < scenarioValues.size(); j++)
                            meanValue += scenarioValues[j]*probs[j];
                        val = meanValue;
                    }

                    // We have to replace the mean value?
                }
            }
            Coefs.push_back(MP::Coef(colNumber, rowNumber, val, varStage,stage, scenarioValues));
           

        }//end if colNumber != outOfBound
    }//end if rowNumber != out of bound
}

