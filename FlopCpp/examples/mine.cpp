// $Id$
#include "flopc.hpp"
using namespace flopc;
#include "OsiCbcSolverInterface.hpp"

int main() {
  MP_model::getDefaultModel().setSolver(new OsiCbcSolverInterface);
  enum {nw,ne,se,sw,numK};

  MP_set l(4);  // identfiers for level row and column labels  / 1*4 /;

  MP_index i,j;

  MP_data conc(l,l,l);

  conc(0,0,0) =  1.5;
  conc(0,0,1) =  1.5;
  conc(0,0,2) =  1.5;
  conc(0,0,3) =  0.75;
 
  conc(0,1,0) =  1.5;
  conc(0,1,1) =  2.0;
  conc(0,1,2) =  1.5;
  conc(0,1,3) =  0.75;
 
  conc(0,2,0) =  1.0;
  conc(0,2,1) =  1.0;
  conc(0,2,2) =  0.75;
  conc(0,2,3) =  0.50;
 
  conc(0,3,0) =  0.75;
  conc(0,3,1) =  0.75;
  conc(0,3,2) =  0.5;
  conc(0,3,3) =  0.25;
 
  conc(1,0,0) =  4;
  conc(1,0,1) =  4;
  conc(1,0,2) =  2;

  conc(1,1,0) =  3;
  conc(1,1,1) =  3;
  conc(1,1,2) =  1;

  conc(1,2,0) =  2;
  conc(1,2,1) =  2;
  conc(1,2,2) =  0.5;

  conc(2,0,0) =  12;
  conc(2,0,1) =  6;

  conc(2,1,0) =  5;
  conc(2,1,1) =  4;

  conc(3,0,0) =  6;

  MP_set  k(numK); //  location of four neighbouring blocks / nw, "ne", se, sw /
  MP_subset<3> c(l,l,l); //  neighbouring blocks related to extraction feasibility
  MP_subset<3> d(l,l,l); //  complete set of block identifiers

  MP_data li(k), // lead for i  / (se,sw)  = 1 /
    lj(k),     // lead for j  / ("ne",se) = 1 /
    cost(l);   // block extraction cost  / 1=3000, 2=6000, 3=8000, 4=10000 /

  li(se) = 1;
  li(sw) = 1;
  lj(ne) = 1;
  lj(se) = 1;

  cost(0) = 3000;
  cost(1) = 6000;
  cost(2) = 8000;
  cost(3) =10000; 

  double value = 200000;

  forall(l*l(i)*l(j).such_that( l+i<l.size()-1 && l+j<l.size()-1 ),  
	 c.insert(l,i,j)
	 );

  c.display("c");

  forall(l*l(i)*l(j).such_that( l+i<l.size() && l+j<l.size() ),  
	 d.insert(l,i,j)
	 );

  d.display("d");

  MP_model::getDefaultModel().verbose();

  MP_variable x(l,l,l); //   extraction of blocks

  MP_constraint pr(k,c); //  precedence relationships

  pr(k,c(l,i,j)) = x(l,i+li(k),j+lj(k)) >= x(l+1,i,j);

  x.upperLimit(l,i,j) = 1;

  maximize( sum(d(l,i,j), (conc(l,i,j)*value/100 - cost(l))*x(l,i,j)) );

  assert(MP_model::getDefaultModel()->getNumRows()==56);
  assert(MP_model::getDefaultModel()->getNumCols()==64);
  assert(MP_model::getDefaultModel()->getNumElements()==112);
  assert(MP_model::getDefaultModel()->getObjValue()>=17500 && MP_model::getDefaultModel()->getObjValue()<=17500);


  x.display("x");
  cout<<"Test mine passed."<<endl;
}
