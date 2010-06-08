// $Id$
#include "flopc.hpp"
using namespace flopc;
#include "OsiCbcSolverInterface.hpp"

/* FLOPC++ implementation of a financial planning and control model from
"StAMPL: A filtration-oriented modeling tool for stochastic programming" by Fourer and Lopes
*/


main() {
    MP_model::getDefaultModel().setSolver(new OsiCbcSolverInterface);
    MP_model::getDefaultModel().verbose();

    enum { STOCKS, BONDS, numINSTR};
    const int numStages = 3;
    const int numScenarios = 8;

    MP_set INSTR(numINSTR);
    MP_set S(numScenarios);
    MP_set T(numStages);

    MP_subset<2> N(T,S);

    N.insert(0,0);
    


    MP_data Return(T,S,INSTR);

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
 
    Return(T,INSTR).probability(0.5) = GoodReturn(INSTR);
    Return(T,INSTR).probability(0.5) = BadReturn(INSTR);

    //     forall(T,
    // 	   Return(T,INSTR).probability(0.5) = GoodReturn(INSTR));
    //     forall(T,
    // 	   Return(T,INSTR).probability(0.5) = BadReturn(INSTR));

    //   Return(T,INSTR) = GoodReturn(INSTR).probability(0.5);
    //   Return(T,INSTR) = BadReturn(INSTR).probability(0.5);
    //---------------------------------


    InvestAll() = sum(INSTR, Buy(0,0,INSTR)) == initial_wealth;

    ReinvestAll(T+1,S) = sum(INSTR, Buy(T,S,INSTR) * Return(T,S,INSTR)) == sum(INSTR, Buy(T+1,S,INSTR));

    Goal(S) = sum(INSTR, Buy(T.last(),S), INSTR) * Return(N(T.last(),S),INSTR)) == goal - Shortage(S) + Overage(S);
    
    maximize( sum(S, prob(S)*(Overage(S) - 4*Shortage(S) );

}
