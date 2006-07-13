// $Id$
#include "flopc.hpp"
using namespace flopc;
#include "OsiCbcSolverInterface.hpp"

/* Peacefully Coexisting Armies of Queens 

 Two armies of queens (balvk and white) peacefully coexsit on a
 chessboard when they are placed pon the board in such a way that
 no two queens from opposing armies can attack each other. The
 problem is to find the maximum two equal-sized armies.


Bosch, R, Mind Sharpener. OPTIMA MPS Newsletter (2000).
*/


int main() {
    MP_model::getDefaultModel().setSolver(new OsiCbcSolverInterface);

    MP_set I(5);  // size of chess board
    
    MP_subset<4> M(I,I,I,I); // shared positions on the board;

    MP_index i,j,ii,jj;

    forall(
	I(i)*I(j)*I(ii)*I(jj).such_that(i==ii||j==jj || abs(i-ii)==abs(j-jj)), 
	M.insert(i,j,ii,jj) 
    );

    MP_variable 
	b(I,I), // square occupied by a black Queen
	w(I,I), // square occupied by a white Queen
	tot;    // total queens in each army;

    b.binary();
    w.binary();

    MP_constraint
	eq1(M), // keeps armies at peace
	eq2,    // add up all the black queens
	eq3;    // add up all the white queens;


    eq1(M(i,j,ii,jj)) =   b(i,j) + w(ii,jj) <= 1;

    eq2() =  tot() == sum(I(i)*I(j), b(i,j));

    eq3() =  tot() == sum(I(i)*I(j), w(i,j));
 
    maximize(tot());

    MP_model::getDefaultModel()->writeMps("coex","mps",1.0);

    assert(MP_model::getDefaultModel()->getNumRows()==347);
    assert(MP_model::getDefaultModel()->getNumCols()==51);
    assert(MP_model::getDefaultModel()->getNumElements()==742);
    assert(MP_model::getDefaultModel()->getObjValue()==4);
    
    b.display("Black");
    w.display("White");
   
    cout<<"Test coex passed."<<endl;
}
