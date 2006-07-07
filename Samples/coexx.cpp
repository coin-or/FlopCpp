// $Id$
#include "flopc.hpp"
using namespace flopc;
#include "OsiCbcSolverInterface.hpp"

/* Peacefully Coexisting Armies of Queens - tight 

 This is a tighter formulation than the original COEX problem.
 We have set the size of the board to 5 in order to find
 solutions quickly. In addition we fix the position of one queen.

 Two armies of queens (balvk and white) peacefully coexsit on a
 chessboard when they are placed pon the board in such a way that
 no two queens from opposing armies can attack each other. The
 problem is to find the maximum two equal-sized armies.

Bosch, R, Mind Sharpener. OPTIMA MPS Newsletter (2000).
*/

int main() {
    MP_model::getDefaultModel().setSolver(new OsiCbcSolverInterface);

    const int boardSize = 5;

    MP_set 
	i(boardSize),  
	s(2*boardSize-3); // diagonal offsets

    MP_data
	sh(s),    // shift values for diagonals
	rev(s,i); // reverse shift order;

    sh(s)  = s - i.size() + 1;

    rev(s,i) = i.size() + 1 - 2*Constant(i) + sh(s);

    MP_binary_variable 
	xw(i,i), // has a white queen
	xb(i,i), // has a black queen
	wa(i),   // white in row i
	wb(i),   // white in column j
	wc(s),   // white in diagonal s
	wd(s);   // white in backward diagonal s;

    MP_variable tot;

    MP_index j;
    
    MP_constraint
	aw(i,i), // white in row i
	bw(i,i), // white in column j
	cw(s,i), // white in diagonal s
	dw(s,i), // white in backward diagonal s
	ew,      // total white
	ab(i,i), // black in row i
	bb(i,i), // black in column j
	cb(s,i), // black in diagonal s
	db(s,i), // black in backward diagonal s
	eb;      // total black;


    aw(i,j) =  wa(i) >=  xw(i,j);

    bw(j,i) =  wb(j) >=  xw(i,j);
    
    cw(s,i) =  wc(s) >=  xw(i,i+sh(s));
    
    dw(s,i) =  wd(s) >= xw(i,i+rev(s,i));
    
    ab(i,j) =  1-wa(i) >= xb(i,j);
       
    bb(j,i) =  1-wb(j) >= xb(i,j);

    cb(s,i) =  1-wc(s) >= xb(i,i+sh(s));
       
    db(s,i) =  1-wd(s) >= xb(i,i+rev(s,i));

    eb() =   tot() == sum(i*i(j), xb(i,j));

    ew() =   tot() == sum(i*i(j), xw(i,j));

    xb.lowerLimit(0,0) = 1;

    maximize(tot());
 
    assert(MP_model::getDefaultModel()->getNumRows()==242);
    assert(MP_model::getDefaultModel()->getNumCols()==75);
    assert(MP_model::getDefaultModel()->getNumElements()==480);
    assert(MP_model::getDefaultModel()->getObjValue()==4);

    xb.display("Black");
    xw.display("White");

    cout<<"Test coexx passed."<<endl;
}
