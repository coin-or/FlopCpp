// $Id$
#include "flopc.hpp"
using namespace flopc;
#include <OsiCbcSolverInterface.hpp>

// Adapted from bid.gms from the GAMS model library:
//    http://www.gams.com/

/* A company obtains a number of bids from vendors for the supply
of a specified number of units of an item. Most of the submitted
bids have prices that depend on the volume of business.

Bracken, J, and McCormick, G P, Chapter 3. In Selected Applications of
Nonlinear Programming. John Wiley and Sons, New York, 1968, pp. 28-36.
*/

int main() {
    MP_model bid(new OsiCbcSolverInterface);

    enum { a, b, c, d, e, nbrV};
    const int numSegments = 5;
    
    MP_set V(nbrV);           // vendors     
    MP_set S(numSegments);    // segments    

    MP_subset<2> VS(V,S); // vendor bid possibilities

    VS.insert(a,0);
    VS.insert(b,0);
    VS.insert(b,1);
    VS.insert(b,2);
    VS.insert(b,3);
    VS.insert(c,0);
    VS.insert(d,0);
    VS.insert(e,0);
    VS.insert(e,1);

    MP_data setup(VS);
    MP_data price(VS);
    MP_data qmin (VS);
    MP_data qmax (VS);

    double req = 239600.48;

    setup(VS(a,0)) =   3855.84;
    setup(VS(b,0)) = 125804.84;
    setup(VS(c,0)) =  13456.00;
    setup(VS(d,0)) =   6583.98;

    price(VS(a,0)) = 61.150;
    price(VS(b,0)) = 68.099;
    price(VS(b,1)) = 66.049;
    price(VS(b,2)) = 64.099;
    price(VS(b,3)) = 62.119;
    price(VS(c,0)) = 62.190;
    price(VS(d,0)) = 72.488;
    price(VS(e,0)) = 70.150;
    price(VS(e,1)) = 68.150;

    qmin(VS(b,0)) =   22000;
    qmin(VS(b,1)) =   70000;
    qmin(VS(b,2)) =  100000;
    qmin(VS(b,3)) =  150000;
    qmin(VS(e,1)) =   42000;

    qmax(VS(a,0)) =  33000;
    qmax(VS(b,0)) =  70000;
    qmax(VS(b,1)) = 100000;
    qmax(VS(b,2)) = 150000;
    qmax(VS(b,3)) = 160000;
    qmax(VS(c,0)) = 165600;
    qmax(VS(d,0)) =  12000;
    qmax(VS(e,0)) =  42000;
    qmax(VS(e,1)) =  77000;

     setup(VS(V,S+1)) = setup(VS(V,S)) +
 	qmax(VS(V,S))*(price(VS(V,S))-price(VS(V,S+1)));

//    setup(VS(V,S+1)) = setup(VS(V,S)) +
//	qmax(VS(V,S))*(price(VS(V,S))-price(VS(V,S+1)));

    setup.display("setup");

    MP_variable         pl(VS);    //  purchase level
    MP_binary_variable  plb(VS);   //  purchase decision

    MP_constraint 
	demand,    // demand constraint
	minpl(VS), // min purchase
	maxpl(VS), // max purchase
	oneonly(V);// at most one deal;

    demand() =   req == sum( VS, pl(VS) );

    minpl(VS) = pl(VS) >= qmin(VS)*plb(VS);

    maxpl(VS) = pl(VS) <= qmax(VS)*plb(VS);

    oneonly(V) =  sum(VS(V,S), plb(VS)) <= 1;

    bid.verbose();

    bid.minimize( sum(VS, price(VS)*pl(VS) + setup(VS)*plb(VS)) );

    assert(bid->getNumRows()==24);
    assert(bid->getNumCols()==18);
    assert(bid->getNumElements()==50);
    assert(bid->getObjValue()>=1.52100e+07 && bid->getObjValue()<=1.52102e+07);

    cout<<"Test bid passed."<<endl;
}
