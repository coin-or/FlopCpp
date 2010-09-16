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
    const int numScenarios = 8;
    const int numNodes = 15;

    MP_set I(numINSTR);
    MP_set S(numScenarios);
    MP_set T(numStages);
    MP_set N(numNodes);

    MP_subset<2> TN(T,N);
    MP_subset<2> NS(N,S);
    MP_subset<2> TS(T,S);

    TN.insert(0,0);
    TN.insert(1,1);
    TN.insert(1,2);
    TN.insert(2,3);
    TN.insert(2,4);
    TN.insert(2,5);
    TN.insert(2,6);
    TN.insert(3,7);
    TN.insert(3,8);
    TN.insert(3,9);
    TN.insert(3,10);
    TN.insert(3,11);
    TN.insert(3,12);
    TN.insert(3,13);
    TN.insert(3,14);

    NS.insert(0,0); NS.insert(0,1); NS.insert(0,2); NS.insert(0,3); NS.insert(0,4); NS.insert(0,5); NS.insert(0,6); NS.insert(0,7);
    NS.insert(1,0); NS.insert(1,1); NS.insert(1,2); NS.insert(1,3); NS.insert(2,4); NS.insert(2,5); NS.insert(2,6); NS.insert(2,7);
    NS.insert(3,0); NS.insert(3,1); NS.insert(4,2); NS.insert(4,3); NS.insert(5,4); NS.insert(5,5); NS.insert(6,6); NS.insert(6,7);
    NS.insert(7,0); NS.insert(8,1); NS.insert(9,2); NS.insert(10,3); NS.insert(11,4); NS.insert(12,5); NS.insert(13,6); NS.insert(14,7);
 
    forall(
      (TN(T,N)*NS(N,S)).such_that(NS(N,S+1)),
      TS.insert(T,S)
    );
    
    TS.display("TS");


    MP_data Return(T,S,I);

    MP_variable Buy(T,S,I);
    MP_variable Shortage(S), Overage(S);

    MP_constraint InvestAll(S);
    MP_constraint ReinvestAll(T,S);
    MP_constraint Goal(S);
    MP_constraint NonAnticipation(TS);

    double initial_wealth = 55;
    double goal = 80;
    
    //---------------------------------
    MP_data prob(S);
    MP_data gb(T,S);

    
    gb(1,S) = Constant(S/4)%2;
    gb(2,S) = Constant(S/2)%2;
    gb(3,S) = S%2;
    

    prob(S) = 0.125;

    MP_data GoodReturn(I);
    MP_data BadReturn(I);

    GoodReturn(STOCKS) = 1.25;   GoodReturn(BONDS) = 1.14;
    BadReturn(STOCKS)  = 1.06;   BadReturn(BONDS)  = 1.12;
 
    Return(T,S,I).such_that(gb(T,S) == Constant(0)) = GoodReturn(I);
    Return(T,S,I).such_that(gb(T,S) == 1) = BadReturn(I);

//     Return(2,S,I).such_that((S/2)%2 == 0) = GoodReturn(I);
//     Return(2,S,I).such_that((S/2)%2 == 1) = BadReturn(I);

//     Return(3,S,I).such_that(S%2 == 0) = GoodReturn(I);
//     Return(3,S,I).such_that(S%2 == 1) = BadReturn(I);

    Return.display("Return");


    //     forall(T,
    // 	   Return(T,I).probability(0.5) = GoodReturn(I));
    //     forall(T,
    // 	   Return(T,I).probability(0.5) = BadReturn(I));

    //   Return(T,I) = GoodReturn(I).probability(0.5);
    //   Return(T,I) = BadReturn(I).probability(0.5);
    //---------------------------------


    InvestAll(S) = sum(I, Buy(0,S,I)) == initial_wealth;

    ReinvestAll(T+1,S) = sum(I, Buy(T,S,I) * Return(T+1,S,I)) == sum(I, Buy(T+1,S,I));
    
    Goal(S) = sum(I, Buy(T.last()-1,S, I) * Return(T.last(),S,I)) == goal - Shortage(S) + Overage(S);
    
    NonAnticipation(TS(T,S)) =  Buy(T,S,I) == Buy(T,S+1,I);
    

    maximize( sum(S, prob(S)*(Overage(S) - 4*Shortage(S) )) );

    Buy.display("Buy");

}
