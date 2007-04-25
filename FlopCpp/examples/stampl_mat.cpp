// $Id$
#include "flopc.hpp"
using namespace flopc;
#include "OsiClpSolverInterface.hpp"
#include "OsiSolverInterface.hpp"

/* FLOPC++ implementation of a financial planning and control model from
"StAMPL: A filtration-oriented modeling tool for stochastic programming" by Fourer and Lopes
*/

const double inf = 9.9e+32;
const double r1 = 1.25;
const double r2 = 1.14;
const double R1 = 1.06;
const double R2 = 1.12;

class MP_tree {
public:
  MP_tree() {}
  
  int nStages() {return 0;}
  int nScenarios() {return 0;}

};


class MP_problem {
public:
  
  int m;
  int n;
  int nz;
  int *Cst;
  int *Rnr;
  double *Elm;
  double *bl;
  double *bu;
  double *c;
  double *l;
  double *u;
};



void loadStochasticProblem(
  int m,
  int n,
  int nz,
  int *Cst,
  int *Rnr,
  double *Elm,
  int *ElmLng,
  double *bl,
  double *bu,
  double *c,
  double *l,
  double *u,
  int *rstage,
  int *cstage,
  MP_tree& T
  ) {

  MP_problem DE;
  
  DE.m = 0;

  


}

void determininistic() {
  int m = 4;
  int n = 8;
  int nz = 14;
  int Cst[] = {0,2,4,6,8,10,12,13,14};
  
  int Rnr[] = {0,1,0,1,1,2,1,2,2,3,2,3,3,3};
  double Elm[] = {1, r1, 1, r2, -1, r1, -1, r2, -1, r1, -1, r2, -1, 1};
  double bl[] = {50,0,0,80};
  double bu[] = {50,0,0,80};
  double c[] = {0,0,0,0,0,0,1,-4};
  double l[] = {0,0,0,0,0,0,0,0};
  double u[] = {inf,inf,inf,inf,inf,inf,inf,inf};

  OsiSolverInterface* Solver = new OsiClpSolverInterface;

  Solver->loadProblem(n, m, Cst, Rnr, Elm, l, u, c, bl, bu);
  Solver->setObjSense(-1);
  Solver->initialSolve();
  
  if (Solver->isProvenOptimal() == true) {
    cout<<"FlopCpp: Optimal obj. value = "<<Solver->getObjValue()<<endl;
    cout<<"FlopCpp: Solver(m, n, nz) = "<<Solver->getNumRows()<<"  "<<
      Solver->getNumCols()<<"  "<<Solver->getNumElements()<<endl;
  }
}
  //////////////////////////////////////////

main() {

  int m = 4;
  int n = 8;
  int nz = 14;
  int Cst[] = {0,2,4,6,8,10,12,13,14};
  
  int Rnr[] = {0,1,0,1,1,2,1,2,2,3,2,3,3,3};
  double Elm[] = {1, r1, R1, 1, r2, R2, -1, r1, R1, r1, R1, -1, r2, R2, r2, R2,
                  -1, r1, R1, r1, R1, r1, R1, r1, R1, -1, r2, R2, r2, R2, r2, R2, r2, R2, -1, 1};
  int ElmLng[] = {1,2,1,2,1,4,1,4,1,8,1,8,1,1};
  double bl[] = {50,0,0,80};
  double bu[] = {50,0,0,80};
  double c[] = {0,0,0,0,0,0,1,-4};
  double l[] = {0,0,0,0,0,0,0,0};
  double u[] = {inf,inf,inf,inf,inf,inf,inf,inf};

  int rstage[] = {0,1,2,3};
  int cstage[] = {0,0,1,1,2,2,3,3};

//   OsiSolverInterface* Solver = new OsiClpSolverInterface;

//   Solver->loadProblem(n, m, Cst, Rnr, Elm, l, u, c, bl, bu);
//   Solver->setObjSense(-1);
//   Solver->initialSolve();
  
//   if (Solver->isProvenOptimal() == true) {
//     cout<<"FlopCpp: Optimal obj. value = "<<Solver->getObjValue()<<endl;
//    cout<<"FlopCpp: Solver(m, n, nz) = "<<Solver->getNumRows()<<"  "<<
//       Solver->getNumCols()<<"  "<<Solver->getNumElements()<<endl;
//   }



  MP_tree T;

  loadStochasticProblem(m,n,nz,Cst,Rnr,Elm,ElmLng,bl,bu,c,l,u,rstage,cstage,T);
  cout<<"done"<<endl;

}
