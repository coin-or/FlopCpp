// $Id$
#include "flopc.hpp"
using namespace flopc;
#include "OsiCbcSolverInterface.hpp"

/* This is a FlopCpp version of the model gapmin.gms from 
   the GAMS model library.
*/

/* A general assignment problem is solved via Lagrangian Relaxation
  by dualizing the multiple choice constraints and solving
  the remaining knapsack subproblems.
  The data for this problem are taken from Martello.
  The optimal value is 223 and the optimal solution is:
      1 1 4 2 3 5 1 4 3 5, where
  in columns 1 and 2, the variable in the first row is equal to 1,
  in column 3, the variable in the fourth row is equal to 1, etc...

  Martello, S, and Toth, P, Knapsack Problems: Algorithms and Computer 
  Implementations. John Wiley and Sons, Chichester, 1990.
  Guignard, M, and Rosenwein, M, An Improved Dual-Based Algorthm for
  the Generalized Assignment Problem. Operations Research 37 (1989), 658-663. 
*/


class Knapsack : public MP_model {
    MP_set j;
    MP_constraint knapsack;
    MP_data a;
    double b;
public:
    MP_binary_variable x;

    Knapsack(int numItems, double* ap, MP_data & f, double b) :	
        MP_model(new OsiCbcSolverInterface), j(numItems), a(ap,j),  x(j) {
	knapsack() = sum(j, a(j)*x(j)) <= b; 
	
	minimize( sum(j, f(j)*x(j)) );
    }
};

int main() {
    const int numResources = 5; const int numItems = 10;
    MP_set  i(numResources), j(numItems);
 
    double atable[numResources][numItems] =
	{ {12,   8,  25,  17,  19,  22,   6,  22,  20,  25},
	  { 5,  15,  15,  14,   7,  11,  14,  16,  17,  15},
	  {21,  24,  13,  24,  12,  16,  23,  20,  15,   5},
	  {23,  17,  10,   6,  24,  20,  15,  10,  19,   9},
	  {17,  20,  15,  16,   5,  13,   7,  16,   8,   5} };

    double ftable[numResources][numItems] =
	{ {16,  26,  30,  47,  18,  19,  33,  37,  42,  31},
	  {38,  42,  15,  21,  26,  11,  11,  50,  24,  19},
	  {48,  17,  14,  22,  14,  18,  47,  32,  17,  42},
	  {22,  32,  28,  39,  37,  23,  25,  12,  44,  17},
	  {31,  42,  31,  40,  16,  15,  29,  31,  44,  41} };

    MP_data b(i), w(j), f(i,j), zfeas, s(j), solution(i,j);
    b(0) = 28;  b(1) = 20;  b(2) = 27;  b(3) = 24;  b(4) = 19;
 
    double tolerance = 1e-5;
    int maxiter = 100;
    double zlbest = 0.0;
    f.value(&ftable[0][0]);

    zfeas() = sum(j, maximum(i, f(i,j)));
    zfeas.display("zfeas");

    double alpha = 1.0;
    int count = 1;
    int reset = 5;

    w(j) = 0;

    MP_data myf(j);

    for (int iter=0; iter<100; iter++) { 
	double zlr = 0.0;
	s(j) = 1.0;
	for (int k=0; k<numResources; k++) {
	    for (int jc=0; jc<j.size(); jc++) {
	      myf(jc) = ftable[k][jc] - w(jc);
	    }
	    Knapsack my_knap(numItems, atable[k], myf, b(k));
	       
	    zlr += (my_knap)->getObjValue();
	    for (int jc=0; jc<j.size(); jc++) {
	      s(jc) = s(jc) - my_knap.x.level(jc);
	    }
	    
	}

	MP_data zl;
	zl() = zlr + sum(j, w(j));

	bool improve = (zl>zlbest);
	zlbest = std::max<double>(zlbest,zl(0));
	
	double norm = 0.0;
	for (int jc=0; jc<j.size(); jc++) {
	    norm += s(jc)*s(jc);
	}
		
	cout<<"Iter Nbr."<<iter+1<<"   "<<zlr<<"   "<<zl<<"   "<<zlbest<<"   "<<norm<<endl;	
	if (norm<tolerance) break;
	
	double target = (zlbest+zfeas)/2.0;
	double step = (alpha*(target-zl)/norm);

	w(j) = w(j)+step*s(j);

	if (count>reset) {
	    alpha = alpha/2.0;
	    count = 1;
	} else if (improve == true) {
	    count = 1;
	} else {
	    count++;
	}
    }

    assert(zlbest>=222.99999 && zlbest<=223.00001);

    vector<Knapsack*> pknap;

    for (int k=0; k<numResources; k++) {
      for (int jc=0; jc<j.size(); jc++) {
	myf(jc) = ftable[k][jc] - w(jc);
      }
      
      pknap.push_back(new Knapsack(numItems, atable[k], myf, b(k)));
    }


    for (int q=0; q<numItems; q++) {
	for (int k=0; k<numResources; k++) {
	    if ( pknap[k]->x.level(q) == 1) cout<<k+1<<"  ";
	}
    }
    cout<<endl;
    cout<<"Test gapmin passed."<<endl;
}
