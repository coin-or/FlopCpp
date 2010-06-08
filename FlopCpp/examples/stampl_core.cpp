// $Id$
#include "flopc.hpp"
using namespace flopc;
#include "OsiCbcSolverInterface.hpp"

/* FLOPC++ implementation of a financial planning and control model from
"StAMPL: A filtration-oriented modeling toll for stochastic programming" by Fourer and Lopes
*/


main() {
    MP_model::getDefaultModel().setSolver(new OsiCbcSolverInterface);
    MP_model::getDefaultModel().verbose();

    enum { STOCKS, BONDS, numINSTR};
    const int numStages = 3;

    MP_set I(numINSTR);
    MP_set T(numStages);

    MP_data Return(T,I);

    MP_variable Buy(T,I);
    MP_variable Shortage, Overage;

    MP_constraint InvestAll;
    MP_constraint ReinvestAll(T);
    MP_constraint Goal;

    double initial_wealth = 55;
    double goal = 80;
    
    MP_data GoodReturn(I);
    MP_data BadReturn(I);
    GoodReturn(STOCKS) = 1.25;   GoodReturn(BONDS) = 1.14;
    BadReturn(STOCKS)  = 1.06;   BadReturn(BONDS)  = 1.12;
 

    InvestAll() = sum(I, Buy(0,I)) == initial_wealth;

    ReinvestAll(T+1) = sum(I, Buy(T,I) * Return(T,I)) == sum(I, Buy(T+1,I));

    Goal() = sum(I, Buy(T.last(),I) * Return(T.last(),I)) == goal - Shortage() + Overage();

    // Optimistic
    Return(T,I) = GoodReturn(I);
    maximize( Overage() - 4*Shortage() );
    Buy.display("Buy (optimistic)");

    // Pessimistic
    Return(T,I) = BadReturn(I);
    maximize( Overage() - 4*Shortage() );
    Buy.display("Buy (pessimistic)");
}
