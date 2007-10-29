// $Id$
#include "flopc.hpp"
using namespace flopc;
#include "OsiClpSolverInterface.hpp"

//SMI
#include "SmiScnModel.hpp"
#include "SmiScnData.hpp"
#include "OsiClpSolverInterface.hpp"

/* FLOPC++ implementation of a financial planning and control model from
   "StAMPL: A filtration-oriented modeling toll for stochastic programming" by Fourer and Lopes
*/


main() {
  MP_model::getDefaultModel().setSolver(new OsiClpSolverInterface);
  MP_model::getDefaultModel().verbose();

  enum { STOCKS, BONDS, numINSTR};
  const int numStages = 3;

  MP_set INSTR(numINSTR);
  MP_stage T(numStages);

  MP_stochastic_data Return(T,INSTR);

  MP_variable Buy(T,INSTR);
  MP_variable Shortage, Overage;

  MP_constraint InvestAll;
  MP_constraint ReinvestAll(T);
  MP_constraint Goal;

  double initial_wealth = 55;
  double goal = 80;
    
  //---------------------------------
  MP_data GoodReturn(INSTR);
  MP_data BadReturn(INSTR);

  GoodReturn(STOCKS) = 1.25;   GoodReturn(BONDS) = 1.14;
  BadReturn(STOCKS)  = 1.06;   BadReturn(BONDS)  = 1.12;
 

  MP_model *M=MP_model::getCurrentModel();

  //SMI: Generate deterministic equivalent
  //SMI: initialize SmiModel
  SmiScnModel *smiModel = new SmiScnModel();
  smiModel->setOsiSolverHandle(*new OsiClpSolverInterface());

  //SMI: Generate core data

  cout << "SMI: Generating Core Data" << endl;

  // set Return for core data (don't have to, but may as use expected values)
  //	forall(T,Return(T,INSTR)= 0.5*GoodReturn(INSTR)+0.5*BadReturn(INSTR));
  Return(T,INSTR)= 0.5*GoodReturn(INSTR)+0.5*BadReturn(INSTR);

  // model generation code
  InvestAll() = sum(INSTR, Buy(0,INSTR)) == initial_wealth;
  ReinvestAll(T+1) = sum(INSTR, Buy(T,INSTR) * Return(T,INSTR)) == sum(INSTR, Buy(T+1,INSTR));
  Goal() = sum(INSTR, Buy(T.last(),INSTR) * Return(T.last(),INSTR)) == goal - Shortage() + Overage();
  M->setObjective( Overage() - 4*Shortage() );

  // generate MP_model 
  OsiClpSolverInterface *osiCore = new OsiClpSolverInterface();
  M->attach(osiCore);

  // set stage vectors

  //begin{TIM}
  //	these interfaces are probably not needed for a typical user, 
  //  but this gives the idea of what is needed to generate the core model

  vector<int> colStage((*M)->getNumCols());
  vector<int> rowStage((*M)->getNumRows());

  for (int j=0; j!=(*M)->getNumCols(); ++j) {
    colStage[j] = M->getColStage()[j];
    cout<<"Col stage  "<<j<<"  "<<colStage[j]<<endl;
  }

  for (int i=0; i!=(*M)->getNumRows(); ++i) {
    rowStage[i] = M->getRowStage()[i];
    cout<<"Row stage  "<<i<<"  "<<rowStage[i]<<endl;
  }
  
  cout<<"--------------------"<<endl;
// generate Core Data object
  SmiCoreData *smiCoreData= new SmiCoreData(osiCore, T.size(), &colStage[0], &rowStage[0]);
//end{TIM}
  cout<<"--------------------"<<endl;
  
  // clean up MP_Model 
  M->detach();

//SMI: Generate Discrete Distribution

  cout << "SMI: Generating Discrete Distribution" << endl;

  // Create discrete distribution 
  SmiDiscreteDistribution *smiDD = new SmiDiscreteDistribution(smiCoreData);

  // Loop over random variables
  for (int t=1; t<=numStages ; ++t){

    //Each Return is based in period t:
    cout << "SMI: Generating Returns for period: " << t<< endl;

    SmiDiscreteRV *smiRV = new SmiDiscreteRV(t);

    //Each has two events:
    //event 1: GoodReturn
		
    cout << "SMI: Event 1: Good return in period: " << t<< endl;

    //forall(INSTR,Return(t,INSTR) = GoodReturn(INSTR));
    Return(t,INSTR) = GoodReturn(INSTR);
    double dprob=0.5;

    // model generation code
    InvestAll() = sum(INSTR, Buy(0,INSTR)) == initial_wealth;
    ReinvestAll(T+1) = sum(INSTR, Buy(T,INSTR) * Return(T,INSTR)) == sum(INSTR, Buy(T+1,INSTR));
    Goal() = sum(INSTR, Buy(T.last(),INSTR) * Return(T.last(),INSTR)) == goal - Shortage() + Overage();
    M->setObjective( Overage() - 4*Shortage() );

    // generate MP_model 
    OsiClpSolverInterface *osi = new OsiClpSolverInterface();
    M->attach(osi);

    // add event
    smiRV->addEvent(*osi,dprob);

    // clean up MP_Model
    M->detach();

    //realization 2: BadReturn
		
    cout << "SMI: Event 2: Bad return in period: " << t<< endl;

    Return(t,INSTR) = BadReturn(INSTR);
    dprob=0.5;

    // model generation code
    InvestAll() = sum(INSTR, Buy(0,INSTR)) == initial_wealth;
    ReinvestAll(T+1) = sum(INSTR, Buy(T,INSTR) * Return(T,INSTR)) == sum(INSTR, Buy(T+1,INSTR));
    Goal() = sum(INSTR, Buy(T.last(),INSTR) * Return(T.last(),INSTR)) == goal - Shortage() + Overage();
    M->setObjective( Overage() - 4*Shortage() );

    // generate MP_model 
    osi = new OsiClpSolverInterface();
    M->attach(osi);

    // add event
    smiRV->addEvent(*osi,dprob);

    // clean up MP_Model
    M->detach();

    // add RV to Distribution
    smiDD->addDiscreteRV(smiRV);

  }

  cout<<"A--------------------"<<endl;
  // Generate scenarios
  smiModel->processDiscreteDistributionIntoScenarios(smiDD);
  cout<<"B--------------------"<<endl;
	
  // load problem data into OsiSolver
  smiModel->loadOsiSolverData();

  // get Osi pointer
  cout<<"C--------------------"<<endl;
  OsiSolverInterface *smiOsi = smiModel->getOsiSolverInterface();

  cout<<"D--------------------"<<endl;
  // set some parameters
  smiOsi->setHintParam(OsiDoPresolveInInitial,true);
  smiOsi->setHintParam(OsiDoScale,true);
  smiOsi->setHintParam(OsiDoCrash,true);
  cout<<"E--------------------"<<endl;

  // solve using Osi Solver
  smiOsi->initialSolve();

  // print optimal value
  cout << "Optimal value: " << smiOsi->getObjValue() << endl;





 }
