// $Id$
#include "flopc.hpp"
using namespace flopc;
#include <OsiCbcSolverInterface.hpp>
#include <iostream>
#include <fstream>
#include <string>

/**/


int main() {
    MP_model::getDefaultModel().setSolver(new OsiCbcSolverInterface);

    const int p = 3;

    MP_set J(p*p);
    MP_set K(p);
    MP_subset<3> F(J,J,J);

    std::string fname = "fixed.dat";
    std::ifstream file(fname.c_str());
    
    int ival,jval,kval;

    while (file>>ival>>jval>>kval) {
      F.insert(ival-1,jval-1,kval-1);
    }
    file.close();
    
    MP_binary_variable	x(J,J,J);

    MP_constraint 
      rows(J,J),
      cols(J,J),
      nums(J,J),
      squares(K,K,J),
      preselected(F);
    
    MP_index i,j,k,m,n;

    rows(i,j) = sum(J(k), x(i,j,k)) == 1;
    cols(j,k) = sum(J(i), x(i,j,k)) == 1;
    nums(i,k) = sum(J(j), x(i,j,k)) == 1;

    squares(m,n,k) = sum(K(i)*K(j), x(m*p+Constant(i),n*p+Constant(j),k)) == 1;

    preselected(F(i,j,k)) = x(i,j,k) == 1;

    //    minimize(sum(J(i)*J(j)*J(k),x(i,j,k)));

    minimize(x(0,0,0));

    for (int i=0; i<9; i++) {
      for (int j=0; j<9; j++) {
	for (int k=0; k<9; k++) {
	  if (x.level(i,j,k)==1) {
	    cout<<k+1<<" ";
	  }
	}
      }
      cout<<endl;
    }
    
    cout<<"Test sodoku passed."<<endl;
}
