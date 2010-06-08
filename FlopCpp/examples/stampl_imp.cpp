// $Id$
#include "flopc.hpp"
using namespace flopc;
#include "OsiClpSolverInterface.hpp"

/* FLOPC++ implementation of a financial planning and control model from
"StAMPL: A filtration-oriented modeling tool for stochastic programming" by Fourer and Lopes
*/


main() {
     MP_model::getDefaultModel().setSolver(new OsiClpSolverInterface);
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
    MP_subset<2> ANC(N,N);

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

    MP_index NN;

    forall(
      (TN(T,N)*NS(N,S)*NS(NN,S)).such_that(TN(T-1,NN)), ANC.insert(N,NN)
    );
    
    ANC.display("ANC");

    MP_data Return(N,I);

    MP_variable Buy(N,I);
    MP_variable Shortage(N), Overage(N);

    MP_constraint InvestAll;
    MP_constraint ReinvestAll(TN);
    MP_constraint Goal(TN);
 
    double initial_wealth = 55;
    double goal = 80;
    

    MP_data gb(N);
    gb(N) = N%2;
    
    MP_data prob(N);
    prob(N) = 0.125;

    MP_data GoodReturn(I);
    MP_data BadReturn(I);

    GoodReturn(STOCKS) = 1.25;   GoodReturn(BONDS) = 1.14;
    BadReturn(STOCKS)  = 1.06;   BadReturn(BONDS)  = 1.12;
 
    Return(N,I).such_that(gb(N) == Constant(0)) = GoodReturn(I);
    Return(N,I).such_that(gb(N) == 1) = BadReturn(I);

    InvestAll.setName("InvestAll");
    ReinvestAll.setName("ReinvestAll");
    Goal.setName("Goal");

    Return.display("Return");

    InvestAll() = sum(I, Buy(0,I)) == initial_wealth;

    ReinvestAll(TN(T,N)).such_that(T<T.last() && T>0) = sum(I*ANC(N,NN),  Buy(NN,I)*Return(N,I) ) == sum(I, Buy(N,I));
    
//    Goal(S) = sum( (I*NS(N,S)*TN(T,N)).such_that(T==T.last()-1), Buy(N,I) * Return(N,I)) == goal - Shortage(S) + Overage(S);
    Goal(TN(T,N)).such_that(T==T.last())  = sum(I*ANC(N,NN), Buy(NN,I) * Return(N,I)) == goal - Shortage(N) + Overage(N);
  
    maximize( sum(N.such_that(N>6) , prob(N)*(Overage(N) - 4*Shortage(N)) ) );

    Buy.display("Buy");
    MP_model::getDefaultModel()->writeMps("stampl_implicit");
}
