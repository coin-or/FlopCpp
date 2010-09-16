// ******************** FlopCpp **********************************************
// File: MP_RandomConstant.hpp
// ****************************************************************************

#ifndef _MP_random_constant_hpp_
#define _MP_random_constant_hpp_

#include <vector>
#include <set>

#include <boost/shared_ptr.hpp>

#include "MP_utilities.hpp"
#include "MP_constant.hpp"
#include "MP_domain.hpp"
#include "MP_index.hpp"
#include "MP_boolean.hpp"


namespace flopc {

    //Forward Declaration for RandomConstant Class
    class RandomVariable;
    class MP_index_exp;
    class MP_random_data;
    class MP_variable;
    class RandomDataRef;
    class MP_random_data_impl;

    /** @brief Base class for all "RandomConstant" types of data.
    @ingroup INTERNAL_USE
    @note FOR INTERNAL USE: This is not normally used directly by the
    calling code.
    */
    class RandomConstant_base {
        friend class RandomConstant;

        friend class Handle<RandomConstant_base*>;
    public:
        virtual double evaluate(int scenario = getZero) const = 0; //get value of RandomConstant (must be double)
        virtual int getStage() const = 0;
        virtual void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const = 0;
        virtual void propagateIndexExpressions( const MP_index_exp& i1, const MP_index_exp& i2,const MP_index_exp& i3, const MP_index_exp& i4,const MP_index_exp& i5) = 0;


    protected:
        RandomConstant_base() : count(0) {}
        virtual ~RandomConstant_base() {}
        int count;

    };
    //forward declarations
    class MP_domain;

    /** @brief Reference counted class for all "RandomConstant" types of data.
    @ingroup INTERNAL_USE
    @note FOR INTERNAL USE: This is not normally used directly by the calling 
    code.
    This contains counters to RandomConstantBase pointers.
    These pointers may be of any of the RandomConstant_base * type.  
    */
    class RandomConstant : public Handle<RandomConstant_base*> {
    public:
        RandomConstant(const RandomDataRef& d);
        RandomConstant(RandomConstant_base* r);
        RandomConstant(const Constant& c); //Allows transformation of Constant to a RandomConstant by default
    };

    class RandomDataRef : public RandomConstant_base { 
        friend class MP_random_data_impl;
    public:

        RandomDataRef(MP_random_data_impl * const rv, 
            const MP_index_exp& i1,
            const MP_index_exp& i2,
            const MP_index_exp& i3,
            const MP_index_exp& i4,
            const MP_index_exp& i5);


        virtual ~RandomDataRef();
        RandomDataRef& such_that(const MP_boolean& b);

        //virtual int getColumn() const;
        //double level() const;
        virtual int getStage() const;
        virtual double evaluate(int scenario) const;
        virtual void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const;
        virtual void propagateIndexExpressions( const MP_index_exp& i1, const MP_index_exp& i2,const MP_index_exp& i3, const MP_index_exp& i4,const MP_index_exp& i5);
        //TODO: See if this methods is still needed
        virtual RandomVariable* getRandomVariable(int index = outOfBound) const;

    
        //Assignment operators to allow assignment in the model
        const RandomDataRef& operator=(const RandomDataRef& c);
        const RandomDataRef& operator=(const RandomConstant& c);
        const RandomDataRef& operator=( RandomVariable* c);
        
        
private:
        //Copy Constructor (needed in initialization of Constant_random_variable
        RandomDataRef(const RandomDataRef&);
        

        //void generate(const MP_domain& domain,
        //    vector<TerminalExpression *> multiplicators,
        //    MP::GenerateFunctor& f,*this
        //    double m) const;

        MP_random_data_impl* const RV;
        //const MP_set_base &S1,&S2,&S3,&S4,&S5;
        MP_index_exp I1,I2,I3,I4,I5;
        MP_boolean B;
        mutable bool visited;

    };

    RandomConstant operator-(const RandomConstant& a);
    /** @brief for computing the absolute value of a RandomConstant value.
    @ingroup PublicInterface
    This is used in the normal formation of an expression such as abs(-5)
    @li input is a RandomConstant.  It cannot be a variable expression.
    @li Returns a RandomConstant evaluating to the absolute value of the parameter 
    */
    RandomConstant abs(const RandomConstant& c);
    /** @brief for returning non-negative value of the RandomConstant. 
    @ingroup PublicInterface
    This is used in the formation of an expression.  It is used to return
    a non-negative value..
    @param c an imput RandomConstant
    @return the absolute value of the RandomConstant.
    @li if the RandomConstant is positive, it returns a positive number.
    @li if the RandomConstant is negative or zero, it returns 0.0
    */
    RandomConstant pos(const RandomConstant& c);
    /** @brief The ceiling integral value of the input RandomConstant.
    @ingroup PublicInterface
    This is used in the formation of an expression.  It is used to "round up"
    a numeric RandomConstant which is potentially non-integer.
    @param c is a RandomConstant
    @return the ceiling or "rounded up" of the parameter 
    @li ceil(3.2) evaluates to 4.0
    */
    RandomConstant ceil(const RandomConstant& c);
    /** @brief The floor integral value of the input RandomConstant.
    @ingroup PublicInterface
    This is used in the formation of an expression.  It is used to "truncate"
    a numeric RandomConstant which is potentially non-integer.
    @param c is a RandomConstant
    @return the floor or "truncated" value of the parameter 
    @li floor(3.7) evaluates to 3.0
    */
    RandomConstant floor(const RandomConstant& c);
    /** @brief Returns the smaller of two RandomConstants.
    @ingroup PublicInterface
    This is used in the formation of an expression.  
    @param a first RandomConstant
    @param b second RandomConstant
    @return the lesser of the two values.
    @li minimum(3.6,3.7) evaluates to 3.6
    */
    RandomConstant minimum(const RandomConstant& a, const RandomConstant& b);
    RandomConstant minimum(const Constant& a, const RandomConstant& b);
    RandomConstant minimum(const RandomConstant& a, const Constant& b);
    /** @brief Returns the larger of two RandomConstants.
    @ingroup PublicInterface
    This is used in the formation of an expression.  
    @param a first RandomConstant
    @param b second RandomConstant
    @return the greater of the two numbers
    @li maximum(3.6,3.7) evaluates to 3.7
    */
    RandomConstant maximum(const RandomConstant& a, const RandomConstant& b);
    RandomConstant maximum(const Constant& a, const RandomConstant& b);
    RandomConstant maximum(const RandomConstant& a, const Constant& b);
    /** @brief Returns the sum of two RandomConstants.
    @ingroup PublicInterface
    This is used in the formation of an expression.  
    @param a first RandomConstant
    @param b second RandomConstant
    @return the sum of the RandomConstants.
    */
    RandomConstant operator+(const RandomConstant& a, const RandomConstant& b);
    RandomConstant operator+(const Constant& a, const RandomConstant& b);
    RandomConstant operator+(const RandomConstant& a, const Constant& b);
    /** @brief Returns the difference of two RandomConstants.
    @ingroup PublicInterface
    This is used in the formation of an expression.  
    @param a first RandomConstant
    @param b second RandomConstant
    @return the difference between the RandomConstants.
    */
    RandomConstant operator-(const RandomConstant& a, const RandomConstant& b);
    RandomConstant operator-(const Constant& a, const RandomConstant& b);
    RandomConstant operator-(const RandomConstant& a, const Constant& b);
    /** @brief Returns the product of two RandomConstants.
    @ingroup PublicInterface
    This is used in the formation of an expression.  
    @param a first RandomConstant
    @param b second RandomConstant
    @return the result of multiplying the RandomConstants.
    */
    RandomConstant operator*(const RandomConstant& a, const RandomConstant& b);
    RandomConstant operator*(const Constant& a, const RandomConstant& b);
    RandomConstant operator*(const RandomConstant& a, const Constant& b);
    /** @brief Returns the quotient of two RandomConstants.
    @ingroup PublicInterface
    This is used in the formation of an expression.  
    @param a first RandomConstant
    @param b second RandomConstant
    @return the result of dividing the first parameter by the second.
    */
    RandomConstant operator/(const RandomConstant& a, const RandomConstant& b);
    RandomConstant operator/(const Constant& a, const RandomConstant& b);
    RandomConstant operator/(const RandomConstant& a, const Constant& b);

    class MP_boolean;
    RandomConstant mpif(const MP_boolean& c, const RandomConstant& a, const RandomConstant& b);

    /** @brief Returns the maximum over the domain of the RandomConstant.
    @ingroup PublicInterface
    @todo description?  Haven't used this.
    @param i MP_domain used in evaluation
    @param e RandomConstant
    */
    RandomConstant maximum(const MP_domain& i, const RandomConstant& e);
    /** @brief Returns the sum of two RandomConstants.
    @ingroup PublicInterface
    @todo description?  Haven't used this.
    @param i MP_domain used in evaluation
    @param e second RandomConstant
    */
    RandomConstant minimum(const MP_domain& i, const RandomConstant& e);
    /** @brief Returns the sum of two RandomConstants.
    @ingroup PublicInterface
    @todo description?  Haven't used this.
    @param i MP_domain used in evaluation
    @param e RandomConstant
    */
    RandomConstant sum(const MP_domain& i, const RandomConstant& e);
    /** @brief Returns the sum of two RandomConstants.
    @ingroup PublicInterface
    @todo description?  Haven't used this.
    @param i MP_domain used in evaluation
    @param e RandomConstant
    */
    RandomConstant product(const MP_domain& i, const RandomConstant& e);
}  // End of namespace flopc
#endif
