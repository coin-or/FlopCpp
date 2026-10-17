// $Id$
#include "flopc.hpp"
using namespace flopc;
using namespace std;
#include "OsiCbcSolverInterface.hpp"

/* FLOPC++ implementation of a financial planning and control model from
"StAMPL: A filtration-oriented modeling toll for stochastic programming" by Fourer and Lopes
*/


main() {
    MP_model::getDefaultModel().setSolver(new OsiCbcSolverInterface);
    MP_model::getDefaultModel().verbose();

    enum { STOCKS, BONDS, numINSTR};
    const int numStages = 3;

    MP_set INSTR(numINSTR);
    MP_stage T(numStages);

    MP_stochastic_data Return(T,INSTR);

    MP_variable Buy(T,INSTR);
    MP_variable Shortage, Overage;

    MP_constraint InvestAll;
    MP_constraint ReinvestAll(T);
    MP_constraint Goal;

    double initial_wealth = 55;
    double goal = 80;
    
    //---------------------------------
    MP_data GoodReturn(INSTR);
    MP_data BadReturn(INSTR);

    GoodReturn(STOCKS) = 1.25;   GoodReturn(BONDS) = 1.14;
    BadReturn(STOCKS)  = 1.06;   BadReturn(BONDS)  = 1.12;
 
    Return(T,INSTR) = 1.33;

    //   Return(T,INSTR) = GoodReturn(INSTR).probability(0.5);
    //   Return(T,INSTR) = BadReturn(INSTR).probability(0.5);
    //---------------------------------

    InvestAll() = sum(INSTR, Buy(0,INSTR)) == initial_wealth;

    ReinvestAll(T+1) = sum(INSTR, Buy(T,INSTR) * Return(T,INSTR)) == sum(INSTR, Buy(T+1,INSTR));

    Goal() = sum(INSTR, Buy(T.last(),INSTR) * Return(T.last(),INSTR)) == goal - Shortage() + Overage();

    maximize( Overage() - 4*Shortage() );

}
