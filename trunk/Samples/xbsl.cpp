// $Id$
/* This simple scheduling model is a widely used as an example by Dash
 * Optimization (www.dashoptimization.com)
 */
 
#include <OsiCbcSolverInterface.hpp>
#include "flopc.hpp"
using namespace flopc;

int main() {
    const int numJ =  4;
    const int numT = 10;

    MP_set J(numJ),T(numT);
    MP_data D(J);

    D(0) = 3;
    D(1) = 4;
    D(2) = 2;
    D(3) = 2;

    MP_variable s(J);
    MP_binary_variable delta(J,T); 

    MP_index t,j;
    MP_model M(new OsiCbcSolverInterface);
    MP_constraint C2_20, C2_30, C3(J), C4(J); 

    C2_20() = s(2)  >= D(0) + s(0);
    C2_30() = s(3)  >= D(0) + s(0);

    C3(J) = sum( T(t).such_that(t <= numT-D(j)), (t+1)*delta(j,t) ) == s(j);

    C4(J) = sum( T(t).such_that(t <= numT-D(j)), delta(j,t) ) == 1;
  
    s.upperLimit(j) = numT-D(j);

    M.minimize_max( J, s(J)+D(J) ); 

    assert(M->getNumRows()==14);
    assert(M->getNumCols()==45);
    assert(M->getNumElements()==80);
    assert(M->getObjValue()>=6 && M->getObjValue()==6);

    s.display("s");
    cout<<"Test xbsl passed."<<endl;
}
