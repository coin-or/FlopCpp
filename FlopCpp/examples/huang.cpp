#include "huang.hpp"
#include <OsiCbcSolverInterface.hpp>


model::model(int numS1,
	     int numS2){
  S1= MP_set(numS1); 
  S2 = MP_set(numS2);
  mm= new MP_model(new OsiCbcSolverInterface);
  Y=new MP_binary_variable(S1);
            
};


model::~model(){
  delete mm; 
  delete Y;
};

    
void model::f1(){
  MP_constraint c2;  // <===will run if this line is commented out.
};


void model::f2(){

  MP_constraint c1; 
  c1=sum(S1, (*Y)(S1))<=2;
  (*mm).add(c1);

  (*mm).maximize(sum(S1, (*Y)(S1)));
  cout<<"Y size: "<<(*Y).size()<<endl;
  (*Y).display();
 
}
