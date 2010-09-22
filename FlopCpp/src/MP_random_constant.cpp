// ******************** FlopCpp **********************************************
// File: MP_RandomConstant.cpp
//****************************************************************************

#include <float.h>
#include <cmath>
#include <sstream>
#include "MP_random_constant.hpp"
#include "MP_data.hpp"
#include "MP_domain.hpp"
#include "MP_index.hpp"
#include "MP_utilities.hpp"

namespace flopc {

    class RandomConstant_constant : public RandomConstant_base {
        //friend RandomConstant abs(const RandomConstant& c);
    public:
        RandomConstant_constant(const Constant& c) : C(c) {}

    private:

        virtual double evaluate(int scenario) const {
            return C->evaluate();
        }
        virtual int getStage() const {
            return 0; //Constants alway have stage 0
        }
        virtual void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const {

        }
        virtual void propagateIndexExpressions( const MP_index_exp& i1, const MP_index_exp& i2,const MP_index_exp& i3, const MP_index_exp& i4,const MP_index_exp& i5){
           C->propagateIndexExpressions(i1,i2,i3,i4,i5);
        }
        Constant C;
    };


    class RandomConstant_abs : public RandomConstant_base {
        friend RandomConstant abs(const RandomConstant& c);
    private:
        RandomConstant_abs(const RandomConstant& c) : C(c) {}
        virtual double evaluate(int scenario) const {
            return fabs(C->evaluate(scenario));
        }
        virtual int getStage() const {
            return C->getStage();
        }
        virtual void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const {
            C->insertRandomVariables(v);
        }
        virtual void propagateIndexExpressions( const MP_index_exp& i1, const MP_index_exp& i2,const MP_index_exp& i3, const MP_index_exp& i4,const MP_index_exp& i5){
            C->propagateIndexExpressions(i1,i2,i3,i4,i5);
        }
        RandomConstant C;
    };
    RandomConstant abs(const RandomConstant& c) {
        return new RandomConstant_abs(c);
    }

    class RandomConstant_pos : public RandomConstant_base {
        friend RandomConstant pos(const RandomConstant& c);
    private:
        RandomConstant_pos(const RandomConstant& c) : C(c) {}
        virtual double evaluate(int scenario) const {
            double temp = C->evaluate(scenario);
            if (temp>0) {
                return temp;
            } else {
                return 0.0;
            }
        }  
        int getStage() const {
            return C->getStage();
        }
        virtual void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const {
            C->insertRandomVariables(v);
        }
        virtual void propagateIndexExpressions( const MP_index_exp& i1, const MP_index_exp& i2,const MP_index_exp& i3, const MP_index_exp& i4,const MP_index_exp& i5){
            C->propagateIndexExpressions(i1,i2,i3,i4,i5);
        }
        RandomConstant C;
    };
    RandomConstant pos(const RandomConstant& c) {
        return new RandomConstant_pos(c);
    }

    class RandomConstant_ceil : public RandomConstant_base {
        friend RandomConstant ceil(const RandomConstant& c);
    private:
        RandomConstant_ceil(const RandomConstant& c) : C(c) {}
        virtual double evaluate(int scenario) const {
            return std::ceil(C->evaluate(scenario));
        }
        virtual int getStage() const {
            return C->getStage();
        }
        virtual void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const {
            C->insertRandomVariables(v);
        }  
        virtual void propagateIndexExpressions( const MP_index_exp& i1, const MP_index_exp& i2,const MP_index_exp& i3, const MP_index_exp& i4,const MP_index_exp& i5){
            C->propagateIndexExpressions(i1,i2,i3,i4,i5);
        }
        RandomConstant C;
    };
    RandomConstant ceil(const RandomConstant& c) {
        return new RandomConstant_ceil(c);
    }

    class RandomConstant_floor : public RandomConstant_base {
        friend RandomConstant floor(const RandomConstant& c);
    private:
        RandomConstant_floor(const RandomConstant& c) : C(c) {}
        virtual double evaluate(int scenario) const {
            return std::floor(C->evaluate(scenario));
        }
        virtual int getStage() const {
            return C->getStage();
        }

        virtual void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const {
            C->insertRandomVariables(v);
        }  
        virtual void propagateIndexExpressions( const MP_index_exp& i1, const MP_index_exp& i2,const MP_index_exp& i3, const MP_index_exp& i4,const MP_index_exp& i5){
            C->propagateIndexExpressions(i1,i2,i3,i4,i5);
        }
        RandomConstant C;
    };
    RandomConstant floor(const RandomConstant& c) {
        return new RandomConstant_floor(c);
    }

    class RandomConstant_exp : public RandomConstant_base {
    protected:
        RandomConstant_exp(const RandomConstant& i, const RandomConstant& j) : left(i),right(j) {}
        int getStage() const {
            return std::max(left->getStage(),right->getStage());
        }
        virtual void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const {
            left->insertRandomVariables(v);
            right->insertRandomVariables(v);
        }
        virtual void propagateIndexExpressions( const MP_index_exp& i1, const MP_index_exp& i2,const MP_index_exp& i3, const MP_index_exp& i4,const MP_index_exp& i5){
            left->propagateIndexExpressions(i1,i2,i3,i4,i5);
            right->propagateIndexExpressions(i1,i2,i3,i4,i5);
        }
        RandomConstant left, right;
    };


    class RandomConstant_min_2 : public RandomConstant_exp {
        friend RandomConstant minimum(const RandomConstant& a, const RandomConstant& b);
        friend RandomConstant minimum(const Constant& a, const RandomConstant& b);
        friend RandomConstant minimum(const RandomConstant& a, const Constant& b);
    private:
        RandomConstant_min_2(const RandomConstant& i, const RandomConstant& j) : RandomConstant_exp(i,j) {}
        RandomConstant_min_2(const Constant& i, const RandomConstant& j) : RandomConstant_exp(i,j) {}
        RandomConstant_min_2(const RandomConstant& i, const Constant& j) : RandomConstant_exp(i,j) {}
        virtual double evaluate(int scenario) const {
            return std::min(left->evaluate(scenario),right->evaluate(scenario));
        }
        virtual int getStage() const {
            return std::max(left->getStage(),right->getStage());
        }
    };

    RandomConstant minimum(const RandomConstant& a, const RandomConstant& b) {
        return new RandomConstant_min_2(a,b);
    }
    RandomConstant minimum(const Constant& a, const RandomConstant& b) {
        return new RandomConstant_min_2(a,b);
    }
    RandomConstant minimum(const RandomConstant& a, const Constant& b) {
        return new RandomConstant_min_2(a,b);
    }

    class RandomConstant_max_2 : public RandomConstant_exp {
        friend RandomConstant maximum(const RandomConstant& a, const RandomConstant& b);
        friend RandomConstant maximum(const Constant& a, const RandomConstant& b);
        friend RandomConstant maximum(const RandomConstant& a, const Constant& b);
    private:
        RandomConstant_max_2(const RandomConstant& i, const RandomConstant& j) : RandomConstant_exp(i,j) {}
        RandomConstant_max_2(const Constant& i, const RandomConstant& j) : RandomConstant_exp(i,j) {}
        RandomConstant_max_2(const RandomConstant& i, const Constant& j) : RandomConstant_exp(i,j) {}
        virtual double evaluate(int scenario) const {
            return std::max(left->evaluate(scenario),right->evaluate(scenario));
        }
        int getStage() const {
            return std::max(left->getStage(),right->getStage());
        }
    };

    RandomConstant maximum(const RandomConstant& a, const RandomConstant& b) {
        return new RandomConstant_max_2(a,b);
    }
    RandomConstant maximum(const Constant& a, const RandomConstant& b) {
        return new RandomConstant_max_2(a,b);
    }
    RandomConstant maximum(const RandomConstant& a, const Constant& b) {
        return new RandomConstant_max_2(a,b);
    }

    class RandomConstant_plus : public RandomConstant_exp {
        friend RandomConstant operator+(const RandomConstant& a, const RandomConstant& b);
        friend RandomConstant operator+(const Constant& a, const RandomConstant& b);
        friend RandomConstant operator+(const RandomConstant& a, const Constant& b);
    private:
        RandomConstant_plus(const RandomConstant& i, const RandomConstant& j) : RandomConstant_exp(i,j) {}
        RandomConstant_plus(const Constant& i, const RandomConstant& j) : RandomConstant_exp(i,j) {}
        RandomConstant_plus(const RandomConstant& i, const Constant& j) : RandomConstant_exp(i,j) {}
        virtual double evaluate(int scenario) const {
            return left->evaluate(scenario)+right->evaluate(scenario);
        }

        int getStage() const {
            return std::max(left->getStage(),right->getStage());
        }

    };

    RandomConstant operator+(const RandomConstant& a, const RandomConstant& b) {
        return new RandomConstant_plus(a,b);
    }
    RandomConstant operator+(const Constant& a, const RandomConstant& b) {
        return new RandomConstant_plus(a,b);
    }
    RandomConstant operator+(const RandomConstant& a, const Constant& b) {
        return new RandomConstant_plus(a,b);
    }


    class RandomConstant_minus : public RandomConstant_exp {
        friend RandomConstant operator-(const RandomConstant& a, const RandomConstant& b);
        friend RandomConstant operator-(const Constant& a, const RandomConstant& b);
        friend RandomConstant operator-(const RandomConstant& a, const Constant& b);
    private:
        RandomConstant_minus(const RandomConstant& i, const RandomConstant& j): RandomConstant_exp(i,j) {}
        RandomConstant_minus(const Constant& i, const RandomConstant& j): RandomConstant_exp(i,j) {}
        RandomConstant_minus(const RandomConstant& i, const Constant& j): RandomConstant_exp(i,j) {}
        virtual double evaluate(int scenario) const {
            return left->evaluate(scenario)-right->evaluate(scenario); 
        }

        int getStage() const {
            return std::max(left->getStage(),right->getStage());
        }
    };

    RandomConstant operator-(const RandomConstant& a, const RandomConstant& b) {
        return new RandomConstant_minus(a,b);
    }
    RandomConstant operator-(const Constant& a, const RandomConstant& b) {
        return new RandomConstant_minus(a,b);
    }
    RandomConstant operator-(const RandomConstant& a, const Constant& b) {
        return new RandomConstant_minus(a,b);
    }


    class RandomConstant_unary_minus : public RandomConstant_base {
        friend RandomConstant operator-(const RandomConstant& a);
    private:
        RandomConstant_unary_minus(const RandomConstant& i) : left(i) {}
        virtual double evaluate(int scenario) const {
            return -left->evaluate(scenario); 
        }
        virtual int getStage() const {
            return left->getStage();
        }
        virtual void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const {
            left->insertRandomVariables(v);
        }
        virtual void propagateIndexExpressions( const MP_index_exp& i1, const MP_index_exp& i2,const MP_index_exp& i3, const MP_index_exp& i4,const MP_index_exp& i5){
            left->propagateIndexExpressions(i1,i2,i3,i4,i5);
        }
        RandomConstant left;
    };
    RandomConstant operator-(const RandomConstant& a) {
        return new RandomConstant_unary_minus(a);
    }

    class RandomConstant_mult : public RandomConstant_exp {
        friend RandomConstant operator*(const RandomConstant& a, const RandomConstant& b);
        friend RandomConstant operator*(const Constant& a, const RandomConstant& b);
        friend RandomConstant operator*(const RandomConstant& a, const Constant& b);
    private:
        RandomConstant_mult(const RandomConstant& i, const RandomConstant& j) : RandomConstant_exp(i,j) {}
        RandomConstant_mult(const Constant& i, const RandomConstant& j) : RandomConstant_exp(i,j) {}
        RandomConstant_mult(const RandomConstant& i, const Constant& j) : RandomConstant_exp(i,j) {}
        virtual double evaluate(int scenario) const {
            return left->evaluate(scenario)*right->evaluate(scenario); 
        }

        int getStage() const {
            return std::max(left->getStage(),right->getStage());
        }

    };

    RandomConstant operator*(const RandomConstant& a, const RandomConstant& b) {
        return new RandomConstant_mult(a,b);
    }
    RandomConstant operator*(const Constant& a, const RandomConstant& b) {
        return new RandomConstant_mult(a,b);
    }
    RandomConstant operator*(const RandomConstant& a, const Constant& b) {
        return new RandomConstant_mult(a,b);
    }

    class RandomConstant_div : public RandomConstant_exp {
        friend RandomConstant operator/(const RandomConstant& a, const RandomConstant& b); 
        friend RandomConstant operator/(const Constant& a, const RandomConstant& b); 
        friend RandomConstant operator/(const RandomConstant& a, const Constant& b); 
    private:
        RandomConstant_div(const RandomConstant& i, const RandomConstant& j) : RandomConstant_exp(i,j) {}
        RandomConstant_div(const Constant& i, const RandomConstant& j) : RandomConstant_exp(i,j) {}
        RandomConstant_div(const RandomConstant& i, const Constant& j) : RandomConstant_exp(i,j) {}
        virtual double evaluate(int scenario) const {
            return left->evaluate(scenario)/right->evaluate(scenario); 
        }

        int getStage() const {
            return std::max(left->getStage(),right->getStage());
        }

    };

    RandomConstant operator/(const RandomConstant& a, const RandomConstant& b) {
        return new RandomConstant_div(a,b);
    }
    RandomConstant operator/(const Constant& a, const RandomConstant& b) {
        return new RandomConstant_div(a,b);
    }
    RandomConstant operator/(const RandomConstant& a, const Constant& b) {
        return new RandomConstant_div(a,b);
    }

    class RandomConstant_if : public RandomConstant_exp {
        friend RandomConstant mpif(const MP_boolean& c, const RandomConstant& a, const RandomConstant& b); 
    private:
        RandomConstant_if(const MP_boolean b, const RandomConstant& i, const RandomConstant& j) : RandomConstant_exp(i,j), B(b) {}
        virtual double evaluate(int scenario) const {
            if (B->evaluate()==true) {
                return left->evaluate(scenario);
            } else {
                return right->evaluate(scenario); 
            }
        }
        int getStage() const {
            if (B->evaluate()==true) {
                return left->getStage();
            } else {
                return right->getStage(); 
            }
        }

        MP_boolean B;
    };

    RandomConstant mpif(const MP_boolean& c, const RandomConstant& a, const RandomConstant& b) {
        return new RandomConstant_if(c,a,b);
    }


    class RandomConstant_max : public RandomConstant_base {
        friend RandomConstant maximum(const MP_domain& i, const RandomConstant& e);
    private:
        RandomConstant_max(const MP_domain& i, const RandomConstant& e) : d(i), exp(e) {}
        virtual double evaluate(int scenario) const {    
            MaxFunctor MF(exp);
            MF.scenario = scenario;
            d.forall(MF);
            return MF.the_max;
        }
        //virtual void propagateIndexExpressions(const MP_index_exp& d1,const MP_index_exp& d2,const MP_index_exp& d3,const MP_index_exp& d4,const MP_index_exp& d5) const{
        //    exp->propagateIndexExpressions(d1,d2,d3,d4,d5);
        //}
        virtual void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const {
            exp->insertRandomVariables(v);
        }
        virtual int getStage() const{
            return exp->getStage();
        }
        virtual void propagateIndexExpressions( const MP_index_exp& i1, const MP_index_exp& i2,const MP_index_exp& i3, const MP_index_exp& i4,const MP_index_exp& i5){
            exp->propagateIndexExpressions(i1,i2,i3,i4,i5);
        }
        class MaxFunctor : public Functor {
        public:
            MaxFunctor(RandomConstant exp) : C(exp), the_max(DBL_MIN),scenario(-1) {}
            void operator()() const {
                double temp = C->evaluate(scenario);
                if (temp > the_max) {
                    the_max = temp;
                }
            }
            RandomConstant C;      
            mutable double the_max;
            int scenario;
        };

        MP_domain d;
        RandomConstant exp;
    };

    class RandomConstant_min : public RandomConstant_base, public Functor {
        friend RandomConstant minimum(const MP_domain& i, const RandomConstant& e);
    private:
        RandomConstant_min(const MP_domain& i, const RandomConstant& e) : d(i), exp(e),the_min(DBL_MAX),scenario(-1) {}
        void operator()() const {
            double temp = exp->evaluate(scenario);
            if (temp < the_min) {
                the_min = temp;
            }
        }
        virtual double evaluate(int scenario) const {    
            the_min = DBL_MAX;
            this->scenario = scenario;
            d.forall(this);
            return the_min;
        }
        virtual int getStage() const{
            return exp->getStage();
        }
        virtual void propagateIndexExpressions( const MP_index_exp& i1, const MP_index_exp& i2,const MP_index_exp& i3, const MP_index_exp& i4,const MP_index_exp& i5){
            exp->propagateIndexExpressions(i1,i2,i3,i4,i5);
        }

        virtual void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const {
            exp->insertRandomVariables(v);
        }

        MP_domain d;
        RandomConstant exp;
        mutable double the_min;
        mutable int scenario;
    };

    class RandomConstant_sum : public RandomConstant_base, public Functor {
        friend RandomConstant sum(const MP_domain& i, const RandomConstant& e);
    private:
        RandomConstant_sum(const MP_domain& i, const RandomConstant& e) : d(i), exp(e),the_sum(0),scenario(-1) {}
        void operator()() const {
            the_sum += exp->evaluate(scenario);
        }
        virtual double evaluate(int scenario) const {  
            the_sum = 0;
            this->scenario = scenario;
            d.forall(this);
            return the_sum;
        }
        virtual int getStage() const {
            return exp->getStage();
        }
        virtual void propagateIndexExpressions( const MP_index_exp& i1, const MP_index_exp& i2,const MP_index_exp& i3, const MP_index_exp& i4,const MP_index_exp& i5){
            exp->propagateIndexExpressions(i1,i2,i3,i4,i5);
        }

        virtual void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const {
            exp->insertRandomVariables(v);
        }

        MP_domain d;
        RandomConstant exp;
        mutable double the_sum;
        mutable int scenario;
    };

    class RandomConstant_product : public RandomConstant_base, public Functor {
        friend RandomConstant product(const MP_domain& i, const RandomConstant& e);
    private:
        RandomConstant_product(const MP_domain& i, const RandomConstant& e) : d(i), exp(e),the_product(1),scenario(-1) {}
        void operator()() const {
            the_product *= exp->evaluate(scenario);
        }
        virtual double evaluate(int scenario) const {  
            the_product = 1;
            this->scenario = scenario;
            d.forall(this);
            return the_product;
        }
        virtual int getStage() const {
            return exp->getStage();
        }

        virtual void propagateIndexExpressions( const MP_index_exp& i1, const MP_index_exp& i2,const MP_index_exp& i3, const MP_index_exp& i4,const MP_index_exp& i5){
            exp->propagateIndexExpressions(i1,i2,i3,i4,i5);
        }

        virtual void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const {
            exp->insertRandomVariables(v);
        }

        MP_domain d;
        RandomConstant exp;
        mutable double the_product;
        mutable int scenario;
    };

    RandomConstant maximum(const MP_domain& i, const RandomConstant& e) {
        return new RandomConstant_max(i,e);
    }
    RandomConstant minimum(const MP_domain& i, const RandomConstant& e) {
        return new RandomConstant_min(i,e);
    }
    RandomConstant sum(const MP_domain& i, const RandomConstant& e) {
        return new RandomConstant_sum(i,e);
    }
    RandomConstant product(const MP_domain& i, const RandomConstant& e) {
        return new RandomConstant_product(i,e);
    }

    RandomConstant::RandomConstant(const RandomDataRef& d) : 
    Handle<RandomConstant_base*>(const_cast<RandomDataRef*>(&d)) {}

    RandomConstant::RandomConstant(const Constant& c) :
    Handle<RandomConstant_base*>(new RandomConstant_constant(c)) {}

    RandomConstant::RandomConstant( RandomConstant_base* r ) : 
    Handle<RandomConstant_base*>(r) {}

    //const RandomDataRef& RandomDataRef::operator=(  Constant c )
    //{
    //    return *this;
    //}

    //const RandomDataRef& RandomDataRef::operator=( RandomConstant c )
    //{
    //    return *this;
    //}



    //int RandomDataRef::getColumn() const
    //{
    //    return -1; //RHS
    //}

    //void RandomDataRef::insertVariables( std::set<MP_variable*>& v ) const
    //{
    //    //Do nothing as there are no variables in a RandomDataRef
    //}



    RandomDataRef::~RandomDataRef()
    {

    }

    void RandomDataRef::operator()() const
    {

    }
} // End of namespace flopc

