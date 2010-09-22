#include <iosfwd>
#include <exception>
#include <stdexcept>
#include <vector>
#include <set>

#include <boost/random.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "MP_random_data.hpp"
#include "MP_expression.hpp"
#include "MP_random_constant.hpp"
#include "MP_model.hpp"
#include "MP_shared_implementation_details.hpp"


using namespace boost::math;

namespace flopc{

    class RandomDataRef;

    //Class that stores a description of a random variable. Still needs to be defined
    //TODO: Do that
    class MP_random_data_impl : public RowMajor, public Functor , private Named{
        friend class RandomDataRef;

    public:

        using Named::getName;
        using Named::setName;

        //If we use this constructor, we need to delete values in the end.
        // This is only necessary if we actually add random_variables to this MP_random_data
        MP_random_data_impl(const MP_set_base &s1, const MP_set_base &s2, const MP_set_base &s3,
            const MP_set_base &s4, const MP_set_base &s5):
        RowMajor(s1.size(),s2.size(),s3.size(),s4.size(),s5.size()),
            S1(s1),S2(s2),S3(s3),S4(s4),S5(s5),c(0),values(new RandomVariable*[size()]),myrefs(),manageData(true),stage(-1) { 
                // Identify stage set.
                findStageSet();
                initialize();
        }; 

        //Do a shallow copy (leave values in original arrays). This means we do not want to delete values in the end
        MP_random_data_impl(RandomVariable** array, const MP_set_base &s1, const MP_set_base &s2,                const MP_set_base &s3, const MP_set_base &s4, const MP_set_base &s5):
        RowMajor(s1.size(),s2.size(),s3.size(),s4.size(),s5.size()),
            S1(s1),S2(s2),S3(s3),S4(s4),S5(s5),i1(const_cast<MP_set_base&>(s1)),i2(const_cast<MP_set_base&>(s2)),i3(const_cast<MP_set_base&>(s3)),i4(const_cast<MP_set_base&>(s4)),i5(const_cast<MP_set_base&>(s5)),c(0),values(new RandomVariable*[size()]),myrefs(),manageData(true),stage(-1) {  //Consistency check?
                initialize();
                // Very ugly code..
                // What is done here: Arrays stage index is one less than normal stage index because first stage needs no RV. 
                int j1i,j2i,j3i,j4i,j5i;
                j1i =j2i = j3i = j4i = j5i = 0;
                findStageSet();
                switch (stage)
                {
                case 0: j1i = 1; break;
                case 1: j2i = 1; break;
                case 2: j3i = 1; break;
                case 3: j4i = 1; break;
                case 4: j5i = 1; break;
                default : throw invalid_argument_exception();

                }
                for (int j1 = j1i; j1 < S1.size();j1++)
                    for (int j2 = j2i; j2 < S2.size(); j2++)
                        for (int j3 = j3i; j3 < S3.size(); j3++)
                            for (int j4 = j4i; j4 < S4.size();j4++)
                                for (int j5 = j5i; j5 < S5.size();j5++){
                                    if (!(j1 < j1i || j2 < j2i || j3 < j3i || j4 < j4i || j5 < j5i)){ //In case of not first stage: assign value
                                        values[f(j1,j2,j3,j4,j5)] = array[f(j1-j1i,j2-j2i,j3-j3i,j4-j4i,j5-j5i)];
                                        // Set current stage to random variable
                                        if (j1i+j2i+j3i+j4i+j5i == 1){
                                            int stage = 0;
                                            if (S1.isStage())
                                                stage = j1;
                                            else if (S2.isStage())
                                                stage = j2;
                                            else if (S3.isStage())
                                                stage = j3;
                                            else if (S4.isStage())
                                                stage = j4;
                                            else if (S5.isStage())
                                                stage = j5;
                                            values[f(j1,j2,j3,j4,j5)]->setStage(stage); //FIXME: Stage does not get set via direct assignment. This has to be done also.
                                        }
                                        else
                                            values[f(j1,j2,j3,j4,j5)]->setStage(1); //Otherwise we may have a two stage problem: set to 1
                                    }
                                }

                                //else throw invalid_argument_exception();
        }; 

        //Do a shallow copy (leave values in original arrays). This means we do want to delete the created RandomVariables
        MP_random_data_impl(double* array, const MP_set_base &s1, const MP_set_base &s2, const                   MP_set_base &s3, const MP_set_base &s4, const MP_set_base &s5):
        RowMajor(s1.size(),s2.size(),s3.size(),s4.size(),s5.size()),
            S1(s1),S2(s2),S3(s3),S4(s4),S5(s5),c(0),values(new RandomVariable*[size()]),myrefs(),manageData(true),stage(-1) {  //Consistency check?
                initialize();
                int scen = MP_model::getCurrentModel()->getScenSet().size();
                //Identify stage set: The set with getStage > 0 is the stage set.
                //If there is no stage set: No Problem ;)
                int j1i,j2i,j3i,j4i,j5i;
                j1i =j2i = j3i = j4i = j5i = 0;
                findStageSet();
                switch (stage)
                {
                case 0: j1i = 1; break;
                case 1: j2i = 1; break;
                case 2: j3i = 1; break;
                case 3: j4i = 1; break;
                case 4: j5i = 1; break;
                default : throw invalid_argument_exception();

                }
                for (int j1 = j1i; j1 < S1.size();j1++)
                    for (int j2 = j2i; j2 < S2.size(); j2++)
                        for (int j3 = j3i; j3 < S3.size(); j3++)
                            for (int j4 = j4i; j4 < S4.size();j4++)
                                for (int j5 = j5i; j5 < S5.size();j5++){
                                    if (!(j1 < j1i || j2 < j2i || j3 < j3i || j4 < j4i || j5 < j5i)) {//In case of not first stage
                                        createRandomVariableFromIndices(array,f(j1-j1i,j2-j2i,j3-j3i,j4-j4i,j5-j5i),f(j1,j2,j3,j4,j5),scen);
                                        // Set current stage to random variable
                                        if (j1i+j2i+j3i+j4i+j5i == 1){
                                            int stage = 0;
                                            if (S1.isStage())
                                                stage = j1;
                                            else if (S2.isStage())
                                                stage = j2;
                                            else if (S3.isStage())
                                                stage = j3;
                                            else if (S4.isStage())
                                                stage = j4;
                                            else if (S5.isStage())
                                                stage = j5;
                                            values[f(j1,j2,j3,j4,j5)]->setStage(stage);
                                        }
                                        else
                                            values[f(j1,j2,j3,j4,j5)]->setStage(1); //Otherwise we may have a two stage problem: set to 1
                                    }

                                }

                                //else throw invalid_argument_exception(); //More like: You added RV without stage set, do you really want to this?
        };

        ~MP_random_data_impl(){
            if(manageData){//Iterate through array and delete pointer one-by-one.
                for (int i = 0; i < size(); i++)
                    delete values[i];
                delete [] values;
            }
        };

        //initialize data members 
        void initialize(); 

        virtual void operator()() const;

        RandomVariable*& operator()(int lcli1, int lcli2=0, int lcli3=0, int lcli4=0, int lcli5=0)  { 
            lcli1 = S1.check(lcli1); 
            lcli2 = S2.check(lcli2); 
            lcli3 = S3.check(lcli3); 
            lcli4 = S4.check(lcli4); 
            lcli5 = S5.check(lcli5); 
            int i = f(lcli1,lcli2,lcli3,lcli4,lcli5);     
            if (i == outOfBound) {
                throw std::out_of_range("Index is not valid"); //Nothrow guarantee?
            } else {
                //TODO: Change to reference and copy instead of copying pointers..
                //switch(stage){
                //    case 0: values[i]->Stage(lcli1); break;
                //    case 1: values[i]->Stage(lcli2); break;
                //    case 2: values[i]->Stage(lcli3); break;
                //    case 3: values[i]->Stage(lcli4); break;
                //    case 4: values[i]->Stage(lcli5); break;
                //}
                return values[i];
            } //We need to set the stage of the RV afterwards, somehow.. 
        }

        RandomDataRef& operator()(
            const MP_index_exp& d1, 
            const MP_index_exp& d2, 
            const MP_index_exp& d3,
            const MP_index_exp& d4, 
            const MP_index_exp& d5) {
                if (MP_model::getCurrentModel()->checkSemantic() ){
                    LOG_IF(WARNING,S1.getIndex() != &MP_set::getEmpty() && d1->getIndex() != S1.getIndex() && d1->getDomain(&MP_set::getEmpty()) != MP_domain::getEmpty()) << "First index given to MP_random_data with name " << this->getName() << " does not correspond to the defined index. This may lead to subtle errors. Continue only if you know what you are doing.";
                    LOG_IF(WARNING,S2.getIndex() != &MP_set::getEmpty() && d2->getIndex() != S2.getIndex() && d1->getDomain(&MP_set::getEmpty()) != MP_domain::getEmpty()) << "Second index given to MP_random_data with name " << this->getName() << " does not correspond to the defined index. This may lead to subtle errors. Continue only if you know what you are doing.";
                    LOG_IF(WARNING,S3.getIndex() != &MP_set::getEmpty() && d3->getIndex() != S3.getIndex() && d1->getDomain(&MP_set::getEmpty()) != MP_domain::getEmpty()) << "Third index given to MP_random_data with name " << this->getName() << " does not correspond to the defined index. This may lead to subtle errors. Continue only if you know what you are doing.";
                    LOG_IF(WARNING,S4.getIndex() != &MP_set::getEmpty() && d4->getIndex() != S4.getIndex() && d1->getDomain(&MP_set::getEmpty()) != MP_domain::getEmpty()) << "Fourth index given to MP_random_data with name " << this->getName() << " does not correspond to the defined index. This may lead to subtle errors. Continue only if you know what you are doing.";
                    LOG_IF(WARNING,S5.getIndex() != &MP_set::getEmpty() && d5->getIndex() != S5.getIndex() && d1->getDomain(&MP_set::getEmpty()) != MP_domain::getEmpty()) << "Fifth index given to MP_random_data with name " << this->getName() << " does not correspond to the defined index. This may lead to subtle errors. Continue only if you know what you are doing.";

                }
                RandomDataRef* tempPtr = new RandomDataRef(this, d1, d2, d3, d4, d5);
                myrefs.push_back(RandomConstant(tempPtr));
                return *tempPtr; 
        }

        void display(const std::string& s, int scenario);

    private:
        void createRandomVariableFromIndices(double* arr,int indexForArr,int indexForValues, int scen){
            double *tempArray;
            tempArray = new double[scen]; //new/delete
            for (int i = 0; i < scen; i++){
                tempArray[i] = arr[indexForArr*scen+i];
            }
            values[indexForValues] = new ScenarioRandomVariable(tempArray,scen);
            delete [] tempArray;
        }

        void findStageSet(){
            if (S1.isStage())
                stage = 0;
            else if (S2.isStage())
                stage = 1;
            else if (S3.isStage())
                stage = 2;
            else if (S4.isStage())
                stage = 3;
            else if (S5.isStage())
                stage = 4;
            else //As long as we only support recourse formulations, we need to adress a stage to a RV
                throw invalid_argument_exception();
        }



        const MP_set_base &S1,&S2,&S3,&S4,&S5;
        mutable MP_index_exp i1,i2,i3,i4,i5;// MP_index. Used to iterate over the sets in the display method
        RandomConstant c;
        //boost::ptr_vector<RandomVariable> values;
        RandomVariable** values; //Array of pointers to RandomVariables : TODO: Redo this
        std::vector<RandomConstant> myrefs;
        bool manageData;
        int stage; // Can be one of 0 to 4
    };



    //Implementation for MP_random_data
    MP_random_data::MP_random_data( const MP_set_base &s1 /*= MP_set::getEmpty()*/, const MP_set_base &s2 /*= MP_set::getEmpty()*/, const MP_set_base &s3 /*= MP_set::getEmpty()*/, const MP_set_base &s4 /*= MP_set::getEmpty()*/, const MP_set_base &s5 /*= MP_set::getEmpty()*/ )
        : ptrImpl(new MP_random_data_impl(s1,s2,s3,s4,s5)){	}

    MP_random_data::MP_random_data( RandomVariable** array, const MP_set_base &s1 /*= MP_set::getEmpty()*/, const MP_set_base &s2 /*= MP_set::getEmpty()*/, const MP_set_base &s3 /*= MP_set::getEmpty()*/, const MP_set_base &s4 /*= MP_set::getEmpty()*/, const MP_set_base &s5 /*= MP_set::getEmpty()*/ )
        : ptrImpl(new MP_random_data_impl(array,s1,s2,s3,s4,s5)) {}

    MP_random_data::MP_random_data( double* array, const MP_set_base &s1 /*= MP_set::getEmpty()*/, const MP_set_base &s2 /*= MP_set::getEmpty()*/, const MP_set_base &s3 /*= MP_set::getEmpty()*/, const MP_set_base &s4 /*= MP_set::getEmpty()*/, const MP_set_base &s5 /*= MP_set::getEmpty()*/ )
        : ptrImpl(new MP_random_data_impl(array,s1,s2,s3,s4,s5))	{

    }
    RandomVariable*& MP_random_data::operator ()(int lcli1, int lcli2, int lcli3, int lcli4, int lcli5){
        return ptrImpl->operator()(lcli1,lcli2,lcli3,lcli4,lcli5);
    }

    RandomDataRef& MP_random_data::operator()( const MP_index_exp& d1 /*= MP_index_exp::getEmpty()*/, const MP_index_exp& d2 /*= MP_index_exp::getEmpty()*/, const MP_index_exp& d3 /*= MP_index_exp::getEmpty()*/, const MP_index_exp& d4 /*= MP_index_exp::getEmpty()*/, const MP_index_exp& d5 /*= MP_index_exp::getEmpty() */ )
    {
        return ptrImpl->operator()(d1,d2,d3,d4,d5);
    }

    void MP_random_data::operator()() const
    {

    }

    void MP_random_data::display( const std::string& s /*= ""*/, int scenario )
    {
        ptrImpl->display(s,scenario);
    }

    void MP_random_data::setName( const std::string& string)
    {
        ptrImpl->setName(string);
    }

    std::string MP_random_data::getName() const
    {
        return ptrImpl->getName();
    }
    //Implementation for MP_random_data_impl TODO: Store values to reduce computational burden
    void MP_random_data_impl::operator ()() const { //Called by display method indirectly via functor evaluation
        if (&S1!=&MP_set::getEmpty()) std::cout << i1->evaluate() << " ";
        if (&S2!=&MP_set::getEmpty()) std::cout << i2->evaluate() << " ";
        if (&S3!=&MP_set::getEmpty()) std::cout << i3->evaluate() << " ";
        if (&S4!=&MP_set::getEmpty()) std::cout << i4->evaluate() << " ";
        if (&S5!=&MP_set::getEmpty()) std::cout << i5->evaluate() << " ";
        if (values[f(i1->evaluate(),i2->evaluate(),i3->evaluate(),
            i4->evaluate(),i5->evaluate())] != NULL){
                for (int i = 0; i < MP_model::getCurrentModel()->getScenSet().size(); i++)
                    std::cout << " Sc." << i << ": " << values[f(i1->evaluate(),i2->evaluate(),i3->evaluate(), i4->evaluate(),i5->evaluate())]->getSampledValue(i) << " ";
        }
        else if (c.isDefined()){ // There is no variable at this point, so we can skip it.
            //c->propagateIndexExpressions(i1,i2,i3,i4,i5);
            for (int i = 0; i < MP_model::getCurrentModel()->getScenSet().size(); i++)
                std::cout << " Sc." << i << ": " << c->evaluate(i) << " "; 
        }
        std::cout << std::endl;
    }

    void MP_random_data_impl::display(const std::string& s,int scenario){
        std::cout << s << std::endl;
        //RandomDataRef& rdr = this->operator()(const_cast<MP_set_base&>(S1),const_cast<MP_set_base&>(S2),const_cast<MP_set_base&>(S3),const_cast<MP_set_base&>(S4),const_cast<MP_set_base&>(S5));
        //((S1)(i1)*(S2)(i2)*(S3)(i3)*(S4)(i4)*(S5)(i5)).forall(this);
        (S1(i1)*S2(i2)*S3(i3)*S4(i4)*S5(i5)).forall(this);
    }

    void MP_random_data_impl::initialize() //TODO: How to do that in a better way? Think about that..
    {
        for (int i = 0; i < size(); i++)
            values[i] = NULL;
    }


    //Implementation for RandomVariable: Think about these constructors.. are they necessary?
    RandomVariable::RandomVariable() : initialized(false),seed(0),values(),valuesMap(),defaultSampleSize(0),currentSampleSize(0),stage(0),dm(new DoubleDiscretization(0)),discreteDistribution() { } 

    // base_generator_type(seed),uniform_distr_gen(uniform.rng,boost::uniform_01<double>()) 



    //const RandomVariable& RandomVariable::operator=( const RandomVariable& rhs )
    //{
    //    if( this == &rhs) //handle assignment to self //item 12
    //        return *this;
    //    //another way: item 12 and item 29 : swap method needs to be implemented
    //    //RandomVariable temp(rhs);
    //    //swap(rhs);
    //    //TODO: Delete values from *this and copy values from rhs to this. Return *this.
    //    stage = rhs.stage;
    //    defaultSampleSize = rhs.defaultSampleSize;
    //    colIndex = rhs.colIndex;
    //    rowIndex = rhs.rowIndex;
    //    values = rhs.values;
    //    valuesMap = rhs.valuesMap;
    //    return *this; //No data values so far in RandomVariable, so we do not have anything to copy..
    //}

    RandomVariable::~RandomVariable()
    {

    }

    void RandomVariable::setScenarioValues(const std::vector<double>& v){
        values = v;
    }

    std::vector<double> RandomVariable::getSampledValues() const
    {
        if ( !values.empty() ) { // if we store values without probabilities, everything is fine. Order is important.
            return values;
        }
        if (values.empty() && !valuesMap.empty()){ // Return values computed with probabilities. We do not want that, probably TODO
            throw not_implemented_error();
        }
        return values; //might be empty, might not be empty..

    }

    std::map<double,double> RandomVariable::getSampledValuesWithProb()
    {
        //Create vector that stores pairs of value with number of occurences. HashSet is ideal to store that..
        if ( !valuesMap.empty() ){
            return valuesMap;
        }
        if ( !values.empty() ){
            if (defaultSampleSize == 0) //Compute Sample Size
                defaultSampleSize = values.size();

            //Count number of different elements and store them in the map
            for (int i = 0; i < values.size(); i++){
                valuesMap[values[i]] = valuesMap[values[i]] + 1.0;
            }
            //Normalize probabilities: Divide occurence counter by defaultSampleSize
            for ( sampleVector::iterator it = valuesMap.begin(); it != valuesMap.end(); ++it){
                (*it).second = (*it).second/defaultSampleSize;
            }
        }
        return valuesMap; //Might be empty or not.. we simply do not know
    }

    double RandomVariable::getSampledValue(int index) const
    {
        if (index != getZero && index != outOfBound && index < values.size())
        {
            return values[index];
        }
        else { // In case of outOfBound we return 0
            return std::numeric_limits<double>::infinity(); // Is this the correct behaviour?!
        }
    }

    double RandomVariable::getMeanValue() const
    {
        if (!values.empty()){
            //Compute mean of values and return it
            double mean = 0;
            for (int i = 0; i < values.size(); i++) {
                // What about probabilities? We assume equal probabilities
                mean += values[i];
            }
            return mean/values.size();
        }
        if (!valuesMap.empty()){
            //Compute mean of values and return it
            double mean = 0;
            for (std::map<double,double>::const_iterator it = valuesMap.begin(); it != valuesMap.end();++it) {
                mean += (*it).first*(*it).second;
            }
            return mean;            
        }
        return outOfBound; //In case nothing works, return outOfBound (at least core node has an entry..)
    }

    void RandomVariable::sample( int howMany /*= 0*/ )
    {
        //Initialization stuff common to all methods..
        if (howMany){ // If method was called with argument = 0, sample from defaultSampleSize. Else we sample howMany values.
            assert( howMany > 0);
            if ( defaultSampleSize != 0 && howMany > defaultSampleSize )// Why do we want to sample from a distribution with less entries?
                throw invalid_argument_exception();
            currentSampleSize = howMany;
        }
        else{
            currentSampleSize = defaultSampleSize;
        }
        // Generate discrete distribution 
        if (discreteDistribution.empty() || discreteDistribution.size() != defaultSampleSize){
            dm->generateDiscreteDistribution(); //This sets discreteDistribution.
        } 

        // We already have a discretized distribution available to us at this point (or a double discretization )
        if (MP_model::getCurrentModel()->getSample()){
            //We want to sample, so we need a fresh valuesMap
            valuesMap.clear();
            values.clear();
            values.reserve(howMany);

            DiscretizationMethod* dmPtr = dynamic_cast<DoubleDiscretization*> ( dm.get() ); //If we have a DoubleDiscretization we want to sample from the double discretized distribution. (The user wants it, maybe he does not know what he is doing)
            if (dmPtr == 0){
                assert( !discreteDistribution.empty() );
                // We have a discretized distribution and we want to sample from that one.
                // We have values together with probabilities. We want howMany samples.
                // Algorithm. Choose random number between 0 and 1. Look at entry in table. Return value.
                sampleVector probVector;
                double probSum = 0;
                for ( sampleVector::iterator it = discreteDistribution.begin(); it != discreteDistribution.end(); ++it) {
                    // We have to store our values in another std::map.
                    probSum += it->second;
                    probVector.insert(std::pair<double,double>( probSum ,it->first ));
                }
                // Setup RNG
                typedef boost::uniform_01<> distr_type;
                boost::variate_generator<uniform_distr_gen&,distr_type> variateGen(*MP_model::getCurrentModel()->getUniformGenerator()->uniform,distr_type());

                // Now we can call the RNG and sample the values
                for (int i = 0; i < currentSampleSize; i++) {
                    double randomNumber = variateGen();
                    sampleVector::iterator element = probVector.upper_bound(randomNumber-std::numeric_limits<double>::epsilon()); //Find first value strictly greater than that
                    values.push_back(element->second);
                }
            }
            else {
                //We have a successfull cast to a DoubleDiscretization.
                // Sample from specific random variable in own method, not here
            }


        } // We already have a discrete distribution and we do not want to sample. Set valuesMap to discreteDistribution
        else {
            valuesMap = discreteDistribution;
            values.clear();
            values.reserve(defaultSampleSize);
        }
    }

    void RandomVariable::setScenarioValuesWithProb( const std::map<double,double>& scenValues)
    {
        valuesMap = scenValues;
        //TODO: Check for probability 1
    }

    //flopc::RandomVariable& RandomVariable::operator=( const flopc::RandomVariable & )
    //{
    //    return *this;
    //}
    //Implementation for RandomDataRef.

    RandomDataRef::RandomDataRef(MP_random_data_impl * const rv, 
        const MP_index_exp& i1,
        const MP_index_exp& i2,
        const MP_index_exp& i3,
        const MP_index_exp& i4,
        const MP_index_exp& i5) :
    RV(rv),I1(i1),I2(i2),I3(i3),I4(i4),I5(i5),copy1(I1.deepCopyIndexExpression()),copy2(I2.deepCopyIndexExpression()),copy3(I3.deepCopyIndexExpression()),copy4(I4.deepCopyIndexExpression()),copy5(I5.deepCopyIndexExpression()),visited(false) { }

    RandomDataRef::RandomDataRef( const RandomDataRef& ref)
        :
    RV(ref.RV),
        I1(ref.I1),
        I2(ref.I2),
        I3(ref.I3),
        I4(ref.I4),
        I5(ref.I5),copy1(I1.deepCopyIndexExpression()),copy2(I2.deepCopyIndexExpression()),copy3(I3.deepCopyIndexExpression()),copy4(I4.deepCopyIndexExpression()),copy5(I5.deepCopyIndexExpression()),
        B(ref.B),
        visited(ref.visited)
    {}

    const RandomDataRef& RandomDataRef::operator=( const RandomConstant& c )
    { //TODO: What about bool, B, index_exp ?
        RV->c = c;
        return *this;
    }

    const RandomDataRef& RandomDataRef::operator=( const RandomDataRef& c )
    { //TODO: What about bool, B, index_exp ?
        if (this == &c)
            return *this;
        RV->c = c;
        return *this;
    }

    const RandomDataRef& RandomDataRef::operator=( RandomVariable* c )
    { 
        if (c == NULL)
            return *this;
        if (RV->c.isDefined()){
            LOG(ERROR) << "FlopCpp Error: you defined this MP_random_data " << RV->getName() << "to hold an algebraic combination of other elements, so you must not assign a RandomVariable to it.";
            throw not_implemented_error(); 
        }

        RV->operator()( (int)I1->evaluate(),(int)I2->evaluate(),(int)I3->evaluate(),(int)I4->evaluate(),(int)I5->evaluate() ) = c; 
        //    // Set correct stage for variable.
        //    //Set stage to correct value
            switch (RV->stage){
        case 0: c->setStage((int)I1->evaluate() ); break;
        case 1: c->setStage((int)I2->evaluate() ); break;
        case 2: c->setStage((int)I3->evaluate() ); break;
        case 3: c->setStage((int)I4->evaluate() ); break;
        case 4: c->setStage((int)I5->evaluate() ); break;
            }
        return *this;
    }

    void RandomDataRef::propagateIndexExpressions( const MP_index_exp& i1, const MP_index_exp& i2,const MP_index_exp& i3, const MP_index_exp& i4,const MP_index_exp& i5 )
    {
        // If we are asked to propagate IndexExpressions with EmptyIndices, we propagate our stored indices
        if (RV->c.isDefined()){ // We do not store RV, so we are more a container that holds an algebraic formula involving random data. Propagate Indices downwards.
            if (&i1 == &MP_index_exp::getEmpty()){ // If the sets are empty
                RV->c->propagateIndexExpressions(I1,I2,I3,I4,I5);
                return;
            }
        } 
        // We have to make deepCopy's only once, this is during construction. TradeOff/evaluation necessary to make a final decision, we can still change this.
        // Restore original decisions so that every RDR can work with the same set of indices.
        I1 = copy1;
        I2 = copy2;
        I3 = copy3;
        I4 = copy4;
        I5 = copy5;

        // Now things get complicated: When is this method called? During coefficient generation. 
        // We are in a RandomDataRef that is not the first one ( i.e. in a chained RandomDataRef, but NOT in the beginning.
        // Ordering of our RDR can differ from ordering of calling RDR. Find correct indices and propagate these
        // We ensured already that we used correct indices during the creation of RandomDataRef with semantic checking!
        // 
        MP_index* tempPtr = i1->getIndex();
        vector<bool> bools(5,false);
        if ( tempPtr == I1->getIndex() )
        { I1 = I1->insertIndexExpr(i1); bools[0] = true; }
        else if (tempPtr == I2->getIndex() )
        {  I2 = I2->insertIndexExpr(i1); bools[1] = true; }
        else if (tempPtr == I3->getIndex() )
        {   I3 = I3->insertIndexExpr(i1);bools[2] = true; }
        else if (tempPtr == I4->getIndex() )
        {   I4 = I4->insertIndexExpr(i1);bools[3] = true; }
        else if (tempPtr == I5->getIndex() )
        {   I5 = I5->insertIndexExpr(i1);bools[4] = true; }

        // Go for the second index
        if ( i2.operator==(MP_index_exp::getEmpty()) )
            goto stop;
        tempPtr = i2->getIndex();
        if ( !bools[1] && tempPtr == I2->getIndex())
        {   I2 = I2->insertIndexExpr(i2); bools[1] = true; }
        else if (!bools[2] && tempPtr == I3->getIndex() )
        {   I3 = I3->insertIndexExpr(i2);bools[2] = true; }
        else if (!bools[3] && tempPtr == I4->getIndex() )
        {   I4 = I4->insertIndexExpr(i2);bools[3] = true; }
        else if (!bools[4] && tempPtr == I5->getIndex() )
        {   I5 = I5->insertIndexExpr(i2);bools[4] = true; }
        else if (!bools[1] &&tempPtr == I1->getIndex() )
        {   I1 = I1->insertIndexExpr(i2);bools[0] = true; }

        // Go for the third index
        if ( i3.operator==(MP_index_exp::getEmpty()) )
            goto stop;
        tempPtr = i3->getIndex();
        if ( !bools[1] && tempPtr == I2->getIndex())
        {   I2 = I2->insertIndexExpr(i2); bools[1] = true; }
        else if (!bools[2] && tempPtr == I3->getIndex() )
        {   I3 = I3->insertIndexExpr(i2);bools[2] = true; }
        else if (!bools[3] && tempPtr == I4->getIndex() )
        {   I4 = I4->insertIndexExpr(i2);bools[3] = true; }
        else if (!bools[4] && tempPtr == I5->getIndex() )
        {   I5 = I5->insertIndexExpr(i2);bools[4] = true; }
        else if (!bools[1] &&tempPtr == I1->getIndex() )
        {   I1 = I1->insertIndexExpr(i2);bools[0] = true; }

        // Go for the fourth index
        if ( i4.operator==(MP_index_exp::getEmpty()) )
            goto stop;
        tempPtr = i4->getIndex();
        if ( !bools[1] && tempPtr == I2->getIndex())
        {   I2 = I2->insertIndexExpr(i2); bools[1] = true; }
        else if (!bools[2] && tempPtr == I3->getIndex() )
        {   I3 = I3->insertIndexExpr(i2);bools[2] = true; }
        else if (!bools[3] && tempPtr == I4->getIndex() )
        {   I4 = I4->insertIndexExpr(i2);bools[3] = true; }
        else if (!bools[4] && tempPtr == I5->getIndex() )
        {   I5 = I5->insertIndexExpr(i2);bools[4] = true; }
        else if (!bools[1] &&tempPtr == I1->getIndex() )
        {   I1 = I1->insertIndexExpr(i2);bools[0] = true; }

        // Go for the fifth index
        if ( i5.operator==(MP_index_exp::getEmpty()) )
            goto stop;
        tempPtr = i5->getIndex();
        if ( !bools[1] && tempPtr == I2->getIndex())
        {   I2 = I2->insertIndexExpr(i2); bools[1] = true; }
        else if (!bools[2] && tempPtr == I3->getIndex() )
        {   I3 = I3->insertIndexExpr(i2);bools[2] = true; }
        else if (!bools[3] && tempPtr == I4->getIndex() )
        {   I4 = I4->insertIndexExpr(i2);bools[3] = true; }
        else if (!bools[4] && tempPtr == I5->getIndex() )
        {   I5 = I5->insertIndexExpr(i2);bools[4] = true; }
        else if (!bools[1] &&tempPtr == I1->getIndex() )
        {   I1 = I1->insertIndexExpr(i2);bools[0] = true; }
        else
            LOG(INFO) << "You linked a MP_random_data to another MP_random_data with different indices.";

stop:
        if (RV->c.isDefined()){
            RV->c->propagateIndexExpressions(I1,I2,I3,I4,I5);
        }


    }

    RandomVariable* RandomDataRef::getRandomVariable(int index) const
    {   //If index has default value of outOfBound we return the RV in the "normal" way: By evaluating the indices
        if (index == outOfBound) {
            //Check for sanity of indices
            int i1 = RV->S1.check(I1->evaluate());
            int i2 = RV->S2.check(I2->evaluate());
            int i3 = RV->S3.check(I3->evaluate());
            int i4 = RV->S4.check(I4->evaluate());
            int i5 = RV->S5.check(I5->evaluate());	
            // Compute index in flattened multidimensional array
            index = RV->f(i1,i2,i3,i4,i5);
        }
        // Return value
        if ( index == outOfBound || index > RV->size() ) {
            return 0;
        } else {
            return RV->values[index]; 
        }
    }

    RandomDataRef& RandomDataRef::such_that( const MP_boolean& b )
    {
        B = b;
        return *this;
    }

    int RandomDataRef::getStage() const
    {	//We do not know which of these sets is the stage set, so we have to check all (but could abort after we found one)
        switch (RV->stage){ // We know which index keeps the stage.
    case 0 : return I1->evaluate();
    case 1 : return I2->evaluate();
    case 2 : return I3->evaluate();
    case 3 : return I4->evaluate();
    case 4 : return I5->evaluate();
    default: throw invalid_argument_exception();
        }
    }


    void RandomDataRef::insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const{
        //Insert all variables available in values** array
        bool containsRandomVariable = false;
        for (int i = 0; i < RV->size();i++){
            if (RV->values[i] != 0){
                v[RV->values[i]->getStage()].insert(RV->values[i]);
                containsRandomVariable = true;
            }
        }
        if (!containsRandomVariable) { //We do not have a random variable in this RandomDataRef, so call insert on Constant. This is exclusive-or.
            RV->c->insertRandomVariables(v);
            // TODO: Break recursion..
            if (visited == true) //We already visited this variable.. this means we have a recursion.
                return;
            visited = true;
            //TODO: Look at this method again..
        }

    }

    double RandomDataRef::evaluate( int scenario ) const
    {
        //If scenario is outOfBound this call happens in case we are not aware of scenarios: Coefficient generation: getValue
        // We can set Expected Value here, as otherwise we would have a propagated scenario index.
        //Check for sanity of indices
        int i1 = RV->S1.check(I1->evaluate());
        int i2 = RV->S2.check(I2->evaluate());
        int i3 = RV->S3.check(I3->evaluate());
        int i4 = RV->S4.check(I4->evaluate());
        int i5 = RV->S5.check(I5->evaluate());	
        // Compute index in flattened multidimensional array
        int i = RV->f(i1,i2,i3,i4,i5);
        // Return value
        if ( i ==  outOfBound ) {
            return 0;
        } else {
            if (RV->values[i] != 0) // MP_random_data is a random variable
                // this does not work correctly when solving EV, EEV or WS problems, because mean value is calculated without considering probabilities
                return scenario == outOfBound? RV->values[i]->getMeanValue() : RV->values[i]->getSampledValue(scenario);
            else // MP_random_data is the result of algebraic manipulation of expressions involving random data..
                if (!RV->c.isDefined()){ // We have no algebraic manipulation. This leads to two things
                    // First: User made an error while typing the model. If this is the case we throw an invalid argument exception.
                    // Second: User used a random parameter in the first stage, we can check for that.
                    switch ( RV->stage){
    case 0: if ( i1 == 0) return 0; break;
    case 1: if ( i2 == 0) return 0; break;
    case 2: if ( i3 == 0) return 0; break;
    case 3: if ( i4 == 0) return 0; break;
    case 4: if ( i5 == 0) return 0; break;
        // We have either no stage defined (which throws an exception earlier) or we are not in the first stage.
    default : throw invalid_argument_exception();
                    }
                }
                return RV->c->evaluate(scenario);
        }
    }

    //void RandomDataRef::generate( const MP_domain& domain, vector<TerminalExpression *> multiplicators, MP::GenerateFunctor& f, double m ) const
    //{
    //    // We need to go over all elements and "generate" the values..
    //    f.setMultiplicator(multiplicators,m);
    //    f.setTerminalExpression(this);
    //    domain.forall(&f);
    //}

    //Implementation for ScenarioRandomVariable
    ScenarioRandomVariable::ScenarioRandomVariable( double* scenValues,int length)
        : DiscreteRandomVariable()
    {
        this->values = std::vector<double>(scenValues,scenValues+length);
    }

    ScenarioRandomVariable::ScenarioRandomVariable() :
    DiscreteRandomVariable()
    {

    }

    void ScenarioRandomVariable::sample(int howMany) 
    {
    }

    double ScenarioRandomVariable::getMeanValue() const {
        // We need the number of scenarios to equal the values we got here. Otherwise something seems to be wrong
        int scenarioSize = MP_model::getCurrentModel()->getScenSet().size();
        std::vector<double> probs = MP_model::getCurrentModel()->getProbabilities();
        assert(scenarioSize == probs.size()); // We have correct probabilities for current scenario set.
        assert(scenarioSize == values.size()); // We have same number of values as number of scenarios.
        //Compute mean of values and return it
        double mean = 0;
        for (int i = 0; i < values.size(); i++) {
            mean += values[i]*probs[i];
        }
        return mean;

    }

    double ScenarioRandomVariable::pdf( double x ) const
    {
        //return boost::math::pdf(distribution,x);
        return 0;
    }

    double ScenarioRandomVariable::cdf( double x ) const
    {
        //return boost::math::cdf(distribution,x);
        return 0;
    }

    double ScenarioRandomVariable::quantile( double p ) const
    {
        //return boost::math::quantile(distribution,p);
        return 0;
    }

    ScenarioRandomVariable::~ScenarioRandomVariable()
    {

    }


    //Implementation for ContinuousRandomVariable

    ContinuousRandomVariable::ContinuousRandomVariable() : RandomVariable() {}

    ContinuousRandomVariable::~ContinuousRandomVariable()
    {

    }

    DiscreteRandomVariable::DiscreteRandomVariable() : RandomVariable()
    {

    }
    DiscreteRandomVariable::~DiscreteRandomVariable()
    {

    }

    DiscreteUniformRandomVariable::DiscreteUniformRandomVariable( int from, int to, int sampleSize, DiscretizationMethod* dmPtr ): DiscreteRandomVariable(),start(from),end(to),distribution(from,to)
    { // Ensure correct functionality
        if (end < start){
            int temp = start;
            start = end;
            end = temp;
        }
        step = double(end-start)/sampleSize;
        defaultSampleSize = sampleSize;
        if (step == 0.0){
            LOG(ERROR) << "FlopCpp Error: UniformRandomVariable from start " << start << " to " << end << "has stepwidth zero.";
            throw invalid_argument_exception();
        }
        dm = boost::shared_ptr<DiscretizationMethod>(dmPtr);
        dm->setSampleNumber(sampleSize);
        dm->setRandomVariable(this); 

    }

    DiscreteUniformRandomVariable::~DiscreteUniformRandomVariable()
    {

    }

    void DiscreteUniformRandomVariable::sample( int howMany )
    {
        //Initialize data structure and discrete distribution.
        RandomVariable::sample(howMany);
        if (discreteDistribution.empty()){ // This should only be the case if we have a DoubleDiscretization..
            if ( defaultSampleSize == 1 ) {
                // Return mean value with prob. 1
                valuesMap.insert(std::pair<double,double> ( double(end-start)/2 ,1.0) );
                discreteDistribution = valuesMap;

            }
            else {
                assert(defaultSampleSize > 1);
                double stepWidth = double(end-start) /(defaultSampleSize-1);
                for (int i = 0; i < defaultSampleSize; i++){
                    valuesMap.insert(std::pair<double,double>(start+stepWidth*i,1.0/defaultSampleSize));
                }
                discreteDistribution = valuesMap;
            }
        }
        if (MP_model::getCurrentModel()->getSample()){ //Sample from discrete distribution
            // If we have EmpiricalDiscretization, we have to compute these values ourselves..
            if( dynamic_cast<DoubleDiscretization*>(dm.get()) != 0 ){
                // Sample from uniform distribution using boost..
                int domainSize = end-start;
                typedef boost::uniform_int<> distr_type;
                boost::variate_generator<uniform_distr_gen,distr_type > variateGen(*MP_model::getCurrentModel()->getUniformGenerator()->uniform,distr_type(0,domainSize));
                for (int i = 0; i < howMany; ++i){
                    values.push_back(start+variateGen());
                }
            }
            else {
                // Sampling from discrete distribution is already done in the base class method
            }
        }

    }


    double DiscreteUniformRandomVariable::getMeanValue() const {
        // We can compute it by sampled values or we can compute mean value for distribution
        int count = 0;
        double sum = 0;
        for (double i = start; i <= end+std::numeric_limits<double>::epsilon(); i+=step){ //double as for counting.. not so good i suppose
            sum += i;
            count++;
        }
        return sum /count;
    }

    double DiscreteUniformRandomVariable::pdf( double x ) const
    {
        return boost::math::pdf(distribution,x);
    }

    double DiscreteUniformRandomVariable::cdf( double x ) const
    {
        return boost::math::cdf(distribution,x);
    }

    double DiscreteUniformRandomVariable::quantile( double p ) const
    {
        return boost::math::quantile(distribution,p);
    }

    DiscreteGeometricRandomVariable::DiscreteGeometricRandomVariable(double probability): DiscreteRandomVariable(),probability(probability),distribution(probability)
    { 
        dm = boost::shared_ptr<DiscretizationMethod>(new DoubleDiscretization(1));
        dm->setRandomVariable(this);
    }

    DiscreteGeometricRandomVariable::~DiscreteGeometricRandomVariable()
    {

    }

    void DiscreteGeometricRandomVariable::sample( int howMany )
    {
        //Initialize data structure
        RandomVariable::sample(howMany);

        if (MP_model::getCurrentModel()->getSample()){ //Sample from discrete distribution
            // If we have EmpiricalDiscretization, we have to compute these values ourselves..
            if( dynamic_cast<DoubleDiscretization*>(dm.get()) != 0 ){
                // Sample from uniform distribution using boost..
                // Typedef to ease use and allow copy code ;)
                typedef boost::geometric_distribution<> distr_type;
                boost::variate_generator<uniform_distr_gen,distr_type > variateGen(*MP_model::getCurrentModel()->getUniformGenerator()->uniform,distr_type(probability));
                for (int i = 0; i < howMany; ++i){
                    values.push_back(variateGen());
                }
            }
            else {
                // Sampling from discrete distribution is already done in the base class method
            }
        }

    }

    double DiscreteGeometricRandomVariable::getMeanValue() const
    {
        return 1.0/probability;
    }

    double DiscreteGeometricRandomVariable::pdf( double x ) const
    {
        //return boost::math::pdf(distribution,x);
        return 0;
    }

    double DiscreteGeometricRandomVariable::cdf( double x ) const
    {
        //return boost::math::cdf(distribution,x);
        return 0;
    }

    double DiscreteGeometricRandomVariable::quantile( double p ) const
    {
        //return boost::math::quantile(distribution,p);
        return 0;
    }

    DiscreteBernoulliRandomVariable::DiscreteBernoulliRandomVariable(double probability): DiscreteRandomVariable(),probability(probability),distribution(probability)
    {     
        dm = boost::shared_ptr<DiscretizationMethod>(new DoubleDiscretization(1));
        dm->setRandomVariable(this);
    }

    DiscreteBernoulliRandomVariable::~DiscreteBernoulliRandomVariable()
    {

    }

    void DiscreteBernoulliRandomVariable::sample( int howMany )
    {
        //Initialize data structure
        RandomVariable::sample(howMany);

        if (MP_model::getCurrentModel()->getSample()){ //Sample from discrete distribution
            // If we have DoubleDiscretization, we have to compute these values ourselves..
            if( dynamic_cast<DoubleDiscretization*>(dm.get()) != 0 ){
                // Typedef to ease use and allow copy code ;)
                typedef boost::bernoulli_distribution<> distr_type;
                boost::variate_generator<uniform_distr_gen,distr_type > variateGen(*MP_model::getCurrentModel()->getUniformGenerator()->uniform,distr_type(probability));
                for (int i = 0; i < currentSampleSize; ++i){
                    values.push_back(variateGen());
                }
            }
            else {
                // Sampling from discrete distribution is already done in the base class method
            }
        }
    }

    double DiscreteBernoulliRandomVariable::getMeanValue() const
    {
        return probability;
    }
    double DiscreteBernoulliRandomVariable::pdf( double x ) const
    {
        return boost::math::pdf(distribution,x);
    }

    double DiscreteBernoulliRandomVariable::cdf( double x ) const
    {
        return boost::math::cdf(distribution,x);
    }

    double DiscreteBernoulliRandomVariable::quantile( double p ) const
    {
        return boost::math::quantile(distribution,p);
    }



    ContinuousUniformRandomVariable::ContinuousUniformRandomVariable( int from, int to , DiscretizationMethod* dmPtr) : ContinuousRandomVariable(),start(from),end(to),distribution(from,to)
    {
        if (end < start){
            int temp = start;
            start = end;
            end = temp;
        }
        dm = boost::shared_ptr<DiscretizationMethod>(dmPtr);
        dm->setRandomVariable(this);

    }

    ContinuousUniformRandomVariable::~ContinuousUniformRandomVariable()
    {

    }

    void ContinuousUniformRandomVariable::sample( int howMany )
    {
        //Initialize data structure
        RandomVariable::sample(howMany);
        if (MP_model::getCurrentModel()->getSample()){
            if( dynamic_cast<DoubleDiscretization*>(dm.get()) != 0 ){
                // Typedef to ease use and allow copy code ;)
                typedef boost::uniform_real<> distr_type;
                double domainSize = end-start;
                boost::variate_generator<uniform_distr_gen,distr_type > variateGen(*MP_model::getCurrentModel()->getUniformGenerator()->uniform,distr_type(0,domainSize));
                for (int i = 0; i < howMany; ++i){
                    values.push_back(start+variateGen());
                }
            }
            else {
                // Sampling from discrete distribution is already done in the base class method
            }
        }
    }

    double ContinuousUniformRandomVariable::getMeanValue() const
    {
        return start + (end-start)/2;
    }
    double ContinuousUniformRandomVariable::pdf( double x ) const
    {
        return boost::math::pdf(distribution,x);
    }

    double ContinuousUniformRandomVariable::cdf( double x ) const
    {
        return boost::math::cdf(distribution,x);
    }

    double ContinuousUniformRandomVariable::quantile( double p ) const
    {
        return boost::math::quantile(distribution,p);
    }


    ContinuousLogNormalRandomVariable::ContinuousLogNormalRandomVariable(double mean, double sigma, DiscretizationMethod* dmPtr) : ContinuousRandomVariable(),mean(mean),sigma(sigma),distribution(mean,sigma)
    {
        dm = boost::shared_ptr<DiscretizationMethod>(dmPtr);
        dm->setRandomVariable(this);
    }

    ContinuousLogNormalRandomVariable::~ContinuousLogNormalRandomVariable()
    {

    }

    void ContinuousLogNormalRandomVariable::sample( int howMany )
    {
        //Initialize data structures
        RandomVariable::sample(howMany);
        if (MP_model::getCurrentModel()->getSample()){
            if( dynamic_cast<DoubleDiscretization*>(dm.get()) != 0 ){
                // Typedef to ease use and allow copy code ;)
                typedef boost::lognormal_distribution<> distr_type;
                boost::variate_generator<uniform_distr_gen,distr_type > variateGen(*MP_model::getCurrentModel()->getUniformGenerator()->uniform,distr_type(mean, sigma));
                for (int i = 0; i < howMany; ++i){
                    values.push_back(variateGen());
                }
            }
            else {
                // Sampling from discrete distribution is already done in the base class method
            }
        }
    }

    double ContinuousLogNormalRandomVariable::getMeanValue() const
    {
        return mean;
    }
    double ContinuousLogNormalRandomVariable::pdf( double x ) const
    {
        return boost::math::pdf(distribution,x);
    }

    double ContinuousLogNormalRandomVariable::cdf( double x ) const
    {
        return boost::math::cdf(distribution,x);
    }

    double ContinuousLogNormalRandomVariable::quantile( double p ) const
    {
        return boost::math::quantile(distribution,p);
    }


    ContinuousNormalRandomVariable::ContinuousNormalRandomVariable(double mean, double sigma, DiscretizationMethod* dmPtr) : ContinuousRandomVariable(),mean(mean),sigma(sigma),distribution(mean,sigma)
    {
        dm = boost::shared_ptr<DiscretizationMethod>(dmPtr);
        dm->setRandomVariable(this);
    }

    ContinuousNormalRandomVariable::~ContinuousNormalRandomVariable()
    {

    }

    void ContinuousNormalRandomVariable::sample( int howMany )
    {
        //Initialize data structures
        RandomVariable::sample(howMany);
        if (MP_model::getCurrentModel()->getSample()){
            if( dynamic_cast<DoubleDiscretization*>(dm.get()) != 0 ){
                typedef boost::normal_distribution<> distr_type;
                boost::variate_generator<uniform_distr_gen,distr_type > variateGen(*MP_model::getCurrentModel()->getUniformGenerator()->uniform,distr_type(mean, sigma));
                for (int i = 0; i < howMany; ++i){
                    values.push_back(variateGen());
                }
            }
            else {
                // Sampling from discrete distribution is already done in the base class method
            }
        }
    }

    double ContinuousNormalRandomVariable::getMeanValue() const
    {
        return mean;
    }
    double ContinuousNormalRandomVariable::pdf( double x ) const
    {
        return boost::math::pdf(distribution,x);
    }

    double ContinuousNormalRandomVariable::cdf( double x ) const
    {
        return boost::math::cdf(distribution,x);
    }

    double ContinuousNormalRandomVariable::quantile( double p ) const
    {
        return boost::math::quantile(distribution,p);
    }

    ContinuousExponentialRandomVariable::ContinuousExponentialRandomVariable(double lambda, DiscretizationMethod* dmPtr) : ContinuousRandomVariable(),lambda(lambda),distribution(lambda)
    {
        dm = boost::shared_ptr<DiscretizationMethod>(dmPtr);
        dm->setRandomVariable(this);
    }

    ContinuousExponentialRandomVariable::~ContinuousExponentialRandomVariable()
    {

    }

    void ContinuousExponentialRandomVariable::sample( int howMany )
    {
        //Initialize data structures
        RandomVariable::sample(howMany);
        if (MP_model::getCurrentModel()->getSample()){
            if( dynamic_cast<DoubleDiscretization*>(dm.get()) != 0 ){
                // Typedef to ease use and allow copy code ;)
                typedef boost::exponential_distribution<> distr_type;
                boost::variate_generator<uniform_distr_gen,distr_type > variateGen(*MP_model::getCurrentModel()->getUniformGenerator()->uniform,distr_type(lambda));
                for (int i = 0; i < howMany; ++i){
                    values.push_back(variateGen());
                }
            }
            else {
                // Sampling from discrete distribution is already done in the base class method
            }
        }
    }

    double ContinuousExponentialRandomVariable::getMeanValue() const
    {
        return 1.0/lambda;
    }
    double ContinuousExponentialRandomVariable::pdf( double x ) const
    {
        return boost::math::pdf(distribution,x);
    }

    double ContinuousExponentialRandomVariable::cdf( double x ) const
    {
        return boost::math::cdf(distribution,x);
    }

    double ContinuousExponentialRandomVariable::quantile( double p ) const
    {
        return boost::math::quantile(distribution,p);
    }

    ContinuousTriangleRandomVariable::ContinuousTriangleRandomVariable(double a, double b, double c, DiscretizationMethod* dmPtr) : ContinuousRandomVariable(),a(a),b(b),c(c), distribution(a,b,c)
    {
        dm = boost::shared_ptr<DiscretizationMethod>(dmPtr);
        dm->setRandomVariable(this);
    }

    ContinuousTriangleRandomVariable::~ContinuousTriangleRandomVariable()
    {

    }

    void ContinuousTriangleRandomVariable::sample( int howMany )
    {
        //Initialize data structures
        RandomVariable::sample(howMany);
        if (MP_model::getCurrentModel()->getSample()){
            if( dynamic_cast<DoubleDiscretization*>(dm.get()) != 0 ){
                // Typedef to ease use and allow copy code ;)
                typedef boost::triangle_distribution<> distr_type;
                boost::variate_generator<uniform_distr_gen,distr_type > variateGen(*MP_model::getCurrentModel()->getUniformGenerator()->uniform,distr_type(a,b,c));
                for (int i = 0; i < howMany; ++i){
                    values.push_back(variateGen());
                }
            }
            else {
                // Sampling from discrete distribution is already done in the base class method
            }
        }
    }

    double ContinuousTriangleRandomVariable::getMeanValue() const
    {   
        throw not_implemented_error();
    }
    double ContinuousTriangleRandomVariable::pdf( double x ) const
    {
        return boost::math::pdf(distribution,x);
    }

    double ContinuousTriangleRandomVariable::cdf( double x ) const
    {
        return boost::math::cdf(distribution,x);
    }

    double ContinuousTriangleRandomVariable::quantile( double p ) const
    {
        return boost::math::quantile(distribution,p);
    }



    DiscretizationMethod::DiscretizationMethod(int samples): samples(samples),rvarPtr(0)
    {

    }

    DiscretizationMethod::~DiscretizationMethod()
    {

    }

    void DiscretizationMethod::setRandomVariable( RandomVariable* ptr )
    {
        rvarPtr = ptr;
        rvarPtr->SampleSize(samples);
    }

    BracketMeanDiscretization::BracketMeanDiscretization(int samples): DiscretizationMethod(samples)
    {

    }


    void BracketMeanDiscretization::generateDiscreteDistribution()
    {
        // At first we compute the intervals, number is given by sampleSize. Each interval has to have the same probability.
        // We should start with the end-points
        double epsilon = std::numeric_limits<double>::epsilon();
        double lowerEnd = rvarPtr->quantile(0+epsilon); //TODO: Maybe we need an epsilon here?!
        double upperEnd = rvarPtr->quantile(1-epsilon);
        int intervals = rvarPtr->SampleSize();
        std::vector<double> intervalPoints(intervals+1);
        intervalPoints[0] = lowerEnd; intervalPoints[intervals] = upperEnd;
        // Compute interval points by evaluating the quantile for rising probabilities
        double increment = (1-2*epsilon)/intervals;
        for (int i = 1; i < intervals; i++){
            intervalPoints[i] = rvarPtr->quantile(0+epsilon+increment*i);
        }

        // Now we compute the mean of each interval.
        std::map<double,double> bracketMeans;
        double probSum = 0;
        for (int i = 1; i <= intervals; i++){
            // Probability of interval is CDF right bracket - CDF left bracket
            double prob = 1.0/intervals;
            // Bracket mean is value that corresponds to mean probability of that interval
            double bracketMean = rvarPtr->quantile(0+epsilon+increment*i-increment/2);
            bracketMeans.insert(std::pair<double,double>(bracketMean,prob));
            probSum += prob;
        }
        assert( compareDouble(1.0,probSum) ); 
        rvarPtr->setDiscreteDistribution(bracketMeans);
    }

    void BracketMedianDiscretization::generateDiscreteDistribution()
    {
        // At first we compute the intervals, number is given by sampleSize. Each interval has to have the same probability.
        // We should start with the end-points
        double epsilon = std::numeric_limits<double>::epsilon();
        double lowerEnd = rvarPtr->quantile(0+epsilon); //TODO: Maybe we need an epsilon here?!
        double upperEnd = rvarPtr->quantile(1-epsilon);
        int intervals = rvarPtr->SampleSize();
        std::vector<double> intervalPoints(intervals+1);
        intervalPoints[0] = lowerEnd; intervalPoints[intervals] = upperEnd;
        // Compute interval points by evaluating the quantile for rising probabilities
        double increment = (1-2*epsilon)/intervals;
        for (int i = 1; i < intervals; i++){
            intervalPoints[i] = rvarPtr->quantile(0+epsilon+increment*i);
        }

        // Now we compute the median of each interval.
        std::map<double,double> bracketMedians;
        double probSum = 0;
        for (int i = 1; i <= intervals; i++){
            // Probability of interval is 1 / #intervals
            double prob = 1.0/intervals;
            // Bracket median is median point of the interval. Compute intervalPoint on the right minus half the interval length.
            double median = intervalPoints[i]-(intervalPoints[i]-intervalPoints[i-1])/2;
            //rvarPtr->quantile(0 + epsilon + increment*(i-1)+ increment/2);         
            bracketMedians.insert(std::pair<double,double>(median,prob));
            probSum += prob;
        }
        assert( compareDouble(1.0,probSum));
        rvarPtr->setDiscreteDistribution(bracketMedians);
    }

    BracketMedianDiscretization::BracketMedianDiscretization( int samples ): DiscretizationMethod(samples)
    {

    }
    void DoubleDiscretization::generateDiscreteDistribution()
    {
        // We do nothing. This allows sampling from a double discretized distribution.. (not so useful.. most likely)
    }

    DoubleDiscretization::DoubleDiscretization( int samples ): DiscretizationMethod(samples)
    {

    }
    //void EmpiricalDiscretization::generateDiscreteDistribution()
    //{
    //    // We do nothing.
    //}

    //EmpiricalDiscretization::EmpiricalDiscretization( int samples ): DiscretizationMethod(samples)
    //{

    //}

    ExtendedPearsonTukeyDiscretization::ExtendedPearsonTukeyDiscretization() : DiscretizationMethod(3)
    {

    }

    void ExtendedPearsonTukeyDiscretization::generateDiscreteDistribution()
    {
        // ExtendedPearson-Tukey: Generate discrete approximation of 5,50,and 95 percent quantile.
        double lowerQuantile = rvarPtr->quantile(0.05); // 0.185 %
        double upperQuantile = rvarPtr->quantile(0.95); // 0.630 %
        double medianQuantile = rvarPtr->quantile(0.5); // 0.185 %
        std::map<double,double> discretizedDistribution;
        discretizedDistribution.insert(std::pair<double,double>(lowerQuantile,0.185));
        discretizedDistribution.insert(std::pair<double,double>(medianQuantile,0.630));
        discretizedDistribution.insert(std::pair<double,double>(upperQuantile,0.185));
        rvarPtr->setDiscreteDistribution(discretizedDistribution);
    }
}
