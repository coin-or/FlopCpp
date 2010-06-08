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
    const int numStages = 4;

    MP_set I(numINSTR);
    MP_set T(numStages);

    MP_subset<1> TB(T);
    MP_subset<1> TR(T);
    MP_subset<1> TT(T);

    forall(T.such_that(T<T.last()), TB.insert(T));
    forall(T.such_that(T>0), TR.insert(T));
    forall(T.such_that(T<T.last() && T>0), TT.insert(T));

    MP_data Return(T,I);
    MP_data initial_wealth;
    MP_data goal;

    MP_variable Buy(T,I);
    MP_variable Shortage, Overage;

    MP_constraint InvestAll;
    MP_constraint ReinvestAll(T);
    MP_constraint Goal;

    InvestAll() = sum(I, Buy(0,I)) == initial_wealth();

    ReinvestAll(T).such_that(T<T.last() && T>0) = sum(I, Buy(T-1,I) * Return(T,I)) == sum(I, Buy(T,I));

    Goal() = sum(I, Buy(T.last()-1,I) * Return(T.last(),I)) == goal() - Shortage() + Overage();

    /// --------------------------------------------------------------
    initial_wealth() = 55;
    goal() = 80;
    
    MP_data GoodReturn(I);
    MP_data BadReturn(I);
    GoodReturn(STOCKS) = 1.25;   GoodReturn(BONDS) = 1.14;
    BadReturn(STOCKS)  = 1.06;   BadReturn(BONDS)  = 1.12;

    // Optimistic
    Return(TR,I) = GoodReturn(I);
    maximize( Overage() - 4*Shortage() );
    Buy.display("Buy (optimistic)");

    // Pessimistic
    Return(TR,I) = BadReturn(I);
    maximize( Overage() - 4*Shortage() );
    Buy.display("Buy (pessimistic)");
}
