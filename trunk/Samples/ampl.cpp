// $Id$
#include "flopc.hpp"
using namespace flopc;
#include <OsiCbcSolverInterface.hpp>

/*   A sample problem to demonstrate the power of modeling systems
Fourer, R, Gay, D M, and Kernighan, B W, AMPL: A Mathematical Programming
Language. AT\&T Bell Laboratories, Murray Hill, New Jersey, 1987.
*/

int main() {
	MP_model::getDefaultModel().setSolver(new OsiCbcSolverInterface);
    MP_model::getDefaultModel().verbose();

    enum {iron, nickel, numRaw};
    enum {nuts, bolts, washers, numPrd};
    const int numT = 4;

    MP_set 
	prd(numPrd),    // products        / nuts, bolts, washers /
	raw(numRaw),    // raw materials   / iron, nickel /
	TL(numT+1);     // extended t      / 1 * 5 /

    MP_subset<1>
	T(TL),     //  periods     / 1 * 4 /
	first(TL), //          / 1     /
	last(TL);  //           /     5 /

    T.insert(0);
    T.insert(1);
    T.insert(2);
    T.insert(3);

    first.insert(0);
    last.insert(4);

    MP_data istock(raw); // initial stock  /  iron  35.8 , nickel  7.32  /
    MP_data scost(raw); // storage cost   /  iron    .03, nickel   .025 /
    MP_data rvalue(raw); //  residual value /  iron    .02, nickel  -.01  /

    istock(iron) = 35.8;   istock(nickel) = 7.32;
    scost(iron) =  0.03;  scost(nickel) = 0.025;
    rvalue(iron) =  0.02;  rvalue(nickel) =-0.01;

    double  m = 123; //  maximum production 

    MP_data units(raw,prd); // raw material inputs to produce a unit of product

    double avalue[numRaw][numPrd] =  {{ .79, .83, .92 }, 
				      { .21, .17, .08 }};
    
    units.value(&avalue[0][0]);

    MP_data profit(prd,T); // profit

    double cvalue[numPrd][numT] = {{ 1.73,  1.8,  1.6,  2.2 },
				 { 1.82,  1.9,  1.7,   .95},
				 { 1.05,  1.1,   .95, 1.33}};

    profit.value(&cvalue[0][0]);

    MP_variable  x(prd,TL); // production level
    MP_variable  s(raw,TL); // storage at beginning of period

    MP_index p,r,t;

    MP_constraint  
	limit(T),       // capacity constraint
	balance(raw,TL);  // raw material balance

    
    limit(T) =   sum(prd(p), x(p,T)) <= m;

    balance(r,TL+1) = 
 	s(r,TL+1) == s(r,TL) - sum(prd(p), units(r,p)*x(p,TL));

    s.upperLimit(r,0) = istock(r);


    // MP_model::default_model.verbose();

    maximize( 
	sum(T(t), sum(prd(p), profit(p,t)*x(p,t)) -
	          sum(raw(r), scost(r)*s(r,t) )) +
	sum(raw(r), rvalue(r)*s(r,numT)) 
    );

    assert(MP_model::getDefaultModel()->getNumRows()==14);
    assert(MP_model::getDefaultModel()->getNumCols()==25);
    assert(MP_model::getDefaultModel()->getNumElements()==60);
    assert(MP_model::getDefaultModel()->getObjValue()>=79.3412 && MP_model::getDefaultModel()->getObjValue()<=79.3414);

    x.display();
    s.display();
    cout<<"Test ampl passed."<<endl;
}

// Optimal objective value: 79.3413
