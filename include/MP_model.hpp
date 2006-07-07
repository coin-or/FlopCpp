// ******************** flopc++ **********************************************
// File: MP_model.hpp
// $Id$
// Author: Tim Helge Hultberg (thh@mat.ua.pt)
// Copyright (C) 2003 Tim Helge Hultberg
// All Rights Reserved.
//****************************************************************************

#ifndef _MP_model_hpp_
#define _MP_model_hpp_

#include <vector>
#include <set>
using std::vector;
using std::set;

#include "MP_expression.hpp"
#include "MP_constraint.hpp"
#include <CoinPackedVector.hpp>
class OsiSolverInterface;

namespace flopc {

  class MP_variable;
  class MP_index;
  class MP_set;

  class Messenger {
  public:
    virtual void logMessage(int level, const char * const msg){}
    friend class MP_model;
  private:
    virtual void constraintDebug(string name, const vector<Coef>& cfs) {}
    virtual void objectiveDebug(const vector<Coef>& cfs) {}
    virtual void statistics(int bm, int m, int bn, int n, int nz) {}
    virtual void generationTime(double t) {}
  protected:
    virtual ~Messenger() {}
  };

  class NormalMessenger : public Messenger {
    friend class MP_model;
  private:
    virtual void statistics(int bm, int m, int bn, int n, int nz);
    virtual void generationTime(double t);
  };

  class VerboseMessenger : public NormalMessenger {
    friend class MP_model;
  private:
    virtual void constraintDebug(string name, const vector<Coef>& cfs);
    virtual void objectiveDebug(const vector<Coef>& cfs);
  };

  class MP_model {
    friend class MP_constraint;
  public:
    MP_model(OsiSolverInterface* s, Messenger* m = new NormalMessenger);
    ~MP_model() {
      delete messenger;
    }
    void silent() {
      delete messenger;
      messenger = new Messenger;
    }
    void verbose() {
      delete messenger;
      messenger = new VerboseMessenger;
    }

    void setSolver(OsiSolverInterface* s) {
      Solver = s;
    }

    OsiSolverInterface* operator->() {
      return Solver;
    }

    MP_model& add(MP_constraint& c);

    void maximize();
    void maximize(const MP_expression &obj);
    void minimize();
    void minimize(const MP_expression &obj);
   
    void minimize_max(MP_set& d, const MP_expression &obj);
    void setObjective(const MP_expression& o);
    const double* solution;
    const double* reducedCost;
    const double* rowPrice;
    const double* rowActivity;
    double getInfinity() const;

    void add(MP_variable* v);
    void addRow(const Constraint& c); 

    static MP_model &getDefaultModel();
    static MP_model *getCurrentModel();
    Messenger *getMessenger(){ 
      return messenger;
    }
  private:
    static MP_model& default_model;
    static MP_model* current_model;
    MP_model(const MP_model&);
    MP_model& operator=(const MP_model&);

    Messenger* messenger;
   
    void generate();
    
    static void assemble(vector<Coef>& v, vector<Coef>& av);
    void add(MP_constraint* c);
    MP_expression Objective;
    set<MP_constraint *> Constraints;
    set<MP_variable *> Variables;
  public:
    OsiSolverInterface* Solver; 
  private:
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

} // End of namespace flopc
#endif
