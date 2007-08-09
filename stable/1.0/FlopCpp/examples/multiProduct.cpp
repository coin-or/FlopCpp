// $Id$
#include <OsiCbcSolverInterface.hpp>
#include <flopc.hpp>
using namespace flopc;

void MultiProduct(
    MP_set& Cities,
    MP_set& Products,
    MP_subset<3>& Routes,
    MP_subset<2>& Suppliers,
    MP_subset<2>& Customers,
    MP_subset<2>& Connections,
    MP_data& supply,
    MP_data& demand,
    double& limit, 
    MP_data& cost
) {
    MP_index p,o,d;
    MP_variable trans(Routes);

    MP_constraint sup(Suppliers);
    MP_constraint dem(Customers);
    MP_constraint lim(Connections);

    sup(Suppliers(p,o)) =
	sum(Routes(p,o,d), trans(Routes(p,o,d))) <= supply(Suppliers(p,o));
							   
    dem(Customers(p,d)) =
 	sum(Routes(p,o,d), trans(Routes(p,o,d))) >= demand(Customers(p,d));

    lim(Connections(o,d)) =
	sum(Routes(p,o,d), trans(Routes(p,o,d))) <= limit;

    minimize( sum(Routes(p,o,d), cost(Routes(p,o,d)) * trans(Routes(p,o,d))) );

    assert(MP_model::getDefaultModel()->getNumRows()==41);
    assert(MP_model::getDefaultModel()->getNumCols()==18);
    assert(MP_model::getDefaultModel()->getNumElements()==54);

    trans.display("solution");
}

double random(double min, double max) {
    return min + (max-min)*(rand()/(RAND_MAX+0.0));
}

int main() {
    MP_model::getDefaultModel().setSolver(new OsiCbcSolverInterface);
    MP_model::getDefaultModel().verbose();
    enum {Godiva,Neuhaus,Leonidas,nbrProducts};
    enum {Brussels,Amsterdam,Antwerpen,Paris,Milan,Cassis,Bonn,Madrid,Bergen,
	  London,nbrCities};
    
    MP_set Cities(nbrCities);
    MP_set Products(nbrProducts);
    MP_subset<3> Routes(Products,Cities,Cities);

    Routes.insert(Godiva,Brussels,Paris);
    Routes.insert(Godiva,Brussels,Bonn);
    Routes.insert(Godiva,Amsterdam,London);
    Routes.insert(Godiva,Amsterdam,Milan);
    Routes.insert(Godiva,Antwerpen,Madrid);
    Routes.insert(Godiva,Antwerpen,Bergen);

    Routes.insert(Neuhaus,Brussels,Madrid);
    Routes.insert(Neuhaus,Brussels,Bergen);
    Routes.insert(Neuhaus,Amsterdam,Madrid);
    Routes.insert(Neuhaus,Amsterdam,Cassis);
    Routes.insert(Neuhaus,Antwerpen,Paris);
    Routes.insert(Neuhaus,Antwerpen,Bonn);

    Routes.insert(Leonidas,Brussels,Bonn);
    Routes.insert(Leonidas,Brussels,Milan);
    Routes.insert(Leonidas,Amsterdam,Paris);
    Routes.insert(Leonidas,Amsterdam,Cassis);
    Routes.insert(Leonidas,Antwerpen,London);
    Routes.insert(Leonidas,Antwerpen,Bergen);

    cout<<"Number of Routes = "<<Routes.size()<<endl;

    MP_subset<2> Suppliers(Products,Cities);
    MP_subset<2> Customers(Products,Cities);
    MP_subset<2> Connections(Cities,Cities);

    MP_index p,o,d;

    // Set "projections"
    Suppliers(p,o) <<= Routes(p, o, MP_index::Any());
    Customers(p,d) <<= Routes(p, MP_index::Any(), d);
    Connections(o,d) <<= Routes(MP_index::Any(), o, d);

    cout<<Suppliers.size()<<endl;
    cout<<Customers.size()<<endl;
    cout<<Connections.size()<<endl;

    Routes.display();
    Suppliers.display();
    Customers.display();
    Connections.display();

    MP_data supply(Suppliers);
    MP_data demand(Customers);

    MP_data cost(Routes);

    for (int i=0; i<Suppliers.size(); i++) {
	supply(i) = random(200,300);
    }
    for (int i=0; i<Customers.size(); i++) {
	demand(i) = random(50,100);
    }
    for (int i=0; i<Routes.size(); i++) {
	cost(i) = random(1,2);
    }
    double limit = 500;

    MultiProduct(Cities,Products,Routes,Suppliers,Customers, Connections,
		 supply,demand,limit,cost);

    cout<<"Test multiproduct passed."<<endl;
}

