#ifndef _MP_random_data_hpp_
#define _MP_random_data_hpp_

#include <vector>
#include <set>
#include <map>

#include <boost/shared_ptr.hpp>
#include <CoinSmartPtr.hpp>
#include <boost/utility.hpp>

#include "MP_data.hpp"
#include "MP_index.hpp"
#include "MP_set.hpp"
#include "MP_utilities.hpp"

//TODO: Define CustomRandomVariable that takes a function void operator()() and is able to generate one sample after another..


namespace flopc{

    //Forward declaration
    class MP_random_data_impl;

    class RandomDataRef : public Constant_base,public Functor, public Coin::ReferencedObject { //Are we really a constant_base object?
        friend class MP_random_data_impl;
    public:

        RandomDataRef(boost::shared_ptr<MP_random_data_impl> rv, 
            const MP_index_exp& i1,
            const MP_index_exp& i2,
            const MP_index_exp& i3,
            const MP_index_exp& i4,
            const MP_index_exp& i5);


        ~RandomDataRef() {} 
        RandomDataRef& such_that(const MP_boolean& b);

        //Get Value via current settings for MP_index_exp
        virtual double evaluate(int scenario) const;
        virtual int getStage() const;
        virtual void propagateIndexExpression(const MP_index_exp& i1,const MP_index_exp& i2,const MP_index_exp& i3,const MP_index_exp& i4,const MP_index_exp& i5) const;
        virtual void insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const;
        virtual RandomVariable* getRandomVariable(int index = outOfBound) const;
        IndexKeeper getIndices(int k) const;

        //Assignment-operators (Not yet defined..)
        const RandomDataRef& operator=(const RandomDataRef& r); 
        const RandomDataRef& operator=(const Constant& c);

        //Copy Constructor (needed in initialization of Constant_random_variable
        RandomDataRef(const RandomDataRef&);

        //Set value via current MP_index_exp 
        //void evaluate_lhs(const RandomVariable &v) const;
        virtual void operator()() const;


    private:
        boost::shared_ptr<MP_random_data_impl> RV;
        mutable MP_index_exp I1,I2,I3,I4,I5;
        Constant C;
        MP_boolean B;
    };
    

    //Class that defines a concrete RandomVariable for one coefficient
    class RandomVariable : boost::noncopyable {
        friend class RandomDataRef;
    public:

        typedef std::map<double,double> sampleVector;
        typedef std::vector< std::vector< RandomVariable* > > randVarVector;

        RandomVariable();
        virtual ~RandomVariable();

        //Samples a number of random variates according to the distribution of the RandomVariable
        // It also sorts the variates in ascending order
        virtual void sample(int howMany = 0) = 0;
        std::map<double,double> getSampledValuesWithProb();
        std::vector<double> getSampledValues() const;
        double getSampledValue(int index) const;
        void setSampledValues(const std::vector<double>&);
        virtual double getMeanValue() const; //Do we want the mean value of the distribution or the mean value of the sampled values?

        int Stage() const { return stage; }
        void Stage(int val) { stage = val; }
        int ColIndex() const { return colIndex; }
        void ColIndex(int val) { colIndex = val; }
        int RowIndex() const { return rowIndex; }
        void RowIndex(int val) { rowIndex = val; }
        int SampleSize() const { return defaultSampleSize; }
        void SampleSize(int val) { defaultSampleSize = val; }
        bool isRandomVariable() const { return initialized; }
        int Seed() const { return seed; }
        void Seed(int val) { seed = val; }
    protected:

        //Boost::random number generator? Needs seed
        bool initialized;
        unsigned int seed;

        //We only need one way to store the values. Speed vs. Storage and what methods are called should determine which way is better.
        //We start with the simple solution: vector of values
        std::vector<double> values; //All values are equally probable
        std::map<double,double> valuesMap; //Value with it's probability. One can get the number of occurences by : prob/1 (and round that)
        int defaultSampleSize;

    private:
        int stage; //Defaults to 0. If this is the case when this variable should get used, we have an error.
        int colIndex;
        int rowIndex;
        

        //flopc::RandomVariable& operator=(const flopc::RandomVariable &);

    };

    class DiscreteRandomVariable : public RandomVariable {
        
    public:
        DiscreteRandomVariable();
        ~DiscreteRandomVariable();

        virtual void sample(int howMany = 0) = 0;

    };

    class DiscreteUniformRandomVariable : public DiscreteRandomVariable {
    public :
        DiscreteUniformRandomVariable(int from = 0, int to = 1, double stepWidth = 1, int sampleSize = 1);
        ~DiscreteUniformRandomVariable();

        virtual void sample(int howMany = 0);

    private:
        //Params for DiscreteUniformDistribution: [start,end] in steps of step
        int start;
        int end;
        double step;

    };
    
    class DiscreteBernoulliRandomVariable : public DiscreteRandomVariable {
    public :
        DiscreteBernoulliRandomVariable(double probability);
        ~DiscreteBernoulliRandomVariable();

        virtual void sample(int howMany = 0);

    private:
        double probability;

    };
    
    class DiscreteGeometricRandomVariable : public DiscreteRandomVariable {
    public :
        DiscreteGeometricRandomVariable(double probability);
        ~DiscreteGeometricRandomVariable();

        virtual void sample(int howMany = 0);

    private:
        double probability;

    };

    class ScenarioRandomVariable : public DiscreteRandomVariable{

    public:
        ScenarioRandomVariable(double* scenValues, int length);
        ScenarioRandomVariable();

        ~ScenarioRandomVariable();
        virtual void sample(int howMany = 0) ;
        virtual double getMeanValue();
    };

    class ContinuousRandomVariable : public RandomVariable {
    public:

        ContinuousRandomVariable();
        ~ContinuousRandomVariable();

        virtual void sample(int howMany = 0) = 0;

    };

    class ContinuousUniformRandomVariable : public ContinuousRandomVariable {
    public:
        ContinuousUniformRandomVariable(int from, int to);
        ~ContinuousUniformRandomVariable();

        virtual void sample(int howMany = 0) ;

    private:
        int start;
        int end;
    };

    class ContinuousLogNormalRandomVariable : public ContinuousRandomVariable {
    public:
        ContinuousLogNormalRandomVariable(double mean, double sigma);
        ~ContinuousLogNormalRandomVariable();

        virtual void sample(int howMany = 0) ;

    private:
        double mean;
        double sigma;
    };

    class ContinuousNormalRandomVariable : public ContinuousRandomVariable {
    public:
        ContinuousNormalRandomVariable(double mean, double sigma);
        ~ContinuousNormalRandomVariable();

        virtual void sample(int howMany = 0) ;

    private:
        double mean;
        double sigma;
    };

    class ContinuousExponentialRandomVariable : public ContinuousRandomVariable {
    public:
        ContinuousExponentialRandomVariable(double lambda);
        ~ContinuousExponentialRandomVariable();

        virtual void sample(int howMany = 0) ;

    private:
        double lambda;
    };
    
    class ContinuousTriangleRandomVariable : public ContinuousRandomVariable {
    public:
        ContinuousTriangleRandomVariable(double a, double b, double c);
        ~ContinuousTriangleRandomVariable();

        virtual void sample(int howMany = 0) ;

    private:
        double a;
        double b;
        double c;
    };

    //Class that defines indexable random_variables. Contains concrete RandomVariables.
    class MP_random_data : public Functor, public Named //, public boost::enable_shared_from_this //We want to allow names, computation of flattened arrays and functors
    {
    public:

        MP_random_data(const MP_set_base &s1 = MP_set::getEmpty(), 
            const MP_set_base &s2 = MP_set::getEmpty(), 
            const MP_set_base &s3 = MP_set::getEmpty(),
            const MP_set_base &s4 = MP_set::getEmpty(), 
            const MP_set_base &s5 = MP_set::getEmpty());

        MP_random_data(RandomVariable** array, const MP_set_base &s1 = MP_set::getEmpty(), 
            const MP_set_base &s2 = MP_set::getEmpty(), 
            const MP_set_base &s3 = MP_set::getEmpty(),
            const MP_set_base &s4 = MP_set::getEmpty(), 
            const MP_set_base &s5 = MP_set::getEmpty());

        MP_random_data(double* array, const MP_set_base &s1 = MP_set::getEmpty(), 
            const MP_set_base &s2 = MP_set::getEmpty(), 
            const MP_set_base &s3 = MP_set::getEmpty(),
            const MP_set_base &s4 = MP_set::getEmpty(), 
            const MP_set_base &s5 = MP_set::getEmpty());

        virtual void operator()() const;

        //Needs virtual assignment operators hierarchy that might look ugly
        RandomVariable*& operator()(int lcli1, int lcli2=0, int lcli3=0, int lcli4=0, int lcli5=0);

        RandomDataRef& operator()( 
            const MP_index_exp& d1 = MP_index_exp::getEmpty(), 
            const MP_index_exp& d2 = MP_index_exp::getEmpty(), 
            const MP_index_exp& d3 = MP_index_exp::getEmpty(),
            const MP_index_exp& d4 = MP_index_exp::getEmpty(), 
            const MP_index_exp& d5 = MP_index_exp::getEmpty()
            );

    protected:

    private:
        //TODO: Think about copying behaviour?
        MP_random_data(const MP_random_data& rhs); //forbid copy constructor
        MP_random_data& operator=(const MP_random_data& rhs); //forbid assignment operator

        boost::shared_ptr<MP_random_data_impl> ptrImpl; //TODO: We can change this to CoinSharedPointer?!
        


    };

    

    //non-member non-friend functions
    std::ostream &operator<<( std::ostream &out, const RandomVariable& rhs ) ;

}

#endif
