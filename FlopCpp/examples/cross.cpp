// $Id$
#include "flopc.hpp"
using namespace flopc;
using namespace std;
#include "OsiCbcSolverInterface.hpp"

/* Alcuin's River Crossing  

A farmer carrying a bushel of corn and accompanied by a goose
and a wolf came to a river. He found a boat capable of
transporting himself plus one of his possessions - corn, goose, or
wolf - but no more. Now, he couldn't leave the corn alone with
the goose, nor the goose alone with the wolf, else one would
consume the other. Nevertheless, he succeeded in getting himself
and his goods across the river safely.


Borndoerfer, R, Groetschel, M, and Loebel, A, Alcuin's
Transportation Problem and Integer Programming. Konrad Zuse
Zentrum for Informationstechnik, Berlin, 1995.

Contributed by Soren Nielsen, Institute for Mathematical Sciences
               University of Copenhagen
*/


int main() {
    MP_model::getDefaultModel().setSolver(new OsiCbcSolverInterface);

    enum {goose, wolf, corn, numI};

    MP_set
	i(numI),  // /goose, wolf, corn/
	t(10);    // time  

    MP_data dir(t);  // crossing - near to far is +1 - far to near -1;

    for (int k=0; k<t.size(); k++) {
	dir(k) = pow((double)-1,(double)k);
    }

    dir.display("dir");

    MP_variable
	cross(i,t), //  crossing the river
	done(t);    //  all items in far side

    MP_binary_variable
	y(i,t);     //  1 iff the item is on the far side at time t

    MP_constraint
	DefDone(i,t),  // everything on far side
	DefCross(i,t), // crossing
	LimCross(t),   
	EatNone1(t),
	EatNone2(t);

    DefCross(i,t) =  y(i,t) == y(i,t-1) + dir(t-1)*cross(i,t-1);

    DefDone(i,t)  =  done(t) <= y(i,t);

    LimCross(t) =  sum(i, cross(i,t-1)) <= 1;
    
    EatNone1(t) =   dir(t)*(y(goose,t) + y(wolf,t) - 1) <= done(t) ;
    
    EatNone2(t) =   dir(t)*(y(goose,t) + y(corn,t) - 1) <= done(t) ;

    y.upperLimit(i,0) = 0;

    maximize(sum(t, done(t)));
    
    assert(MP_model::getDefaultModel()->getNumRows()==90);
    assert(MP_model::getDefaultModel()->getNumCols()==70);
    assert(MP_model::getDefaultModel()->getNumElements()==231);
    assert(MP_model::getDefaultModel()->getObjValue()==3);

    y.display("y");
    cross.display("cross");

    cout<<"Test cross passed."<<endl;
}
