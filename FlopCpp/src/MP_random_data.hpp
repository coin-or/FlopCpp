#ifndef _MP_random_data_hpp_
#define _MP_random_data_hpp_

#include <vector>
#include <map>

#include <boost/math/distributions.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/utility.hpp>

#include "MP_expression.hpp"
#include "MP_data.hpp"
#include "MP_index.hpp"
#include "MP_set.hpp"
#include "MP_domain.hpp"
#include "MP_utilities.hpp"

//TODO: Define CustomRandomVariable that takes a function void operator()() and is able to generate one sample after another..

namespace flopc{



    //Forward declaration
    class MP_random_data_impl;
    class RandomDataRef;
    class RandomVariable;


    // This class is a virtual base class
    class DiscretizationMethod {
    public:

        DiscretizationMethod(int samples);
        virtual ~DiscretizationMethod();

        void setRandomVariable(RandomVariable* ptr);
        virtual void generateDiscreteDistribution() = 0;

        int getSampleNumber() const { return samples; }
        void setSampleNumber(int val) { samples = val; }

    protected:
        int samples;

        RandomVariable* rvarPtr;
    };

    // A random variable that has no discretization method can use the double discretization method as the method of choice.
    // This means that the natural discretization applied by a computer is used. No defaultSampleSize 
    class DoubleDiscretization : public DiscretizationMethod {

    public:
        DoubleDiscretization(int samples);

        void generateDiscreteDistribution();
    };

    // Class that discretizes 
    //class EmpiricalDiscretization : public DiscretizationMethod {

    //public:
    //    EmpiricalDiscretization(int samples);
    //    void generateDiscreteDistribution();
    //};

    // BracketMeanDiscretization divides distribution in equal probable intervals and assigns probability of interval to the mean value of the interval.
    // This value can be computed via the quantile of the mean probability of that interval.
    class BracketMeanDiscretization : public DiscretizationMethod {

    public:
        explicit BracketMeanDiscretization(int samples);
        void generateDiscreteDistribution();
    };

    // BracketMedianDiscretization divides distribution in equal probable intervals and assigns probability of interval to the median value of the interval.
    class BracketMedianDiscretization : public DiscretizationMethod {

    public:
        explicit BracketMedianDiscretization(int samples);
        void generateDiscreteDistribution();
    };

    class ExtendedPearsonTukeyDiscretization : public DiscretizationMethod {
    public:
        ExtendedPearsonTukeyDiscretization();
        void generateDiscreteDistribution();
    };

    //Class that defines a concrete RandomVariable 
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
        void setScenarioValues(const std::vector<double>&);
        void setScenarioValuesWithProb(const std::map<double,double>&);
        double getSampledValue(int index) const;

        virtual double getMeanValue() const = 0; //Usually we want mean of distribution. Sample mean only for scenario/empirical distributions.
        virtual double pdf(double x) const = 0;
        virtual double cdf(double x) const = 0;
        virtual double quantile(double p) const = 0;
        //virtual double complementcdf(double x) const = 0;

        int getStage() const { return stage; }
        void setStage(int val) { stage = val; }
        int SampleSize() const { return defaultSampleSize; }
        void SampleSize(int val) { defaultSampleSize = val; }
        bool isRandomVariable() const { return initialized; }
        int Seed() const { return seed; }
        void Seed(int val) { seed = val; }
        boost::shared_ptr<DiscretizationMethod> getDiscretizationMethod() const { return dm; }
        void setDiscretizationMethod(boost::shared_ptr<DiscretizationMethod> val) { dm = val; dm->setRandomVariable(this); }
        sampleVector getDiscreteDistribution() { return discreteDistribution;  }
        void setDiscreteDistribution(const sampleVector& map) { discreteDistribution = map; }



    protected:

        //Boost::random number generator? Needs seed
        bool initialized;
        unsigned int seed;

        //We only need one way to store the values. Speed vs. Storage and what methods are called should determine which way is better.
        //We start with the simple solution: vector of values
        std::vector<double> values; //All values are equally probable
        std::map<double,double> valuesMap; //Value with it's probability. One can get the number of occurences by : prob/1 (and round that)
        int defaultSampleSize;
        boost::shared_ptr<DiscretizationMethod> dm;
        sampleVector discreteDistribution;



    private:
        int stage; //Defaults to 0. If this is the case when this variable should get used, we have an error.
        //flopc::RandomVariable& operator=(const flopc::RandomVariable &);

    };






    class DiscreteRandomVariable : public RandomVariable {

    public:
        DiscreteRandomVariable();
        ~DiscreteRandomVariable();
    };

    class DiscreteUniformRandomVariable : public DiscreteRandomVariable {
    public :
        DiscreteUniformRandomVariable(int from = 0, int to = 1, int sampleSize = 2, DiscretizationMethod* dmPtr = new DoubleDiscretization(2));
        ~DiscreteUniformRandomVariable();

        virtual void sample(int howMany = 0);
        virtual double getMeanValue() const;
        virtual double pdf(double x) const;
        virtual double cdf(double x) const;
        virtual double quantile(double p) const;

    private:
        //Params for DiscreteUniformDistribution: [start,end] in steps of step
        int start;
        int end;
        double step;
        boost::math::uniform distribution;


    };

    class DiscreteBernoulliRandomVariable : public DiscreteRandomVariable {
    public :
        DiscreteBernoulliRandomVariable(double probability);
        ~DiscreteBernoulliRandomVariable();

        virtual void sample(int howMany = 0);
        virtual double getMeanValue() const;
        virtual double pdf(double x) const;
        virtual double cdf(double x) const;
        virtual double quantile(double p) const;

    private:
        double probability;
        boost::math::bernoulli distribution;

    };

    // Shifted geometric Distribution
    class DiscreteGeometricRandomVariable : public DiscreteRandomVariable {
    public :
        DiscreteGeometricRandomVariable(double probability);
        ~DiscreteGeometricRandomVariable();

        virtual void sample(int howMany = 0);
        virtual double getMeanValue() const;
        virtual double pdf(double x) const;
        virtual double cdf(double x) const;
        virtual double quantile(double p) const;

    private:
        double probability;
        boost::math::bernoulli distribution;

    };

    class ScenarioRandomVariable : public DiscreteRandomVariable{

    public:
        ScenarioRandomVariable(double* scenValues, int length);
        ScenarioRandomVariable();

        ~ScenarioRandomVariable();
        virtual void sample(int howMany = 0) ;
        virtual double getMeanValue() const;
        virtual double pdf(double x) const;
        virtual double cdf(double x) const;
        virtual double quantile(double p) const;
    };

    class ContinuousRandomVariable : public RandomVariable {
    public:

        ContinuousRandomVariable();
        ~ContinuousRandomVariable();
    protected:
    };

    class ContinuousUniformRandomVariable : public ContinuousRandomVariable {
    public:
        ContinuousUniformRandomVariable(int from, int to, DiscretizationMethod* dmPtr);
        ~ContinuousUniformRandomVariable();

        virtual void sample(int howMany = 0) ;
        virtual double getMeanValue() const;
        virtual double pdf(double x) const;
        virtual double cdf(double x) const;
        virtual double quantile(double p) const;

    private:
        int start;
        int end;
        boost::math::uniform distribution;
    };

    class ContinuousLogNormalRandomVariable : public ContinuousRandomVariable {
    public:
        ContinuousLogNormalRandomVariable(double mean, double sigma, DiscretizationMethod* dmPtr);
        ~ContinuousLogNormalRandomVariable();

        virtual void sample(int howMany = 0) ;
        virtual double getMeanValue() const;
        virtual double pdf(double x) const;
        virtual double cdf(double x) const;
        virtual double quantile(double p) const;

    private:
        double mean;
        double sigma;
        boost::math::lognormal distribution;
    };

    class ContinuousNormalRandomVariable : public ContinuousRandomVariable {
    public:
        ContinuousNormalRandomVariable(double mean, double sigma, DiscretizationMethod* dmPtr);
        ~ContinuousNormalRandomVariable();

        virtual void sample(int howMany = 0) ;
        virtual double getMeanValue() const;
        virtual double pdf(double x) const;
        virtual double cdf(double x) const;
        virtual double quantile(double p) const;

    private:
        double mean;
        double sigma;
        boost::math::normal distribution;
    };

    class ContinuousExponentialRandomVariable : public ContinuousRandomVariable {
    public:
        ContinuousExponentialRandomVariable(double lambda, DiscretizationMethod* dmPtr);
        ~ContinuousExponentialRandomVariable();

        virtual void sample(int howMany = 0) ;
        virtual double getMeanValue() const;
        virtual double pdf(double x) const;
        virtual double cdf(double x) const;
        virtual double quantile(double p) const;

    private:
        double lambda;
        boost::math::exponential distribution;
    };

    class ContinuousTriangleRandomVariable : public ContinuousRandomVariable {
    public:
        ContinuousTriangleRandomVariable(double a, double b, double c, DiscretizationMethod* dmPtr);
        ~ContinuousTriangleRandomVariable();

        virtual void sample(int howMany = 0) ;
        virtual double getMeanValue() const;
        virtual double pdf(double x) const;
        virtual double cdf(double x) const;
        virtual double quantile(double p) const;

    private:
        double a;
        double b;
        double c;
        boost::math::triangular distribution;
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

        /// For displaying data in a human readable format.
        void display(const std::string& s = "", int scenario = 0); //TODO: Eventually we can remove scenario

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
