// Copyright (C) 2003, International Business Machines
// Corporation and others.  All Rights Reserved.

#if defined(_MSC_VER)
// Turn off compiler warning about long names
#  pragma warning(disable:4786)
#endif

#include <string>
using namespace std;

#include <cassert>
#include <iostream>


#include "SmiScnModel.hpp"
#include "SmiScnData.hpp"
#include "OsiClpSolverInterface.hpp"


//forward declarations

void testingMessage(const char * const);
void SmpsIO(const char* const);
void ModelScenario(const char* const);
void ModelDiscrete(const char* const);

int main()
{

	testingMessage( "Model generation using SMPS files for Cambridge-Watson problems.\n" );
	SmpsIO("../../../../Data/Stochastic/wat_10_C_32");		

	testingMessage( "Model generation using scenario tree construction methods.\n");
	ModelScenario("Dantzig-Ferguson Aircraft Allocation using Scenarios");

	testingMessage( "Model generation using discrete distribution specification methods.\n" );
	ModelDiscrete("Dantzig-Ferguson Aircraft Allocation using Discrete Distribution");


	testingMessage( "*** Done! *** \n");

 	return 0;
}

void SmpsIO(const char * const name )
{
		SmiScnModel smi;	

		// read SMPS model from files
		//	<name>.core, <name>.time, and <name>.stoch
		smi.readSmps(name);		

		// generate OSI solver object
		// 	here we use OsiClp
		OsiClpSolverInterface *clp = new OsiClpSolverInterface();

		// set solver object for SmiScnModel
		smi.setOsiSolverHandle(*clp);	

		// load solver data
		// 	this step generates the deterministic equivalent 
		//	and returns an OsiSolver object 
		OsiSolverInterface *osiStoch = smi.loadOsiSolverData();

		// set some nice Hints to the OSI solver
		osiStoch->setHintParam(OsiDoPresolveInInitial,true);
		osiStoch->setHintParam(OsiDoScale,true);
		osiStoch->setHintParam(OsiDoCrash,true);

		// solve
		osiStoch->initialSolve();		

		// print results
		printf("Solved stochastic program %s\n", name);
		printf("Number of rows: %d\n",osiStoch->getNumRows());
		printf("Number of cols: %d\n",osiStoch->getNumCols());
		printf("Optimal value: %g\n",osiStoch->getObjValue());		

		// print solution to file
		string outfilename(name);
		const string suffix(".out");
		outfilename = outfilename + suffix;
		FILE *fp = fopen(outfilename.c_str(),"w");
		int numScenarios=smi.getNumScenarios();

		for (int i=0 ; i<numScenarios; ++i) {
			double *dsoln=NULL;
			int numCols=0;
			fprintf(fp,"Scenario %d \n",i);
			dsoln = smi.getColSolution(i,&numCols);
			for (int j=0; j<numCols; j++)
				fprintf(fp,"%g \n",dsoln[j]);
			free(dsoln);
		}
		fclose(fp);


}	

void ModelScenario(const char * const name )
{
	OsiClpSolverInterface *osiClp1 = new OsiClpSolverInterface();
	double INF=osiClp1->getInfinity();

	// example of direct interfaces for scenario generation
	
    /* Model dimensions */
    int ncol=27, nrow=9, nels=44;
	
	/* Sparse matrix data...organized by row */
    int mrow[]={ 0, 0, 0, 0, 0,
		1, 1, 1, 1,
		2, 2, 2,
		3, 3, 3, 3, 3,
		4, 4, 4, 4,
		5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6,
		7, 7, 7, 7, 7, 7,
		8, 8, 8, 8, 8, 8 };
	  int mcol[]={ 0, 1, 2, 3, 4,
		5, 6, 7, 8,
		9,10, 11, 
		12, 13, 14, 15, 16, 
		0,        12, 17, 18,
		1, 5, 9,  13, 19, 20,
		2, 6,     14, 21, 22,
		3, 7, 10, 15, 23, 24,
		4, 8, 11, 16, 25, 26 };

    double dels[] = { 1.0, 1.0, 1.0, 1.0, 1.0,
		1.0, 1.0, 1.0, 1.0,
		1.0, 1.0, 1.0,
		1.0, 1.0, 1.0, 1.0, 1.0,
		16.0,              9.0, -1.0, 1.0,
		15.0, 10.0,  5.0, 11.0, -1.0, 1.0,
		28.0, 14.0,       22.0, -1.0, 1.0,
		23.0, 15.0,  7.0, 17.0, -1.0, 1.0,
		81.0, 57.0, 29.0, 55.0, -1.0, 1.0 };
	
    /* Objective */
    double dobj[]={ 18.0, 21.0, 18.0, 16.0, 10.0, 15.0, 16.0, 14.0, 9.0,
		10.0,  9.0,  6.0, 17.0, 16.0, 17.0, 15.0, 10.0, 0.0,
		13.0,  0.0, 13.0,  0.0,  7.0,  0.0,  7.0,  0.0, 1.0 };
	
    /* Column bounds */
    double dclo[]={ 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
		0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
		0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0 };



    double dcup[]={ INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,
		INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,
		INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF };
	
    /* Row bounds */
    double drlo[]={ -INF, -INF, -INF, -INF,  0.0, 4.0, 0.0, 8.0, 10.0 };
    double drup[]={ 10.0, 19.0, 25.0, 15.0,  0.0, 7.0, 0.0, 8.0, 90.0 };
	
    /* Stages */
    int nstg=2;
    int n_first_stg_rows=4;
    int rstg[]={ 0,0,0,0,1,1,1,1,1 };
    int cstg[]={ 0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,1,
		1,1,1,1,1,1,1,1,1 };
	
    /* Stochastic data */
    int nindp=5;
    int nsamp[]={ 5, 2, 5, 5, 3 };
    double demand[]={ 200, 220, 250, 270, 300,
		50, 150,
		140, 160, 180, 200, 220,
		10, 50, 80, 100, 340,
		580, 600, 620 };
    double dprobs[]={ 0.2, 0.05, 0.35, 0.2, 0.2,
		0.3, 0.7,
		0.1, 0.2, 0.4, 0.2, 0.1,
		0.2, 0.2, 0.3, 0.2, 0.1,
		0.1, 0.8, 0.1 };
	
    /* local variables */
    int ns=1,ii,iii,jj,*indx,*incr;
    double dp=1.0;

    for (ii=0;ii<nindp;ii++) ns *= nsamp[ii];     /* Compute number of scenarios */

	
	// initialize SmiModel
	SmiScnModel *smiModel = new SmiScnModel();
	smiModel->setOsiSolverHandle(*osiClp1);

	// set core model using Osi interface
	OsiClpSolverInterface ocsi;
	ocsi.loadProblem(CoinPackedMatrix( 1,mrow,mcol,dels,nels),dclo,dcup,dobj,drlo,drup);
	SmiCoreData *osiCore = new SmiCoreData(&ocsi,nstg,cstg,rstg);

	cout << "ModelScenario: Created CoreData" << endl;

	// Coin structures for scenario updates to right hand sides
	CoinPackedVector cpv_rlo;
	CoinPackedVector cpv_rup;
		
    // initialize right hand side data for first scenario
    indx = (int *) malloc( (1+nindp)*sizeof(int) );
    memset( indx,0,(1+nindp)*sizeof(int));
    for (jj=0;jj<nindp;jj++) {
		indx[jj+1] += indx[jj] + nsamp[jj];
		dp *= dprobs[ indx[jj] ];

		drlo[n_first_stg_rows + jj] = demand[ indx[jj] ];
		drup[n_first_stg_rows + jj] = demand[ indx[jj] ];
		
		cpv_rlo.insert(n_first_stg_rows + jj,demand[ indx[jj] ]);
		cpv_rup.insert(n_first_stg_rows + jj,demand[ indx[jj] ]);
    }
	
	cout << "ModelScenario: Adding " << ns << " scenarios" << endl;

	// first scenario
	int anc = 0;
	int branch = 1;
	int	is = smiModel->generateScenario(osiCore,NULL,NULL,NULL,NULL,
									&cpv_rlo,&cpv_rup,branch,anc,dp);	
	

	
	/***** ...main loop to generate scenarios from discrete random variables
		For each scenario index ii:
        If the sample size nsamp[jj] divides the scenario index ii,
		reverse the increment direction incr[jj]
		and increase the random variable index jj by 1.
        Increment the jj'th random variable by incr[jj]
		and generate new sample data.
    ***** */
	
    /* sample space increment initialized to 1 */
    incr = (int *) malloc( nindp*sizeof(int) );
    for (jj=0;jj<nindp;jj++) incr[jj] = 1;
	
    for (int iss=1;iss<ns;iss++) {
		iii=iss; jj=0;
		while ( !(iii%nsamp[jj]) ) {
			iii /= nsamp[jj];
			incr[jj] = -incr[jj];
			jj++;
		}
		dp /= dprobs[ indx[jj] ];
		indx[jj] += incr[jj];
		dp *= dprobs[ indx[jj] ];

		// set data
		drlo[n_first_stg_rows + jj] = demand[ indx[jj] ];
		drup[n_first_stg_rows + jj] = demand[ indx[jj] ];

		cpv_rlo.setElement(cpv_rlo.findIndex(n_first_stg_rows + jj),demand[ indx[jj] ]);
		cpv_rup.setElement(cpv_rup.findIndex(n_first_stg_rows + jj),demand[ indx[jj] ]);
		
		// genScenario
		is = smiModel->generateScenario(osiCore,NULL,NULL,NULL,NULL,
			&cpv_rlo,&cpv_rup,branch,anc,dp);	
		
		
	}
	
	assert(ns==smiModel->getNumScenarios());
	cout << "ModelScenario: Finished adding scenarios" << endl;


	
	// load problem data into OsiSolver
	smiModel->loadOsiSolverData();
	// get Osi pointer
	OsiSolverInterface *smiOsi = smiModel->getOsiSolverInterface();
	// set some parameters
	smiOsi->setHintParam(OsiDoPresolveInInitial,true);
	smiOsi->setHintParam(OsiDoScale,true);
	smiOsi->setHintParam(OsiDoCrash,true);
	// solve using Osi Solver
	smiOsi->initialSolve();
	// test optimal value
    	assert(fabs(smiOsi->getObjValue()-1566.042)<0.01);

	// test solutions
	const double *dsoln = smiOsi->getColSolution();
	double objSum = 0.0;

	/* The canonical way to traverse the tree:
	   For each scenario, get the leaf node.
	   Then get the parent.  Repeat until parent is NULL.
	   (Only the root node has a NULL parent.)
	 */
	

	for(is=0; is<ns; ++is)
	{
		/* this loop calculates the scenario objective value */
		double scenSum = 0.0;

		// start with leaf node
		SmiScnNode *node = smiModel->getLeafNode(is);

		// leaf node probability is the scenario probability
		double scenprob = node->getModelProb();
	
		while (node != NULL)
		{

			// getColStart returns the starting index of node in OSI model
			for(int j=node->getColStart(); j<node->getColStart()+node->getNumCols(); ++j)
			{
				// getCoreColIndex returns the corresponding Core index
				// in the original (user's) ordering
				scenSum += dobj[node->getCoreColIndex(j)]*dsoln[j];	
				

			}			
			// get parent of node
			node = node->getParent();
		}
		objSum += scenSum*scenprob;
	}

	assert(fabs(smiOsi->getObjValue()-objSum) < 0.01);

		// print results
		printf("Solved stochastic program %s\n", name);
		printf("Number of rows: %d\n",smiOsi->getNumRows());
		printf("Number of cols: %d\n",smiOsi->getNumCols());
		printf("Optimal value: %g\n",smiOsi->getObjValue());

}

void ModelDiscrete(const char * const name)
{
	// example of direct interfaces for discrete distribution
	
	OsiClpSolverInterface *osiClp1 = new OsiClpSolverInterface();
	double INF=osiClp1->getInfinity();
	
    /* Model dimensions */
    int ncol=27, nrow=9, nels=44;
	
	/* Sparse matrix data...organized by row */
    int mrow[]={ 0, 0, 0, 0, 0,
		1, 1, 1, 1,
		2, 2, 2,
		3, 3, 3, 3, 3,
		4, 4, 4, 4,
		5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6,
		7, 7, 7, 7, 7, 7,
		8, 8, 8, 8, 8, 8 };
	  int mcol[]={ 0, 1, 2, 3, 4,
		5, 6, 7, 8,
		9,10, 11, 
		12, 13, 14, 15, 16, 
		0,        12, 17, 18,
		1, 5, 9,  13, 19, 20,
		2, 6,     14, 21, 22,
		3, 7, 10, 15, 23, 24,
		4, 8, 11, 16, 25, 26 };

    double dels[] = { 1.0, 1.0, 1.0, 1.0, 1.0,
		1.0, 1.0, 1.0, 1.0,
		1.0, 1.0, 1.0,
		1.0, 1.0, 1.0, 1.0, 1.0,
		16.0,              9.0, -1.0, 1.0,
		15.0, 10.0,  5.0, 11.0, -1.0, 1.0,
		28.0, 14.0,       22.0, -1.0, 1.0,
		23.0, 15.0,  7.0, 17.0, -1.0, 1.0,
		81.0, 57.0, 29.0, 55.0, -1.0, 1.0 };
	
    /* Objective */
    /* Objective */
    double dobj[]={ 18.0, 21.0, 18.0, 16.0, 10.0, 15.0, 16.0, 14.0, 9.0,
		10.0,  9.0,  6.0, 17.0, 16.0, 17.0, 15.0, 10.0, 0.0,
		13.0,  0.0, 13.0,  0.0,  7.0,  0.0,  7.0,  0.0, 1.0 };
	
    /* Column bounds */
    double dclo[]={ 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
		0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
		0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0 };



    double dcup[]={ INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,
		INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,
		INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF };
	
    /* Row bounds */
    double drlo[]={ -INF, -INF, -INF, -INF,  0.0, 4.0, 0.0, 8.0, 10.0 };
    double drup[]={ 10.0, 19.0, 25.0, 15.0,  0.0, 7.0, 0.0, 8.0, 90.0 };
	
    /* Stages */
    int nstg=2;
    int n_first_stg_rows=4;
    int rstg[]={ 0,0,0,0,1,1,1,1,1 };
    int cstg[]={ 0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,1,
		1,1,1,1,1,1,1,1,1 };
	
    /* Stochastic data */
    int nindp=5;
    int nsamp[]={ 5, 2, 5, 5, 3 };
    double demand[]={ 200, 220, 250, 270, 300,
		50, 150,
		140, 160, 180, 200, 220,
		10, 50, 80, 100, 340,
		580, 600, 620 };
    double dprobs[]={ 0.2, 0.05, 0.35, 0.2, 0.2,
		0.3, 0.7,
		0.1, 0.2, 0.4, 0.2, 0.1,
		0.2, 0.2, 0.3, 0.2, 0.1,
		0.1, 0.8, 0.1 };
	
    /* local variables */
    int ii,jj;

	// initialize SmiModel
	SmiScnModel *smiModel = new SmiScnModel();
	smiModel->setOsiSolverHandle(*osiClp1);

		
	// set core model using Osi interface
	OsiClpSolverInterface ocsi;
	ocsi.loadProblem(CoinPackedMatrix( 1,mrow,mcol,dels,nels),dclo,dcup,dobj,drlo,drup);
	
	// core model 
	SmiCoreData *smiCore = new SmiCoreData(&ocsi,nstg,cstg,rstg);

	cout << "ModelDiscrete: generated Core data" << endl;

	// Create discrete distribution
	SmiDiscreteDistribution *smiDD = new SmiDiscreteDistribution(smiCore);

	cout << "ModelDiscrete: adding random variables" << endl;

	int index=0;
	for (jj=0;jj<nindp;jj++)
	{
		SmiDiscreteRV *smiRV = new SmiDiscreteRV(1);
		for (ii=0;ii<nsamp[jj];ii++)
		{			
			CoinPackedVector empty_vec;
			CoinPackedMatrix empty_mat;
			CoinPackedVector cpv_rlo ;
			CoinPackedVector cpv_rup ;
			cpv_rlo.insert(n_first_stg_rows + jj, demand[index+ii]);
			cpv_rup.insert(n_first_stg_rows + jj, demand[index+ii]);
			smiRV->addEvent(empty_mat,empty_vec,empty_vec,empty_vec,cpv_rlo,cpv_rup,dprobs[index+ii]);
			cpv_rlo.clear();
			cpv_rup.clear();
		}
		smiDD->addDiscreteRV(smiRV);
		index+=nsamp[jj];
	}

	assert(smiDD->getNumRV() == nindp);
	cout << "ModelDiscrete: added " << nindp << " random variables" << endl;


	
	cout << "ModelDiscrete: processing into scenarios" << endl;

	smiModel->processDiscreteDistributionIntoScenarios(smiDD);
	
	// load problem data into OsiSolver
	smiModel->loadOsiSolverData();
	// get Osi pointer
	OsiSolverInterface *smiOsi = smiModel->getOsiSolverInterface();
	// set some parameters
	smiOsi->setHintParam(OsiDoPresolveInInitial,true);
	smiOsi->setHintParam(OsiDoScale,true);
	smiOsi->setHintParam(OsiDoCrash,true);
	// solve using Osi Solver
	smiOsi->initialSolve();
	// test optimal value
    	assert(fabs(smiOsi->getObjValue()-1566.042)<0.01);

	// test solutions
	const double *dsoln = smiOsi->getColSolution();
	double objSum = 0.0;

	/* The canonical way to traverse the tree:
	   For each scenario, get the leaf node.
	   Then get the parent.  Repeat until parent is NULL.
	   (Only the root node has a NULL parent.)
	 */

	for(int is=0; is<smiModel->getNumScenarios(); ++is)
	{
		/* this loop calculates the scenario objective value */
		double scenSum = 0.0;

		// start with leaf node
		SmiScnNode *node = smiModel->getLeafNode(is);

		// leaf node probability is the scenario probability
		double scenprob = node->getModelProb();
	
		while (node != NULL)
		{
			// getColStart returns the starting index of node in OSI model
			for(int j=node->getColStart(); j<node->getColStart()+node->getNumCols(); ++j)
			{
				// getCoreColIndex returns the corresponding Core index
				// in the original (user's) ordering
				scenSum += dobj[node->getCoreColIndex(j)]*dsoln[j];	
				

			}			
			
			// get parent of node
			node = node->getParent();
		}
		objSum += scenSum*scenprob;
	}

	assert(fabs(smiOsi->getObjValue()-objSum) < 0.01);

		// print results
		printf("Solved stochastic program %s\n", name);
		printf("Number of rows: %d\n",smiOsi->getNumRows());
		printf("Number of cols: %d\n",smiOsi->getNumCols());
		printf("Optimal value: %g\n",smiOsi->getObjValue());


}

// Display message on stdout and stderr
void testingMessage( const char * const msg )
{
//  std::cerr <<msg;
  cout <<endl <<"*****************************************"
       <<endl <<msg <<endl;
}
