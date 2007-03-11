// ******************** FlopCpp **********************************************
// File: MP_model.cpp
// $Id$
// Author: Tim Helge Hultberg (thh@mat.ua.pt)
// Copyright (C) 2003 Tim Helge Hultberg
// All Rights Reserved.
//****************************************************************************

#include <iostream>
#include <sstream>
using std::cout;
using std::endl;
#include <algorithm>

#include <CoinPackedMatrix.hpp>
#include <OsiSolverInterface.hpp>
#include "MP_model.hpp"
#include "MP_variable.hpp"
#include "MP_constraint.hpp"
#include <CoinTime.hpp>

using namespace flopc;

MP_model& MP_model::default_model = *new MP_model(0);
MP_model* MP_model::current_model = &MP_model::default_model;
MP_model &MP_model::getDefaultModel() { return default_model;}
MP_model *MP_model::getCurrentModel() { return current_model;}

void NormalMessenger::statistics(int bm, int m, int bn, int n, int nz) {
  cout<<"FlopCpp: Number of constraint blocks: " <<bm<<endl;
  cout<<"FlopCpp: Number of individual constraints: " <<m<<endl;
  cout<<"FlopCpp: Number of variable blocks: " <<bn<<endl;
  cout<<"FlopCpp: Number of individual variables: " <<n<<endl;
  cout<<"FlopCpp: Number of non-zeroes (including rhs): " <<nz<<endl;
}

void NormalMessenger::generationTime(double t) {
  cout<<"FlopCpp: Generation time: "<<t<<endl;
}

void VerboseMessenger::constraintDebug(string name, const vector<Coef>& cfs) {
  cout<<"FlopCpp: Constraint "<<name<<endl;
  for (unsigned int j=0; j<cfs.size(); j++) {
    int col=cfs[j].col;
    int row=cfs[j].row;
    double elm=cfs[j].val;
    int stage=cfs[j].stage;
    cout<<row<<"   "<<col<<"  "<<elm<<"  "<<stage<<endl;
  }
}

void VerboseMessenger::objectiveDebug(const vector<Coef>& cfs) {
  cout<<"Objective "<<endl;
  for (unsigned int j=0; j<cfs.size(); j++) {
    int col=cfs[j].col;
    int row=cfs[j].row;
    double elm=cfs[j].val;
    cout<<row<<"   "<<col<<"  "<<elm<<endl;
  }
}

MP_model::MP_model(OsiSolverInterface* s, Messenger* m) : 
  solution(0), messenger(m), Objective(0), Solver(s), 
  m(0), n(0), nz(0), bl(0),
  mSolverState(((s==NULL)?(MP_model::DETACHED):(MP_model::SOLVER_ONLY))) {
  MP_model::current_model = this;
}

MP_model::~MP_model() {
  delete messenger;
  if (Solver!=0) {
    delete Solver;
  }
}


MP_model& MP_model::add(MP_constraint& c) {
  Constraints.insert(&c);
  return *this;
}

void MP_model::add(MP_constraint* c) {
  c->M = this;
  if (c->left.isDefined() && c->right.isDefined()) {
    c->offset = m;
    m += c->size();
  }
}

double MP_model::getInfinity() const {
  if (Solver==0) {
    return 9.9e+32;
  } else {
    return Solver->getInfinity();
  }
}

void MP_model::add(MP_variable* v) {
  v->M = this;
  v->offset = n;
  n += v->size();
}

void MP_model::addRow(const Constraint& c) {
  vector<Coef> cfs;
  vector<Constant> v;
  ObjectiveGenerateFunctor f(cfs);
  c.left->generate(MP_domain::getEmpty(),v,f,1.0);
  c.right->generate(MP_domain::getEmpty(),v,f,-1.0);
  CoinPackedVector newRow;
  double rhs = 0.0;
  for (unsigned int j=0; j<cfs.size(); j++) {
    int col=cfs[j].col;
    //int row=cfs[j].row;
    double elm=cfs[j].val;
    //cout<<row<<"   "<<col<<"  "<<elm<<endl;
    if (col>=0) {
      newRow.insert(col,elm);
    } else if (col==-1) {
      rhs = elm;
    }
  }
  // NB no "assembly" of added row yet. Will be done....
 
  double bl = -rhs;
  double bu = -rhs;

  double inf = Solver->getInfinity();
  switch (c.sense) {
      case LE:
        bl = - inf;
        break;
      case GE:
        bu = inf;
        break;
      case EQ:
        // Nothing to do
        break;          
  }

  Solver->addRow(newRow,bl,bu);
}

void MP_model::setObjective(const MP_expression& o) { 
  Objective = o; 
}

void MP_model::minimize_max(MP_set &s, const MP_expression  &obj) {
  MP_variable v;
  MP_constraint c(s);
  add(c);
  c(s) = v() >= obj;
  minimize(v());
} 


class flopc::CoefLess {
public:
  bool operator() (const Coef& a, const Coef& b) const {
    if (a.col < b.col) {
      return true;
    } else if (a.col == b.col && a.row < b.row) {
      return true;
    } else {
      return false;
    }
  }
};

void MP_model::assemble(vector<Coef>& v, vector<Coef>& av) {
  std::sort(v.begin(),v.end(),CoefLess());
  int c,r,s;
  double val;
  std::vector<Coef>::const_iterator i = v.begin();
  while (i!=v.end()) {
    c = i->col;
    r = i->row;
    val = i->val;
    s = i->stage;
    i++;
    while (i!=v.end() && c==i->col && r==i->row) {
      val += i->val;
      if (i->stage>s) {
        s = i->stage;
      }
      i++;
    }
    av.push_back(Coef(c,r,val,s));
  }
}

void MP_model::maximize() {
  if (Solver!=0) {
    attach(Solver);
    solve(MP_model::MAXIMIZE);
  } else {
    cout<<"no solver specified"<<endl;
  }
}

void MP_model::maximize(const MP_expression &obj) {
  if (Solver!=0) {
    Objective = obj;
    attach(Solver);
    solve(MP_model::MAXIMIZE);
  } else {
    cout<<"no solver specified"<<endl;
  }
}

void MP_model::minimize() {
  if (Solver!=0) {
    attach(Solver);
    solve(MP_model::MINIMIZE);
  } else {
    cout<<"no solver specified"<<endl;
  }
}

void MP_model::minimize(const MP_expression &obj) {
  if (Solver!=0) {
    Objective = obj;
    attach(Solver);
    solve(MP_model::MINIMIZE);
  } else {
    cout<<"no solver specified"<<endl;
  }
}

void MP_model::attach(OsiSolverInterface *_solver) {
  if (_solver==NULL) {
    if (Solver==NULL) {
      mSolverState = MP_model::DETACHED;
      return;
    }  
  } else {  // use pre-attached solver.
    if(Solver && Solver!=_solver) {
      detach();
    }
    Solver=_solver;
  }
  double time = CoinCpuTime();
  m=0;
  n=0;
  vector<Coef> coefs;
  vector<Coef> cfs;

  typedef std::set<MP_variable* >::iterator varIt;
  typedef std::set<MP_constraint* >::iterator conIt;
    
  Objective->insertVariables(Variables);
  for (conIt i=Constraints.begin(); i!=Constraints.end(); i++) {
    add(*i);
    (*i)->insertVariables(Variables);
  }
  for (varIt j=Variables.begin(); j!=Variables.end(); j++) {
    add(*j);
  }

  // Generate coefficient matrix and right hand side
  bool doAssemble = true;
  if (doAssemble == true) {
    GenerateFunctor f(cfs);
    for (conIt i=Constraints.begin(); i!=Constraints.end(); i++) {
      (*i)->coefficients(f);
      messenger->constraintDebug((*i)->getName(),cfs);
      assemble(cfs,coefs);
      cfs.erase(cfs.begin(),cfs.end());
    }
  } else {
    GenerateFunctor f(coefs);
    for (conIt i=Constraints.begin(); i!=Constraints.end(); i++) {
      (*i)->coefficients(f);
    }
  }
  nz = coefs.size();

  messenger->statistics(Constraints.size(),m,Variables.size(),n,nz);

  if (nz>0) {    
    Elm = new double[nz]; 
    Rnr = new int[nz];    
  }
  Cst = new int[n+2];   
  Clg = new int[n+1];   
  if (n>0) {
    l =   new double[n];  
    u =   new double[n];  
  }
  if (m>0) {
    bl  = new double[m];  
    bu  = new double[m];  
  }
  const double inf = Solver->getInfinity();

  for (int j=0; j<n; j++) {
    Clg[j] = 0;
  }
  Clg[n] = 0;
    
  // Treat right hand side as n'th column
  for (int j=0; j<=n; j++) {
    Clg[j] = 0;
  }
  for (int i=0; i<nz; i++) {
    int col = coefs[i].col;
    if (col == -1)  {
      col = n;
    }
    Clg[col]++;
  }
  Cst[0]=0;
  for (int j=0; j<=n; j++) {
    Cst[j+1]=Cst[j]+Clg[j]; 
  }
  for (int i=0; i<=n; i++) {
    Clg[i]=0;
  }
  for (int i=0; i<nz; i++) {
    int col = coefs[i].col;
    if (col==-1) {
      col = n;
    }
    int row = coefs[i].row;
    double elm = coefs[i].val;
    Elm[Cst[col]+Clg[col]] = elm;
    Rnr[Cst[col]+Clg[col]] = row;
    Clg[col]++;
  }

  // Row bounds
  for (int i=0; i<m; i++) {
    bl[i] = 0;
    bu[i] = 0;
  }
  for (int j=Cst[n]; j<Cst[n+1]; j++) {
    bl[Rnr[j]] = -Elm[j];
    bu[Rnr[j]] = -Elm[j];
  }

  for (conIt i=Constraints.begin(); i!=Constraints.end(); i++) {
    if ((*i)->left.isDefined() && (*i)->right.isDefined() ) {
      int begin = (*i)->offset;
      int end = (*i)->offset+(*i)->size();
      switch ((*i)->sense) {
          case LE:
            for (int k=begin; k<end; k++) {
              bl[k] = - inf;
            } 
            break;
          case GE:
            for (int k=begin; k<end; k++) {
              bu[k] = inf;
            }     
            break;
          case EQ:
            // Nothing to do
            break;                
      }
    }
  }

  // Generate objective function coefficients
  vector<Constant> v;
  if (doAssemble == true) {
    ObjectiveGenerateFunctor f(cfs);
    coefs.erase(coefs.begin(),coefs.end());
    Objective->generate(MP_domain::getEmpty(), v, f, 1.0);

    messenger->objectiveDebug(cfs);
    assemble(cfs,coefs);
  } else {
    ObjectiveGenerateFunctor f(coefs);
    coefs.erase(coefs.begin(),coefs.end());
    Objective->generate(MP_domain::getEmpty(), v, f, 1.0);
  }     

  c =  new double[n]; 
  for (int j=0; j<n; j++) {
    c[j] = 0.0;
  }
  for (size_t i=0; i<coefs.size(); i++) {
    int col = coefs[i].col;
    double elm = coefs[i].val;
    c[col] = elm;
  } 

  // Column bounds
  for (int j=0; j<n; j++) {
    l[j] = 0.0;
    u[j] = inf;
  }

  for (varIt i=Variables.begin(); i!=Variables.end(); i++) {
    for (int k=0; k<(*i)->size(); k++) {
      l[(*i)->offset+k] = (*i)->lowerLimit.v[k];
      u[(*i)->offset+k] = (*i)->upperLimit.v[k];
    }
  }

  Solver->loadProblem(n, m, Cst, Rnr, Elm, l, u, c, bl, bu);

  if (nz>0) {    
    delete [] Elm; 
    delete [] Rnr;    
  }
  delete [] Cst;   
  delete [] Clg;   
  if (n>0) {
    delete [] l;  
    delete [] u;  
  }
  if (m>0) {
    delete [] bl;  
    delete [] bu;
  }  

  for (varIt i=Variables.begin(); i!=Variables.end(); i++) {
    int begin = (*i)->offset;
    int end = (*i)->offset+(*i)->size();
    if ((*i)->type == discrete) {
      for (int k=begin; k<end; k++) {
        Solver->setInteger(k);
      }
    }
  }
  mSolverState = MP_model::ATTACHED;
  messenger->generationTime(time-CoinCpuTime());
}

void MP_model::detach() {
  assert(Solver);
  mSolverState=MP_model::DETACHED;
  /// @todo strip all data out of the solver.
  delete Solver;
  Solver=NULL;
}

MP_model::MP_status MP_model::solve(const MP_model::MP_direction &dir) {
  assert(Solver);
  assert(mSolverState != MP_model::DETACHED && 
         mSolverState != MP_model::SOLVER_ONLY);
  Solver->setObjSense(dir);
  bool isMIP = false;
  for (varIt i=Variables.begin(); i!=Variables.end(); i++) {
    if ((*i)->type == discrete) {
      isMIP = true;
      break;
    }
  }
  if (isMIP == true) {
    try {
      Solver->branchAndBound();
    } catch  (CoinError e) {
      cout<<e.message()<<endl;
      cout<<"Solving the LP relaxation instead."<<endl;
      try {
        Solver->initialSolve();
      } catch (CoinError e) {
        cout<<e.message()<<endl;
      }
    }
  } else {
    try {
      Solver->initialSolve();
    }  catch (CoinError e) {
      cout<<e.message()<<endl;
    }
  }
     
  if (Solver->isProvenOptimal() == true) {
    cout<<"FlopCpp: Optimal obj. value = "<<Solver->getObjValue()<<endl;
    cout<<"FlopCpp: Solver(m, n, nz) = "<<Solver->getNumRows()<<"  "<<
      Solver->getNumCols()<<"  "<<Solver->getNumElements()<<endl;
    solution = Solver->getColSolution();
    reducedCost = Solver->getReducedCost();
    rowPrice = Solver->getRowPrice();
    rowActivity = Solver->getRowActivity();
  } else if (Solver->isProvenPrimalInfeasible() == true) {
    return mSolverState=MP_model::PRIMAL_INFEASIBLE;
    //cout<<"FlopCpp: Problem is primal infeasible."<<endl;
  } else if (Solver->isProvenDualInfeasible() == true) {
    return mSolverState=MP_model::DUAL_INFEASIBLE;
    //cout<<"FlopCpp: Problem is dual infeasible."<<endl;
  } else {
    return mSolverState=MP_model::ABANDONED;
    //cout<<"FlopCpp: Solution process abandoned."<<endl;
  }
  return mSolverState=MP_model::OPTIMAL;
}

namespace flopc {
  std::ostream &operator<<(std::ostream &os, 
                           const MP_model::MP_status &condition) {
    switch(condition) {
        case MP_model::OPTIMAL:
          os<<"OPTIMAL";
          break;
        case MP_model::PRIMAL_INFEASIBLE:
          os<<"PRIMAL_INFEASIBLE";
          break;
        case MP_model::DUAL_INFEASIBLE:
          os<<"DUAL_INFEASIBLE";
          break;
        case MP_model::ABANDONED:
          os<<"ABANDONED";
          break;
        default:
          os<<"UNKNOWN";
    };
    return os;
  }

  std::ostream &operator<<(std::ostream &os, 
                           const MP_model::MP_direction &direction) {
    switch(direction) {
        case MP_model::MINIMIZE:
          os<<"MINIMIZE";
          break;
        case MP_model::MAXIMIZE:
          os<<"MAXIMIZE";
          break;
        default:
          os<<"UNKNOWN";
    };
    return os;
  }
}
