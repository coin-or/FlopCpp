// ******************** FlopCpp **********************************************
// File: MP_model.cpp
//****************************************************************************

#include <iostream>
#include <sstream>
using std::cout;
using std::endl;
#include <algorithm>
#include <limits>
#include <map>
#include <list>
#include <deque>
#include <ctime>
#include <iterator>



#include <glog/logging.h>

#include <boost/shared_ptr.hpp>
#include <boost/random.hpp>

#include <CoinTime.hpp>
#include <CoinPackedMatrix.hpp>
#include <CoinPackedVector.hpp>
#include <OsiSolverInterface.hpp>



#include "SmiScnData.hpp"
#include "SmiScenarioTree.hpp"
#include "SmiScnModel.hpp"

#include "MP_model.hpp"
#include "MP_variable.hpp"
#include "MP_constraint.hpp"
#include "MP_shared_implementation_details.hpp"

// Workaround necessary for glog, if WinGDI.h is included (I do not know who includes the file)
#ifdef ERROR
#undef ERROR
#endif

using namespace flopc;

const std::string MP_model::stageString("StageSet");
MP_model& MP_model::default_model = *new MP_model(0);
MP_model* MP_model::current_model = &MP_model::default_model;
MP_model& MP_model::getDefaultModel() { return default_model;}
MP_model* MP_model::getCurrentModel() { return current_model;}

void NormalMessenger::statistics(int bm, int m, int bn, int n, int nz) {
    DLOG(INFO) <<"FlopCpp: Number of constraint blocks: " <<bm<<endl;
    DLOG(INFO) <<"FlopCpp: Number of individual constraints: " <<m<<endl;
    DLOG(INFO) <<"FlopCpp: Number of variable blocks: " <<bn<<endl;
    DLOG(INFO) <<"FlopCpp: Number of individual variables: " <<n<<endl;
    DLOG(INFO) <<"FlopCpp: Number of non-zeroes (including rhs): " <<nz<<endl;
}

void NormalMessenger::generationTime(double t) {
    DLOG(INFO) <<"FlopCpp: Generation time: "<<t<<endl;
}

void NormalMessenger::solutionStatus(const OsiSolverInterface* Solver) {
    if (Solver->isProvenOptimal()) {
        DLOG(INFO) <<"FlopCpp: Optimal obj. value = "<<Solver->getObjValue()<<endl;
        DLOG(INFO) <<"FlopCpp: Solver(m, n, nz) = "<<Solver->getNumRows()<<"  "<<
            Solver->getNumCols()<<"  "<<Solver->getNumElements()<<endl;
    } else if (Solver->isProvenPrimalInfeasible()) {
        DLOG(INFO) <<"FlopCpp: Problem is primal infeasible."<<endl;
    } else if (Solver->isProvenDualInfeasible()) {
        DLOG(INFO) <<"FlopCpp: Problem is dual infeasible."<<endl;
    } else {
        DLOG(INFO) <<"FlopCpp: Solution process abandoned."<<endl;
    }
}

void VerboseMessenger::constraintDebug(string name, const vector<MP::Coef>& cfs) const {
    DLOG(INFO) <<"FlopCpp: Constraint "<<name<<endl;
    for (unsigned int j=0; j<cfs.size(); j++) {
        int col=cfs[j].col;
        int row=cfs[j].row;
        double elm=cfs[j].val;
        int stage=cfs[j].varStage;
        DLOG(INFO) <<row<<"   "<<col<<"  "<<elm<<"  "<<stage<<endl;
    }
}

void VerboseMessenger::objectiveDebug(const vector<MP::Coef>& cfs) const {
    DLOG(INFO) <<"FlopCpp: Objective "<<endl;
    for (unsigned int j=0; j<cfs.size(); j++) {
        int col=cfs[j].col;
        int row=cfs[j].row;
        double elm=cfs[j].val;
        DLOG(INFO) <<row<<"   "<<col<<"  "<<elm<<endl;
    }
}

MP_model::MP_model(OsiSolverInterface* s, Messenger* m, unsigned int seed ) : 
Solver(s),messenger(m),stage(0),scenSet(0),Objective(0),defaultSampleSize(0),sampleOnly(false),doSample(false),semantic(true),
m(0), n(0), nz(0),Cst(0),Clg(0),Rnr(0),Elm(0), bl(0), bu(0), c(0),l(0),u(0),colStage(0),rowStage(0),colIndirection(0),rowIndirection(0),
mSolverState(((s==0)?(MP_model::DETACHED):(MP_model::SOLVER_ONLY))),uniformGenerator(new Uniform01Generator()),smiModel(0) {
    MP_model::current_model = this;
    uniformGenerator->rng = boost::shared_ptr<base_generator_type>(new base_generator_type(seed ? seed : uniformGenerator->seed));
    uniformGenerator->uniform = boost::shared_ptr<uniform_distr_gen>(new uniform_distr_gen(*uniformGenerator->rng ,boost::uniform_01<double>() ));
    // Create a new taskmgr for PFunc
}

MP_model::~MP_model() {
    delete messenger;
    if (Solver) { //If solver was not deleted, call detach and therefore delete the Solver and smiModel
        this->detach();
    }
    assert(Solver == 0);
    assert(smiModel == 0);
    //Reset currentModel Pointer to default model IF it points to this model to prevent dangling pointer
    if (MP_model::current_model == this)
        MP_model::current_model = &MP_model::default_model;

}

unsigned int MP_model::getSeed() {
	return uniformGenerator->seed;
}

MP_model& MP_model::add(MP_constraint& constraint) {
    Constraints.insert(&constraint);
    return *this;
}

void MP_model::add(MP_constraint* constraint) { //Counts total number of rows in this model and sets correct offsets
    assert(constraint != 0);
    constraint->M = this;
    if (constraint->left.isDefined() && constraint->right.isDefined()) {
        constraint->offset = m;
        m += constraint->size();
    }
}

void MP_model::setProbabilities(const MP_data& prob){
    // set probabilities
    if (scenSet.size() == 0)
        throw invalid_argument_exception(); //We need to set MP_scenario_set first..
    MP_scenario_set* scenPtr = dynamic_cast<MP_scenario_set*> (const_cast<MP_set_base*>(&prob.S1));
    if (scenPtr != 0){ //Cast was succesfull..
        // Now we can set values. Check for correct probabilities also..
        probabilities.clear();
        probabilities.reserve(scenSet.size());
        double sum = 0;
        for (int i = 0; i<scenSet.size();i++){
            double probability = prob.v[prob.f(i)];
            probabilities.push_back(probability);
            sum += probability;
        }
        if ( !compareDouble(sum,1.0)) //double equal 
            throw invalid_argument_exception(); //Probabilities do not sum up to one
    }
    else {
        throw invalid_argument_exception(); // We call Probabilities but no scenario_set is specified for this MP_data prob.
    }
}

std::vector<double> MP_model::getProbabilities(){
    // set probabilities
    return probabilities;
}

double MP_model::getInfinity() const {
    if (Solver==0) {
        return std::numeric_limits<double>::infinity();
    } else {
        return Solver->getInfinity();
    }
}

void MP_model::add(MP_variable* v) { //Counts total number of variables in this model
    assert(v != 0);
    v->M = this;
    v->offset = n;
    n += v->size();
}

void MP_model::addRow(const Constraint& constraint) {
    vector<MP::Coef> cfs;
    vector<TerminalExpression*> v;
    MP::GenerateFunctor f(0,cfs);
    constraint->left->generate(MP_domain::getEmpty(),v,f,1.0);
    constraint->right->generate(MP_domain::getEmpty(),v,f,-1.0);
    CoinPackedVector newRow;
    double rhs = 0.0;
    for (unsigned int j=0; j<cfs.size(); j++) {
        int col=cfs[j].col;
        //int row=cfs[j].row;
        double elm=cfs[j].val;
        //cout<<row<<"   "<<col<<"  "<<elm<<endl;
        if (col>=0) {
            newRow.insert(col,elm);
        } else if (col==-1) {
            rhs = elm;
        }
    }
    // NB no "assembly" of added row yet. Will be done....

    double local_bl = -rhs;
    double local_bu = -rhs;

    assert(Solver);
    double inf = Solver->getInfinity();
    switch (constraint->sense) {
      case LE:
          local_bl = - inf;
          break;
      case GE:
          local_bu = inf;
          break;
      case EQ:
          // Nothing to do
          break;          
    }

    Solver->addRow(newRow,local_bl,local_bu);
}

void MP_model::setObjective(const MP_expression& o) { 
    Objective = o; 
}

void MP_model::minimize_max(MP_set &s, const MP_expression  &obj) {
    MP_variable v;
    MP_constraint constraint(s);
    add(constraint);
    constraint(s) = v() >= obj;
    minimize(v());
} 


void MP_model::assemble(vector<MP::Coef>& v, vector<boost::shared_ptr<MP::Coef> >& av) {
    std::sort(v.begin(),v.end(),&MP::CoefLess);
    int c,r,s,rs;
    double val;
    std::vector<double> scenVector;
    std::vector<MP::Coef>::const_iterator i = v.begin();
    while (i!=v.end()) {
        c = i->col;
        r = i->row;
        val = i->val;
        s = i->varStage;
        rs = i->randomStage;
        scenVector = i->scenVector;
        ++i;
        //In case of RHS with a constant term consisting of more than one constant
        //These values were added one after another, otherwise this does not make sense.
        while (i!=v.end() && c==i->col && r==i->row) {
            val += i->val;
            if (i->varStage>0) {//If Coefficient was added with a random variable..
                //This is for right-hand-side addition
                s = i->varStage;
                //We do not add random variables, so we better do not have a valid pointer yet
                //Add values of other scenarios
                assert(scenVector.size() == i->scenVector.size());
                for (int j = 0; j < scenVector.size(); j++){
                    scenVector[j] = scenVector[j] + (i->scenVector)[j];
                }
            }
            ++i;
        }
        boost::shared_ptr<MP::Coef> temp(new MP::Coef(c,r,val,s,rs,scenVector));
        av.push_back(temp);
    }
}

void MP_model::maximize() {
    if (Solver!=0) {
        attach(Solver);
        solve(MP_model::MAXIMIZE);
    } else {
        LOG(ERROR) << "FlopCpp Error: no solver specified"<<endl;
    }
}

void MP_model::maximize(const MP_expression &obj) {
    if (Solver!=0) {
        Objective = obj;
        attach(Solver);
        solve(MP_model::MAXIMIZE);
    } else {
        LOG(ERROR) << "FlopCpp Error: no solver specified" << endl;
    }
}

//void MP_model::solveWS(){
//    if (this->getStatus() == MP_model::ATTACHED){
//
//
//    }
//    else {
//        cout << "Model not yet attached" << endl;
//    }
//
//}

void MP_model::minimize() {
    if (Solver!=0) {
        attach(Solver);
        solve(MP_model::MINIMIZE);
    } else {
        DLOG(WARNING) <<"no solver specified"<<endl;
    }
}

void MP_model::minimize(const MP_expression &obj) {
    if (Solver!=0) {
        Objective = obj;
        attach(Solver);
        solve(MP_model::MINIMIZE);
    } else {
        DLOG(WARNING) <<"no solver specified"<<endl;
    }
}

void MP_model::attachStochastic(){
    if (stage.size() <= 1) //In case of no defined stage set, we stop.
        return;
    if (Solver == 0)
        throw invalid_argument_exception();

    // Maybe we have changed RandomVariables, so we have to empty the RV set and fill it again
    for (int i = 0; i < RandomVariables.size(); i++){
        RandomVariables[i].clear();
    }
    for (conIt i=Constraints.begin(); i!=Constraints.end(); ++i)
        (*i)->insertRandomVariables(RandomVariables);

    // Resample values
    sampleRandomVariates(RandomVariables);

    vector<boost::shared_ptr<MP::Coef> > coefs;
    // TODO
    vector<MP::Coef> cfs;

    // Generate new coefficients
    for (conIt i=Constraints.begin(); i!=Constraints.end(); i++) {
        (*i)->coefficients(cfs);
        //Sort and Store Coefficients of current constraint and add them to constraint set coefs.
        assemble(cfs,coefs);
        cfs.erase(cfs.begin(),cfs.end());
    }
    // Initialize vector that stores shared ptrs to Coefs that are result of a combination of random variates.
    vector< vector<boost::shared_ptr<MP::Coef> > > randomCoefs(stage.size()+1);//we store nStages, but without first place, so nStages+1 places are needed
    //In case of a stage set we need to add all random coefficients to randomCoefs, to cope with them later. 
    if (stage.size()){

        //Generate random bounds for all variables TODO
        //for (varIt j=Variables.begin(); j!=Variables.end(); j++) {
        //    (*j)->bounds(randomCoefs);
        //}
        // Add all other random coefs to randomCoefs.
        for ( int i = 0; i != coefs.size(); i++){
            if ( !coefs[i]->scenVector.empty() ) { //We have scenario values
                //We have a random parameter with values
                // We need to deal with right hand side
                randomCoefs[coefs[i]->randomStage].push_back(coefs[i]);
            }
        }

        // As these bounds were generated in the variables section, the columns are wrong. This means we have to update that. 
        //IMPORTANT: This differs from attach method as the coefficients already have correct colIndirection
        for (int i = 0; i < randomCoefs.size();i++) {
            for (int j = 0; j < randomCoefs[i].size();j++) {
                if (randomCoefs[i][j]->col == -1)
                    randomCoefs[i][j]->col = n; //This is not very consistent with realn..
                assert( colIndirection[randomCoefs[i][j]->col] != outOfBound);
                if (colIndirection[randomCoefs[i][j]->col] != -1 ) //In case of RHS?!
                    randomCoefs[i][j]->col = colIndirection[randomCoefs[i][j]->col];
            }
        }
    }//end if stage

    // Generate objective function coefficients
    vector<TerminalExpression*> v;
    MP::GenerateFunctor f(0,cfs);
    coefs.erase(coefs.begin(),coefs.end());
    Objective->generate(MP_domain::getEmpty(), v, f, 1.0);
    messenger->objectiveDebug(cfs);
    assemble(cfs,coefs);
    //add random coefficients (if any) and we are in the stochastic setting
    if (stage.size()){
        for ( int i = 0; i != coefs.size(); i++){
            //Set correct rowIndex and colIndex via colIndirection
            assert(colIndirection[coefs[i]->col] != outOfBound);
            coefs[i]->col = colIndirection[coefs[i]->col];
            if (coefs[i]->row == -1){ //Objective function 
                coefs[i]->row = m;
            }
            //Add coefficient to Random Coefficients
            if ( !coefs[i]->scenVector.empty() ) { //We have scenario values
                randomCoefs[coefs[i]->randomStage].push_back(coefs[i]);
            }
        }
    }


    // Feed the random coefficients to a generate Scenario tree method
    smiModel = generateScenarioTree(randomCoefs,smiModel->getCore());

}



void MP_model::attach(OsiSolverInterface *_solver) { //TODO: give pointer to sample-function?!
    if (_solver == 0) {//No solver was given
        if (Solver == 0) {// No solver was set: Attachment is not possible, so we return and set state to detached.
            mSolverState = MP_model::DETACHED;
            return;
        }  
    } else {  // use pre-attached solver.
        if(Solver && Solver!=_solver) {
            detach(); //If old solver is not new solver, go delete the old solver and continue to work with the new one
        }
        Solver=_solver;
    }
    // In case we have an already attached (or solved) model AND we are in the stochastic setting: call attachStochastic
    if (mSolverState != MP_model::DETACHED && mSolverState != MP_model::SOLVER_ONLY && stage.size() > 1 ){
        attachStochastic();
        return;
    }

    double time = CoinCpuTime();
    m=0;
    n=0;

    typedef std::set<MP_variable* >::iterator varIt;
    typedef std::set<MP_constraint* >::iterator conIt;

    if (RandomVariables.empty())
        RandomVariables = std::vector<std::set<RandomVariable*> >(stage.size());

    //Insert decision variables into list of variables
    Objective->insertVariables(Variables);
    if (stage.size()){ //if stochastic insert Random Variables
        Objective->insertRandomVariables(RandomVariables);
    }
    //Count number of columns and rows in the add methods. 
    //Insert the remaining variables not in the objective to the list of variables stored in container Variables.
    for (conIt i=Constraints.begin(); i!=Constraints.end(); ++i) {
        add(*i);
        if (stage.size()) // if stochastic insert Random Variables
            (*i)->insertRandomVariables(RandomVariables);
        (*i)->insertVariables(Variables);
    }
    //Count the number of real variables in the model with the add Method. 
    // The variables in the Variables set are inserted from constraints and objective function.
    for (varIt j=Variables.begin(); j!=Variables.end(); ++j) {
        add(*j); 
    }// We have number of rows and number of columns at this point

    // We need to generate all the values we need at this point, if they are not already present.
    // For now: Simple Check: All randomVariables are either ScenarioRandomVars OR (exclusive) Independently distributed RandomVars. (This check is done in sampleRandomVariates method).
    sampleRandomVariates(RandomVariables);

    // Generate coefficient matrix and right hand side. All Variables and Constraints needs their offset for that to work
    //At first, generate coefficients for every constraint and store them in coefs vector: Assigns column and row number
    vector<boost::shared_ptr<MP::Coef> > coefs;
    vector<MP::Coef> cfs;
    conIt it;
    //We can do this in different ways, according to which system we use..
    //#pragma omp parallel private(it,cfs,coefs)

    //{
        //#pragma omp for 
    double timeBegin = CoinCpuTime();
    for (conIt it=Constraints.begin(); it!=Constraints.end(); ++it) {
        (*it)->coefficients(cfs);
        messenger->constraintDebug((*it)->getName(),cfs);
        //Sort and Store Coefficients of current constraint and add them to constraint set coefs.
        //#pragma omp critical 
        //{
        assemble(cfs,coefs);
        //}
        cfs.clear();
        //}//End parallel
    }
    DLOG(INFO) << "Time for coefficient generation: " << CoinCpuTime()-timeBegin << "s";
    nz = coefs.size();
    realn = n;

#pragma region colIndirection;
    if (stage.size()){
        //At this point we have all coefficients for the matrix. We need to sort them by stage, if we are in the stochastic setting and keep the old ordering to be able to set correct values for FlopC++ calls
        // TODO: Column indirection is not necessary for sucess, but it helps to eliminate empty variables 
        std::sort(coefs.begin(),coefs.end(),&MP::CoefLessWithStageShared);
        //Initialize all values with outOfBound to prevent misuse
        colIndirection = new int[n+1]; //store all variables (n) starting from index 0 and rhs as value n => n+1 size needed to store all these values
        for (int i = 0; i < n+1; i++){
            colIndirection[i] = outOfBound;
        }
        //Iterate over set to set correct indices in the coefficients and store the old indices in indirection array. This way we get correct column indices. Rows are not sorted by then
        int j = 0; //index counter
        for (int i = 0; i < nz; i++){
            int oldCol = coefs[i]->col;
            if (oldCol == -1)//Handle RHS
                oldCol = n;
            if ( colIndirection[oldCol] == outOfBound ){ //this Coefficient is not already assigned to a new index, store new index
                if (oldCol == n){//Handle RHS
                    colIndirection[oldCol] = -1;
                }
                else {
                    colIndirection[oldCol] = j++;
                }
            }
            coefs[i]->col = colIndirection[oldCol]; //Assign new value in case of correct column.
        }
        //Check for empty variables, i.e. variables not assigned in the problem.
        //TODO: Write flopc++ presolver to eliminate empty variables in the formulation prior to scenario generation.

        if (j != n){
            assert(j <n);
            realn = j;
            colIndirection[n] = realn; // Set correct new index for RHS
            DLOG(INFO) << "We have empty variables in this formulation";
            // We have to g
            //assert(j == n);
        }

        //Now we can access the new column ordering via colIndirection from FlopC++.
        //Now we changed coefficients and we should sort the coefs again, so, that they obey the new ordering
        //We need to extract all the random Coefficients that belong to the variable bounds and store them in the random coefficients array.
        std::sort(coefs.begin(),coefs.end());

    }
#pragma endregion colIndirection;

    messenger->statistics(Constraints.size(),m,Variables.size(),n,nz);

#pragma region coreModelGeneration
    ////
    // Generate Core Model
    ////

    //nz should only count number of elements in the matrix and not the RHS also..
    realnz = nz;
    Cst = new int[realn+2];   
    Clg = new int[realn+1];   
    if (n>0) {
        l =   new double[realn];  
        u =   new double[realn];  
        c =  new double[realn+1]; //The +1 accounts for the RHS term in the objective 
        if (stage.size()){
            colStage = new int[realn+1]; //Col Stage of RHS is not necessary..
            for (int i=0; i<realn+1; i++) {
                colStage[i] = 0;
            }
        }
    }
    if (m>0) {
        bl  = new double[m];  
        bu  = new double[m];
        if (stage.size()){
            rowStage = new int[m];
            for (int i=0; i<m; i++) {
                rowStage[i] = 0;
            }
        }
    }
    const double inf = Solver->getInfinity();
    // Compute Column Length Array to easily compute start array from there
    for (int j=0; j<=realn; j++) {
        Clg[j] = 0;
    }
    // Treat right hand side as n'th column
    for (int j=0; j<=realn; j++) {
        Clg[j] = 0;
    }
    for (int i=0; i<nz; i++) { //count occurence of a variable (in how many rows is a variable present?
        int col = coefs[i]->col;
        if (col == -1){ //If col is -1 it is the right hand side. We set col to n in the following and reduce number of realnz in matrix by 1
            realnz--;
            col = realn;
            coefs[i]->col = realn; //Also set coefficient to n to do it permanently
        }
        if (coefs[i]->row >= 0) //We have a correct variable here that occurs in a row 
            Clg[col]++;
        //In the else case we do not want to count the coefficient here, but we want to reduce number of realnz: We have a random variable bound
        else 
            realnz--;
    }
    //Compute Column Start Array
    Cst[0]=0; //count number of non-zero-elements up to column i: cst[n+1] = anzahl an nicht-null-elementen
    for (int j=0; j<=realn; j++) {
        Cst[j+1]=Cst[j]+Clg[j]; 
    }
    for (int i=0; i<=realn; i++) { //Reset column length array, to compute Elm and Rnr 
        Clg[i]=0;
    }
    if (realnz>0) {    
        Elm = new double[realnz]; 
        Rnr = new int[realnz];    
    }
    // Compute row bounds, that are explicitly stored in the elements array as the right-hand-side
    for (int i=0; i<m; i++) {
        bl[i] = 0;
        bu[i] = 0;
    }

    // In the stochastic setting.
    if (stage.size()){ 
        // Set correct Row Stage for every row by iterating over all elements
        // Row Stage is equal to the highest stage of a belonging variable
        for (int i=0; i<nz; i++) { //Set row and column stage in corresponding arrays
            int s = coefs[i]->varStage; //Stage of Variable that belongs to current coefficient
            assert( coefs[i]->col >= 0 && coefs[i]->col <= realn );
            colStage[coefs[i]->col] = s; //Set correct colStage. Must be the same for every coefficient for the same variable
            int row = coefs[i]->row;
            if ( row >= 0 && s > rowStage[row] )  {
                rowStage[row] = s;
            }
        }
        //Now we need to sort the rows by stage
        std::vector<int> rowCounter(stage.size(),0); //Store overall number of rows of a stage
        std::vector<int> rowOffset(m); //store offset of current row in a stage
        std::vector<int> tempRowStage(m);
        rowIndirection = new int[m];
        for (int i = 0; i < m; i++){
            rowOffset[i] = rowCounter[rowStage[i]]++; //In the end, rowCounter gives the number of rows in a stage.
            tempRowStage[i] = rowStage[i];
            rowIndirection[i] = outOfBound;
        }
        for (int i = 0; i < m; i++)//Reset rowStage. Empty rows are now rows with stage 0
            rowStage[i] = 0;
        //Now reset the rowIndex in the coefficient set (and RandomVars..)
        //Iterate over every coefficient. Set old coefficient to new coefficient
        //Something goes wrong here..
        for (int i = 0; i < nz; i++){
            int oldRow = coefs[i]->row;
            if (oldRow < 0) //In the case of Random coefficient for variable bound, continue with next coeff
                continue;
            int curRowStage = tempRowStage[oldRow];
            //Count number of rows before this row
            int rowCount = 0;
            for (int j = 0; j < curRowStage;j++){
                rowCount += rowCounter[j];
            }
            coefs[i]->row = rowCount + rowOffset[oldRow]; //compute correct offset
            rowStage[coefs[i]->row] = curRowStage; //Set rowStage of new indexed row to the stage of the row
            rowIndirection[oldRow] = coefs[i]->row;
        }
    }
    // Are there any empty rows? That means rows with no coefficient assigned to them? Is this possible?
    // Yes it is possible. One example: Constraint over stage set T but indexed over T+1 so first stage constraint is empty. 
    // Handled like this: every time rowIndirection is used we check for outOfBound.
    // TODO: Write presolver that eliminates empty rows.
    // for (int i = 0; i < m; i++) {
    //    if( rowIndirection[i] == outOfBound) { // We have an empty row. Can we delete that row without messing everything up?!
    //        //Assign row index of an empty row. Not so easy to do..
    //    }
    //}

    // Now we can access the new column/row ordering via colIndirection/rowIndirection from FlopC++ later on.
    // Coefficients store new ordering.


    //Compute Element and Index Array (to which row does this coefficient belong) with aid of the column-start and the column lengths
    // Also compute row bounds on the way (they are stored as rhs).
    for (int i=0; i<nz; i++) {
        int col = coefs[i]->col;
        int row = coefs[i]->row;
        double elm = coefs[i]->val;
        if (col == realn){ //RHS:
            bl[row] = -elm;
            bu[row] = -elm;
        }
        else {
            Elm[Cst[col]+Clg[col]] = elm;
            Rnr[Cst[col]+Clg[col]] = row;
        }
        Clg[col]++;
    }

    //Depending on the sense of the constraint, create less/greater/equal constraint via setting of the boundLower and boundUpper Arrays
    //TODO: Empty row handling done here..
    for (conIt i=Constraints.begin(); i!=Constraints.end(); i++) {
        if ((*i)->left.isDefined() && (*i)->right.isDefined() ) {
            int begin = (*i)->offset;
            int end = (*i)->offset+(*i)->size();
            switch ((*i)->sense) {
                  case LE:
                      for (int k=begin; k<end; k++) {
                          if (stage.size()){
                              if (rowIndirection[k] != outOfBound) //Else empty row
                                  bl[rowIndirection[k]] = - inf;
                          }
                          else
                              bl[k] = -inf;
                      } 
                      break;
                  case GE:
                      for (int k=begin; k<end; k++) {
                          if (stage.size()){
                              if (rowIndirection[k] != outOfBound) //Else empty row
                                  bu[rowIndirection[k]] = inf;
                          }
                          else
                              bu[k] = inf;
                      }     
                      break;
                  case EQ:
                      // Nothing to do
                      break;                
            }
        }
    }

    // Compute column bounds. 
    //Initialize all Variables with standard bounds.
    for (int j=0; j<realn; j++) {
        l[j] = 0.0;
        u[j] = inf;
    }
    //Set Bounds obtained via MP_variable via direct access
    // Furthermore: Variables obtain solver infinity bounds
    const double numericInfinity = std::numeric_limits<double>::infinity();
    for (varIt i=Variables.begin(); i!=Variables.end(); i++) {
        for (int k=0; k<(*i)->size(); k++) {
            if (stage.size()){ //In case of stochastic, we have to obey colIndirection
                if (colIndirection[(*i)->offset+k] == outOfBound)
                    continue; //go to next round, we have empty variable here
                //Evaluate values to mean value in case of stochastic and to real value in case of normal bounds
                l[colIndirection[(*i)->offset+k]] = (*i)->lowerLimit.v[k];
                u[colIndirection[(*i)->offset+k]] = (*i)->upperLimit.v[k];
                //Update values to solver infinity if we find numeric infinity.
                if (l[colIndirection[(*i)->offset+k]] == -numericInfinity)
                    l[colIndirection[(*i)->offset+k]] = -inf;
                if (u[colIndirection[(*i)->offset+k]] == numericInfinity)
                    u[colIndirection[(*i)->offset+k]] = inf;

            }
            else {
                l[(*i)->offset+k] = (*i)->lowerLimit.v[k];
                u[(*i)->offset+k] = (*i)->upperLimit.v[k];
                if (l[(*i)->offset+k] == -numericInfinity)
                    l[(*i)->offset+k] = -inf;
                if (u[(*i)->offset+k] == numericInfinity)
                    u[(*i)->offset+k] = inf;
            }
        }
    }

    // Initialize vector that stores shared ptrs to Coefs that are result of a combination of random variates.
    vector< vector<boost::shared_ptr<MP::Coef> > > randomCoefs(stage.size()+1);//we store nStages, but without first place, so nStages+1 places are needed
    //In case of a stage set we need to add all random coefficients to randomCoefs, to cope with them later. 
    if (stage.size()){

        //TODO Generate random bounds for all variables
        //for (varIt j=Variables.begin(); j!=Variables.end(); j++) {
        //    (*j)->bounds(randomCoefs);
        //}
        // As these bounds were generated in the variables section, the columns are wrong. This means we have to update that 
        //for (int i = 0; i < randomCoefs.size();i++) {
        //    for (int j = 0; j < randomCoefs[i].size();j++) {
        //        assert( colIndirection[randomCoefs[i][j]->col] != outOfBound);
        //        randomCoefs[i][j]->col = colIndirection[randomCoefs[i][j]->col];
        //    }
        //}
        // Add all other random coefs to randomCoefs.
        for ( int i = 0; i != coefs.size(); i++){
            if ( !coefs[i]->scenVector.empty() ) { //We have scenario values
                //We have a random parameter with values
                randomCoefs[coefs[i]->randomStage].push_back(coefs[i]);
            }
        }
    }//end if stage


    // Generate objective function coefficients
    vector<TerminalExpression*> v;
    MP::GenerateFunctor f(0,cfs);
    coefs.erase(coefs.begin(),coefs.end());
    Objective->generate(MP_domain::getEmpty(), v, f, 1.0);
    messenger->objectiveDebug(cfs);
    assemble(cfs,coefs);
    //add random coefficients (if any) and we are in the stochastic setting
    if (stage.size()){
        for ( int i = 0; i != coefs.size(); i++){
            //Set correct rowIndex and colIndex via colIndirection
            LOG_IF(INFO,colIndirection[coefs[i]->col] == outOfBound) << "Some of your variables in your objective function do not appear in your model. Please check your model for validity. Take a closer look at indexed variables.";
            assert(colIndirection[coefs[i]->col] != outOfBound); // Asssert that all variables in the objective function already appear in a constraint.
            if (coefs[i]->col == -1){ //Handle constant terms in the objective function later (for now set coefficient value)
               coefs[i]->col = colIndirection[n]; //or realn
            }
            else {
                coefs[i]->col = colIndirection[coefs[i]->col];     
            }
            if (coefs[i]->row == -1){ //Objective function 
                coefs[i]->row = m;
            }
            //Add coefficient to Random Coefficients
            if ( !coefs[i]->scenVector.empty() ) { //We have scenario values
                randomCoefs[coefs[i]->randomStage].push_back(coefs[i]);
            }
            // Check if we have random parameters in every stage. Otherwise weird behaviour can occur while trying to load it into the solver.
            for (int i = 1; i < stage.size(); i++){
                LOG_IF(ERROR, randomCoefs[i].empty()) << "FlopCpp Error: there are no Random Parameters defined for stage " << i+1 << ".";
            }
        }
    }
    else {
        for (int i = 0; i != coefs.size(); i++){ 
            coefs[i]->row = m;
        }
    }

    for (int j=0; j<=realn; j++) {
        c[j] = 0.0;
    }
    //Set objective coefficients
    for (size_t i=0; i<coefs.size(); i++) {
        int col = coefs[i]->col;
        if ( coefs[i]->row == m){
            double elm = coefs[i]->val;
            c[col] += elm; // a simple = should to the trick, as all coefficents are already added during the assemble step.
        }
    } 


    // Set integer information..
    std::vector<int> integerIndices;
    std::vector<int> binaryIndices; // Contains only binary information..
    for (varIt i=Variables.begin(); i!=Variables.end(); i++) {
        int begin = (*i)->offset;
        int end = (*i)->offset+(*i)->size();
        if ( (*i)->type == type::discrete || (*i)->type == type::binary ) {
            for (int k=begin; k<end; k++) {
                if (stage.size()){
                    if (colIndirection[k] != outOfBound){
                        integerIndices.push_back(colIndirection[k]);
                        if ( (*i)->type == type::binary)
                            binaryIndices.push_back(colIndirection[k]);
                    }
                }
                else{
                    integerIndices.push_back(k);
                    if ( (*i)->type == type::binary)
                        binaryIndices.push_back(k);
                }
            }
        }
    }
    std::sort(integerIndices.begin(),integerIndices.end()); //Sort indices in ascending order
    std::sort(binaryIndices.begin(),binaryIndices.end()); //Sort indices in ascending order

    ////
    // Core Model generated..
    // All rows and cols are sorted by stage.
    // RHS = n, Objective = m (indices)
    // Old indices are stored in colIndirection and rowIndirection
    ////
#pragma endregion coreModelGeneration


    //Check for stochasticity: This is the case if we have a stage set with size greater than 0.
    if (stage.size() > 0){
        ////
        // Feed Core Model to SmiScnModel
        ////
        // Generate Core ModelMatrix: Need Only the matrix, no rhs, so we need realnz (Elm,Rnr and Cst are too big..)	
        CoinPackedMatrix matrix(true,m,realn,realnz,Elm,Rnr,Cst,NULL); //Matrix without right-hand-side..
        matrix.reverseOrdering(); //Smi needs matrix in row ordered form
        //TODO: Debug switch
        // DLOG(INFO) << "Core-Matrix" << std::endl;
        // DLOG(INFO) << printMatrix(&matrix,realn,m,l,u,c,bl,bu,colStage,rowStage);
        
        //matrix.removeGaps(); //What is this for?
        Solver->loadProblem(matrix, l, u, c, bl, bu); //Generate Core Data, in Solver
        SmiCoreData *osiCore = 0; //Create CoreData Node. Needs to be deleted in case of stochastic solver.
        if (!integerIndices.empty()){ //suffices, because every binary is also an integer
            Solver->setInteger(&integerIndices[0],integerIndices.size());
            osiCore = new SmiCoreData(Solver,stage.size(),colStage,rowStage,&integerIndices[0]); //Create CoreData Node. Needs to be deleted in case of stochastic solver.
            if (!binaryIndices.empty())
                osiCore = new SmiCoreData(Solver,stage.size(),colStage,rowStage,&integerIndices[0],integerIndices.size(), &binaryIndices[0],binaryIndices.size()); 
            else
                osiCore = new SmiCoreData(Solver,stage.size(),colStage,rowStage,&integerIndices[0],integerIndices.size()); 
        }
        else {
            osiCore = new SmiCoreData(Solver,stage.size(),colStage,rowStage); //Create CoreData Node. Needs to be deleted in case of stochastic solver.
        }

        // Get SmiScnModel that holds a generated ScenarioTree.
        smiModel = generateScenarioTree(randomCoefs,osiCore);
        // Check if we eliminated RV in some stages ( could be
        if (smiModel == 0)
            //We were not sucessful in creating a scenario tree. Try normal operation. TODO: Think about reversed matrix..
            Solver->loadProblem(n,m,Cst,Rnr,Elm,l,u,c,bl,bu);
        else
            smiModel->Core(osiCore); //Let smiModel handle disposal of SmiCoreData
    }
    else {
        Solver->loadProblem(n, m, Cst, Rnr, Elm, l, u, c, bl, bu);
        if (!integerIndices.empty())
            Solver->setInteger(&integerIndices[0],integerIndices.size());
        //printMatrix( const_cast<CoinPackedMatrix*>(Solver->getMatrixByRow()),n,m,l,u,c,bl,bu,colStage,rowStage);
    }

    mSolverState = MP_model::ATTACHED;

#pragma region cleanUp;
    //Clean Up
    //ColIndirection and rowIndirection arrays may be needed later on
    if (nz>0) {    
        delete [] Elm; 
        delete [] Rnr;    
    }
    delete [] Cst;   
    delete [] Clg;   
    if (n>0) {
        delete [] l;  
        delete [] u;  
        delete [] c;
    }
    if (m>0) {
        delete [] bl;  
        delete [] bu;
    }  
#pragma endregion cleanUp;



    messenger->generationTime(CoinCpuTime()-time);
}

//TODO: Implement using MultiArray from boost?!
void MP_model::sampleRandomVariates(std::vector<std::set<RandomVariable*> >& rv){
    if (stage.size() <= 1) { //Incorrect stage set, return
        return;
    }
    //More sophisticated check
    bool scenarios = false;
    bool independent = false;
    for (std::vector<std::set<RandomVariable*> >::iterator it = rv.begin(); it != rv.end();++it){
        for ( std::set<RandomVariable*>::iterator rit = (*it).begin();rit != (*it).end();++rit){
            ScenarioRandomVariable* scenVar = dynamic_cast<ScenarioRandomVariable*> (*rit);
            if (scenVar == 0){
                //We do not have a scenarioVar, we have something else, this means:
                independent = true;
            }
            else if (scenVar != 0){ //We have scenarios in here, we do not want this to happen.
                scenarios = true;
            }
        }
    }
    if (scenarios && independent){
        DLOG(INFO) << "ScenarioRandomVariables and normal RandomVariables where specified. We do not know how to handle this.";
        throw invalid_argument_exception(); //User has to specify what he wants, we can not guess
    }
    if (scenarios && !independent){ //In case of scenarios, we return as we have nothing to do here. But: We assure that probabilities are set in a correct way, if not already done by the user.
        if (probabilities.empty()){ //If probabilities are not of the correct size, we are wrong, probably a user error (assert)
            probabilities = std::vector<double>(scenSet.size(),1.0/scenSet.size());
        }
        CHECK_EQ(probabilities.size(),scenSet.size()) << "You have a different number of probabilities compared to your scenario set.";
        return;
    }

    // Set sample size common to all random variables.
    int sampleSize = this->defaultSampleSize;
    typedef std::set<RandomVariable*>::iterator rvIterator;

    std::vector<int> randVarsPerStage(stage.size(),0);
    //For every stage: We have stage independence. Sample concrete values for this current stage, combine them all-against-all. Insert them in the right places to obtain correct scenario values..
    std::vector< std::vector< std::vector<double> > > scenariosPerStage(stage.size()+1); //Ordering of values via RV ordering.

    // For every stage: Sample Values for all RV in this stage independently and combine all-against-all to achieve correct behaviour. In case of sampleOnly flag, we just sample and leave it with that.
    for( int i = 1; i < stage.size(); i++){
        // Sample values for every variable independently. We might not have equally probable scenarios.
        std::vector< RandomVariable::sampleVector > sampledValuesOfVars;

        DLOG_IF(INFO,rv[i].empty()) << "There are no random variables in this stage, but you specified a stage set. Please check your model";
        assert(!rv[i].empty()); //Assume we have random variables in every stage. If not specifying a stage set does not make sense.
        for (rvIterator it = rv[i].begin(); it != rv[i].end();++it){
            (*it)->sample(sampleSize);
            randVarsPerStage[i]++;
            sampledValuesOfVars.push_back((*it)->getSampledValuesWithProb());
        }
        if (sampleOnly) //In case of sample only, we do not want to combine all against all.
            continue;
        std::vector< RandomVariable::sampleVector::iterator > iteratorVector(randVarsPerStage[i]+1); //We need one more as we add an iterator to an empty map later on.
        //Ensure we have at least two random variables, otherwise a combine all against all does not makes sense..
        sampledValuesOfVars.push_back(std::map<double,double>());

        // Now we prepare the iterator vector. We do it here as otherwise we would invalidate the previous iterators
        DLOG(INFO) << "Random Variable in Stage " << i << " : " << randVarsPerStage[i] << std::endl;
        for (int j = 0; j < randVarsPerStage[i]+1; j++){
            iteratorVector[j] = sampledValuesOfVars[j].begin();
        }
        assert(iteratorVector.size() == sampledValuesOfVars.size());

        int nextVar = 1;
        int curElements = randVarsPerStage[i];
        int scenCounter = 0;
        // Push back empty vector to scenariosPerStage[i] to store scenario probabilities in there
        scenariosPerStage[i] = std::vector< std::vector<double> >();

        // Combine all-against-all for this stage. Include probabilities of previous stages
        // At first we go to the first value where the current value and the next sampled Value differ
        while (true){
            while ( iteratorVector[nextVar] != sampledValuesOfVars[nextVar].end() ){ // As long as we have not reached the empty map, everything is fine: Go to the next map.
                ++nextVar;
            }
            // we reached the end..

            --nextVar; //We incremented nextVar one too much, so go to correct value.
            //Do computational stuff only at the end
            if (nextVar == curElements-1){

                // Computation start: Compute probability of the given scenario and store scenario values
                scenariosPerStage[i].push_back(std::vector<double>(randVarsPerStage[i]+1));
                //Insert values
                double prob = 1.0;
                //Compute probability and insert values per randomvar to form a scenario.
                for (int j = 0; j < randVarsPerStage[i];j++){
                    scenariosPerStage[i][scenCounter][j] = ((*iteratorVector[j]).first);
                    prob *= (*iteratorVector[j]).second;
                }
                scenariosPerStage[i][scenCounter][randVarsPerStage[i]] = prob; //
                ++scenCounter;
                //Computation end

                // Increment iterator
                ++iteratorVector[nextVar];
            }
            else { //we reached the end of the iteration but we are not at the last map => Increment previous map and restart current map. Check in the next round with previous map.
                if (nextVar == -1)
                    break;
                // iteratorVector[nextVar] == sampledValuesOfVars[nextVar].end() holds 
                iteratorVector[nextVar+1] = sampledValuesOfVars[nextVar+1].begin();
                ++iteratorVector[nextVar];
            }
        } // end while
    } //end for
    if (sampleOnly){ //If we sampled only, we have equiprobable values and do not have to generate the scenario fan => resampling with preset probabilities does not makes sense..
        this->probabilities.clear();
        this->setScenSet(MP_scenario_set(sampleSize));//Important <- otherwise scenario generation does not work as coefs get initialized without scenario values
        probabilities = std::vector<double>(scenSet.size(),1.0/scenSet.size());
        return;
    }
    // Now we have all scenarios independent for all RandomVariables..
    // Overall scenario count: for all stage count size of vector scenariosPerStage[stage].size()
    // What we can do now: Generate scenario fan..
    std::vector< std::vector<double> > overallScenarios;
    std::vector< int > curScenario(stage.size()+1);
    int curStage = 1;
    int scenCounter = 0;
    assert(stage.size()>=2);
    while (true){
        //Go to last stage
        while ( curScenario[curStage] != scenariosPerStage[curStage].size() ){ // As long as we have not reached the empty map, everything is fine: Go to the next map.
            // Go with current scen to the next stage
            ++curStage;
        }
        // we reached the end..
        --curStage;
        //Do computational stuff only at the end
        if (curStage == stage.size()-1){

            //Computation start
            double prob = 1.0;
            ////Compute probability and insert values per randomvar to form a scenario.
            overallScenarios.push_back(std::vector<double>()); //TODO: Reserve some space?
            for (int j = 1; j < stage.size();j++){ // For all stages

                overallScenarios.back().insert(overallScenarios.back().end(),scenariosPerStage[j][curScenario[j]].begin(),--scenariosPerStage[j][curScenario[j]].end()); //Insert values for all RV in order from iterator given above, but do not insert probability of scenario.
                prob *= scenariosPerStage[j][curScenario[j]].back();

            }
            overallScenarios.back().push_back(prob);
            scenCounter++;
            //Computation end

            // Increment scenario index
            ++curScenario[curStage];

        }
        else { //we reached the end of the iteration but we are not at the last stage => Increment previous scenario counter and restart current scenario counter. Check in the next round with previous map.
            if (curStage == 0) //cur Stage get's reduced twice in the last step
                break;
            curScenario[curStage+1] = 0;
            ++curScenario[curStage];
        }
    } // end while

    // Scenario Fan generated. Fill RandomVariables with correct values
    int curRVIndex = 0;
    for( int i = 1; i < stage.size(); i++){ // For every stage
        for (rvIterator it = rv[i].begin(); it != rv[i].end();++it){ // For every RandomVariable: Assign values for all scenarios
            // Lambda-Expression? Build std::vector out of values and insert the vector in RV.
            std::vector<double> temp;
            temp.reserve(overallScenarios.size()); //Only reserve space, no initialization.
            for (int j = 0; j < overallScenarios.size(); j++) {
                temp.push_back(overallScenarios[j][curRVIndex]);
            }
            (*it)->setScenarioValues(temp);
            ++curRVIndex;
        }//end for rv
    }//end for stage
    //Prepare everything in the model: Set scenario set and add probabilities..
    this->setScenSet(MP_scenario_set(overallScenarios.size()));
    this->probabilities.clear();
    this->probabilities.reserve(overallScenarios.size());
    double sum = 0;
    for (int i = 0; i < overallScenarios.size(); i++){
        probabilities.push_back(overallScenarios[i].back());
        sum += probabilities.back();
    }
    //Check for consistency
    if ( !compareDouble(sum,1.0)) //double equal 
        throw invalid_argument_exception(); //Probabilities do not sum up to one
}

void MP_model::detach() {
    mSolverState = MP_model::DETACHED;
    if (Solver)
        delete Solver;
    Solver = 0;
    //Delete stochastic information. We have this, if we have a time structure AND we have attached a solver.
    if (stage.size() > 0 && smiModel != 0){ 
        smiModel->releaseSolver(); //TODO: This can be deleted, if stochasticSolverInterface is ready.
        delete smiModel;
        smiModel = 0;
        if (n > 0){
            delete [] colStage;
            delete [] colIndirection;
        }
        if (m > 0){
            delete [] rowStage;
            delete [] rowIndirection;
        }
        ////It might be necessary to do a fresh start..
        //scenSet = MP_scenario_set(0);
        //probabilities.clear();
        //RandomVariables.clear();
        //Variables.clear();
        //Constraints.clear();
    }


}

int * MP_model::getRowStage() {
    return rowStage;
}

int * MP_model::getColStage() {
    return colStage;
}

MP_model::MP_status MP_model::solve(const MP_model::MP_direction &dir) {
    assert(Solver);
    if ( mSolverState == MP_model::DETACHED || mSolverState == MP_model::SOLVER_ONLY )
        attach();
    assert(mSolverState != MP_model::DETACHED && mSolverState != MP_model::SOLVER_ONLY);
    Solver->setObjSense(dir);
    bool isMIP = false;
    for (varIt i=Variables.begin(); i!=Variables.end(); i++) {
        if ( (*i)->type == type::discrete || (*i)->type == type::binary ) {
            isMIP = true;
            break;
        }
    }
    // We want to inform the user about wall clock solving time..
    double startClock = CoinCpuTime();
    double stochasticClock = 0;
    if (stage.size() > 0 && smiModel != 0) { // We have a time structure AND a smiModel ready. If only timeStructure, there were no RandomVariables specified.
        smiModel->setOsiSolverHandle(Solver); //We create a copy of the solver with this method..
        // Delete current solver before we overwrite the Pointer with the new solver interface
        delete Solver;
        Solver = smiModel->loadOsiSolverData();
        Solver->setObjSense(dir); //Important: After loading solver data, objSense is not specified..
        stochasticClock = CoinCpuTime();        
        DLOG(INFO) << "Generation of deterministic equivalent took " << CoinCpuTime()-startClock << "s";
        try {
            if (isMIP == true)
                Solver->branchAndBound();
            else
                Solver->initialSolve();
        }  catch (CoinError e) {
            DLOG(ERROR) << e.message();
        }
    }
    else {
        if (isMIP == true) {
            try {
                Solver->branchAndBound();
            } catch  (CoinError e) {
                DLOG(ERROR) << e.message();
                DLOG(INFO) << "Solving the LP relaxation instead.";
                try {
                    Solver->initialSolve();
                } catch (CoinError e) {
                    DLOG(ERROR) << e.message();
                }
            }
        } else {
            try {
                Solver->initialSolve();
            }  catch (CoinError e) {
                DLOG(ERROR) << e.message();
            }
        }
    }
    DLOG(INFO) << "Overall wall clock time for solution : " << CoinCpuTime()-startClock << "s";
    if (stage.size())
        DLOG(INFO) << "Solution time for stochastic programs using deterministic equivalent: " << CoinCpuTime()-stochasticClock << "s";

    messenger->solutionStatus(Solver);

    if (Solver->isProvenOptimal() == true) {
        return mSolverState=MP_model::OPTIMAL;
    } else if (Solver->isProvenPrimalInfeasible() == true) {
        return mSolverState=MP_model::PRIMAL_INFEASIBLE;

    } else if (Solver->isProvenDualInfeasible() == true) {
        return mSolverState=MP_model::DUAL_INFEASIBLE;

    } else {
        return mSolverState=MP_model::ABANDONED;
    }
}

namespace flopc {
    std::ostream &operator<<(std::ostream &os, 
        const MP_model::MP_status &condition) {
            switch(condition) {
        case MP_model::OPTIMAL:
            os<<"OPTIMAL";
            break;
        case MP_model::PRIMAL_INFEASIBLE:
            os<<"PRIMAL_INFEASIBLE";
            break;
        case MP_model::DUAL_INFEASIBLE:
            os<<"DUAL_INFEASIBLE";
            break;
        case MP_model::ABANDONED:
            os<<"ABANDONED";
            break;
        default:
            os<<"UNKNOWN";
            };
            return os;
    }

    std::ostream &operator<<(std::ostream &os, 
        const MP_model::MP_direction &direction) {
            switch(direction) {
        case MP_model::MINIMIZE:
            os<<"MINIMIZE";
            break;
        case MP_model::MAXIMIZE:
            os<<"MAXIMIZE";
            break;
        default:
            os<<"UNKNOWN";
            };
            return os;
    }

    SmiScnModel* MP_model::generateScenarioTree(const std::vector< std::vector<boost::shared_ptr<MP::Coef> > >& randomCoefficientVector, SmiCoreData* smiCore){
        //TODO: Check with own test if scenario generation is correct. It looks like it, but we will never now until we test it.
        typedef std::vector<boost::shared_ptr<MP::Coef> >::const_iterator randVarIterator;
        //Get basic important values
        int nStages = smiCore->getNumStages();
        int nScen = scenSet.size(); //If nScen is not set, curStage.e. equal to 0, we need to count number of Scenarios 
        std::vector<int> randVarsPerStage(nStages);
        bool count = false;
        if (nScen==0) // We have no specified scenario set, so we rely on the default sample size and count the scenario size afterwards.
            count = true;
        for (int curStage = 1; curStage < nStages; curStage++){ //Count number of random coefficients for each stage..
            for (randVarIterator it = randomCoefficientVector[curStage].begin(); it != randomCoefficientVector[curStage].end();++it){
                randVarsPerStage[curStage]++;
            }
        }
        if (nScen == 0 || randVarsPerStage[1] == 0)//If we have nothing to do, we have nothing to. 
            return 0;
        //At this point we have a fan of values, represented by values inside each RV
        //Now we want to make a tree out of it
        int m = smiCore->getNumRows();
        int n = smiCore->getNumCols(); 
        // Initialize data structures needed later on
        std::vector<CoinPackedMatrix*> matrixVec(nScen); // Column Ordered Matrices
        std::vector<CoinPackedMatrix*> objVec(nScen); //Empty Column ordered matrices for objective function and row bounds.. Necessary as CoinPackedVector does not allow adding of elements
        std::vector<CoinPackedMatrix*> rloVec(nScen);
        std::vector<CoinPackedMatrix*> rupVec(nScen);
        std::vector<CoinPackedMatrix*> bloVec(nScen);
        std::vector<CoinPackedMatrix*> bupVec(nScen);
        for (int curScen = 0; curScen < nScen;curScen++){ //Think about that in terms of lazy evaluation..?!
            //Initialize all matrices
            matrixVec[curScen] = new CoinPackedMatrix();
            matrixVec[curScen]->reverseOrdering();
            matrixVec[curScen]->setDimensions(m,n);
            //Initialize all vectors
            objVec[curScen] = new CoinPackedMatrix();
            objVec[curScen]->setDimensions(n,1);
            rloVec[curScen] = new CoinPackedMatrix();
            rloVec[curScen]->setDimensions(m,1);
            rupVec[curScen] = new CoinPackedMatrix();
            rupVec[curScen]->setDimensions(m,1);
            bloVec[curScen] = new CoinPackedMatrix();
            bloVec[curScen]->setDimensions(n,1);
            bupVec[curScen] = new CoinPackedMatrix();
            bupVec[curScen]->setDimensions(n,1);

        }

        std::vector<int> tempAncScenVec(nScen,outOfBound); //Initalized Vector with outOfBound (not valid)
        std::vector<int> branchStageVec(nScen,outOfBound); //Initialized Vector with 0
        // Usage: Branch: stage in which scenario differs from ancestor scenario
        // => if we differ in stage 1 we need 0 as ancestor (or otherwise this does not makes so much sense)
        //set correct values for first scenario:
        tempAncScenVec[0] = 0;
        branchStageVec[0] = 1;

        std::vector<int> scenarioIndirection(nScen,outOfBound); //Get virtual (SMI) scenario by real scenario ordering
        std::vector<int> realOrderingFromVirtualScenario(nScen,outOfBound);
        if (probabilities.empty())
            probabilities = std::vector<double>(nScen,1.0/nScen); //If probability set is present, use that, otherwise use equiprobable values.

        typedef std::list<int>::iterator scenIt;
        std::list<int> scenarioHeap;

        for (int i = 0; i < nScen; i++)
            scenarioHeap.push_back(i);

        std::vector<std::deque<int> > subScenarioQueueVector(nStages,std::deque<int> ()); //For each stage an own queue
        subScenarioQueueVector[0].push_front(0);

        //We do not like recursion, therefore we need to store all the elements in a list of vectors and stuff
        std::vector< std::vector< std::list<int> > > subScenariosPerStageAndScenario(nStages, std::vector<std::list<int> > (nScen)); //TODO: Redo, this seems to be expensive..
        for (int j = 0; j < nScen; j++){ // Assign scenarioHeap to all the first stage sets. TODO: Improve memory consumption and thus running time due to allocation and deletion
            subScenariosPerStageAndScenario[0][j] = scenarioHeap;
        }


        int curStage = 0; //We start at stage 0
        int curScen = 0; //We start with scenario 0
        int scenCounter = 0; //We only need this counter in the very last round.
        //We do the following: Add all subnodes to this node to the subqueue list with their list of scenarios. Then go to the subqueue list and do the same.
        while( !subScenarioQueueVector[curStage].empty()){
            //Look at top node and remove it from the queue
            curScen = subScenarioQueueVector[curStage].front();
            subScenarioQueueVector[curStage].pop_front();
            // Loop over all subset scenarios of the current scenario to form new nodes
            while (!subScenariosPerStageAndScenario[curStage][curScen].empty()){
                int parentScen = *subScenariosPerStageAndScenario[curStage][curScen].begin(); //the parentScen will be the "parent" or head of the new node.
                //If the node is already a parent of a node in a lower stage, we keep the old values
                if (branchStageVec[parentScen] == outOfBound)
                    branchStageVec[parentScen] = curStage+1;
                if (tempAncScenVec[parentScen] == outOfBound)
                    tempAncScenVec[parentScen] = curScen;

                if (curStage < nStages-1){//Add to queue for the next stage, if we are not in the last stage
                    subScenarioQueueVector[curStage+1].push_back(parentScen);
                }
                else{ //In the last stage: increase scenario counter and set scenarioIndirection and delete the parent scenario from subScenarios so that only the same scenarios as the parentScen scenario are left
                    scenarioIndirection[parentScen] = scenCounter++;
                    realOrderingFromVirtualScenario[scenCounter -1] = parentScen;
                    subScenariosPerStageAndScenario[curStage][curScen].erase(subScenariosPerStageAndScenario[curStage][curScen].begin()); //Remove from subsetSet to only contain strict subset in the last stage: This ensures correct probability calculation
                    //probabilities[scenCounter-1] = probabilities[scenCounter-1];
                }

#pragma region setCoefficients
                //Do the real stuff: Add values for the current scenario.
                //Add all random coefficients to corresponding places
                for (int j = 0; j < randVarsPerStage[curStage];j++){  //Values differ 
                    boost::shared_ptr<MP::Coef> it = randomCoefficientVector[curStage][j];
                    double coefficient = it->scenVector[parentScen];
                    int colIndex = it->col;
                    int rowIndex = it->row;
                    if (colIndex < n) {//Matrix-Element or variable bounds
                        assert(rowIndex <= m); //matrix element or objective function or variable bound
                        if (rowIndex < 0){ //variable bound
                            assert(colIndex >= 0); //col index needs to be inside bounds
                            if (rowIndex == lowerBound)
                                bloVec[parentScen]->modifyCoefficient(colIndex,0,coefficient,true);
                            else if (rowIndex == upperBound)
                                bupVec[parentScen]->modifyCoefficient(colIndex,0,coefficient,true);
                            else
                                throw invalid_argument_exception();
                        }
                        else if (rowIndex < m){ //matrix element
                            assert(colIndex >= 0); //col index needs to be inside bounds
                            matrixVec[parentScen]->modifyCoefficient(rowIndex,colIndex,coefficient,true); // Keeps added zeroes. If set to false, zeroes get changed to ones
                        }
                        else{ //Objective Function
                            assert(rowIndex == m);
                            objVec[parentScen]->modifyCoefficient(colIndex,0,coefficient);
                        }
                    }
                    else{ //RHS : This means coefficient is multiplied with -1. TODO: Change this behaviour in FlopC++ so that also right hand side has original values.
                        assert(colIndex == n);
                        if ( rowIndex == m){ // We have a constant term in the objective..
                            objVec[parentScen]->modifyCoefficient(colIndex,0,coefficient);
                            continue; // move to the next round of the for loop
                        }
                        //We need the row sense to insert in the right vector..
                        switch (this->Solver->getRowSense()[rowIndex])
                        { 
                        case 'E' : rloVec[parentScen]->modifyCoefficient(rowIndex,0,-coefficient,true); rupVec[parentScen]->modifyCoefficient(rowIndex,0,-coefficient,true); break;
                        case 'G' : rloVec[parentScen]->modifyCoefficient(rowIndex,0,-coefficient,true); break;
                        case 'L' : rupVec[parentScen]->modifyCoefficient(rowIndex,0,-coefficient,true); break; 
                        case 'N' : break;
                        default : DLOG(FATAL) << this->Solver->getRowSense()[rowIndex] << " row sense of row " << rowIndex;
                            const char* rowSense = this->Solver->getRowSense();
                            throw invalid_argument_exception(); 
                        }//end switch
                    }//end else
                }//end for: randVars
#pragma endregion setCoefficients

                // Remove elements from subsetHeap that are equal to this one and add them to subset for next stage.
                for (scenIt innerIt = subScenariosPerStageAndScenario[curStage][curScen].begin(); innerIt != subScenariosPerStageAndScenario[curStage][curScen].end();){
                    if (curStage == nStages-1){
                        //We are in the last stage. All scenarios that still have the same values end up in the same scenario, but that increases the probability
                        //Remove the scenario from current stage and set realOdering to outOfBound
                        tempAncScenVec[*innerIt] = tempAncScenVec[parentScen];
                        scenarioIndirection[*innerIt] = outOfBound;
                        realOrderingFromVirtualScenario[scenCounter] = outOfBound;
                        probabilities[parentScen] += probabilities[*innerIt]; //add one scenario prob for each added scenario.
                        probabilities[*innerIt] = 0;
                        subScenariosPerStageAndScenario[curStage][curScen].erase(innerIt++);
                    }
                    // Define the new scenario subset node for the next stage
                    else if (!differFromPrevious(randomCoefficientVector,curStage+1,parentScen,*innerIt)){// We have a scenario subset node        
                        //Add to scenario subset heap of next stage node and scenario
                        subScenariosPerStageAndScenario[curStage+1][parentScen].push_back(*innerIt); 
                        //Erase from subsetHeap, so that this subset scenario does not form a new node in a subsequent run of the while loop
                       subScenariosPerStageAndScenario[curStage][curScen].erase(innerIt++);
                    }
                    else{ // Scenario is in another node, skip it for now. Will be looked at during the inner while loop
                        ++innerIt;
                    }
                }//end for
            }//end while: subScenariosPerStageAndScenario

            // All nodes for the current stages are done, turn to the next stage. If we are in the last stage, do nothing do leave outer while loop.
            if (subScenarioQueueVector[curStage].empty() && curStage < nStages-1){
                curStage++;
            }
        }//end while: deque

        //Generate scenarios in SMI
        // SMI expects a "correct" scenario tree ordering, so we reorder the scenarios first.
        //We need to use scenarioIndirection to get correct index for scenario i (natural ordering as expected from smi).
        std::vector<int> ancScenVec(scenCounter);
        for (int i = 0; i < nScen; i++){
            if (scenarioIndirection[i] != outOfBound) //If we are out of Bound this scenario is the same as another one.
                ancScenVec[scenarioIndirection[i]] = scenarioIndirection[tempAncScenVec[i]]; //Get program index for natural scenario i. Need to transfer program index to natural scenario index. 
        }
        if (smiModel){ //If we already have a smiModel we do not want to create a new one, but only throw away the scenario tree.
            smiModel->releaseCore(); //Keep Core Model.
            smiModel->releaseSolver(); //Keep Solver.
            delete smiModel;
        }

        SmiScnModel *smiModel = new SmiScnModel(); //Create new SmiScnModel
        smiModel->Core(smiCore);

        //int numOfIrrelevantScen = std::count(branchStageVec.begin(),branchStageVec.end(),outOfBound);
        // scenarios needs to be added in correct ordering, i.e. every scenario that depends on another one can be only inserted after the dependent one.
        // We need to sort the scenarioIndirection array to get the correct ordering for adding scenarios and we need to keep the old ordering.

        std::vector<double> newProbs(scenCounter);
        for (int curScen = 0; curScen < scenCounter; curScen++){
            int flopScen = realOrderingFromVirtualScenario[curScen];
            newProbs[curScen] = probabilities[flopScen];
            //If ancScen of an arbitrary scenario is not set the data is such that the number of Scenarios reduces by this scenario, as it does not differ from another one.
            if (ancScenVec[curScen]!=outOfBound){ //If ancScen is set, we can generate a scenario, otherwise, we won't.
                //We need to convert the CoinPackedMatrix to CoinPackedVectors..
                CoinPackedVector curBloVec(bloVec[flopScen]->getVector(0));
                CoinPackedVector curBupVec(bupVec[flopScen]->getVector(0));
                CoinPackedVector curObjVec(objVec[flopScen]->getVector(0));
                CoinPackedVector curRloVec(rloVec[flopScen]->getVector(0));
                CoinPackedVector curRupVec(rupVec[flopScen]->getVector(0));

                smiModel->generateScenario(smiCore,matrixVec[flopScen],&curBloVec,&curBupVec,&curObjVec,&curRloVec,&curRupVec,branchStageVec[flopScen],ancScenVec[curScen],probabilities[flopScen]);
            }
        }

        DLOG(INFO) << "Generated " << scenCounter << " scenarios.";
        if (scenSet.size() != scenCounter){
            DLOG(INFO) << "We changed scenario set size from " << scenSet.size() << " to " << scenCounter << " during scenario tree generation.";
            scenSet = MP_scenario_set(scenCounter);
            probabilities = newProbs;
            
        }
        // Need a debug assertment statement

        //Delete temporary variables
        for (int curScen = 0; curScen < nScen; curScen++){
            delete matrixVec[curScen];
            delete objVec[curScen];
            delete rloVec[curScen];
            delete rupVec[curScen];
            delete bloVec[curScen];
            delete bupVec[curScen];
        }
        return smiModel;
    }




    bool MP_model::differFromPrevious(const std::vector<std::vector<boost::shared_ptr<MP::Coef> > >& randomVariableVector,int curStage,int curScen,int nextScen){
        if (curScen == nextScen)
            return false;
        bool identical = true;
        for (int i = 0; i < randomVariableVector[curStage].size();i++){
            // If there is any value in the curStage that is different from the value of nextScen, the scenarios differ.
            if (!compareDouble(randomVariableVector[curStage][i]->scenVector[curScen],randomVariableVector[curStage][i]->scenVector[nextScen]) ){
                identical = false;
                break; //break loop and return
            }
        }
        return !identical; 
    }

    void MP_model::enableSemanticCheck( bool semanticCheck )
    {
        semantic = semanticCheck;
    }

    bool MP_model::checkSemantic()
    {
        return semantic;
    }
    std::string printMatrix(CoinPackedMatrix* matrix_,int n, int m, double* clo, double* cup, double* obj, double* rlo, double* rup, int* colStage = 0, int* rowStage = 0){
        printf("Print Problem\n");
        printf("Objective: ");
        for (int i = 0; i < n;i++){
            cout << " " << obj[i] << "*x" << i; 
        }
        cout << endl;
        for (int mi = 0; mi < matrix_->getMajorDim(); mi++){
            printf("%g<= ",rlo[mi]);
            for (int mj = 0; mj < matrix_->getMinorDim(); mj++){
                printf("%g ",matrix_->getCoefficient(mi,mj));
            }
            printf("<= %g",rup[mi]);
            if (rowStage != 0)
                cout << " # stage " << rowStage[mi];
            cout << endl;
        }
        cout << "\n";
        //Print variables with bounds
        for (int i = 0; i < n; i++){
            cout << clo[i] << " <= x" << i << " <= " << cup[i];
            if (colStage != 0)
                cout << " in stage " << colStage[i];
            cout << endl;
        }
        printf("\nMatrix printed \n");
        return "";
    }

    void Messenger::logMessage( int level, const char * const msg )
    {

    }

    void Messenger::solutionStatus( const OsiSolverInterface* Solver )
    {

    }

    void Messenger::statistics( int bm, int m, int bn, int n, int nz )
    {

    }

    void Messenger::objectiveDebug( const vector<MP::Coef>& cfs ) const
    {

    }

    void Messenger::constraintDebug( string name, const vector<MP::Coef>& cfs ) const
    {

    }

    void Messenger::generationTime( double t )
    {

    }
}
