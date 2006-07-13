// $Id$
#include "flopc.hpp"
using namespace flopc;
#include <OsiCbcSolverInterface.hpp>

/*The objective of this model is to allocate aircrafts to routes to maximize
the expected profit when traffic demand is uncertain. Two different
formulations are used, the delta and the lambda formualation.

Dantzig, G B, Chapter 28. In Linear Programming and Extensions.
Princeton University Press, Princeton, New Jersey, 1963.
*/

namespace Aircraft {
    enum {a, b, c, d, numA};
} 

int main() {
    enum {route_1, route_2, route_3, route_4, route_5, numRoutes};
    const int numDemandStates = 5;

    MP_set i(Aircraft::numA); // aircraft types and unassigned passengers
    MP_set j(numRoutes);      // assigned and unassigned routes
    MP_set h(numDemandStates);// demand states 

    double ddval[5][5] = {{200,     220,    250,    270,    300},
		      { 50,     150,      0,      0,      0},  
		      {140,     160,    180,    200,    220},
		      { 10,      50,     80,    100,    340},
		      {580,     600,    620,      0,      0}};

    double lambdaval[5][5] =
	{{.2,     .05,    .35,    .2,     .2},
	 {.3,     .7,    0.0,    0.0,    0.0},
	 {.1,     .2,     .4,     .2,     .1},
	 {.2,     .2,     .3,     .2,     .1},
	 {.1,     .8,     .1,    0.0,    0.0}};

    
    double cval[4][5] =
	{{18,         21,         18,          16,           10},
	 {0,          15,         16,          14,           9},
	 {0,          10,          0,          9,            6},
	 {17,         16,         17,          15,           10}};

    double pval[4][5] =
	{{16,         15,          28,          23,          81},
	 {0,         10,          14,         15,          57},
	 {0,          5,           0,           7,          29},
	 {9,         11,          22,          17,          55}};

    MP_data dd(&ddval[0][0],j,h); //    dd.value(&ddval[0][0]);
    MP_data lambda(&lambdaval[0][0],j,h); //lambda.value(&lambdaval[0][0]);
    MP_data c(&cval[0][0],i,j); //     c.value(&cval[0][0]);
    MP_data p(&pval[0][0],i,j); //     p.value(&pval[0][0]);

    c.display("c");

    MP_data 
	aa(i),      // aircraft availiability 
	k(j),       // revenue lost (1000 per 100  bumped) 
	ed(j),      // expected demand
	gamma(j,h), // probability of exceeding demand increment h on route j
	deltb(j,h); // incremental passenger load in demand states;

    aa(Aircraft::a) = 10;  aa(Aircraft::b) = 19;  
    aa(Aircraft::c) = 25;  aa(Aircraft::d) = 15;

    k(route_1) = 13;
    k(route_2) = 13;
    k(route_3) =  7;
    k(route_4) =  7;
    k(route_5) =  1;

    ed(j) = sum(h, lambda(j,h)*dd(j,h));

    MP_index hp;

    gamma(j,h) = sum( h(hp).such_that(hp >= h), lambda(j,hp));

    deltb(j,h) = pos(dd(j,h)-dd(j,h-1));

    ed.display("ed");
    gamma.display("gamma");
    deltb.display("deltb");
    aa.display("aa");

    MP_variable
	x(i,j),   // number of aircraft type i assigned to route j
	y(j,h),   // passengers actually carried
	b(j,h),   // passengers bumped
	oc,       // operating cost
	bc;       // bumping cost

    MP_constraint
	ab(i),    // aircraft balance
	db(j),    // demand balance
	yd(j,h),  // definition of boarded passangers
	bd(j,h),  // definition of bumped passangers
	ocd,     // 
	bcd1,     // bumping cost definition: version 1
	bcd2;     // bumping cost definition: version 2

    
    ab(i) =   sum(j, x(i,j)) <= aa(i);
	ab.setName("aircraft balance");

    db(j) =  sum(i, p(i,j)*x(i,j)) >= sum( h.such_that(deltb(j,h)>0), y(j,h));
	db.setName("demand balance");

    yd(j,h) = y(j,h) <= sum(i, p(i,j)*x(i,j));
	yd.setName("definition of boarded passengers");

    bd(j,h) = b(j,h) == dd(j,h) - y(j,h);
	bd.setName("definition of bumped passengers");

    ocd() =     oc() == sum(i*j, c(i,j)*x(i,j));

    bcd1() =    bc() == sum(j, k(j)*(ed(j)-sum(h, gamma(j,h)*y(j,h))));

    bcd2() =    bc() == sum(j*h, k(j)*lambda(j,h)*b(j,h));

    MP_model m1(new OsiCbcSolverInterface), m2(new OsiCbcSolverInterface);

    y.upperLimit(j,h) = deltb(j,h);

    y.upperLimit.display("y upper");

    m1.add(ab).add(db).add(bcd1).add(ocd);
    m1.minimize(oc() + bc());

    assert(m1->getNumRows()==11);
    assert(m1->getNumCols()==47);
    assert(m1->getNumElements()==96);
    assert(m1->getObjValue()>=1566.03 && m1->getObjValue()<=1566.05);
   
    y.display("y first model");
    ab.display("ab");
    db.display("db");
    bcd1.display("bcd1");
    ocd.display("ocd");

    m2.add(ab).add(yd).add(bd).add(bcd2).add(ocd);
    y.upperLimit(j,h) = m2->getInfinity();

    m2.minimize(oc() + bc());
 
    assert(m2->getNumRows()==56);
    assert(m2->getNumCols()==72);
    assert(m2->getNumElements()==219);
    assert(m2->getObjValue()>=1566.03 && m2->getObjValue()<=1566.05);

    y.display("y second model");
    cout<<"Test aircraft passed."<<endl;
}

// Optimal objective value m1: 1566.04
// Optimal objective value m2: 1566.04 (like m1)
