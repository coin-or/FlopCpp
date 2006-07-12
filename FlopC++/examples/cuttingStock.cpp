// $Id$
#include "flopc.hpp"
using namespace flopc;
#include <OsiCbcSolverInterface.hpp>
#include <CoinPackedVector.hpp>

class Paper : public MP_model {
public:
    MP_set WIDTHS,PATTERNS;
    MP_data widths;
    MP_data demand;
    MP_data pattern;
    MP_variable use;
    MP_constraint demC;

    Paper(int numWidths) : MP_model(new OsiCbcSolverInterface),
	      WIDTHS(numWidths), PATTERNS(numWidths),
	      widths(WIDTHS), demand(WIDTHS),
	      pattern(WIDTHS,PATTERNS), use(WIDTHS),
	      demC(WIDTHS) {

	MP_index i,j;
    
	demC(i) = sum(WIDTHS(j), pattern(i,j)*use(j)) >= demand (i);

        setObjective( sum(WIDTHS(j), use(j)) );
    }
};

double knapsack(int n, double* a, const double* c, double b, double* pat) {
    MP_model KnapSack(new OsiCbcSolverInterface);

    MP_set I(n);
    MP_data A(I);
    MP_data C(I);

    A.value(a);
    C.value(c);

    A.display("A");
    C.display("C");

    MP_variable x(I);
    x.integer();

    MP_constraint constr;

    constr = sum(I, A(I)*x(I)) <= b;
    KnapSack.add(constr);
    KnapSack.maximize( sum(I, C(I)*x(I)) );

    x.display("x");
    for (int i=0; i<n; i++) {
	pat[i] = x.level(i);
    }
    return KnapSack->getObjValue();
}

int main() {
    const int numWidths = 5;
    double tabDemand[] = {150, 96, 48, 108, 227};
    double tabWidth[] = {17, 21, 22.5, 24, 29.5};
    const double maxWidth = 94;

    MP_model Paper(new OsiCbcSolverInterface);

    MP_set WIDTHS(numWidths);
    MP_set PATTERNS(numWidths);
    MP_data width(WIDTHS);
    MP_data demand(WIDTHS);
    MP_data pattern(WIDTHS,PATTERNS);

    width.value(tabWidth);
    demand.value(tabDemand);

    for (int i=0; i<numWidths; i++) {
	pattern(i,i) = floor(maxWidth/width(i));
    }

    pattern.display("pattern");

    MP_variable use(PATTERNS);
    MP_index i,j;
    MP_constraint demC(WIDTHS);

    demC(i) = sum(WIDTHS(j), pattern(i,j)*use(j)) >= demand (i);
  
    Paper.add(demC);
    Paper.minimize( sum(WIDTHS(j), use(j)) );

    double pat[numWidths];
    double ob;
    do {
	ob = knapsack(numWidths, tabWidth, &Paper.rowPrice[demC.offset], maxWidth, pat);

	CoinPackedVector Pat;	
	Pat.setFull(numWidths, pat);
	
	Paper->addCol(Pat,0,100,1);
	Paper->resolve();
    } while(ob>1.0001);

    Paper.minimize( sum(WIDTHS(j), use(j)) );

    assert(Paper->getNumRows()==5);
    assert(Paper->getNumCols()==5);
    assert(Paper->getNumElements()==5);
    assert(Paper->getObjValue()>=177.666 && Paper->getObjValue()<=177.667);
   
    cout<<"Test cuttingstock passed."<<endl;
}

