// $Id$
#include "flopc.hpp"
using namespace flopc;
#include "SmiScnModel.hpp"
#include "SmiScnData.hpp"
#include "OsiClpSolverInterface.hpp"

/* FLOPC++ implementation of a financial planning and control model from
"StAMPL: A filtration-oriented modeling tool for stochastic programming" by Fourer and Lopes
*/


main() {
  OsiClpSolverInterface *osiClp1 = new OsiClpSolverInterface();
  double INF=osiClp1->getInfinity();
   
  int ncol=8, nrow=4, nels=14;
   
  int mrow[]={ 0,0,0,0,1,2,1,2,1,2,1,2,3,3 };
  int mcol[]={ 6,7,1,0,2,4,3,5,4,6,5,7,2,3 };
   
  double dels[] = { 1.06, 1.12, 1, -1, 1.06, 1.06, 1.12, 1.12, -1, -1,
                    -1, -1, 1, 1 };
  double dobj[]={ 4,-1,0,0,0,0,0,0 };

  /* Column bounds */
  double dclo[]={ 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0 };
  double dcup[]={INF,INF,INF,INF,INF,INF,INF,INF};

  /* Row bounds */
  double drlo[]={80,0,0,55};
  double drup[]={80,0,0,55};
   
  /* Stages */
  int rstg[]={ 3,1,2,0};
  int cstg[]={ 3,3,0,0,1,1,2,2};
               
  SmiScnModel *smiModel = new SmiScnModel();
  smiModel->setOsiSolverHandle(*osiClp1);
               
  OsiClpSolverInterface ocsi;
  ocsi.loadProblem(CoinPackedMatrix(
                     1,mrow,mcol,dels,nels),dclo,dcup,dobj,drlo,drup);
               
  SmiCoreData *smiCore = new SmiCoreData(&ocsi,4,cstg,rstg);
   
  int rw[] = {0,0,2,2,1,1};
  int cl[] = {6,7,4,5,2,3};
  double el1[] = {1.25,1.14,1.25,1.14,1.25,1.14};
  double el2[] = {1.25,1.14,1.25,1.14,1.06,1.12};
  double el3[] = {1.25,1.14,1.06,1.12,1.25,1.14};
  double el4[] = {1.25,1.14,1.06,1.12,1.06,1.12};
  double el5[] = {1.06,1.12,1.25,1.14,1.25,1.14};
  double el6[] = {1.06,1.12,1.25,1.14,1.06,1.12};
  double el7[] = {1.06,1.12,1.06,1.12,1.25,1.14};
  double el8[] = {1.06,1.12,1.06,1.12,1.06,1.12};

  CoinPackedMatrix *cpm_mat1 = new
    CoinPackedMatrix(false,rw,cl,el1,6);
  CoinPackedMatrix *cpm_mat2 = new
    CoinPackedMatrix(false,rw,cl,el2,2);
  CoinPackedMatrix *cpm_mat3 = new
    CoinPackedMatrix(false,rw,cl,el3,4);
  CoinPackedMatrix *cpm_mat4 = new
    CoinPackedMatrix(false,rw,cl,el4,2);
  CoinPackedMatrix *cpm_mat5 = new
    CoinPackedMatrix(false,rw,cl,el5,6);
  CoinPackedMatrix *cpm_mat6 = new
    CoinPackedMatrix(false,rw,cl,el6,2);
  CoinPackedMatrix *cpm_mat7 = new
    CoinPackedMatrix(false,rw,cl,el7,4);
  CoinPackedMatrix *cpm_mat8 = new
    CoinPackedMatrix(false,rw,cl,el8,2);

//     DOESNT HARM TO DO THIS INSTEAD
//     CoinPackedMatrix *cpm_mat1 = new
  CoinPackedMatrix(false,rw,cl,el1,6);
//     CoinPackedMatrix *cpm_mat2 = new
  CoinPackedMatrix(false,rw,cl,el2,6);
//     CoinPackedMatrix *cpm_mat3 = new
  CoinPackedMatrix(false,rw,cl,el3,6);
//     CoinPackedMatrix *cpm_mat4 = new
  CoinPackedMatrix(false,rw,cl,el4,6);
//     CoinPackedMatrix *cpm_mat5 = new
  CoinPackedMatrix(false,rw,cl,el5,6);
//     CoinPackedMatrix *cpm_mat6 = new
  CoinPackedMatrix(false,rw,cl,el6,6);
//     CoinPackedMatrix *cpm_mat7 = new
  CoinPackedMatrix(false,rw,cl,el7,6);
//     CoinPackedMatrix *cpm_mat8 = new
  CoinPackedMatrix(false,rw,cl,el8,6);

 
  smiModel->generateScenario(smiCore,cpm_mat1,NULL,NULL,NULL,NULL,NULL, 1,
                             0, 0.125);
 
  smiModel->generateScenario(smiCore,cpm_mat2,NULL,NULL,NULL,NULL,NULL, 3,
                             0, 0.125);
 
  smiModel->generateScenario(smiCore,cpm_mat3,NULL,NULL,NULL,NULL,NULL, 2,
                             0, 0.125);
 
  smiModel->generateScenario(smiCore,cpm_mat4,NULL,NULL,NULL,NULL,NULL, 3,
                             2, 0.125);
 
  smiModel->generateScenario(smiCore,cpm_mat5,NULL,NULL,NULL,NULL,NULL, 1,
                             0, 0.125);
 
  smiModel->generateScenario(smiCore,cpm_mat6,NULL,NULL,NULL,NULL,NULL, 3,
                             4, 0.125);
 
  smiModel->generateScenario(smiCore,cpm_mat7,NULL,NULL,NULL,NULL,NULL, 2,
                             4, 0.125);
 
  smiModel->generateScenario(smiCore,cpm_mat8,NULL,NULL,NULL,NULL,NULL, 3,
                             6, 0.125);
   
  smiModel->loadOsiSolverData();
   
  OsiSolverInterface *smiOsi = smiModel->getOsiSolverInterface();

  smiOsi->writeMps("stampl2");

  // set some parameters
  smiOsi->setHintParam(OsiDoPresolveInInitial,true);
  smiOsi->setHintParam(OsiDoScale,true);
  smiOsi->setHintParam(OsiDoCrash,true);
  // solve using Osi Solver
  smiOsi->initialSolve();
               
  printf("Solved stochastic program \n");
  printf("Number of rows: %d\n",smiOsi->getNumRows());
  printf("Number of cols: %d\n",smiOsi->getNumCols());
  printf("Optimal value: %g\n",smiOsi->getObjValue());
}

