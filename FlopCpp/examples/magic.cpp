// $Id$
#include "flopc.hpp"
using namespace flopc;
#include <OsiCbcSolverInterface.hpp>

// M A G I C   Power Scheduling Problem 
/*   A number of power stations are committed to meet demand for a particular
   day. three types of generators having different operating characteristics
   are available.  Generating units can be shut down or operate between
   minimum and maximum output levels.  Units can be started up or closed down
   in every demand block.

   Reference: R E Day and H P Williams, MAGIC: The design and use of an
              interactive modeling language for mathematical programming,
              Department Business Studies, University of Edinburgh, Edinburgh,
              UK, February 1982.

              H P Williams, Model Building in Mathematical Programming, Wiley,
              New York, 1978.
*/

int main() {
    enum {t12pm_6am, t6am_9am, t9am_3pm, t3pm_6pm, t6pm_12pm, numT};
    enum {type_1, type_2, type_3, numG};

    MP_set T(numT); T.cyclic();
    MP_set G(numG);

    MP_data dem(T); // demand (1000mw)   
    dem(t12pm_6am) = 15; 
    dem(t6am_9am) = 30;
    dem(t9am_3pm) = 25; 
    dem(t3pm_6pm) = 40;
    dem(t6pm_12pm) = 27;

    MP_data dur(T);  // duration (hours)  
    dur(t12pm_6am) = 6; 
    dur(t6am_9am) = 3;
    dur(t9am_3pm) = 6; 
    dur(t3pm_6pm) = 3;
    dur(t6pm_12pm) = 6;

    MP_data 
	min_pow(G), 
	max_pow(G), 
	cost_min(G), 
	cost_inc(G), 
	start(G), 
	number(G);

    min_pow(type_1) = 0.85; max_pow(type_1) = 2.0;     
    min_pow(type_2) = 1.25; max_pow(type_2) = 1.75;    
    min_pow(type_3) = 1.5;  max_pow(type_3) = 4.0;     

    cost_min(type_1) = 1000; cost_inc(type_1) = 2.0;     
    cost_min(type_2) = 2600; cost_inc(type_2) = 1.3;   
    cost_min(type_3) = 3000; cost_inc(type_3) = 3.0;     

    start(type_1) = 2000; number(type_1) = 12;     
    start(type_2) = 1000; number(type_2) = 10;     
    start(type_3) = 500;  number(type_3) =  5;     

    MP_data  
	peak,     // peak power (1000 mw)
	ener(T),  // energy demand in load block (1000mwh)
	tener,    // total energy demanded (1000mwh)
	lf;       // load factor 

    MP_index i,j;

    peak() = maximum(T(i), dem(i));

    ener(i) = dur(i)*dem(i);  

    tener() = sum(T(i), ener(i));  

    lf() = tener()/(peak() * 24);

    peak.display();
    tener.display();
    lf.display();
    ener.display("ener");
  
    MP_variable
	x(G,T),  // generator output (1000mw)
	n(G,T),  // number of generators in use
	s(G,T);  // number of generators started up

    n.integer();
    n.upperLimit(j,i) = number(j);
    n.upperLimit.display("n upper");

    MP_constraint
	pow(T),    // demand for power (1000mw)
	res(T),    // spinning reserve requirements (1000mw)
	st(G,T),   // start_up definition
	minu(G,T), // minimum generation level (1000mw)
	maxu(G,T); // maximum generation level (1000mw)

    pow(i) =  sum(G(j), x(j,i)) >= dem(i);

    res(i) =  sum(G(j), max_pow(j)*n(j,i)) >= 1.15*dem(i);

    st(j,i) = s(j,i) >= n(j,i) - n(j,i-1);

    minu(j,i) =  x(j,i) >= min_pow(j)*n(j,i);

    maxu(j,i) =  x(j,i) <= max_pow(j)*n(j,i);

    MP_expression  cost;    // total operating cost (l)
    cost = sum(G(j)*T(i), 
	       dur(i)* cost_min(j)*n(j,i) + start(j)*s(j,i) +
	       1000*dur(i)*cost_inc(j)*(x(j,i)-min_pow(j)*n(j,i)) );
 

    MP_model::getDefaultModel().setSolver(new OsiCbcSolverInterface);
    MP_model::getDefaultModel().verbose();
    
    minimize(cost);

    assert(MP_model::getDefaultModel()->getNumRows()==55);
    assert(MP_model::getDefaultModel()->getNumCols()==45);
    assert(MP_model::getDefaultModel()->getNumElements()==135);
    assert(MP_model::getDefaultModel()->getObjValue()>=988539 && MP_model::getDefaultModel()->getObjValue()<=988541);


    x.display("x");
    n.display("n");
    s.display("s");
	const int TSize=T.size();
    double **rep = new double *[TSize];
	for(int cnt=0;cnt<TSize;cnt++)
	{
		rep[cnt]=new double[4];
	}
    for (unsigned i=0; i<T.size(); i++) {
	rep[i][0] = dem(i);

	rep[i][1] = 0;
	for (unsigned j=0; j<G.size(); j++) { 
	    rep[i][1] +=  max_pow(j) * n.level(j,i);
	}

	rep[i][2] = 0;
	for (unsigned j=0; j<G.size(); j++) { 
	    rep[i][2] +=  s.level(j,i);
	}

 	rep[i][3] = -pow.price(i)/dur(i)/1000;
    }

    for (unsigned i=0; i<T.size(); i++) {
	cout<<i<<"  "<<rep[i][0]<<"  "<<rep[i][1]<<"  "<<rep[i][2]<<"  "<<rep[i][3]<<endl;
    }

      cout<<"Test magic passed."<<endl;
}
