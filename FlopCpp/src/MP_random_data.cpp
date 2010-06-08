#include "MP_random_data.hpp"
#include <exception>
#include <stdexcept>
#include <vector>
#include <set>

#include <boost/random.hpp>
#include <boost/enable_shared_from_this.hpp>

#include "MP_expression.hpp"
#include "MP_model.hpp"
#include "MP_shared_implementation_details.hpp"



namespace flopc{

    //Class that stores a description of a random variable. Still needs to be defined
    //TODO: Do that
    class MP_random_data_impl : public RowMajor, public Functor , public Named, public boost::enable_shared_from_this<MP_random_data_impl> {
        friend class RandomDataRef;

    public:
        //If we use this constructor, we need to delete values in the end.
        // This is only necessary if we actually add random_variables to this MP_random_data
        MP_random_data_impl(const MP_set_base &s1 = MP_set::getEmpty(), 
            const MP_set_base &s2 = MP_set::getEmpty(), 
            const MP_set_base &s3 = MP_set::getEmpty(),
            const MP_set_base &s4 = MP_set::getEmpty(), 
            const MP_set_base &s5 = MP_set::getEmpty()):
        RowMajor(s1.size(),s2.size(),s3.size(),s4.size(),s5.size()),
            S1(s1),S2(s2),S3(s3),S4(s4),S5(s5),c(0),values(new RandomVariable*[size()]),myrefs(),manageData(true) { 
                // Identify stage set.
                //if (MP_model::getCurrentModel()->Stage().size() > 0){
                //    if (S1.isStage())
                //        j1i = 1;
                //    else if (S2.isStage())
                //        j2i = 1;
                //    else if (S3.isStage())
                //        j3i = 1;
                //    else if (S4.isStage())
                //        j4i = 1;
                //    else if (S5.isStage())
                //        j5i = 1;
                //    else
                //        throw invalid_argument_exception();
                //}
                initialize();
        }; 

        //Do a shallow copy (leave values in original arrays). This means we do not want to delete values in the end
        MP_random_data_impl(RandomVariable** array, const MP_set_base &s1 = MP_set::getEmpty(), 
            const MP_set_base &s2 = MP_set::getEmpty(), 
            const MP_set_base &s3 = MP_set::getEmpty(),
            const MP_set_base &s4 = MP_set::getEmpty(), 
            const MP_set_base &s5 = MP_set::getEmpty()):
        RowMajor(s1.size(),s2.size(),s3.size(),s4.size(),s5.size()),
            S1(s1),S2(s2),S3(s3),S4(s4),S5(s5),c(0),values(new RandomVariable*[size()]),myrefs(),manageData(true) {  //Consistency check?
                initialize();
                // Very ugly code..
                // What is done here: Arrays stage index is one less than normal stage index because first stage needs no RV. 
                int j1i,j2i,j3i,j4i,j5i;
                j1i =j2i = j3i = j4i = j5i = 0;
                if (MP_model::getCurrentModel()->Stage().size() > 0){
                    if (S1.isStage())
                        j1i = 1;
                    else if (S2.isStage())
                        j2i = 1;
                    else if (S3.isStage())
                        j3i = 1;
                    else if (S4.isStage())
                        j4i = 1;
                    else if (S5.isStage())
                        j5i = 1;
                    else
                        throw invalid_argument_exception();
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
                                            values[f(j1,j2,j3,j4,j5)]->Stage(stage); //FIXME: Stage does not get set via direct assignment. This has to be done also.
                                        }
                                        else
                                            values[f(j1,j2,j3,j4,j5)]->Stage(1); //Otherwise we may have a two stage problem: set to 1
                                    }
                                }

                                //else throw invalid_argument_exception();
        }; 

        //Do a shallow copy (leave values in original arrays). This means we do want to delete the created RandomVariables
        MP_random_data_impl(double* array, const MP_set_base &s1 = MP_set::getEmpty(), 
            const MP_set_base &s2 = MP_set::getEmpty(), 
            const MP_set_base &s3 = MP_set::getEmpty(),
            const MP_set_base &s4 = MP_set::getEmpty(), 
            const MP_set_base &s5 = MP_set::getEmpty()):
        RowMajor(s1.size(),s2.size(),s3.size(),s4.size(),s5.size()),
            S1(s1),S2(s2),S3(s3),S4(s4),S5(s5),c(0),values(new RandomVariable*[size()]),myrefs(),manageData(true) {  //Consistency check?
                initialize();
                int scen = MP_model::getCurrentModel()->ScenSet().size();
                //Identify stage set: The set with getStage > 0 is the stage set.
                //If there is no stage set: No Problem ;)
                int j1i,j2i,j3i,j4i,j5i;
                j1i =j2i = j3i = j4i = j5i = 0;
                if (MP_model::getCurrentModel()->Stage().size()){
                    if (S1.isStage())
                        j1i = 1;
                    else if (S2.isStage())
                        j2i = 1;
                    else if (S3.isStage())
                        j3i = 1;
                    else if (S4.isStage())
                        j4i = 1;
                    else if (S5.isStage())
                        j5i = 1;
                    else
                        throw invalid_argument_exception();
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
                                            values[f(j1,j2,j3,j4,j5)]->Stage(stage);
                                        }
                                        else
                                            values[f(j1,j2,j3,j4,j5)]->Stage(1); //Otherwise we may have a two stage problem: set to 1
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
                return values[i];
            }
        }

        RandomDataRef& operator()(
            const MP_index_exp& d1, 
            const MP_index_exp& d2, 
            const MP_index_exp& d3,
            const MP_index_exp& d4, 
            const MP_index_exp& d5) {
                Coin::SmartPtr<RandomDataRef> tempPtr(new RandomDataRef(this->shared_from_this(), d1, d2, d3, d4, d5));
                c->propagateIndexExpression(d1,d2,d3,d4,d5);
                myrefs.push_back(tempPtr);
                return *tempPtr; 

        }




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



        const MP_set_base &S1,&S2,&S3,&S4,&S5;
        MP_index i1,i2,i3,i4,i5;
        Constant c; //This constant gets set by corresponding RandomDataRef
        //boost::ptr_vector<RandomVariable> values;
        RandomVariable** values; //Array of pointers to RandomVariables : TODO: Redo this
        std::vector<Coin::SmartPtr<RandomDataRef> > myrefs;
        bool manageData;
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
    //Implementation for MP_random_data_impl
    void MP_random_data_impl::operator ()() const {
        if (&S1!=&MP_set::getEmpty()) cout << i1.evaluate() << " ";
        if (&S2!=&MP_set::getEmpty()) cout << i2.evaluate() << " ";
        if (&S3!=&MP_set::getEmpty()) cout << i3.evaluate() << " ";
        if (&S4!=&MP_set::getEmpty()) cout << i4.evaluate() << " ";
        if (&S5!=&MP_set::getEmpty()) cout << i5.evaluate() << " ";
        cout<<"  "<< values[f(i1.evaluate(),i2.evaluate(),i3.evaluate(),
            i4.evaluate(),i5.evaluate())] << endl;

    }

    void MP_random_data_impl::initialize() //TODO: How to do that in a better way? Think about that..
    {
        for (int i = 0; i < size(); i++)
            values[i] = 0;
    }


    //Implementation for RandomVariable: Think about these constructors.. are they necessary?
    RandomVariable::RandomVariable() : initialized(false),seed(0),values(),valuesMap(),defaultSampleSize(1),
        stage(0),colIndex(0),rowIndex(0) {
    } 

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

    void RandomVariable::setSampledValues(const std::vector<double>& v){
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
        if (index != getZero && index != outOfBound)
        {
            return values[index];
        }
        else { // In case of outOfBound we return 0
            return 0; // Is this the correct behaviour?!
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
        if (howMany) // If method was called with argument = 0, sample from defaultSampleSize. Else we set defaultSampleSize to howMany.
            defaultSampleSize = howMany;
        values.clear();
        valuesMap.clear();
        values.reserve(defaultSampleSize);
    }

    //flopc::RandomVariable& RandomVariable::operator=( const flopc::RandomVariable & )
    //{
    //    return *this;
    //}
    //Implementation for RandomDataRef.

    RandomDataRef::RandomDataRef(boost::shared_ptr<MP_random_data_impl> rv, 
        const MP_index_exp& i1,
        const MP_index_exp& i2,
        const MP_index_exp& i3,
        const MP_index_exp& i4,
        const MP_index_exp& i5) :
    RV(rv),I1(i1),I2(i2),I3(i3),I4(i4),I5(i5),C(rv->c) {

    }

    RandomDataRef::RandomDataRef( const RandomDataRef& ref)
        :
    RV(ref.RV),
        I1(ref.I1),
        I2(ref.I2),
        I3(ref.I3),
        I4(ref.I4),
        I5(ref.I5),
        C(ref.C),
        B(ref.B)
    {}

    void RandomDataRef::propagateIndexExpression(const MP_index_exp& i1,const MP_index_exp& i2,const MP_index_exp& i3,const MP_index_exp& i4,const MP_index_exp& i5) const {
        //Delete old index expression and set new one afterwards?!
        I1 = i1;
        I2 = i2;
        I3 = i3;
        I4 = i4;
        I5 = i5;
    }

    const RandomDataRef& RandomDataRef::operator=(const Constant& c) {
        //TODO: Delete current values of this RandomDataRef.. 
        // Set Constant to RV
        RV->c = c;
        //((D->S1(I1)*D->S2(I2)*D->S3(I3)*D->S4(I4)*D->S5(I5)).such_that(B)).forall(this); //TODO: What happens if Dimensions are not right?
        // We need to update all index expression to point to the index expression used in this RandomDataRef..
        return *this;
    }

    const RandomDataRef& RandomDataRef::operator=(const RandomDataRef& r) { 
        //Check for assignment to self
        if (this == &r)
            return *this;
        return operator=(Constant(r)); //TODO: Do not do this, it is evil..
    }


    //void RandomDataRef::evaluate_lhs( const RandomVariable &rv ) const
    //{
    //    //Check sanity of indices
    //    int i1 = RV->S1.check(I1->evaluate());
    //    int i2 = RV->S2.check(I2->evaluate());
    //    int i3 = RV->S3.check(I3->evaluate());
    //    int i4 = RV->S4.check(I4->evaluate());
    //    int i5 = RV->S5.check(I5->evaluate());
    //    //Compute index in flattened multidimensional array
    //    int i = RV->f(i1,i2,i3,i4,i5);
    //    // Set Value at computed index to v
    //    if (i != outOfBound) { //evaluate_lhs: When is this function actually called?
    //        *RV->values[i] = rv;
    //    }
    //}

    double RandomDataRef::evaluate(int scenario) const
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
                return scenario == outOfBound? RV->values[i]->getMeanValue() : RV->values[i]->getSampledValue(scenario);
            else // MP_random_data is the result of algebraic manipulation of expressions involving random data..
                return RV->c->evaluate(scenario);
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

    IndexKeeper RandomDataRef::getIndices(int k) const {
        return RV->getIndices(k);
    }

    RandomDataRef& RandomDataRef::such_that( const MP_boolean& b )
    {
        B = b;
        return *this;
    }

    int RandomDataRef::getStage() const
    {	//We do not know which of these sets is the stage set, so we have to check all (but could abort after we found one)
        int i1 = RV->S1.checkStage(I1->evaluate());
        int i2 = RV->S2.checkStage(I2->evaluate());
        int i3 = RV->S3.checkStage(I3->evaluate());
        int i4 = RV->S4.checkStage(I4->evaluate());
        int i5 = RV->S5.checkStage(I5->evaluate());

        int stage = 0; //Set stage to the correct value. Only one int should be larger than 0.
        if (i1>stage) stage = i1;
        else if (i2>stage) stage = i2;
        else if (i3>stage) stage = i3;
        else if (i4>stage) stage = i4;
        else if (i5>stage) stage = i5;
        return stage;
    }


    void RandomDataRef::operator()() const
    {
        throw not_implemented_error();
        //TODO: If C is not a constant_random_variable, something went wrong. Obviously
        //evaluate_lhs(C->evaluate());
    }

    void RandomDataRef::insertRandomVariables(std::vector< std::set<RandomVariable*> >& v) const{
        //Insert all variables available in values** array
        bool containsRandomVariable = false;
        for (int i = 0; i < RV->size();i++){
            if (RV->values[i] != 0){
                //TODO: We need to insert correct stage. If Stage is not given, variable is inserted in the wrong stage.
                v[RV->values[i]->Stage()].insert(RV->values[i]);
                containsRandomVariable = true;
            }
        }
        if (!containsRandomVariable) { //We do not have a random variable in this RandomDataRef, so call insert on Constant.
            RV->c->insertRandomVariables(v);
        }
        
    }


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
        //Apply the constant values to every element.

    }

    double ScenarioRandomVariable::getMeanValue(){
        // We need the number of scenarios to equal the values we got here. Otherwise something seems to be wrong
        int scenarioSize = MP_model::getCurrentModel()->ScenSet().size();
        std::vector<double> probs = MP_model::getCurrentModel()->Probabilities();
        assert(scenarioSize == probs.size()); // We have correct probabilities for current scenario set.
        assert(scenarioSize == values.size()); // We have same number of values as number of scenarios.
        //Compute mean of values and return it
        double mean = 0;
        for (int i = 0; i < values.size(); i++) {
            // What about probabilities? We assume equal probabilities
            mean += values[i]*probs[i];
        }
        return mean;

    }

    ScenarioRandomVariable::~ScenarioRandomVariable()
    {

    }

    //const ScenarioRandomVariable& ScenarioRandomVariable::operator=( const ScenarioRandomVariable& rhs )
    //{
    //    if( this == &rhs)
    //        return *this;
    //    //Copy all values
    //    //Copy base class
    //    DiscreteRandomVariable::operator=(rhs);
    //    return *this;
    //}

    //Implementation for ContinuousRandomVariable

    ContinuousRandomVariable::ContinuousRandomVariable() : RandomVariable() {}

    void ContinuousRandomVariable::sample(int howMany) 
    {
        //call std::sort
    }

    ContinuousRandomVariable::~ContinuousRandomVariable()
    {

    }
    //Implementation for DiscreteRandomVariable
    void DiscreteRandomVariable::sample(int howMany)
    {
        //call std::sort
    }

    DiscreteRandomVariable::DiscreteRandomVariable() : RandomVariable()
    {

    }
    DiscreteRandomVariable::~DiscreteRandomVariable()
    {

    }

    DiscreteUniformRandomVariable::DiscreteUniformRandomVariable( int from, int to, double stepWidth, int sampleSize ): DiscreteRandomVariable(),start(from),end(to),step(stepWidth)
    { // Ensure correct functionality
        //TODO: Include test for stepWidth
        if (end < start){
            int temp = start;
            start = end;
            end = temp;
        }
        if (step < 0){
            step = std::fabs(step);
        }
        else if (step == 0.0){
            throw invalid_argument_exception();
        }
        defaultSampleSize = sampleSize;
    }

    DiscreteUniformRandomVariable::~DiscreteUniformRandomVariable()
    {

    }

    void DiscreteUniformRandomVariable::sample( int howMany )
    {
        //Initialize data structure
        RandomVariable::sample(howMany);
        // We want to compute values from start to end with a stepWidth of step (double)
        // So we first compute size of domain : (end-start)*stepWidth
        int domainSize = round((end-start)/step);
        // Then generate the variates: start+step*random_variate
        typedef boost::uniform_int<> distr_type;
        boost::variate_generator<uniform_distr_gen&,distr_type> variateGen(*MP_model::getCurrentModel()->getUniformGenerator()->uniform,distr_type(0,domainSize));
        for (int i = 0; i < defaultSampleSize; ++i){
            values.push_back(start+step*variateGen());
        }
    }

    DiscreteGeometricRandomVariable::DiscreteGeometricRandomVariable(double probability): DiscreteRandomVariable(),probability(probability)
    { 
    
    }

    DiscreteGeometricRandomVariable::~DiscreteGeometricRandomVariable()
    {

    }

    void DiscreteGeometricRandomVariable::sample( int howMany )
    {
        //Initialize data structure
        RandomVariable::sample(howMany);

        // Typedef to ease use and allow copy code ;)
        typedef boost::geometric_distribution<> distr_type;
        boost::variate_generator<uniform_distr_gen,distr_type > variateGen(*MP_model::getCurrentModel()->getUniformGenerator()->uniform,distr_type(probability));
        for (int i = 0; i < howMany; ++i){
            values.push_back(variateGen());
        }
    }
    
    DiscreteBernoulliRandomVariable::DiscreteBernoulliRandomVariable(double probability): DiscreteRandomVariable(),probability(probability)
    { 
    
    }

    DiscreteBernoulliRandomVariable::~DiscreteBernoulliRandomVariable()
    {

    }

    void DiscreteBernoulliRandomVariable::sample( int howMany )
    {
        //Initialize data structure
        RandomVariable::sample(howMany);

        // Typedef to ease use and allow copy code ;)
        typedef boost::bernoulli_distribution<> distr_type;
        boost::variate_generator<uniform_distr_gen,distr_type > variateGen(*MP_model::getCurrentModel()->getUniformGenerator()->uniform,distr_type(probability));
        for (int i = 0; i < howMany; ++i){
            values.push_back(variateGen());
        }
    }




    ContinuousUniformRandomVariable::ContinuousUniformRandomVariable( int from, int to ) : ContinuousRandomVariable(),start(from),end(to)
    {
        if (end < start){
            int temp = start;
            start = end;
            end = temp;
        }

    }

    ContinuousUniformRandomVariable::~ContinuousUniformRandomVariable()
    {

    }

    void ContinuousUniformRandomVariable::sample( int howMany )
    {
        //Initialize data structure
        RandomVariable::sample(howMany);
        // Typedef to ease use and allow copy code ;)
        typedef boost::uniform_real<> distr_type;
        double domainSize = end-start;
        boost::variate_generator<uniform_distr_gen,distr_type > variateGen(*MP_model::getCurrentModel()->getUniformGenerator()->uniform,distr_type(0,domainSize));
        for (int i = 0; i < howMany; ++i){
            values.push_back(start+variateGen());
        }
    }
    

    ContinuousLogNormalRandomVariable::ContinuousLogNormalRandomVariable(double mean, double sigma) : ContinuousRandomVariable(),mean(mean),sigma(sigma)
    {

    }

    ContinuousLogNormalRandomVariable::~ContinuousLogNormalRandomVariable()
    {

    }

    void ContinuousLogNormalRandomVariable::sample( int howMany )
    {
        //Initialize data structures
        RandomVariable::sample(howMany);

        // Typedef to ease use and allow copy code ;)
        typedef boost::lognormal_distribution<> distr_type;
        boost::variate_generator<uniform_distr_gen,distr_type > variateGen(*MP_model::getCurrentModel()->getUniformGenerator()->uniform,distr_type(mean, sigma));
        for (int i = 0; i < howMany; ++i){
            values.push_back(variateGen());
        }
    }
    
    
    ContinuousNormalRandomVariable::ContinuousNormalRandomVariable(double mean, double sigma) : ContinuousRandomVariable(),mean(mean),sigma(sigma)
    {

    }

    ContinuousNormalRandomVariable::~ContinuousNormalRandomVariable()
    {

    }

    void ContinuousNormalRandomVariable::sample( int howMany )
    {
        //Initialize data structures
        RandomVariable::sample(howMany);
        // Typedef to ease use and allow copy code ;)
        typedef boost::normal_distribution<> distr_type;
        boost::variate_generator<uniform_distr_gen,distr_type > variateGen(*MP_model::getCurrentModel()->getUniformGenerator()->uniform,distr_type(mean, sigma));
        for (int i = 0; i < howMany; ++i){
            values.push_back(variateGen());
        }
    }
    
    ContinuousExponentialRandomVariable::ContinuousExponentialRandomVariable(double lambda) : ContinuousRandomVariable(),lambda(lambda)
    {

    }

    ContinuousExponentialRandomVariable::~ContinuousExponentialRandomVariable()
    {

    }

    void ContinuousExponentialRandomVariable::sample( int howMany )
    {
        //Initialize data structures
        RandomVariable::sample(howMany);

        // Typedef to ease use and allow copy code ;)
        typedef boost::exponential_distribution<> distr_type;
        boost::variate_generator<uniform_distr_gen,distr_type > variateGen(*MP_model::getCurrentModel()->getUniformGenerator()->uniform,distr_type(lambda));
        for (int i = 0; i < howMany; ++i){
            values.push_back(variateGen());
        }
    }
    
    ContinuousTriangleRandomVariable::ContinuousTriangleRandomVariable(double a, double b, double c) : ContinuousRandomVariable(),a(a),b(b),c(c)
    {

    }

    ContinuousTriangleRandomVariable::~ContinuousTriangleRandomVariable()
    {

    }

    void ContinuousTriangleRandomVariable::sample( int howMany )
    {
        //Initialize data structures
        RandomVariable::sample(howMany);

        // Typedef to ease use and allow copy code ;)
        typedef boost::triangle_distribution<> distr_type;
        boost::variate_generator<uniform_distr_gen,distr_type > variateGen(*MP_model::getCurrentModel()->getUniformGenerator()->uniform,distr_type(a,b,c));
        for (int i = 0; i < howMany; ++i){
            values.push_back(variateGen());
        }
    }

}
