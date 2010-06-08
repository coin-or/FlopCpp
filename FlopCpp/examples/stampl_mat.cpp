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
  
  virtual int nStages() {return 0;}
  virtual int nScenarios(int stage) {return 0;}
  virtual int getindex(int rs, int cs, int q) {return 0;}
  virtual double probability(int stage, int q) {return 1;}
};



class MP_binarytree : public MP_tree {
public:
    MP_binarytree(int depth) : D(depth) {}
    
    virtual int nStages() {
        return D+1;
    }
    
    virtual int nScenarios(int stage) {
        return int(pow(2,stage));
    }

    virtual int getindex(int rs, int cs, int q) {
        return q / int(pow(2,(rs-cs)));
    }

    virtual double probability(int stage, int q) {
        return 1.0 / double(nScenarios(stage));
    }

    int D;
};


class MP_problem {
public:
  
  int m;
  int n;
  int nz;
  int *Cst;
  int *Clg;
  int *Rnr;
  double *Elm;
  double *bl;
  double *bu;
  double *c;
  double *l;
  double *u;
};

struct Cof {
  Cof(int r, int c, double v) : 
    row(r), col(c), val(v)  { 
    cout<<row<<" "<<col<<" "<<val<<endl;
  }
  int col, row;
  double val;
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
    MP_tree& T) 
{
    MP_problem DE;
    
    int *m_off = new int[m];
    int *n_off = new int[n];

    DE.m = 0;
    for (int i=0; i<m; i++) {
        m_off[i] = DE.m;
        DE.m += T.nScenarios(rstage[i]);
    }
    DE.n = 0;
    for (int j=0; j<n; j++) {
        n_off[j] = DE.n;
        DE.n += T.nScenarios(cstage[j]); 
    }
    
    vector<Cof> coeficients;

    int k = 0;
    int ke = 0;
    for (int j=0; j<n; j++) {
        int cs = cstage[j];
        for ( ; k<Cst[j+1]; k++) {
            int i = Rnr[k];
            int s = rstage[i];
            if (ElmLng[k] == 1) { 
                for (int q=0; q<T.nScenarios(s); q++) {
                    coeficients.push_back(Cof(m_off[i]+q,
                                              n_off[j]+T.getindex(s,cs,q),
                                              Elm[ke]));
                }
                ke++;
            } else {
                for (int q=0; q<T.nScenarios(s); q++) {
                    coeficients.push_back(Cof(m_off[i]+q,
                                              n_off[j]+T.getindex(s,cs,q),
                                              Elm[ke]));
                    ke++;
                }
            }
        }
    }
                
    DE.nz = coeficients.size();
                
    cout<<coeficients.size()<<endl;

    if (DE.nz>0) {    
      DE.Elm = new double[DE.nz]; 
      DE.Rnr = new int[DE.nz];    
    }
    DE.Cst = new int[DE.n+1];   
    DE.Clg = new int[DE.n];   
    for (int j=0; j<DE.n; j++) {
      DE.Clg[j] = 0;
    }
    for (int i=0; i<DE.nz; i++) {
      int col = coeficients[i].col;
      DE.Clg[col]++;
    }
    DE.Cst[0]=0;
    for (int j=0; j<DE.n; j++) {
      DE.Cst[j+1]=DE.Cst[j]+DE.Clg[j]; 
    }
    for (int i=0; i<DE.n; i++) {
      DE.Clg[i]=0;
    }
    
    for (int i=0; i<DE.nz; i++) {
      int col = coeficients[i].col;
      int row = coeficients[i].row;
      double elm = coeficients[i].val;
      DE.Elm[DE.Cst[col]+DE.Clg[col]] = elm;
      DE.Rnr[DE.Cst[col]+DE.Clg[col]] = row;
      DE.Clg[col]++;
    }


    cout<<DE.Cst[DE.n]<<"="<<DE.nz<<endl;

    DE.bl = new double[DE.m];
    DE.bu = new double[DE.m];
    k = 0;
    for (int i=0; i<m; i++) {
      for (int q=0; q<T.nScenarios(rstage[i]); q++) {
        DE.bl[k] = bl[i];
        DE.bu[k] = bu[i];
        cout<<k<<" .."<<DE.bu[k]<<endl;
        k++;
      }
    }

    cout<<k<<"="<<DE.m<<endl;

    DE.c = new double[DE.n];
    DE.l = new double[DE.n];
    DE.u = new double[DE.n];
    k = 0;
    for (int j=0; j<n; j++) {
      for (int q=0; q<T.nScenarios(cstage[j]); q++) {
        DE.l[k] = l[j];
        DE.u[k] = u[j];
        DE.c[k] = c[j] * T.probability(cstage[j],q);
        cout<<DE.c[k]<<endl;
        k++;
      }
    }

    cout<<k<<"="<<DE.n<<endl;

    OsiSolverInterface* Solver = new OsiClpSolverInterface;

    Solver->loadProblem(DE.n, DE.m, DE.Cst, DE.Rnr, DE.Elm, DE.l, DE.u, DE.c, 
                        DE.bl, DE.bu);

    Solver->writeMps("stampl_mat","mps",-1.0);
    Solver->setObjSense(-1);
    Solver->initialSolve();
}

void determininistic() {
  int m = 4;
  int n = 8;
  int nz = 14;
  int Cst[] = {0,2,4,6,8,10,12,13,14};
  
  int Rnr[] = {0,1,0,1,1,2,1,2,2,3,2,3,3,3};
  double Elm[] = {1, r1, 1, r2, -1, r1, -1, r2, -1, r1, -1, r2, -1, 1};
  double bl[] = {55,0,0,80};
  double bu[] = {55,0,0,80};
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
  double bl[] = {55,0,0,80};
  double bu[] = {55,0,0,80};
  double c[] = {0,0,0,0,0,0,1,-4};
  double l[] = {0,0,0,0,0,0,0,0};
  double u[] = {inf,inf,inf,inf,inf,inf,inf,inf};

  int rstage[] = {0,1,2,3};
  int cstage[] = {0,0,1,1,2,2,3,3};

  MP_binarytree T(3);

  loadStochasticProblem(m,n,nz,Cst,Rnr,Elm,ElmLng,bl,bu,c,l,u,rstage,cstage,T);
  cout<<"done"<<endl;
}
