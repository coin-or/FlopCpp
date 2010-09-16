// ******************** FlopCpp **********************************************
// File: MP_index.cpp
//****************************************************************************

#include "MP_index.hpp"
#include "MP_domain.hpp"
#include "MP_set.hpp"
#include "MP_model.hpp"

namespace flopc {
    MP_index &MP_index::getEmpty() {
        static MP_index Empty;
        return Empty;
    }
    MP_index &MP_index::Any() {
        static MP_index Any_index;
        return Any_index;
    }

    MP_index_base* MP_index::deepCopy() const
    {
        return getIndex();
    }

    MP_index_base* MP_index::insertIndexExpr( const MP_index_exp& expr)
    {
        if (this->getIndex() == expr->getIndex()){
            return expr.operator->();
        }
        else{ // We should check for indices, which can be instantiated during the display and generation methiod
            return this->getIndex();
        }
        throw not_implemented_error();
        return 0;
    }
    const MP_index_exp &MP_index_exp::getEmpty() {
        static MP_index_exp Empty(Constant(0.0));
        return Empty;
    }

    flopc::MP_index_exp MP_index_exp::deepCopyIndexExpression() const
    {
        return MP_index_exp( operator->()->deepCopy());
    }

    class MP_index_constant : public MP_index_base {
        friend class MP_index_exp;
    public:
    private:
        MP_index_constant(const Constant& c) : indexPtr(new MP_index()),C(c) { indexPtr->assign(c->evaluate());}
        ~MP_index_constant() { delete indexPtr; }
        int evaluate() const {
            return int(C->evaluate()); 
        }
        MP_index* getIndex() const {
            // Return a new MP_index pointer (?)
            // We can not return a null pointer. We have to return a pointer to this?
            return indexPtr;
        }
        virtual MP_domain getDomain(MP_set* s) const{
            return MP_domain::getEmpty();
        }
        virtual MP_index_base* deepCopy() const { return MP_index_exp::getEmpty().operator->(); }
        virtual MP_index_base* insertIndexExpr(const MP_index_exp& expr) { 
            return expr.operator->();
            throw invalid_argument_exception(); //TODO: What is it with this?
        }
        MP_index* indexPtr;
        Constant C;
    };

    class MP_index_subsetRef : public MP_index_base {
        friend class MP_index_exp;
    private:
        MP_index_subsetRef(const SUBSETREF& s) : S(&s) {}
        int evaluate() const {
            return int(S->evaluate()); 
        }
        MP_index* getIndex() const {
            return S->getIndex();
        }
        virtual MP_domain getDomain(MP_set* s) const{
            return MP_domain(S->getDomain(s));
        }
        virtual MP_index_base* deepCopy() const { return S->getIndex(); }
        virtual MP_index_base* insertIndexExpr(const MP_index_exp& expr) { 
            throw not_implemented_error();return expr.operator->(); }
        const SUBSETREF* S;
    };

    MP_index_exp operator+(MP_index& i,const Constant& j) {
        return new MP_index_sum(i,j);
    }

    MP_index_exp operator+(MP_index& i,const int& j) {
        return new MP_index_sum(i,Constant(j));
    }

    MP_index_exp operator-(MP_index& i,const Constant& j) {
        return new MP_index_dif(i,j);
    }

    MP_index_exp operator-(MP_index& i,const int& j) {
        return new MP_index_dif(i,Constant(j));
    }

    MP_index_exp operator*(MP_index& i,const Constant& j) {
        return new MP_index_mult(i,j);
    }



    flopc::MP_index_base* MP_index_mult::deepCopy() const
    {
        return new MP_index_mult(dynamic_cast<MP_index&>(*left->getIndex()),right);
    }

    MP_index_base* MP_index_mult::insertIndexExpr( const MP_index_exp& expr) 
    {
        if (expr->getIndex() == left->getIndex()){
            left = expr;
            return this;
        }
        else {
            throw invalid_argument_exception();
        }
        return this;
        

    }

    flopc::MP_index_base* MP_index_sum::deepCopy() const
    {
        return new MP_index_sum(dynamic_cast<MP_index&>(*left->getIndex()),right);
    }

    MP_index_base* MP_index_sum::insertIndexExpr( const MP_index_exp& expr)
    {
        if (expr->getIndex() == left->getIndex()){
            left = expr;
        }
        else {
            throw invalid_argument_exception();
        }
        return this;

    }

    flopc::MP_index_base* MP_index_dif::deepCopy() const
    {
        return new MP_index_dif(dynamic_cast<MP_index&>(*left->getIndex()),right);
    }

    MP_index_base* MP_index_dif::insertIndexExpr( const MP_index_exp& expr)
    {
        if (expr->getIndex() == left->getIndex()){
            left = expr;
       }
        else {
            throw invalid_argument_exception();
        }
        return this;

    }
} // End of namespace flopc

using namespace flopc;


MP_domain MP_index::getDomain(MP_set* s) const{
    return new MP_domain_set(s,const_cast<MP_index*>(this)) ; 
}

MP_domain MP_index_mult::getDomain(MP_set* s) const{
    return left->getDomain(s); 
}

MP_domain MP_index_sum::getDomain(MP_set* s) const{
    return left->getDomain(s); 
}

MP_domain MP_index_dif::getDomain(MP_set* s) const{
    return left->getDomain(s);
}

MP_index_exp::MP_index_exp(int i) : 
Handle<MP_index_base*>(new MP_index_constant(Constant(i))) {} 

MP_index_exp::MP_index_exp(const SUBSETREF& s) : 
Handle<MP_index_base*>(new MP_index_subsetRef(s)) {}

MP_index_exp::MP_index_exp(const Constant& c) : 
Handle<MP_index_base*>(new MP_index_constant(c)) {}

MP_index_exp::MP_index_exp(MP_index& i) : //Create MP_index_exp from MP_index reference.. Maybe we want to create an MP_index_exp from an MP_index?
Handle<MP_index_base*>(&i) { operator->()->count++; }

MP_index_exp::MP_index_exp(const MP_index_exp &other): //Create a MP_index_expression from another one by using this cast?!
Handle<MP_index_base*>((const Handle<MP_index_base*> &)other) { }

