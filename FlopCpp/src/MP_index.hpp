// ******************** FlopCpp **********************************************
// File: MP_index.hpp
// ****************************************************************************

#ifndef _MP_index_hpp_
#define _MP_index_hpp_

#include "MP_utilities.hpp"
#include "MP_constant.hpp"

namespace flopc {

    class MP_index;
    class MP_domain;
    class MP_set;
    class MP_index_exp;

    /** @brief Internal representation of a index
    @ingroup INTERNAL_USE
    @note FOR INTERNAL USE: This is not normally used directly by the
    calling code.
    */
    class MP_index_base {
        friend class Handle<MP_index_base*>;
        friend class MP_index_exp;
    public:
        virtual int evaluate() const = 0;
        virtual MP_index* getIndex() const = 0;
        virtual MP_domain getDomain(MP_set* s) const = 0;
        virtual MP_index_base* deepCopy() const = 0;
        virtual MP_index_base* insertIndexExpr(const MP_index_exp&) = 0;

        //	virtual void display()const;
    protected:
        MP_index_base() : count(0) {}
        virtual ~MP_index_base() {}
    private:
        int count;
    };

    class MP_index_constant;

    /** @brief Representation of an index.
    @ingroup PublicInterface
    This is one of the main public interface classes.  
    It is used to iterate through, or index into an MP_domain.
    It is also used to share the 'current' index offsets between
    expressions which share an index.
    @li these can be built stand-alone
    @li these are sometime constructed as needed.
    @li there is a special "empty" which is a unique constant. \
    This constant is used when defaulting passed parameters for \
    extra dimensions which are unused.
    */
    class MP_index : public MP_index_base {
        friend class MP_domain_set;
        friend class MP_index_constant;
        // We need friend classes that can do semantic checking..
        friend class MP_data;
        friend class MP_random_data_impl;
        friend class MP_variable;
        //friend class MP_subset<nbr>;
        // End Semantic check

        template<int nbr> friend class MP_domain_subset;
    public:
        /// Default constructor. 
        MP_index() : index(0), instantiated(false) {}
        virtual ~MP_index() {}

        virtual int evaluate() const { 
            return index; 
        }
        virtual MP_index_base* deepCopy() const;
        virtual MP_index_base* insertIndexExpr(const MP_index_exp&);


        /// returns a reference to the distinct "empty" index.
        static MP_index &getEmpty();
        /// returns a reference to the distinct "wildcard" index.
        static MP_index &Any();

    private:
        /** interrogate state of instantiation of data.
        */
        bool isInstantiated() const { 
            return instantiated; 
        }
        /** Setter for the index.
        @todo should this assert "instatiated"?
        */
        void assign(int i) { 
            index = i;
        }
        /** unsetter for instatiated.
        */
        void unInstantiate() {
            instantiated = false; 
        }
        /** setter for instatiated.
        */
        void instantiate() {
            instantiated = true; 
        }
        /** getter for MP_index * data type.  
        */
        virtual MP_index* getIndex() const {
            return const_cast<MP_index*>(this);
        }
        /// Getter for domain over which this index is applied.
        // return new MP_domain_set over s and this
        virtual MP_domain getDomain(MP_set* s) const;

        int index; //current index
        bool instantiated;
    };


    /// returns a Constant as a result of addition of two MP_index values.
    Constant operator+(MP_index& a, MP_index& b);
    /// returns a Constant as a result of a difference of two MP_index values.
    Constant operator-(MP_index& a, MP_index& b);

    /** returns an index expression from a difference between an MP_index
    and an integer.  (i-5)
    */
    MP_index_exp operator-(MP_index& i,const int& j);
    /** returns an index expression from a sum between an MP_index
    and an integer.  (i+5)
    */
    MP_index_exp operator+(MP_index& i,const int& j);
    /** returns an index expression from a sum between an MP_index
    and a Constant.  
    */
    MP_index_exp operator+(MP_index& i,const Constant& j);
    /** returns an index expression from a product between an MP_index
    and a Constant.  
    */
    MP_index_exp operator*(MP_index& i,const Constant& j);

    class SUBSETREF;

    /** @brief Representation of an expression involving an index.
    @ingroup PublicInterface
    This is one of the main public interface classes.  
    It is used to create complex arrangements of index values.
    <br> Index expressions can involve:
    @li constants
    @li other indexes
    @li subset references
    @li other index expressions.
    <br>
    There is a unique 'empty' version for use in defaulting extra
    dimensions.
    */
    class MP_index_exp : public Handle<MP_index_base*> {
    public:
        /// For internal use.
        MP_index_exp(MP_index_base* r) : Handle<MP_index_base*>(r) {} 
        /// create an index expression from a constant integer.
        MP_index_exp(int i=0); 
        /// create an index expression from a Constant
        MP_index_exp(const Constant& c);
        /// create an index expression from an MP_index.
        MP_index_exp(MP_index& i);
        /** create an index expression from a SUBSETREF
        @todo internal? or explain?
        */
        MP_index_exp(const SUBSETREF& d);
        /// copy constructor from another MP_index_exp
        MP_index_exp(const MP_index_exp& other);
        virtual ~MP_index_exp() {}
        /// Return the unique empty expression.
        static const MP_index_exp &getEmpty();
        
        MP_index_exp deepCopyIndexExpression() const;


    };

    /** @brief Internal representation of an index expression
    @ingroup INTERNAL_USE
    @note FOR INTERNAL USE: This is not normally used directly by the
    calling code.
    @see operator*(MP_index& i, const Constant & j);
    */
    class MP_index_mult : public MP_index_base {
        friend MP_index_exp operator*(MP_index& i,const Constant& j);
    private:
        MP_index_mult(MP_index& i, const Constant& j) : left(i), right(j) {}

        int evaluate() const {
            return left->evaluate()*int(right->evaluate()); 
        }
        MP_index* getIndex() const {
            return left->getIndex();
        }
        virtual MP_domain getDomain(MP_set* s) const;

        virtual MP_index_base* deepCopy() const;
        virtual MP_index_base* insertIndexExpr(const MP_index_exp&);

        MP_index_exp left;
        Constant right;
    };

    /** @brief Internal representation of an index expression
    @ingroup INTERNAL_USE
    @note FOR INTERNAL USE: This is not normally used directly by the
    calling code.
    @see operator+(MP_index& i, const Constant & j);
    */
    class MP_index_sum : public MP_index_base {
        friend MP_index_exp operator+(MP_index& i,const Constant& j);
        friend MP_index_exp operator+(MP_index& i,const int& j);
    private:
        MP_index_sum(MP_index& i, const Constant& j) : left(i), right(j) {}

        int evaluate() const {
            return left->evaluate()+int(right->evaluate()); 
        }
        MP_index* getIndex() const {
            return left->getIndex();
        }
        virtual MP_domain getDomain(MP_set* s) const;

        virtual MP_index_base* deepCopy() const;
        virtual MP_index_base* insertIndexExpr(const MP_index_exp&);
        MP_index_exp left;
        Constant right;
    };

    /** @brief Internal representation of an index expression
    @ingroup INTERNAL_USE
    @note FOR INTERNAL USE: This is not normally used directly by the
    calling code.
    @see operator+(MP_index& i, const Constant & j);
    */
    class MP_index_dif : public MP_index_base {
        friend MP_index_exp operator-(MP_index& i,const Constant& j);
        friend MP_index_exp operator-(MP_index& i,const int& j);
    private:
        MP_index_dif(MP_index& i, const Constant& j) : left(i), right(j) {}

        int evaluate() const {
            return left->evaluate()-int(right->evaluate()); 
        }
        MP_index* getIndex() const {
            return left->getIndex();
        }
        virtual MP_domain getDomain(MP_set* s) const;
        virtual MP_index_base* deepCopy() const;
        virtual MP_index_base* insertIndexExpr(const MP_index_exp&);
        MP_index_exp left;
        Constant right;
    };

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
        virtual MP_domain getDomain(MP_set* s) const;
        virtual MP_index_base* deepCopy() const { return MP_index_exp::getEmpty().operator->(); }
        virtual MP_index_base* insertIndexExpr(const MP_index_exp& expr) { 
            return expr.operator->();
            throw invalid_argument_exception(); //TODO: What is it with this?
        }
        MP_index* indexPtr;
        Constant C;
    };


} // End of namespace flopc
#endif
