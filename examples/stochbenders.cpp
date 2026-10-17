// $Id$
#include "flopc.hpp"
using namespace flopc;
using namespace std;
#include "OsiClpSolverInterface.hpp"

/* Benders decomposition for a simple transportation problem
with stochastic demands. For details see: Erwin Kalvelagen:
"Benders decomposition for stochastic programming with GAMS"
(www.gams.com/~erwin/stochbenders.pdf)
*/


int main() {
    const int numScenarios = 3;
    const int numFactories = 3;
    const int numDistributionCenters = 5;
    MP_set i(numFactories), j(numDistributionCenters);

    MP_data 
	capacity(i),
	transcost(i,j);

    transcost(0,0) = 2.49; transcost(1,0) = 1.46; transcost(2,0) = 3.26; 
    transcost(0,1) = 5.21; transcost(1,1) = 2.54; transcost(2,1) = 3.08; 
    transcost(0,2) = 3.76; transcost(1,2) = 1.84; transcost(2,2) = 2.60; 
    transcost(0,3) = 4.85; transcost(1,3) = 1.86; transcost(2,3) = 3.76; 
    transcost(0,4) = 2.07; transcost(1,4) = 4.76; transcost(2,4) = 4.45; 

    capacity(0) = 500;
    capacity(1) = 450;
    capacity(2) = 650;

    const double prodcost = 14;
    const double price = 24;
    const double wastecost = 4;

    const double Demand[numScenarios][numDistributionCenters] = 
	{{150,100,250,300,600},
	 {160,120,270,325,700},
	 {170,135,300,350,800}};

    const double prob[numScenarios] = {0.25,0.5,0.25};

    ///// Master problem /////////////////////////////////////////////

    MP_model masterproblem(new OsiClpSolverInterface);

    MP_variable 
	theta,
	ship(i,j),
	product(i),
	slackproduct(i),
	received(j);

    MP_constraint
	production(i),
	receive(j),
	prodcap(i);

    production(i) = product(i) == sum(j,ship(i,j));
    receive(j) =    received(j) == sum(i,ship(i,j));
    prodcap(i) =    product(i) + slackproduct(i) == capacity(i);

    masterproblem.minimize( sum(i*j,transcost(i,j)*ship(i,j)) +
			    sum(i, prodcost*product(i)) + theta() );

    ///// Sub problem ///////////////////////////////////////////////

    MP_model subproblem(new OsiClpSolverInterface);

    MP_variable
	sales(j),
	waste(j),
	slacksales(j);

    MP_constraint
	selling(j),
	selmax(j);

    selling(j) = sales(j) + waste(j) == 0.0;
    selmax(j) =  sales(j) + slacksales(j) == 0.0;

    subproblem.minimize( sum(j, wastecost*waste(j)) - sum(j, price*sales(j)) );

    ///// Benders algorithm //////////////////////////////////////////

    masterproblem->setColLower( theta().getColumn(), -masterproblem->getInfinity());

    double lowerbound = -masterproblem->getInfinity();
    double upperbound = masterproblem->getInfinity();
    double objmaster = 0.0;

    int kk;
    for (kk=0; kk<25; kk++) { // max 25 iterations
	double cutconst = 0.0;
	MP_data cutcoeff(j);

	double objsub = 0.0;
	received.display("received");
	for (int jj=0; jj<numDistributionCenters; jj++) {
	    subproblem->setRowBounds( selling(jj), received.level(jj), received.level(jj));
	}
	for (int s=0; s<numScenarios; s++) {
	    for (int j=0; j<numDistributionCenters; j++) {
		subproblem->setRowBounds( selmax(j), Demand[s][j], Demand[s][j]);
	    }
	    subproblem->resolve();
	    objsub += prob[s]*subproblem->getObjValue();
	    for (int j=0; j<numDistributionCenters; j++) {
		cout<<selmax.price(j)<<"  "<<prob[s]<<"  "<<Demand[s][j]<<endl;
		cutconst += prob[s]*selmax.price(j)*Demand[s][j];
		cutcoeff(j) += prob[s]*selling.price(j);
	    }
	}

	upperbound = CoinMin(upperbound, objmaster + objsub);
	cout<<"Lower and upper bounds: "<<lowerbound<<"   "<<upperbound<<endl;
	if ((upperbound-lowerbound) < 0.0001*(1+fabs(lowerbound))) {
	    break;
	}

	cout<<"cutconst="<<cutconst<<endl;
	masterproblem.addRow( theta() >= cutconst + sum(j, cutcoeff(j)*received(j)) );

	masterproblem->resolve();
	ship.display("ship");

	lowerbound = masterproblem->getObjValue();
	objmaster = lowerbound - theta.level();
    }
    assert(kk<25);

// Expected solution:
// ship
// 0 0   0
// 0 1   0
// 0 2   0
// 0 3   0
// 0 4   500
// 1 0   150
// 1 1   0
// 1 2   0
// 1 3   300
// 1 4   0
// 2 0   0
// 2 1   100
// 2 2   270
// 2 3   0
// 2 4   100

    cout<<"Test stochbenders passed."<<endl;
}
