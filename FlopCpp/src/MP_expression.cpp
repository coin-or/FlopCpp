// ******************** FlopCpp **********************************************
// File: MP_expression.cpp
//****************************************************************************

#include <sstream>
#include "MP_expression.hpp"
#include "MP_constant.hpp"
#include "MP_boolean.hpp"
#include "MP_constraint.hpp"
#include "MP_set.hpp"
#include "MP_variable.hpp"
#include "MP_model.hpp"
#include <OsiSolverInterface.hpp>

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
        vector<Constant > multiplicators,
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
        V->lowerLimit->insertRandomVariables(v);
        V->upperLimit->insertRandomVariables(v);
    }



    class Expression_constant : public TerminalExpression, public MP {
        friend class MP_expression;
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
            vector<Constant> multiplicators,
            MP::GenerateFunctor& f,
            double m) const {
                f.setMultiplicator(multiplicators,m);
                f.setTerminalExpression(this);
                domain.forall(&f);
        }
        void insertVariables(set<MP_variable*>& v) const {}

        void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const {
         C->insertRandomVariables(v);
        }

        virtual RandomVariable* getRandomVariable() const {
            return C->getRandomVariable();
        }

        Constant C;
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

    class Expression_plus : public Expression_operator {
        friend MP_expression operator+(const MP_expression& e1, const MP_expression& e2);
        friend MP_expression operator+(const MP_expression& e1, const Constant& e2);
        friend MP_expression operator+(const Constant& e1, const MP_expression& e2);	
    private:
        Expression_plus(const MP_expression& e1, const MP_expression& e2) : 
           Expression_operator(e1,e2) {}
           double level() const { 
               return left->level()+right->level(); 
           }
           void generate(const MP_domain& domain,
               vector<Constant> multiplicators,
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
    private:
        Expression_minus(const MP_expression& e1, const MP_expression& e2) : 
           Expression_operator(e1,e2) {}
           double level() const { 
               return left->level()-right->level(); 
           }
           void generate(const MP_domain& domain,
               vector<Constant> multiplicators,
               MP::GenerateFunctor& f,
               double m) const {
                   left->generate(domain, multiplicators, f, m);
                   right->generate(domain, multiplicators, f, -m);
           }
    };

    class Expression_mult : public MP_expression_base,  MP  {
        friend MP_expression operator*(const Constant& e1, const MP_expression& e2); 
        friend MP_expression operator*(const MP_expression& e1, const Constant& e2);

    private:
        Expression_mult(const Constant& e1, const MP_expression& e2) : 
           left(e1), right(e2) {}
           double level() const { 
               return left->evaluate()*right->level(); 
           }
           void generate(const MP_domain& domain,
               vector<Constant> multiplicators,
               MP::GenerateFunctor& f,
               double m) const {
                   multiplicators.push_back(left);
                   right->generate(domain, multiplicators, f, m);
           }
           void insertVariables(set<MP_variable*>& v) const {
               right->insertVariables(v);
           }
           void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const {
               left->insertRandomVariables(v);
           }
           Constant left;
           MP_expression right;
    };

    class Expression_div : public MP_expression_base, MP  {
        friend MP_expression operator/(const MP_expression& e1, const Constant& e2);
    private:
        Expression_div(const MP_expression& e, const Constant& c) : 
           left(e), right(c) {}
           double level() const { 
               return left->level()/right->evaluate(); 
           }
           void generate(const MP_domain& domain,
               vector<Constant> multiplicators,
               MP::GenerateFunctor& f,
               double m) const {
                   multiplicators.push_back(1/right);
                   left->generate(domain, multiplicators, f, m);
           }
           void insertVariables(set<MP_variable*>& v) const {
               left->insertVariables(v);
           }
           void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const {
               right->insertRandomVariables(v);
           }
           MP_expression left;
           Constant right;
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
            vector<Constant> multiplicators,
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

    MP_expression operator-(const MP_expression& e1, const MP_expression& e2) {  
        return new Expression_minus(e1, e2);
    }
    MP_expression operator-(const MP_expression& e1, const Constant& e2) {
        return new Expression_minus(e1, e2);
    }
    MP_expression operator-(const Constant& e1, const MP_expression& e2) {
        return new Expression_minus(e1, e2);
    }

    MP_expression operator*(const Constant& e1, const MP_expression& e2) {
        return new Expression_mult(e1, e2);
    }
    MP_expression operator*(const MP_expression& e1, const Constant& e2) {
        return new Expression_mult(e2, e1);
    }

    MP_expression operator/(const MP_expression& e1, const Constant& e2) {
        return new Expression_div(e1, e2);
    }

    MP_expression sum(const MP_domain& d, const MP_expression& e) {
        return new Expression_sum(d, e);  
    }


} // End of namespace flopc

using namespace flopc;


MP_expression::MP_expression(const Constant &c) :
Handle<MP_expression_base*>(new Expression_constant(c)) {} 

MP_expression::MP_expression(const VariableRef &v) : 
Handle<MP_expression_base*>(const_cast<VariableRef*>(&v)) {} 


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
    std::vector<double> scenarioValues;
    if (var->V->lowerLimit->getStage()){ //
        scenarioValues.clear();
        for (int i = 0; i < MP_model::getCurrentModel()->ScenSet().size(); i++) {
            scenarioValues.push_back(var->V->lowerLimit->evaluate(i));
        }
        boost::shared_ptr<MP::Coef> temp(new MP::Coef(var->getColumn(),lowerBound,var->V->lowerLimit->evaluate(outOfBound),var->getStage(),var->V->lowerLimit->getStage(),scenarioValues));
        Coefs[temp->randomStage].push_back(temp);
    }
    if (var->V->upperLimit->getStage()){ //
        scenarioValues.clear();
        for (int i = 0; i < MP_model::getCurrentModel()->ScenSet().size(); i++) {
            scenarioValues.push_back(var->V->upperLimit->evaluate(i));
        }
        boost::shared_ptr<MP::Coef> temp(new MP::Coef(var->getColumn(),upperBound,var->V->upperLimit->evaluate(outOfBound),var->getStage(),var->V->upperLimit->getStage(),scenarioValues));
        Coefs[temp->randomStage].push_back(temp);
    }
}

//This Function adds a coefficient to the Coefs Array
void MP::GenerateFunctor::operator()() const {
    assert(M==-1 || M==1);
    double sideOfConstraint = M; //LHS (=1) or RHS (=-1)?
    int stage = 0;
    std::vector<double> scenarioValues;
    //TODO: Remove this, debugging only
    //if (multiplicators.size() > 1){
    //    cout << multiplicators.size() << endl;
    //    cout << "test multiplicator" <<endl;
    //}
    RandomVariable* ptrRV = 0;
    for (unsigned int i=0; i<multiplicators.size(); i++) {
        //When is more than 1 element in the vector??
        if (multiplicators[i]->getStage() > stage) { //TODO: There must be only one stage..
            // If we have a stage, we are in a stochastic setting. This does not necessarily mean that every Constant is random.
            stage = multiplicators[i]->getStage();
            //ptrRV = multiplicators[i]->getRandomVariable();
            for (int j = 0; j < MP_model::getCurrentModel()->ScenSet().size();j++){
                double temp = sideOfConstraint*(multiplicators[i]->evaluate(j));
                scenarioValues.push_back(temp);
            }
            //if (ptrRV == 0){ // current Constant does not contain a random Var, so we skip this 
            //    //continue; //Go to next loop
            //    throw not_implemented_error(); //TODO: Do this until problem is resolved. Problem: random_param should be able to interact with normal AML statements..
            //}
        }
        else {
            sideOfConstraint *= multiplicators[i]->evaluate();
        }
        
    }
    //If ptrRV!= 0, we have a RandomVariable and not a double coefficient
    //I still need to find out why multiplicators can be of size > 0? Maybe for rhs?

    // If objective, rowNumber is -1
    int rowNumber = -1;
    if (R != 0) { //If we are at a normal constraint get correct rowNumber
        rowNumber = R->row_number();
    }
    if (rowNumber != outOfBound) { //C 
        assert(C != 0);
        int colNumber = C->getColumn(); //Get Column number.
        if ( colNumber != outOfBound  ) {
            double val = sideOfConstraint*C->getValue(outOfBound); //Get Value of Coefficient. In case of RV get mean value.
            // Set variable stage.
            int varStage = C->getStage();
            //Now comes the fun part: If varStage > stage this means one of two things:
            //1: VariableRef with fixed Coefficient
            //2: We added a RandomVariable without a variable
            //as a terminalExpression: => No Multiplicative constant whatsoever..
            // What is not allowed? varStage > stage && stage > 0 . 
            if (stage == 0 && varStage > 0) {//Assume the second case: This can be specified to varStage > 1 and stage == 0
                stage = varStage;
                //We need to check if we have a lonesome random variable here. This means a dynamic_cast to MP_expression instead of VariableRef..
                const Expression_constant* ePtr = dynamic_cast<const Expression_constant*> (C);
                if (ePtr != 0)
                    for (int i = 0; i < MP_model::getCurrentModel()->ScenSet().size(); i++ ){
                        scenarioValues.push_back(sideOfConstraint*C->getValue(i));
                    }

            }
            Coefs.push_back(MP::Coef(colNumber, rowNumber, val, varStage,stage, scenarioValues));
           

        }//end if colNumber != outOfBound
    }//end if rowNumber != out of bound
}

