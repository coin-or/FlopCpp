// ******************** flopc++ **********************************************
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
#include "Timer.hpp"

using namespace flopc;

  MP_model& MP_model::default_model = *new MP_model(0);
  MP_model* MP_model::current_model = &MP_model::default_model;
  MP_model &MP_model::getDefaultModel() { return default_model;}
  MP_model *MP_model::getCurrentModel() { return current_model;}

void NormalMessenger::statistics(int bm, int m, int bn, int n, int nz) {
    cout<<"FLOPC++: Number of constraint blocks: " <<bm<<endl;
    cout<<"FLOPC++: Number of individual constraints: " <<m<<endl;
    cout<<"FLOPC++: Number of variable blocks: " <<bn<<endl;
    cout<<"FLOPC++: Number of individual variables: " <<n<<endl;
    cout<<"FLOPC++: Number of non-zeroes (including rhs): " <<nz<<endl;
}

void NormalMessenger::generationTime(double t) {
    cout<<"FLOPC++: Generation time: "<<t<<endl;
}

void VerboseMessenger::constraintDebug(string name, const vector<Coef>& cfs) {
    cout<<"FLOPC++: Constraint "<<name<<endl;
    for (unsigned int j=0; j<cfs.size(); j++) {
	int col=cfs[j].col;
	int row=cfs[j].row;
	double elm=cfs[j].val;
	cout<<row<<"   "<<col<<"  "<<elm<<endl;
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
    m(0), n(0), nz(0), bl(0) {
  MP_model::current_model = this;
}

MP_model& MP_model::add(MP_constraint& c) {
    Constraints.insert(&c);
    return *this;
}

void MP_model::add(MP_constraint* c) {
    c->M = this;
    c->offset = m;
    m += c->size();
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
    int c,r;
    double val;
    std::vector<Coef>::const_iterator i = v.begin();
    while (i!=v.end()) {
	c = i->col;
	r = i->row;
	val = i->val;
	i++;
	while (i!=v.end() && c==i->col && r==i->row) {
	    val += i->val;
	    i++;
	}
	av.push_back(Coef(c,r,val));
    }
}

void MP_model::maximize() {
    if (Solver!=0) {
	Solver->setObjSense(-1);
	generate();
    } else {
	cout<<"no solver specified"<<endl;
    }
}

void MP_model::maximize(const MP_expression &obj) {
    if (Solver!=0) {
	Objective = obj;
	Solver->setObjSense(-1);
	generate();
    } else {
	cout<<"no solver specified"<<endl;
    }
}

void MP_model::minimize() {
    if (Solver!=0) {
	Solver->setObjSense(1);
	generate();
    } else {
	cout<<"no solver specified"<<endl;
    }
}

void MP_model::minimize(const MP_expression &obj) {
    if (Solver!=0) {
	Objective = obj;
	Solver->setObjSense(1);
	generate();
    } else {
	cout<<"no solver specified"<<endl;
    }
}

void MP_model::generate() {
    Timer t;
    t.start();
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

    Elm = new double[nz]; 
    Rnr = new int[nz];    
    Cst = new int[n+2];   
    Clg = new int[n+1];   
    l =   new double[n];  
    u =   new double[n];  
    bl  = new double[m];  
    bu  = new double[m];  

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

    CoinPackedMatrix A(true,m,n,Cst[n],Elm,Rnr,Cst,Clg);
    Solver->loadProblem(A, l, u, c, bl, bu);

    // Instead of the 2 lines above we should be able to use
    // the line below, but due to a bug in OsiGlpk it does not work
    // Solver->loadProblem(n, m, Cst, Rnr, Elm, l, u, c, bl, bu);

    delete [] Elm; 
    delete [] Rnr;    
    delete [] Cst;   
    delete [] Clg;   
    delete [] l;  
    delete [] u;  
    delete [] bl;  
    delete [] bu;  

    bool isMIP = false;
    for (varIt i=Variables.begin(); i!=Variables.end(); i++) {
	int begin = (*i)->offset;
	int end = (*i)->offset+(*i)->size();
	if ((*i)->type == discrete) {
	    isMIP = true;
	    for (int k=begin; k<end; k++) {
		Solver->setInteger(k);
	    }
	}
    }

    t.stop();    
    messenger->generationTime(t.time());

    //Solver->setDblParam(OsiDualObjectiveLimit,1.0e50);

    //Solver->writeMps("mps","mps",1);

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
	cout<<"FLOPC++: Optimal obj. value = "<<Solver->getObjValue()<<endl;
	cout<<"FLOPC++: Solver(m, n, nz) = "<<Solver->getNumRows()<<"  "<<
	    Solver->getNumCols()<<"  "<<Solver->getNumElements()<<endl;
	solution = Solver->getColSolution();
	reducedCost = Solver->getReducedCost();
	rowPrice = Solver->getRowPrice();
	rowActivity = Solver->getRowActivity();
    } else if (Solver->isProvenPrimalInfeasible() == true) {
	cout<<"FLOPC++: Problem is primal infeasible."<<endl;
    } else if (Solver->isProvenDualInfeasible() == true) {
	cout<<"FLOPC++: Problem is dual infeasible."<<endl;
    } else {
	cout<<"FLOPC++: Solution process abandoned."<<endl;
    }
}

