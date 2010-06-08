// ******************** FlopCpp **********************************************
// File: MP_constant.hpp
// ****************************************************************************

#ifndef _MP_constant_hpp_
#define _MP_constant_hpp_

#include <set>

#include "MP_utilities.hpp"


namespace flopc {

    //Forward Declaration for Constant Class
    class RandomVariable;
    class Constant_random_variable;
    class MP_index_exp;
    class MP_random_data;
    class RandomVariable;

    /** @brief Base class for all "constant" types of data.
    @ingroup INTERNAL_USE
    @note FOR INTERNAL USE: This is not normally used directly by the
    calling code.
    */
    class Constant_base {
        friend class Constant;
        friend class Handle<Constant_base*>;
    public:
        virtual double evaluate(int scenario = getZero) const = 0; //get value of constant (must be double)

        virtual int getStage() const { //get Stage. If no stochastic variables are present, i.e. no MP_stage defined and assigned, return 0.
            return 0;
        }
        virtual RandomVariable* getRandomVariable() const {
            return 0; // If we want to get a RandomVariable, but no subclass returns one, return 0 to indicate there is no RandomVariable present..
        }

        virtual void propagateIndexExpression(const MP_index_exp& d1,const MP_index_exp& d2,const MP_index_exp& d3,const MP_index_exp& d4,const MP_index_exp& d5) const = 0;

        virtual void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const = 0;


    protected:
        Constant_base() : count(0) {}
        virtual ~Constant_base() {}
        int count;

    };

    //forward declarations
    class DataRef;
    class RandomDataRef;
    class MP_domain;
   



    /** @brief Reference counted class for all "constant" types of data.
    @ingroup INTERNAL_USE
    @note FOR INTERNAL USE: This is not normally used directly by the calling 
    code.
    This contains counters to ConstantBase pointers.
    These pointers may be of any of the Constant_base * type.  
    */
    class Constant : public Handle<Constant_base*> {
    public:
        Constant(const MP_index_exp& i);
        Constant(const RandomDataRef& r);
        Constant(const DataRef& d);
        Constant(double d);
        Constant(int d);
        // operator-();
        //   private:
        Constant(Constant_base* r) : Handle<Constant_base*>(r) {}

        // Transitional method: Can be removed if all of the flopc++ programs have switched to the new standard (for variable bounds?!)
        Constant& operator()() { return *this; };
        Constant& operator()(const MP_index_exp&) { return *this; };
        Constant& operator()(const MP_index_exp&,const MP_index_exp&) { return *this; };
        Constant& operator()(const MP_index_exp&,const MP_index_exp&,const MP_index_exp&) { return *this; };
        Constant& operator()(const MP_index_exp&,const MP_index_exp&,const MP_index_exp&,const MP_index_exp&) { return *this; };
        Constant& operator()(const MP_index_exp&,const MP_index_exp&,const MP_index_exp&,const MP_index_exp&,const MP_index_exp&) { return *this; };

    };

    Constant operator-(const Constant& a);

    /** @brief for computing the absolute value of a constant value.
    @ingroup PublicInterface
    This is used in the normal formation of an expression such as abs(-5)
    @li input is a constant.  It cannot be a variable expression.
    @li Returns a Constant evaluating to the absolute value of the parameter 
    */
    Constant abs(const Constant& c);
    /** @brief for returning non-negative value of the constant. 
    @ingroup PublicInterface
    This is used in the formation of an expression.  It is used to return
    a non-negative value..
    @param c an imput constant
    @return the absolute value of the constant.
    @li if the Constant is positive, it returns a positive number.
    @li if the Constant is negative or zero, it returns 0.0
    */
    Constant pos(const Constant& c);
    /** @brief The ceiling integral value of the input constant.
    @ingroup PublicInterface
    This is used in the formation of an expression.  It is used to "round up"
    a numeric constant which is potentially non-integer.
    @param c is a constant
    @return the ceiling or "rounded up" of the parameter 
    @li ceil(3.2) evaluates to 4.0
    */
    Constant ceil(const Constant& c);
    /** @brief The floor integral value of the input constant.
    @ingroup PublicInterface
    This is used in the formation of an expression.  It is used to "truncate"
    a numeric constant which is potentially non-integer.
    @param c is a constant
    @return the floor or "truncated" value of the parameter 
    @li floor(3.7) evaluates to 3.0
    */
    Constant floor(const Constant& c);
    /** @brief Returns the smaller of two constants.
    @ingroup PublicInterface
    This is used in the formation of an expression.  
    @param a first constant
    @param b second constant
    @return the lesser of the two values.
    @li minimum(3.6,3.7) evaluates to 3.6
    */
    Constant minimum(const Constant& a, const Constant& b);
    /** @brief Returns the larger of two constants.
    @ingroup PublicInterface
    This is used in the formation of an expression.  
    @param a first constant
    @param b second constant
    @return the greater of the two numbers
    @li maximum(3.6,3.7) evaluates to 3.7
    */
    Constant maximum(const Constant& a, const Constant& b);
    /** @brief Returns the sum of two constants.
    @ingroup PublicInterface
    This is used in the formation of an expression.  
    @param a first constant
    @param b second constant
    @return the sum of the constants.
    */
    Constant operator+(const Constant& a, const Constant& b);
    /** @brief Returns the difference of two constants.
    @ingroup PublicInterface
    This is used in the formation of an expression.  
    @param a first constant
    @param b second constant
    @return the difference between the constants.
    */
    Constant operator-(const Constant& a, const Constant& b);
    /** @brief Returns the product of two constants.
    @ingroup PublicInterface
    This is used in the formation of an expression.  
    @param a first constant
    @param b second constant
    @return the result of multiplying the constants.
    */
    Constant operator*(const Constant& a, const Constant& b);
    /** @brief Returns the quotient of two constants.
    @ingroup PublicInterface
    This is used in the formation of an expression.  
    @param a first constant
    @param b second constant
    @return the result of dividing the first parameter by the second.
    */
    Constant operator/(const Constant& a, const Constant& b);

    class MP_boolean;
    Constant mpif(const MP_boolean& c, const Constant& a, const Constant& b);

    /** @brief Returns the maximum over the domain of the constant.
    @ingroup PublicInterface
    @todo description?  Haven't used this.
    @param i MP_domain used in evaluation
    @param e constant
    */
    Constant maximum(const MP_domain& i, const Constant& e);
    /** @brief Returns the sum of two constants.
    @ingroup PublicInterface
    @todo description?  Haven't used this.
    @param i MP_domain used in evaluation
    @param e second constant
    */
    Constant minimum(const MP_domain& i, const Constant& e);
    /** @brief Returns the sum of two constants.
    @ingroup PublicInterface
    @todo description?  Haven't used this.
    @param i MP_domain used in evaluation
    @param e constant
    */
    Constant sum(const MP_domain& i, const Constant& e);
    /** @brief Returns the sum of two constants.
    @ingroup PublicInterface
    @todo description?  Haven't used this.
    @param i MP_domain used in evaluation
    @param e constant
    */
    Constant product(const MP_domain& i, const Constant& e);
}  // End of namespace flopc
#endif
