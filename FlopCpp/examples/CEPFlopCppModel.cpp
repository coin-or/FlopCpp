#include "flopc.hpp"
#include "OsiClpSolverInterface.hpp"

#include "OsiCbcSolverInterface.hpp"


using namespace flopc;


//Eduardo F. Silva, PhD
//Advanced Optimization Lab
//Energy Planning Program
//Rio de Janeiro Federal University 
//
//FlopCpp simple example for the Stochastic Capacity Expansion Problem (SCEP)
//        showing how to use indirect index. 
//
//Problem: The Capacity Expansion Problem (CEP) consists on finding multi-period
//         capacity-expansion decisions to attend some anticipated demand growth,
//         while minimizing the overall cost. 
//         In addition, this problem is made stochastic, or more precisely, a multistage
//         stochastic problem.  The scenario tree are described below.
//         To solve this problem, a deterministic equivalent formulation is used. Decision
//         variables are split over scenarios, meaning that decisions that should be
//         considered the same for different scenarios are initially modeled as distinct
//         variables (e.g. node 1 decisions for first and second scenarios).
//         Finally, an artificial variable that associate (scenarios & time periods) to nodes
//         is used to force that scenario variables sharing the same nodes have same values.
//         Those constraints are known as nonanticipativity constraints. 
//
//         Scenario tree for de Capacity Expansion Problem
//
//                   +---+      
//                  /| 3 |      
//           +---+ / +---+      
//         / | 1 |+             
//        /  +---+ \ +---+      
//       /          \| 4 |      
//   +---+           +---+      
//   | 0 |                      
//   +---+           +---+      
//       \          /| 5 |      
//        \  +---+ / +---+      
//         \ | 2 |+             
//           +---+ \ +---+      
//                  \| 6 |      
//                   +---+      
//                              



int main(int argc, char ** argv){

	//Data 
	//Actually, There is no need to define all constants below when 
	//you are reading data directly.# of Scenarios and time periods are easily
	//calculated from the scenario tree.
	const int nScenarios   = 4;  //# of scenarios
	const int nTimePeriods = 3;  //# of time periods
	const int nNodes       = 7;  //# of nodes
	const int nPlants      = 3;  //# of plants

	MP_model CEPDetEquiCbc(new OsiCbcSolverInterface);

	MP_set s(nScenarios),
		     t(nTimePeriods),
		     p(nPlants),
		     n(nNodes);

	MP_index  ii;

	MP_data   capacity(p), //Plant capacity
				    cost_f(p),   //Setup (fixed) cost
				    cost_v(p),   //Variable cost

						
						probS(s),      //The scenario probability could be calculated from nodes probabilities 
						demand(n),     //Each node contains a realization of an unceirtain demand

				    stToNodes(s,t);//Associate nodes to scenarios-time periods. One node might belong 
	                         //to many scenarios (e.g. node 0). 
							
  //initialize Data-----------------------------------
	//Plants data
  //Just to make thing simpler, costs over periods are being considered constant
   	capacity(0)=12; capacity(1)=8 ;  capacity(2)=5;
	  cost_v(0)  =3 ; cost_v(1)  =5 ;  cost_v(2)  =6;
    cost_f(0)  =20; cost_f(1)  =15;  cost_f(2)  =10;
 
  //scenario data
    probS(0)=0.3;probS(1)=0.2;probS(2)=0.4;probS(3)=0.1;

  //Node data  -stochastic data
		demand(0)=10;demand(1)=15;demand(2)=12;demand(3)=18;
    demand(4)=20;demand(5)=15;demand(6)=13;

  //mapping scenario x time periods to nodes
	//for big problems this mapping has to be made when reading data
    stToNodes(0,0)=0;stToNodes(0,1)=1;stToNodes(0,2)=3;
		stToNodes(1,0)=0;stToNodes(1,1)=1;stToNodes(1,2)=4;
		stToNodes(2,0)=0;stToNodes(2,1)=2;stToNodes(2,2)=5;
		stToNodes(3,0)=0;stToNodes(3,1)=2;stToNodes(3,2)=6;
		
	//-----------------------------------------------------		

	MP_variable  x_flow(s,t,p),
		           y_binary(s,t,p),
							 z(n,p);

	MP_constraint  openOnce(s,p),
		             flowLimit(s,t,p),
								 demandSat(s,t),
								 nonAnticip(s,t,p);
  
	//This will force Clp Solver use a simple enumeration Tree 
	y_binary.binary();

	openOnce.setName("Open one time only");
	nonAnticip.setName("Nonanticipativity constraints");

	openOnce(s,p)    = sum(t,y_binary(s,t,p))<=1;
	flowLimit(s,ii,p)= x_flow(s,ii,p)<=capacity(p)*sum(t.such_that(t<=ii),y_binary(s,t,p));
	demandSat(s,t)   = sum(p,x_flow(s,t,p))>=demand(MP_index_exp(stToNodes(s,t)));
	
	nonAnticip(s,t,p)=  z(MP_index_exp(stToNodes(s,t)),p)==x_flow(s,t,p);
  
	MP_expression ObjF =sum(s*t*p,probS(s)*(cost_v(p)*x_flow(s,t,p)+cost_f(p)*y_binary(s,t,p)));

	//----------------------
	cout<<endl<<"Now with Clp solver"<<endl;
	
	MP_model CEPDetEquiClp(new OsiClpSolverInterface);
	CEPDetEquiClp.add(openOnce).add(flowLimit).add(demandSat).add(nonAnticip);
	CEPDetEquiClp.silent();
	//Log level 0-none 1- minimal 
	CEPDetEquiClp.Solver->messageHandler()->setLogLevel(0);

	CEPDetEquiClp.minimize(ObjF);
  CEPDetEquiClp.Solver->writeLp("scep_Clp","txt");
  //--------------------------

	//--------------------------
	cout<<endl<<"Now with Cbc solver"<<endl;
	
	CEPDetEquiCbc.add(openOnce).add(flowLimit).add(demandSat).add(nonAnticip);
  CEPDetEquiCbc.silent();
	//Log level 0-none 1- minimal 
	CEPDetEquiCbc.Solver->messageHandler()->setLogLevel(0);

	CEPDetEquiCbc.minimize(ObjF);
	CEPDetEquiCbc.Solver->writeLp("scep_Cbc","txt");
}
