// $Id$
#include "flopc.hpp"
using namespace flopc;
#include <OsiClpSolverInterface.hpp>

int main() {
    MP_model::getDefaultModel().setSolver(new OsiClpSolverInterface);
    enum  {seattle, sandiego, numS}; 
    enum  {newyork, chicago, topeka,numD};

    MP_set S(numS);          // Sources 
    MP_set D(numD);          // Destinations 
    MP_subset<2> Link(S,D);  // Transportation links (sparse subset of S*D)

    Link.insert(seattle,newyork);
    Link.insert(seattle,chicago);
    Link.insert(sandiego,chicago);
    Link.insert(sandiego,topeka);

    MP_data SUPPLY(S);
    MP_data DEMAND(D);

    SUPPLY(seattle)=350;  SUPPLY(sandiego)=600;
    DEMAND(newyork)=325;  DEMAND(chicago)=300;  DEMAND(topeka)=275;
    
    MP_data COST(Link);

    COST(Link(seattle,newyork)) = 2.5;  
    COST(Link(seattle,chicago)) = 1.7;  
    COST(Link(sandiego,chicago))= 1.8;  
    COST(Link(sandiego,topeka)) = 1.4;

    COST(Link) = 90 * COST(Link) / 1000.0;
    
    MP_variable x(Link);
    x.display("...");

    MP_constraint supply(S);
    MP_constraint demand(D);  

    supply.display("...");

    supply(S) =  sum( Link(S,D), x(Link) ) <= SUPPLY(S);
    demand(D) =  sum( Link(S,D), x(Link) ) >= DEMAND(D);
    
    cout<<"Here"<<endl;

    minimize( sum(Link, COST(Link)*x(Link)) );
    assert(MP_model::getDefaultModel()->getNumRows()==5);
    assert(MP_model::getDefaultModel()->getNumCols()==4);
    assert(MP_model::getDefaultModel()->getNumElements()==8);
    assert(MP_model::getDefaultModel()->getObjValue()>=156.14 && MP_model::getDefaultModel()->getObjValue()<=156.16);
    
    x.display("Optimal solution:");
    supply.display("Supply dual solution");
    cout<<"Test transport passed."<<endl;
}
