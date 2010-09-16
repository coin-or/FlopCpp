// ******************** FlopCpp **********************************************
// File: MP_expression.hpp
// ****************************************************************************

#ifndef _MP_expression_hpp_
#define _MP_expression_hpp_

#include <vector>
#include <set>
using std::vector;
using std::set;

#include <boost/shared_ptr.hpp>

#include "MP_domain.hpp"
#include "MP_constant.hpp"
#include "MP_utilities.hpp"

namespace flopc {

    class MP_constraint;
    class TerminalExpression; 
    class MP_variable;
    class VariableRef;
    class RandomDataRef;
    class RandomConstant;

    class MP {
        friend class MP_expression;
        friend class MP_constraint;
        friend class MP_model;
        friend class Messenger;
        friend class VerboseMessenger; 
        friend class CoefLess;
        friend class MP_expression_base;
        friend class VariableRef;
        friend class MP_variable;
        friend class RandomDataRef;

        struct Coef {
            Coef(int c, int r, double v, int s = 0,int rs = 0,const std::vector<double>& scen = std::vector<double>()) : 
        col(c), row(r), varStage(s),randomStage(rs), val(v), scenVector(scen)  {}
        int col, row, varStage,randomStage;
        double val;
        std::vector<double> scenVector;
        //Col = Column Number of Coef
        //Row = Row Number of Coef
        //Stage = Stage of belonging Variable (Column)
        //Val = Coefficient value
        //scenVal = stores random values for given scenarios..
        bool operator< (const MP::Coef& rhs) {
            if (this->col < rhs.col) {
                return true;
            } else if (this->col == rhs.col && this->row < rhs.row) {
                return true;
            } else {
                return false;
            }
        }


        };

        static bool CoefLess (const MP::Coef& a, const MP::Coef& b) ;
        static bool CoefLessShared (const boost::shared_ptr<MP::Coef>& a, const boost::shared_ptr<MP::Coef>& b) ;
        static bool CoefLessWithStageShared  (const boost::shared_ptr<MP::Coef>& a, const boost::shared_ptr<MP::Coef>& b) ;
        /*    struct CoefLess {
        bool operator()  const;
        };

        struct CoefLessShared {
        bool operator() (const MP::Coef& a, const MP::Coef& b) const;
        };
        struct CoefLessWithStageShared {
        bool operator() (const boost::shared_ptr<MP::Coef>& a, const boost::shared_ptr<MP::Coef>& b) const;
        };*/

    protected:
        //This class is used to set all coefficients of an constraint to 
        class GenerateFunctor : public Functor {
        public:
            GenerateFunctor(MP_constraint* r, vector<Coef>& cfs): R(r), Coefs(cfs) {}

            virtual~ GenerateFunctor() {  }

            void setMultiplicator(vector<TerminalExpression*>& mults, double m) {
                multiplicators = mults;
                M = m;
            }
            void setTerminalExpression(const TerminalExpression* c) {
                C = c;
            }

            void operator()() const;
        private:
            //Disable copy contructor and assignment operator
            GenerateFunctor(const GenerateFunctor&);
            GenerateFunctor& operator=(const GenerateFunctor&);


            MP_constraint* R;
            //Temporary values
            vector<TerminalExpression*> multiplicators; //Coefficients?
            double M; // sign (1=lhs, -1=rhs)
            const TerminalExpression* C; //Current 
            //End Temporary values
            vector<MP::Coef>& Coefs;
        };

        class VariableBoundsFunctor : public Functor {
        public:
            VariableBoundsFunctor(const VariableRef* v, std::vector< std::vector< boost::shared_ptr<MP::Coef> > >& cfs): var(v), Coefs(cfs) {}
            void operator()() const;
        private:
            //Disable copy contructor and assignment operator
            VariableBoundsFunctor(const VariableBoundsFunctor&);
            VariableBoundsFunctor& operator=(const VariableBoundsFunctor&);


            const VariableRef* var;
            std::vector<std::vector< boost::shared_ptr<MP::Coef> > >& Coefs;
        };
    };


    /** @brief The base class for all expressions.
    @ingroup INTERNAL_USE
    @note FOR INTERNAL USE: This is not normally used directly by the
    calling code.
    */

    class MP_expression_base {
        friend class MP_expression;
        friend class Handle<MP_expression_base*>;
    private:
        int count;
    public:
        MP_expression_base() : count(0) {}

        // Return values
        virtual double level() const = 0; 

        // Generate coefficients for current expression
        virtual void generate(const MP_domain& domain,
            vector<TerminalExpression*> multiplicators,
            MP::GenerateFunctor &f,
            double m) const = 0;

        // Insert variables available in this expression in v
        virtual void insertVariables(set<MP_variable *>& v) const = 0; //Insert given Variables to the Model?!

        // Insert random variables available in this expression in v
        virtual void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const = 0;

        // Destruct this MP_expression_base, virtual due to virtual inheritance hierarchy with polymorphism..
        virtual ~MP_expression_base() {}
    };


    /** @brief Symbolic representation of a linear expression.
    @ingroup PublicInterface
    This is one of the main public interface classes.  It is the basis for   
    all linear expressions, including constraints, objective function,
    and expressions involving indexes.
    <br> Although these can be created directly and independently, it
    is expected these will be created through the use of the operators
    which are later in this file.  (operator+, operator-, etc.)
    @note There are constructors which are (silently) used to convert \
    other componenets into expressions.
    */

    class MP_expression : public Handle<MP_expression_base*> {
    public:
        MP_expression() : Handle<MP_expression_base*>(0) {}
        /// Constructor which (silently) converts a Constant to a MP_expression
        MP_expression(const Constant& c);
        /// Constructor which (silently) converts a VariableRef to a MP_expression
        MP_expression(const VariableRef& v);
        /// Constructor which (silently) converts a RandomDataRef to a MP_expression
        MP_expression(const RandomConstant &r);
        MP_expression(MP_expression_base* r);
        
    };

    /** @brief The base class for all terminal expressions, i.e. expressions that lead to a certain value for a given column/row.
    @ingroup INTERNAL_USE
    @note FOR INTERNAL USE: This is not normally used directly by the
    calling code.
    @todo can this be moved to the cpp file?
    */
    class TerminalExpression : public MP_expression_base {
    public:
        virtual ~TerminalExpression() {};

        virtual double getValue(int scenario = getZero) const = 0; 
        virtual int getColumn() const = 0;
        virtual int getStage() const = 0;
    };

    //class Expression_random : public TerminalExpression {
    //public:
    //    virtual ~Expression_random() {};

    //    // Return value of this expression for current random variable
    //    virtual double getValue(int scenario) const; 
    //    // Return column to which this expression belongs
    //    virtual int getColumn() const;
    //    // Return stage to which this expression belongs
    //    virtual int getStage() const;
    //    // Return one of the random variable present in this random expression
    //    void generate(const MP_domain& domain,
    //        vector<TerminalExpression *> multiplicators,
    //        MP::GenerateFunctor& f,
    //        double m) const;

    //};

    /** Semantic representation of a variable in a Math Program
    @ingroup INTERNAL_USE
    @see MP_variable for a public interface.
    */
    class VariableRef : public TerminalExpression {
        friend class MP_variable;
    public:

        virtual ~VariableRef();

        virtual int getColumn() const;
        double level() const;

        void insertVariables(set<MP_variable*>& v) const {
            v.insert(V);
        }

        void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const;

        virtual double getValue(int scenario) const;
        virtual int getStage() const;

    private:
        VariableRef(MP_variable* v, 
            const MP_index_exp& i1,
            const MP_index_exp& i2,
            const MP_index_exp& i3,
            const MP_index_exp& i4,
            const MP_index_exp& i5);

        //VariableRef& operator=(const &VariableRef); Why is there a compiler warning about assignment operator can not be generated? due to the private constructor?

        //Disable copy constructor and assignment operator ? 
        VariableRef(const VariableRef&);
        VariableRef& operator=(const VariableRef&);

        void generate(const MP_domain& domain,
            vector<TerminalExpression *> multiplicators,
            MP::GenerateFunctor& f,
            double m) const;

    public:
        MP_variable* V;
    private:
        int offset;
        const MP_index_exp I1,I2,I3,I4,I5;
    };

    /// @ingroup PublicInterface
    MP_expression operator+(const MP_expression& e1, const MP_expression& e2);
    /// @ingroup PublicInterface
    MP_expression operator+(const MP_expression& e1, const RandomConstant& e2);
    /// @ingroup PublicInterface
    MP_expression operator+(const RandomConstant& e1, const MP_expression& e2);
    /// @ingroup PublicInterface
    MP_expression operator+(const MP_expression& e1, const Constant& e2);
    /// @ingroup PublicInterface
    MP_expression operator+(const Constant& e1, const MP_expression& e2);
    /// @ingroup PublicInterface
    MP_expression operator-(const MP_expression& e1, const MP_expression& e2);
    /// @ingroup PublicInterface
    MP_expression operator-(const MP_expression& e1, const RandomConstant& e2);
    /// @ingroup PublicInterface
    MP_expression operator-(const RandomConstant& e1, const MP_expression& e2);
    /// @ingroup PublicInterface
    MP_expression operator-(const MP_expression& e1, const Constant& e2);
    /// @ingroup PublicInterface
    MP_expression operator-(const Constant& e1, const MP_expression& e2);
    /// @ingroup PublicInterface
    MP_expression operator*(const RandomConstant& e1, const MP_expression& e2); 
    /// @ingroup PublicInterface
    MP_expression operator*(const MP_expression& e1, const RandomConstant& e2);
    /// @ingroup PublicInterface
    MP_expression operator*(const Constant& e1, const MP_expression& e2); 
    /// @ingroup PublicInterface
    MP_expression operator*(const MP_expression& e1, const Constant& e2);
    /// @ingroup PublicInterface
    //MP_expression operator*(const RandomDataRef& e1, const MP_expression& e2); 
    ///// @ingroup PublicInterface
    //MP_expression operator*(const MP_expression& e1, const RandomDataRef& e2); 
    /// @ingroup PublicInterface
    MP_expression operator/(const MP_expression& e1, const RandomConstant& e2);
    /// @ingroup PublicInterface
    MP_expression operator/(const MP_expression& e1, const Constant& e2);
    /// @ingroup PublicInterface
    //MP_expression operator/(const RandomDataRef& e1, const MP_expression& e2);
    ///// @ingroup PublicInterface
    //MP_expression operator/(const MP_expression& e1, const RandomDataRef& e2);
    /// @ingroup PublicInterface
    MP_expression sum(const MP_domain& d, const MP_expression& e);

} // End of namespace flopc
#endif
