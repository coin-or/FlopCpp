// Copyright (C) 2003, International Business Machines
// Corporation and others.  All Rights Reserved.


#include "SmiScnModel.hpp"
#include "OsiClpSolverInterface.hpp"

//#############################################################################

#ifdef NDEBUG
#undef NDEBUG
#endif


 
// Display message on stdout and stderr
void testingMessage( const char * const msg )
{
//  std::cerr <<msg;
  cout <<endl <<"*****************************************"
       <<endl <<msg <<endl;
}

void
SmiScenarioTreeUnitTest()
{

	int  *i1 = new int(1);
	int  *i2= new int(12);
	int  *i3= new int(13);
	int  *i4= new int(24);
	int  *i5= new int(25);
	int  *i6= new int(36);
	int  *i7= new int(37);
	int  *i8= new int(38);

	vector<int *> path4(3);
	path4[0]= i1; 
	path4[1]= i2; 
	path4[2]= i4; 

	vector<int *> path5(1, i5);
	
	vector<int *> path6(2);
	path6[0]= i3; 
	path6[1]= i6; 

	vector<int *> path7(1, i7);
	
	vector<int *> path8(1, i8);
	
	
	SmiScenarioTree<int*> s;

	int is1 = s.addPathtoLeaf(0,0,path4);
	int is2 = s.addPathtoLeaf(is1,1,path5);
	int is3 = s.addPathtoLeaf(is2,0,path6);
	int is4 = s.addPathtoLeaf(is3,1,path7);
	int is5 = s.addPathtoLeaf(is3,1,path8);

	int  *ii1 = new int(1);
	int  *ii2= new int(12);
	int  *ii3= new int(13);
	int  *ii4= new int(24);
	int  *ii5= new int(25);
	int  *ii6= new int(36);
	int  *ii7= new int(37);
	int  *ii8= new int(38);

	
	vector<int *> ppath4(3);
	vector<int> label4(3);
	ppath4[0]= ii1; label4[0]=*ii1;
	ppath4[1]= ii2; label4[1]=*ii2;
	ppath4[2]= ii4; label4[2]=*ii4;

	vector<int *> ppath5(3);
	ppath5[0] = ppath4[0];
	ppath5[1] = ppath4[1];
	ppath5[2] = ii5;
	vector<int> label5(3);
	label5[0] = label4[0];
	label5[1] = label4[1];
	label5[2] = *ii5;

	vector<int *> ppath6(3);
	ppath6[0] = ppath5[0];
	vector<int> label6(3);
	label6[0] = label5[0];
	ppath6[1]= ii3; label6[1] = *ii3;
	ppath6[2]= ii6; label6[2] = *ii6;

	vector<int *> ppath7(3);
	ppath7[0] = ppath6[0];
	ppath7[1] = ppath6[1];
	ppath7[2] = ii7;

	vector<int> label7(3);
	label7[0] = label6[0];
	label7[1] = label6[1];
	label7[2] = *ii7;

	vector<int *> ppath8(3);
	ppath8[0] = ppath6[0];
	ppath8[1] = ppath6[1];
	ppath8[2] = ii8;
	vector<int> label8(3);
	label8[0] = label6[0];
	label8[1] = label6[1];
	label8[2] = *ii8;

	// tree s1 is identical to s just uses labels to id nodes 
	SmiScenarioTree<int*> s1;

	
	SmiTreeNode<int *> *node;

	int is11 = s1.addPathtoLeaf(label4,ppath4);
	s1.setChildLabels(s1.getRoot(),label4);

	node = s1.find(label5);	
	int is12 = s1.addPathtoLeaf(node->scenario(),node->depth(),ppath5,node->depth()+1);
	s1.setChildLabels(node,label5);
	
	node = s1.find(label6);
	int is13 = s1.addPathtoLeaf(node->scenario(),node->depth(),ppath6,node->depth()+1);
	s1.setChildLabels(node,label6);

	node = s1.find(label7);
	int is14 = s1.addPathtoLeaf(node->scenario(),node->depth(),ppath7,node->depth()+1);
	s1.setChildLabels(node,label7);

	node = s1.find(label8);
	int is15 = s1.addPathtoLeaf(node->scenario(),node->depth(),ppath8,node->depth()+1);
	s1.setChildLabels(node,label8);
	


	vector<int *> vec1 = s.getScenario(is1);
	assert( vec1[0] == i1 );
	assert( vec1[1] == i2 );
	assert( vec1[2] == i4 );

	vector<int *> vec11 = s1.getScenario(is11);
	assert( vec11[0] == ii1 );
	assert( vec11[1] == ii2 );
	assert( vec11[2] == ii4 );

	vector<int *>::iterator vbeg = s.scenBegin(is1);
	vector<int *>::iterator vend = s.scenEnd(is1);
	assert( *vbeg++ == i1 );
	assert( *vbeg++ == i2 );
	assert( *vbeg++ == i4 );
	assert( vbeg == vend );

	vector<int *>::iterator vbeg1 = s1.scenBegin(is11);
	vector<int *>::iterator vend1 = s1.scenEnd(is11);
	assert( *vbeg1++ == ii1 );
	assert( *vbeg1++ == ii2 );
	assert( *vbeg1++ == ii4 );
	assert( vbeg1 == vend1 );

	
	vector<int *> vec2 = s.getScenario(is2);
	assert( vec2[0] == i1 );
	assert( vec2[1] == i2 );
	assert( vec2[2] == i5 );

		
	vector<int *> vec12 = s1.getScenario(is12);
	assert( vec12[0] == ii1 );
	assert( vec12[1] == ii2 );
	assert( vec12[2] == ii5 );

	vector<int *> vec3 = s.getScenario(is3);
	assert( vec3[0] == i1 );
	assert( vec3[1] == i3 );
	assert( vec3[2] == i6 );

	vector<int *> vec13 = s1.getScenario(is13);
	assert( vec13[0] == ii1 );
	assert( vec13[1] == ii3 );
	assert( vec13[2] == ii6 );

	vector<int *> vec4 = s.getScenario(is4);
	assert( vec4[0] == i1 );
	assert( vec4[1] == i3 );
	assert( vec4[2] == i7 );

	
	vector<int *> vec14 = s1.getScenario(is14);
	assert( vec14[0] == ii1 );
	assert( vec14[1] == ii3 );
	assert( vec14[2] == ii7 );

	vector<int *> vec5 = s.getScenario(is5);
	assert( vec5[0] == i1 );
	assert( vec5[1] == i3 );
	assert( vec5[2] == i8 );

	
	vector<int *> vec15 = s1.getScenario(is15);
	assert( vec15[0] == ii1 );
	assert( vec15[1] == ii3 );
	assert( vec15[2] == ii8 );

	vector<int*>::iterator i;
	i=s.treeBegin();
	assert(*i==i1);
	i++;
	assert(*i==i2);
	i = s.treeEnd();
	i--;
	assert(*i==i8);

	vector<int*>::iterator ii;
	ii=s1.treeBegin();
	assert(*ii==ii1);
	ii++;
	assert(*ii==ii2);
	ii = s1.treeEnd();
	ii--;
	assert(*ii==ii8);

	vbeg = s.scenBegin(is1);
	vend = s.scenEnd(is1);
	assert( *vbeg++ == i1 );
	assert( *vbeg++ == i2 );
	assert( *vbeg++ == i4 );
	assert( vbeg == vend );

	vbeg1 = s1.scenBegin(is11);
	vend1 = s1.scenEnd(is11);
	assert( *vbeg1++ == ii1 );
	assert( *vbeg1++ == ii2 );
	assert( *vbeg1++ == ii4 );
	assert( vbeg1 == vend1 );


}

void
SmiTreeNodeUnitTest()
{

	int  *i1 = new int(1);
	int  *i2= new int(12);
	int  *i3= new int(13);
	int  *i4= new int(24);
	int  *i5= new int(25);
	int  *i6= new int(36);
	int  *i7= new int(37);
	int  *i8= new int(38);

	SmiTreeNode<int *> *n1 = new SmiTreeNode<int *>(i1);

	SmiTreeNode<int *> *n2;
	SmiTreeNode<int *> *n3;
	SmiTreeNode<int *> *n4;
	SmiTreeNode<int *> *n5;
	SmiTreeNode<int *> *n6;
	SmiTreeNode<int *> *n7;
	SmiTreeNode<int *> *n8;

	n2 = n1->addChild(i2,0);
	n3 = n1->addChild(i3,1);
	n4 = n2->addChild(i4,0);
	n5 = n2->addChild(i5,2);
	n6 = n3->addChild(i6,1);
	n7 = n3->addChild(i7,3);
	n8 = n3->addChild(i8,4);

	assert( n1->depth() == 0 );
	assert( n2->depth() == 1 );
	assert( n3->depth() == 1 );
	assert( n4->depth() == 2 );
	assert( n5->depth() == 2 );
	assert( n6->depth() == 2 );
	assert( n7->depth() == 2 );
	assert( n8->depth() == 2 );

	// parents point to last children
	assert( n1->getChild() == n3 );
	assert( n3->getChild() == n8 );

	// siblings
	assert( n3->hasSibling() );
	assert( n3->getSibling() == n2 );
	assert( !n2->hasSibling() );

	// sibling and cousin pointers are a linked list
	// for same level nodes
	assert( n8->hasSibling() );
	assert( n8->getSibling() == n7 );
	assert( n7->hasSibling() );
	assert( n7->getSibling() == n6 );
	assert( !n6->hasSibling());

	assert( n6->getParent()->getSibling()  == n2 );
	assert( n2->getChild() == n5 );
	assert( n5->hasSibling() );
	assert( n5->getSibling() == n4 );
	// last element of same level list
	assert( !n4->hasSibling());

	vector<SmiTreeNode<int *> *> *vec1 = n1->getChildren();
	assert ((*vec1)[0] == n2 );
	assert ((*vec1)[1] == n3 );
	delete vec1;
}




void SmiScnSmpsIOUnitTest()
{
  	std::string dataDir="../../Data/Stochastic";
	int nrows, ncols;
	{
		// test SMPS files app0110R
		SmiScnModel smi;	
		smi.readSmps((dataDir+"/app0110R").c_str());		
		OsiClpSolverInterface *clp = new OsiClpSolverInterface();
		smi.setOsiSolverHandle(*clp);	
		OsiSolverInterface *osiStoch = smi.loadOsiSolverData();
		
		nrows = osiStoch->getNumRows();
		ncols = osiStoch->getNumCols();
		assert(nrows==129);
		assert(ncols==268);
		
		osiStoch->initialSolve();		
		assert(fabs(osiStoch->getObjValue()-44.66666) < 0.0001);
		printf(" *** Successfully tested SMPS interfaces on app0110 with Replace option.\n");
	}
	
	{
		// test SMPS files app0110 -- ADD scenario values
		SmiScnModel smi;	
		smi.readSmps((dataDir+"/app0110").c_str());		
		OsiClpSolverInterface *clp = new OsiClpSolverInterface();
		smi.setOsiSolverHandle(*clp);	
		OsiSolverInterface *osiStoch = smi.loadOsiSolverData();
		
		nrows = osiStoch->getNumRows();
		ncols = osiStoch->getNumCols();
		assert(nrows==129);
		assert(ncols==268);
		
		osiStoch->initialSolve();		
		assert(fabs(osiStoch->getObjValue()-44.66666) < 0.0001);
		printf(" *** Successfully tested SMPS interfaces on app0110 with Add option.\n");
	}
	{
		// test SMPS files from Watson test suite (Cambridge, UK)
		
		SmiScnModel smi;
		smi.readSmps((dataDir+"/wat_10_C_32").c_str());
		OsiClpSolverInterface *clp = new OsiClpSolverInterface();
		smi.setOsiSolverHandle(*clp);	
		OsiSolverInterface *osiStoch = smi.loadOsiSolverData();
		osiStoch->setHintParam(OsiDoPresolveInInitial,true);
		osiStoch->setHintParam(OsiDoScale,true);
		osiStoch->setHintParam(OsiDoCrash,true);
		osiStoch->initialSolve();		
		assert(fabs(osiStoch->getObjValue()+2622.062) < 0.01);
		printf(" *** Successfully tested SMPS interfaces on wat_10_32_C.\n");
	}
}	
void SmiScnModelScenarioUnitTest()
{


	OsiClpSolverInterface *osiClp1 = new OsiClpSolverInterface();
	double INF = osiClp1->getInfinity();

	// exhaustive test of direct interfaces for scenario generation
	
    /* Model dimensions */
    int ncol=27, nrow=9, nels=44;
	
	/* Sparse matrix data...organized by row */
    int *mrow,cmrow[]={ 0, 0, 0, 0, 0,
		1, 1, 1, 1,
		2, 2, 2,
		3, 3, 3, 3, 3,
		4, 4, 4, 4,
		5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6,
		7, 7, 7, 7, 7, 7,
		8, 8, 8, 8, 8, 8 };
	  int *mcol,cmcol[]={ 0, 1, 2, 3, 4,
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
    double *dobj,cdobj[]={ 18.0, 21.0, 18.0, 16.0, 10.0, 15.0, 16.0, 14.0, 9.0,
		10.0,  9.0,  6.0, 17.0, 16.0, 17.0, 15.0, 10.0, 0.0,
		13.0,  0.0, 13.0,  0.0,  7.0,  0.0,  7.0,  0.0, 1.0 };
	
    /* Column bounds */
    double *dclo,cdclo[]={ 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
		0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
		0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0 };

    double *dcup,cdcup[]={ INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,
		INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,
		INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF };
	
    /* Row bounds */
    double *drlo,cdrlo[]={ -INF, -INF, -INF, -INF,  0.0, 4.0, 0.0, 8.0, 10.0 };
    double *drup,cdrup[]={ 10.0, 19.0, 25.0, 15.0,  0.0, 7.0, 0.0, 8.0, 90.0 };
	
    /* Stages */
    //int nstg=2;
    int n_first_stg_rows=4;
	int n_second_stg_rows=4;
    int n_first_stg_cols=17;
	int n_second_stg_cols=8;
    int *rstg,crstg[]={ 0,0,0,0,1,1,1,1,2 };
    int *cstg,ccstg[]={ 0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,1,
		1,1,1,1,1,1,1,2,2 };
	
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
	
	/* scramble */

	int irow[]={ 1,2,7,8,0,3,4,5,6};
    int icol[]={ 9,2,3,4,5,6,7,8,1,
		19,21,23,25,0,26,24,22,20,
		10,11,12,13,14,15,16,17,18 };

    /* local variables */
    int ns=1,ii,iii,jj,*indx,*incr;
    double dp=1.0;

    for (ii=0;ii<nindp;ii++) ns *= nsamp[ii];     /* Compute number of scenarios */

	// debug small sample
	// ns = 3;
	
	// initialize SmiModel
	SmiScnModel *smiModel = new SmiScnModel();



	smiModel->setOsiSolverHandle(*osiClp1);
	
	/* scramble LP entries */
	mrow = (int*)malloc(nels*sizeof(int));
	mcol = (int*)malloc(nels*sizeof(int));
	for (ii=0;ii<nels;ii++)
	{
		mcol[ii] = icol[cmcol[ii]];
		mrow[ii] = irow[cmrow[ii]];
	}
	
	drlo = (double *)malloc(nrow*sizeof(double));
	drup = (double *)malloc(nrow*sizeof(double));
	rstg = (int *)malloc(nrow*sizeof(int));
	for (ii=0;ii<nrow;ii++)
	{
		drlo[irow[ii]] = cdrlo[ii];
		drup[irow[ii]] = cdrup[ii];
		rstg[irow[ii]] = crstg[ii];
	}
	
	dclo = (double *)malloc(ncol*sizeof(double));
	dcup = (double *)malloc(ncol*sizeof(double));
	dobj = (double *)malloc(ncol*sizeof(double));
	cstg = (int *)malloc(ncol*sizeof(int));
	for (ii=0;ii<ncol;ii++)
	{
		dclo[icol[ii]] = cdclo[ii];
		dcup[icol[ii]] = cdcup[ii];
		dobj[icol[ii]] = cdobj[ii];
		cstg[icol[ii]] = ccstg[ii];
	}

	// this to test the matrix update stanza in genScenario
	CoinPackedMatrix *origmat = new CoinPackedMatrix(false,mrow,mcol,dels,nels);
	int corenels = nels - 4;
	
	
	// set core model using Osi interface
	OsiClpSolverInterface ocsi;
	ocsi.loadProblem(CoinPackedMatrix( 1,mrow,mcol,dels,corenels),dclo,dcup,dobj,drlo,drup);
	
	OsiSolverInterface *ohoh= ocsi.clone();
	//smiModel->setCore(ohoh,3,cstg,rstg);

	// test Core Model
	SmiCoreData *osiCore = new SmiCoreData(ohoh,3,cstg,rstg);

	assert(osiCore->getNumCols(0) == n_first_stg_cols);
	assert(osiCore->getNumCols(1) == n_second_stg_cols);
	assert(osiCore->getNumCols(2) == ncol - n_first_stg_cols - n_second_stg_cols);

	assert(osiCore->getNumRows(0) == n_first_stg_rows);
	assert(osiCore->getNumRows(1) == n_second_stg_rows);
	assert(osiCore->getNumRows(2) == nrow - n_first_stg_rows - n_second_stg_rows);

	assert(osiCore->getColStart(0) == 0);
	assert(osiCore->getColStart(1) == n_first_stg_cols );
	assert(osiCore->getColStart(2) == n_first_stg_cols + n_second_stg_cols);
	assert(osiCore->getColStart(3) == ncol);
	
	assert(osiCore->getRowStart(0) == 0);
	assert(osiCore->getRowStart(1) == n_first_stg_rows );
	assert(osiCore->getRowStart(2) == n_first_stg_rows + n_second_stg_rows);
	assert(osiCore->getRowStart(3) == nrow);

	for (ii = 0; ii < n_first_stg_cols ; ii++)
		assert(cstg[osiCore->getColExternalIndex(ii)] == 0);
	for (ii = n_first_stg_cols; ii < n_first_stg_cols + n_second_stg_cols ; ii++)
		assert(cstg[osiCore->getColExternalIndex(ii)] == 1);
	for (ii = n_first_stg_cols + n_second_stg_cols; ii < ncol ; ii++)
		assert(cstg[osiCore->getColExternalIndex(ii)] == 2);

	for (ii = 0; ii < n_first_stg_rows ; ii++)
		assert(rstg[osiCore->getRowExternalIndex(ii)] == 0);
	for (ii = n_first_stg_rows; ii < n_first_stg_rows + n_second_stg_rows ; ii++)
		assert(rstg[osiCore->getRowExternalIndex(ii)] == 1);
	for (ii = n_first_stg_rows + n_second_stg_rows; ii < nrow ; ii++)
		assert(rstg[osiCore->getRowExternalIndex(ii)] == 2);

	const CoinPackedMatrix *origCore = ohoh->getMatrixByRow();
  int t;
	for ( t=0;t<3;t++)
	{
		const CoinPackedVector cpvdrlo = osiCore->getRowLower(t);
		const CoinPackedVector cpvdrup = osiCore->getRowUpper(t);
		const CoinPackedVector cpvdclo = osiCore->getColLower(t);
		const CoinPackedVector cpvdcup = osiCore->getColUpper(t);
		const CoinPackedVector cpvdobj = osiCore->getObjCoefficients(t);

		double *core_drlo = new double[osiCore->getNumRows(t)];
		osiCore->getNode(t)->copyRowLower(core_drlo);

		double elt1,elt2;
		int ic;
		for(ii=osiCore->getColStart(t);ii<osiCore->getColStart(t+1);ii++)
		{
			ic = osiCore->getColExternalIndex(ii);
			elt1 = cpvdclo[ii];
			elt2 = dclo[ic];
			assert(elt1==elt2);
			elt1 = cpvdcup[ii];
			elt2 = dcup[ic];
			assert(elt1==elt2);
			elt1 = cpvdobj[ii];
			elt2 = dobj[ic];
			assert(elt1==elt2);
		}
		for(ii=osiCore->getRowStart(t);ii<osiCore->getRowStart(t+1);ii++)
		{
			assert(cpvdrlo[ii]==drlo[osiCore->getRowExternalIndex(ii)]);
			assert(cpvdrup[ii]==drup[osiCore->getRowExternalIndex(ii)]);
			assert(core_drlo[ii-osiCore->getRowStart(t)] ==drlo[osiCore->getRowExternalIndex(ii)]);

			CoinPackedVector *row1 = osiCore->getMatrixRow(t,ii);
			const CoinPackedVector row2 = origCore->getVector(osiCore->getRowExternalIndex(ii));
			assert(row1->getNumElements() == row2.getNumElements());
			int *indx = row1->getIndices();
			double *els = row1->getElements();
			for (int j=0; j<row1->getNumElements(); j++)
			{
				elt1 = els[j];
				ic = osiCore->getColExternalIndex(indx[j]);
				elt2 = row2[ic];
				assert(elt1==elt2);
			}
		}		
	}

	printf(" *** Successfully tested SmiScnCoreModel.\n");
	
	
	
	// Coin structures for scenario updates to right hand sides
	CoinPackedVector cpv_rlo;
	CoinPackedVector cpv_rup;

	// Coin structure for scenario "updates" to core matrix
	// ..row-ordered
	CoinPackedMatrix *cpm_mat = new CoinPackedMatrix(false,mrow+corenels,mcol+corenels,dels+corenels,nels-corenels);
		
    // initialize right hand side data for first scenario
    indx = (int *) malloc( (1+nindp)*sizeof(int) );
    memset( indx,0,(1+nindp)*sizeof(int));
    for (jj=0;jj<nindp;jj++) {
		indx[jj+1] += indx[jj] + nsamp[jj];
		dp *= dprobs[ indx[jj] ];

		drlo[irow[n_first_stg_rows + jj]] = demand[ indx[jj] ];
		drup[irow[n_first_stg_rows + jj]] = demand[ indx[jj] ];
		
		cpv_rlo.insert(irow[n_first_stg_rows + jj],demand[ indx[jj] ]);
		cpv_rup.insert(irow[n_first_stg_rows + jj],demand[ indx[jj] ]);
    }
	
	// first scenario
	int anc = 0;
	int branch = 1;
	int	is = smiModel->generateScenario(osiCore,cpm_mat,NULL,NULL,NULL,
									&cpv_rlo,&cpv_rup,branch,anc,dp);	
	

	assert(smiModel->getNumScenarios()==1);

	// test first scenario

	// load problem data into OsiSolver
	smiModel->loadOsiSolverData();
	// get Osi pointer
	OsiSolverInterface *smiOsi1 = smiModel->getOsiSolverInterface();

	int nStochCol = smiOsi1->getNumCols();
	int nStochRow = smiOsi1->getNumRows();
	double totalProb = dp;

	// get arrays	
	const double *stochdrlo = smiOsi1->getRowLower();
	const double *stochdrup = smiOsi1->getRowUpper();
	const double *stochdclo = smiOsi1->getColLower();
	const double *stochdcup = smiOsi1->getColUpper();
	const double *stochdobj = smiOsi1->getObjCoefficients();

	// get matrix
	const CoinPackedMatrix *stochmat = smiOsi1->getMatrixByRow();
	for ( t=0;t<3;t++)
	{
		double elt1,elt2;
		int ic;
		for(ii=osiCore->getColStart(t);ii<osiCore->getColStart(t+1);ii++)
		{
			ic = osiCore->getColExternalIndex(ii);
			elt1 = stochdclo[ii];
			elt2 = dclo[ic];
			assert(elt1==elt2);
			elt1 = stochdcup[ii];
			elt2 = dcup[ic];
			assert(elt1==elt2);
			elt1 = stochdobj[ii];
			elt2 = dobj[ic];
			assert(elt1==elt2);
		}
		int ir;
		for(ii=osiCore->getRowStart(t);ii<osiCore->getRowStart(t+1);ii++)
		{
		
			ir = osiCore->getRowExternalIndex(ii);

			assert(stochdrlo[ii]==drlo[ir]);
			assert(stochdrup[ii]==drup[ir]);

			const CoinPackedVector row1 = stochmat->getVector(ii);
			const CoinPackedVector row2 = origmat->getVector(ir);
			assert(row1.getNumElements() == row2.getNumElements());
			const int *indx = row1.getIndices();
			const double *els = row1.getElements();
			for (int j=0; j<row1.getNumElements(); j++)
			{
				elt1 = els[j];
				ic = osiCore->getColExternalIndex(indx[j]);
				elt2 = row2[ic];
				assert(elt1==elt2);
			}
		}		
	}

	printf(" *** Successfully tested problem with scenario %d.\n",is);

	
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
		drlo[irow[n_first_stg_rows + jj]] = demand[ indx[jj] ];
		drup[irow[n_first_stg_rows + jj]] = demand[ indx[jj] ];

		cpv_rlo.setElement(cpv_rlo.findIndex(irow[n_first_stg_rows + jj]),demand[ indx[jj] ]);
		cpv_rup.setElement(cpv_rup.findIndex(irow[n_first_stg_rows + jj]),demand[ indx[jj] ]);
		
		// genScenario
		is = smiModel->generateScenario(osiCore,cpm_mat,NULL,NULL,NULL,
			&cpv_rlo,&cpv_rup,branch,anc,dp);	
		
		
		if (is < 3)
		{		
			// test scenario
			
			// load problem data into OsiSolver
			smiModel->loadOsiSolverData();
			// get Osi pointer
			OsiSolverInterface *smiOsi1 = smiModel->getOsiSolverInterface();
			
			totalProb += dp;
			
			// get arrays	
			stochdrlo = smiOsi1->getRowLower()+nStochRow;
			stochdrup = smiOsi1->getRowUpper()+nStochRow;
			stochdclo = smiOsi1->getColLower()+nStochCol;
			stochdcup = smiOsi1->getColUpper()+nStochCol;
			stochdobj = smiOsi1->getObjCoefficients()+nStochCol;
			
			// get matrix
			const CoinPackedMatrix *stochmat = smiOsi1->getMatrixByRow();
			int t;
			for (t=1;t<3;t++)
			{
				double elt1,elt2;
				int ic;
				int colOff = osiCore->getColStart(1);
				for(ii=osiCore->getColStart(t);ii<osiCore->getColStart(t+1);ii++)
				{
					ic = osiCore->getColExternalIndex(ii);
					elt1 = stochdclo[ii-colOff];
					elt2 = dclo[ic];
					assert(elt1==elt2);
					elt1 = stochdcup[ii-colOff];
					elt2 = dcup[ic];
					assert(elt1==elt2);
					elt1 = stochdobj[ii-colOff];
					elt2 = dobj[ic];
					assert(fabs(elt1 - (elt2*dp/totalProb)) < 1.0e-8);
				}
				int ir,rowOff;
				rowOff = osiCore->getRowStart(1);
				for(ii=osiCore->getRowStart(t);ii<osiCore->getRowStart(t+1);ii++)
				{
					
					ir = osiCore->getRowExternalIndex(ii);
					
					assert(stochdrlo[ii-rowOff]==drlo[ir]);
					assert(stochdrup[ii-rowOff]==drup[ir]);
					
					const CoinPackedVector row1 = stochmat->getVector(ii);
					const CoinPackedVector row2 = origmat->getVector(ir);
					assert(row1.getNumElements() == row2.getNumElements());
					const int *indx = row1.getIndices();
					const double *els = row1.getElements();
					for (int j=0; j<row1.getNumElements(); j++)
					{
						elt1 = els[j];
						ic = osiCore->getColExternalIndex(indx[j]);
						elt2 = row2[ic];
						assert(elt1==elt2);
					}
				}		
			}
			
			nStochCol = smiOsi1->getNumCols();
			nStochRow = smiOsi1->getNumRows();
			
			printf(" *** Successfully tested problem with scenario %d.\n",is);
		}	
	}
	
	assert(ns==smiModel->getNumScenarios());

	// solve with decomp solver
	
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
	
//	FILE *fp = fopen("scenario.txt","wb");

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

//			fprintf(fp,"probability \t %16f \n",scenprob);
			// getColStart returns the starting index of node in OSI model
			for(int j=node->getColStart(); j<node->getColStart()+node->getNumCols(); ++j)
			{
				// getCoreColIndex returns the corresponding Core index
				// in the original (user's) ordering
				scenSum += dobj[node->getCoreColIndex(j)]*dsoln[j];	
//				fprintf(fp,"solution %16f\t objective %16f\n",dsoln[j],dobj[node->getCoreColIndex(j)]);
				

			}			
			// get parent of node
			node = node->getParent();
		}
		objSum += scenSum*scenprob;
	}

//	fclose(fp);
	assert(fabs(smiOsi->getObjValue()-objSum) < 0.01);
        free (incr);
        free (indx);
	free( mrow) ;
	free( mcol) ;
	free( drlo) ;
	free( drup) ;
	free( rstg) ;
	free( dclo) ;
	free( dcup) ;
	free( dobj) ;
	free( cstg) ;
        delete smiModel;
}

#if 0
void SmiDiscreteUnitTest()
{
	// 3 period model
	//
	// min S_0 X_0 + B_0 Y_0
	// s.t.
	//     S_1 X_1 + B_1 Y_1 - S_1 X_0 - B_1 Y_0 = 0
	//     S_2 X_2 + B_2 Y_2 - S_2 X_1 - B_2 Y_2 = 0
	//     S_2 X_2 + B_2 Y_2                     \ge max(S_2 - K, B_2 - K)
	//
	//  X_0, Y_0, X_1, Y_1, X_2, Y_2 free.

	OsiClpSolverInterface *osiClp1 = new OsiClpSolverInterface();
	double INF=osiClp1->getInfinity();

    /* Model dimensions */
    int ncol=6, nrow=3, nels=10;
	
	/* Sparse matrix data...organized by row */
    int cmcol[]={ 2, 3, 0, 1,
				  4, 5, 2, 3,
				  4, 5 };
		
	int cmrow[]={ 0, 0, 0, 0,
				  1, 1, 1, 1,
				  2, 2 };
		

    double cdels[] = { 100.0, 100.0, -100.0, -100.0,
				      100.0, 100.0, -100.0, -100.0,
					  100.0, 100.0 };
    /* Objective */
	double *cdobj[]={ 100.0, 100.0, 0.0, 0.0, 0.0, 0.0 };
	
    /* Column bounds */
    double cdclo[]={ INF, INF, INF, INF, INF, INF };
    double cdcup[]={ INF, INF, INF, INF, INF, INF };
	
    /* Row bounds */
    double cdrlo[]={ 0.0, 0.0, 100.0 };
    double cdrup[]={ 0.0, 0.0, INF };
	
    /* Stages */
	int crstg[]={ 0,1,2 };
    int ccstg[]={ 0,0,1,1,2,2 };

	// initialize SmiModel
	SmiScnModel *smiModel = new SmiScnModel();

	// set core model using Osi interface
	OsiClpSolverInterface ocsi;
	ocsi.loadProblem(CoinPackedMatrix( 1,cmrow,cmcol,cdels,nels),cdclo,cdcup,cdobj,cdrlo,cdrup);
	
	// core model with 3 stages
	SmiCoreData *smiCore = new SmiCoreData(&ocsi,3,ccstg,crstg);

	SmiDiscreteDistribution *smiDD=new SmiDiscreteDistribution();

	int nI = 4;
	int ns[] = { 3, 3, 3, 3 };
	int nt[] = { 1,2,1,2 };

	for (int t=0; t<2; t++)
	{
		j = t+0;
		SmiDiscreteRV *s=new SmiDiscreteRV(nt[j]);

		double p = 0.5*ns[j]*(ns[j]+1);
		for (int i=0; i<ns[j]; i++)
		{
			int cindx=j;
			int rindx=i;
			double elem=(double) (j*i);
			CoinPackedVector c(1,&cindx,&elem);
			CoinPackedVector r(1,&rindx,&elem);
			CoinPackedMatrix m(false,&rindx,&cindx,&elem,1);
			SmiLinearData d(m,c,c,c,r,r);
			SmiDiscreteEvent *e = new SmiDiscreteEvent(d,(i+1)/p);
			s->events_.push_back(e);

			assert(e->getColLower().getIndices()[0]==j);
			assert(e->getRowLower().getIndices()[0]==i);
			assert(e->getMatrix().getCoefficient(i,j)==(double)(j*i));
			assert(e->getEventProb()==(i+1)/p);
		}
		smiDD->addDiscreteRV(s);
	}

	SmiScnModel test;
	test.processDiscreteDistributionIntoScenarios(smiDD,true);
}
#endif

//forward declarations
void replaceFirstWithSecond(CoinPackedVector &dfirst, const CoinPackedVector &dsecond);
void SmiScnModelDiscreteUnitTest()
{
	// test of direct interfaces for discrete distribution
	
	OsiClpSolverInterface *osiClp1 = new OsiClpSolverInterface();
	double INF=osiClp1->getInfinity();

    /* Model dimensions */
    int ncol=27, nrow=9, nels=44;
	
	/* Sparse matrix data...organized by row */
    int *mrow,cmrow[]={ 0, 0, 0, 0, 0,
		1, 1, 1, 1,
		2, 2, 2,
		3, 3, 3, 3, 3,
		4, 4, 4, 4,
		5, 5, 5, 5, 5, 5,
		6, 6, 6, 6, 6,
		7, 7, 7, 7, 7, 7,
		8, 8, 8, 8, 8, 8 };
	  int *mcol,cmcol[]={ 0, 1, 2, 3, 4,
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
    double *dobj,cdobj[]={ 18.0, 21.0, 18.0, 16.0, 10.0, 15.0, 16.0, 14.0, 9.0,
		10.0,  9.0,  6.0, 17.0, 16.0, 17.0, 15.0, 10.0, 0.0,
		13.0,  0.0, 13.0,  0.0,  7.0,  0.0,  7.0,  0.0, 1.0 };
	
    /* Column bounds */
    double *dclo,cdclo[]={ 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
		0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
		0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0 };


    double *dcup,cdcup[]={ INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,
		INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,
		INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF };
	
    /* Row bounds */
    double *drlo,cdrlo[]={ -INF, -INF, -INF, -INF,  0.0, 4.0, 0.0, 8.0, 10.0 };
    double *drup,cdrup[]={ 10.0, 19.0, 25.0, 15.0,  0.0, 7.0, 0.0, 8.0, 90.0 };
	
    /* Stages */
    //int nstg=2;
    int n_first_stg_rows=4;
    int *rstg,crstg[]={ 0,0,0,0,1,1,1,1,2 };
    int *cstg,ccstg[]={ 0,0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0,1,
		1,1,1,1,1,1,1,2,2 };
	
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
	
	/* scramble */

	int irow[]={ 1,2,7,8,0,3,4,5,6};
    int icol[]={ 9,2,3,4,5,6,7,8,1,
		19,21,23,25,0,26,24,22,20,
		10,11,12,13,14,15,16,17,18 };

    /* local variables */
    int ii,jj;


	// initialize SmiModel
	SmiScnModel *smiModel = new SmiScnModel();



	smiModel->setOsiSolverHandle(*osiClp1);
	
	/* scramble LP entries --- just for testing!! */
	mrow = (int*)malloc(nels*sizeof(int));
	mcol = (int*)malloc(nels*sizeof(int));
	for (ii=0;ii<nels;ii++)
	{
		mcol[ii] = icol[cmcol[ii]];
		mrow[ii] = irow[cmrow[ii]];
	}
	
	drlo = (double *)malloc(nrow*sizeof(double));
	drup = (double *)malloc(nrow*sizeof(double));
	rstg = (int *)malloc(nrow*sizeof(int));
	for (ii=0;ii<nrow;ii++)
	{
		drlo[irow[ii]] = cdrlo[ii];
		drup[irow[ii]] = cdrup[ii];
		rstg[irow[ii]] = crstg[ii];
	}
	
	dclo = (double *)malloc(ncol*sizeof(double));
	dcup = (double *)malloc(ncol*sizeof(double));
	dobj = (double *)malloc(ncol*sizeof(double));
	cstg = (int *)malloc(ncol*sizeof(int));
	for (ii=0;ii<ncol;ii++)
	{
		dclo[icol[ii]] = cdclo[ii];
		dcup[icol[ii]] = cdcup[ii];
		dobj[icol[ii]] = cdobj[ii];
		cstg[icol[ii]] = ccstg[ii];
	}

	// this to test the matrix update stanza in genScenario
	CoinPackedMatrix *origmat = new CoinPackedMatrix(false,mrow,mcol,dels,nels);
	int corenels = nels - 4;
	
	
	// set core model using Osi interface
	OsiClpSolverInterface ocsi;
	ocsi.loadProblem(CoinPackedMatrix( 1,mrow,mcol,dels,corenels),dclo,dcup,dobj,drlo,drup);
	
	// core model with 3 stages
	SmiCoreData *smiCore = new SmiCoreData(&ocsi,3,cstg,rstg);

	// Coin structure for scenario "updates" to core matrix
	// ..row-ordered
	CoinPackedMatrix *cpm_mat = new CoinPackedMatrix(false,mrow+corenels,mcol+corenels,dels+corenels,nels-corenels);



	// Create discrete distribution
	SmiDiscreteDistribution *smiDD = new SmiDiscreteDistribution(smiCore);

	int index=0;
	for (jj=0;jj<nindp;jj++)
	{
		SmiDiscreteRV *smiRV = new SmiDiscreteRV(1);
		for (ii=0;ii<nsamp[jj];ii++)
		{			
			CoinPackedVector empty_vec;
			CoinPackedVector cpv_rlo ;
			CoinPackedVector cpv_rup ;
			cpv_rlo.insert(irow[n_first_stg_rows + jj], demand[index+ii]);
			cpv_rup.insert(irow[n_first_stg_rows + jj], demand[index+ii]);
			smiRV->addEvent(*cpm_mat,empty_vec,empty_vec,empty_vec,cpv_rlo,cpv_rup,dprobs[index+ii]);
			cpv_rlo.clear();
			cpv_rup.clear();
		}
		assert(smiRV->getNumEvents()==nsamp[jj]);
		for (ii=0;ii<nsamp[jj];ii++)
		{
			assert(smiRV->getEventColLower(ii).getNumElements()==0);
			assert(smiRV->getEventColUpper(ii).getNumElements()==0);
			assert(smiRV->getEventObjective(ii).getNumElements()==0);
			assert(smiRV->getEventMatrix(ii).getNumElements()==cpm_mat->getNumElements());
			assert(smiRV->getEventRowLower(ii).getElements()[0] == demand[index+ii]);
			assert(smiRV->getEventRowLower(ii).getIndices()[0] == irow[n_first_stg_rows + jj]);
			assert(smiRV->getEventRowUpper(ii).getElements()[0] == demand[index+ii]);
			//printf("event prob %g\n",smiRV->getEventProb(ii));
			assert(fabs(smiRV->getEventProb(ii) - dprobs[index+ii]) < 0.0000001);
		}
		smiDD->addDiscreteRV(smiRV);
		index+=nsamp[jj];
	}

	assert(smiDD->getNumRV() == nindp);

	// this is cut-pasted from 	
	smiModel->processDiscreteDistributionIntoScenarios(smiDD);

	if (0)
	{
	SmiCoreData *core=smiDD->getCore();

	int nindp = smiDD->getNumRV();
	assert(nindp > 0);

	int ns=1;
	double dp=1.0;

	CoinPackedMatrix matrix ;
	CoinPackedVector cpv_dclo ;
	CoinPackedVector cpv_dcup ;
	CoinPackedVector cpv_dobj ;
	CoinPackedVector cpv_drlo ;
	CoinPackedVector cpv_drup ;

	cpv_dclo.setTestForDuplicateIndex(true);
	cpv_dcup.setTestForDuplicateIndex(true);
	cpv_dobj.setTestForDuplicateIndex(true);
	cpv_drlo.setTestForDuplicateIndex(true);
	cpv_drup.setTestForDuplicateIndex(true);
	
	// initialize data for first scenario
	vector<int> indx(nindp);
	vector<int> nsamp(nindp);
	vector<int> label(core->getNumStages());
	vector<int>::iterator iLabel;

	for (iLabel=label.begin(); iLabel<label.end(); ++iLabel)
		*iLabel=0;
	
	int jj;
	int index=0;
	for (jj=0;jj<nindp;jj++) {
		SmiDiscreteRV *smiRV = smiDD->getDiscreteRV(jj);

		indx[jj] = 0;
		nsamp[jj] = smiRV->getNumEvents();
		ns *= nsamp[jj];
		dp *= smiRV->getEventProb(indx[jj]);
		
		//AJK -debug code for unitTest
		drlo[irow[n_first_stg_rows + jj]] = demand[ index + indx[jj] ];
		drup[irow[n_first_stg_rows + jj]] = demand[ index + indx[jj] ];
		index += nsamp[jj];
		//AJK -debug end

		
		cpv_dclo.append(smiRV->getEventColLower(indx[jj]));
		
		cpv_dcup.append(smiRV->getEventColUpper(indx[jj]));
		
		cpv_dobj.append(smiRV->getEventObjective(indx[jj]));
		
		cpv_drlo.append(smiRV->getEventRowLower(indx[jj]));
		
		cpv_drup.append(smiRV->getEventRowUpper(indx[jj]));
		
		//TODO test smiModel code
		CoinPackedMatrix m = smiRV->getEventMatrix(indx[jj]);
		assert(!m.isColOrdered());
		if (matrix.getNumElements())
		{
			for (int i=0; i<m.getNumRows(); ++i)
			{
				CoinPackedVector row=m.getVector(i);
				CoinPackedVector rrow=matrix.getVector(i);
				for (int j=m.getVectorFirst(i); j<m.getVectorLast(j); ++j)
				{
					
					assert(rrow[j] == 0.0);//tests duplicate index
					matrix.modifyCoefficient(i,j,row[j],true);
				}
			}
		}
		else
			matrix = m;
		

    }
	
	// first scenario
	int anc = 0;
	int branch = 1;
	int	is = smiModel->generateScenario(core,&matrix,&cpv_dclo,&cpv_dcup,&cpv_dobj,
									&cpv_drlo,&cpv_drup,branch,anc,dp);

		// test first scenario

	// load problem data into OsiSolver
	smiModel->loadOsiSolverData();
	// get Osi pointer
	OsiSolverInterface *smiOsi1 = smiModel->getOsiSolverInterface();

	int nStochCol = smiOsi1->getNumCols();
	int nStochRow = smiOsi1->getNumRows();
	double totalProb = dp;

	// get arrays	
	const double *stochdrlo = smiOsi1->getRowLower();
	const double *stochdrup = smiOsi1->getRowUpper();
	const double *stochdclo = smiOsi1->getColLower();
	const double *stochdcup = smiOsi1->getColUpper();
	const double *stochdobj = smiOsi1->getObjCoefficients();

	// get matrix
	const CoinPackedMatrix *stochmat = smiOsi1->getMatrixByRow();
	for (int t=0;t<3;t++)
	{
		double elt1,elt2;
		int ic;
		for(ii=core->getColStart(t);ii<core->getColStart(t+1);ii++)
		{
			ic = core->getColExternalIndex(ii);
			elt1 = stochdclo[ii];
			elt2 = dclo[ic];
			assert(elt1==elt2);
			elt1 = stochdcup[ii];
			elt2 = dcup[ic];
			assert(elt1==elt2);
			elt1 = stochdobj[ii];
			elt2 = dobj[ic];
			assert(elt1==elt2);
		}
		int ir;
		for(ii=core->getRowStart(t);ii<core->getRowStart(t+1);ii++)
		{
		
			ir = core->getRowExternalIndex(ii);

			assert(stochdrlo[ii]==drlo[ir]);
			assert(stochdrup[ii]==drup[ir]);

			const CoinPackedVector row1 = stochmat->getVector(ii);
			const CoinPackedVector row2 = origmat->getVector(ir);
			assert(row1.getNumElements() == row2.getNumElements());
			const int *indx = row1.getIndices();
			const double *els = row1.getElements();
			for (int j=0; j<row1.getNumElements(); j++)
			{
				elt1 = els[j];
				ic = core->getColExternalIndex(indx[j]);
				elt2 = row2[ic];
				assert(elt1==elt2);
			}
		}		
	}

	printf(" *** Successfully tested problem with scenario %d.\n",is);



	SmiTreeNode<SmiScnNode *> *root = smiModel->smiTree_.getRoot();
	smiModel->smiTree_.setChildLabels(root,label);

	/* sample space increment initialized to 1 */
    int *incr = (int *) malloc( nindp*sizeof(int) );
    for (jj=0;jj<nindp;jj++) incr[jj] = 1;
		
	/***** ...main loop to generate scenarios from discrete random variables
		For each scenario index ii:
        If the sample size nsamp[jj] divides the scenario index ii,
		reverse the increment direction incr[jj]
		and increase the random variable index jj by 1.
        Increment the jj'th random variable by incr[jj]
		and generate new sample data.
    ***** */
	
    for (int iss=1;iss<ns;iss++) {
		int iii=iss; jj=0;
		while ( !(iii%nsamp[jj]) ) {
			iii /= nsamp[jj];
			incr[jj] = -incr[jj];
			jj++;
		}

		SmiDiscreteRV *smiRV = smiDD->getDiscreteRV(jj);

		dp /= smiRV->getEventProb(indx[jj]);
		indx[jj] += incr[jj];
		dp *= smiRV->getEventProb(indx[jj]);

		//AJK -debug code for unitTest
		index = 0;


		for (iLabel=label.begin(); iLabel<label.end(); ++iLabel)
			*iLabel=0;

		for (int jjj=0;jjj<smiDD->getNumRV();++jjj)
		{
			int nEvents = smiDD->getDiscreteRV(jjj)->getNumEvents();
			int iStage = smiDD->getDiscreteRV(jjj)->getStage();

			//label[iStage-1] += indx[jjj]*nEvents;
			label[iStage] *= nEvents;
			label[iStage] += indx[jjj];

			drlo[irow[n_first_stg_rows + jjj]] = demand[ indx[jjj]+index ];
			drup[irow[n_first_stg_rows + jjj]] = demand[ indx[jjj]+index ];
			index += nsamp[jjj];

		}
		//AJK -debug end

		// set data
		//TODO -- should we declare NULL entries to have 0 entries?  
		//smiModel would eliminate these tests
		replaceFirstWithSecond(cpv_dclo,smiRV->getEventColLower(indx[jj]));

		
		replaceFirstWithSecond(cpv_dcup,smiRV->getEventColUpper(indx[jj]));
		
		replaceFirstWithSecond(cpv_dobj,smiRV->getEventObjective(indx[jj]));
		
		replaceFirstWithSecond(cpv_drlo,smiRV->getEventRowLower(indx[jj]));


		replaceFirstWithSecond(cpv_drup,smiRV->getEventRowUpper(indx[jj]));


		//TODO test smiModel code
		CoinPackedMatrix m = smiRV->getEventMatrix(indx[jj]);
		assert(!m.isColOrdered());
		if (matrix.getNumElements())
		{
			for (int i=0; i<m.getNumRows(); ++i)
			{
				CoinPackedVector row=m.getVector(i);
				CoinPackedVector rrow=matrix.getVector(i);
				for (int j=m.getVectorFirst(i); j<m.getVectorLast(j); ++j)
				{
					matrix.modifyCoefficient(i,j,row[j],true);
				}
			}
		}
		else
			matrix = m;		
		
		// find ancestor node
		SmiTreeNode<SmiScnNode *> *tnode = smiModel->smiTree_.find(label);
		
		// add scenario
		anc = tnode->scenario();
		assert(anc==0);
//		branch = tnode->depth()+1;
		branch = tnode->depth();
		assert(branch==1);
	    is = smiModel->generateScenario(core,&matrix,&cpv_dclo,&cpv_dcup,&cpv_dobj,
									&cpv_drlo,&cpv_drup,branch,anc,dp);

		smiModel->smiTree_.setChildLabels(tnode,label);

		if (is < 3)
		{		
			// test scenario
			
			// load problem data into OsiSolver
			smiModel->loadOsiSolverData();
			// get Osi pointer
			OsiSolverInterface *smiOsi1 = smiModel->getOsiSolverInterface();
			
			totalProb += dp;
			
			// get arrays	
			stochdrlo = smiOsi1->getRowLower()+nStochRow;
			stochdrup = smiOsi1->getRowUpper()+nStochRow;
			stochdclo = smiOsi1->getColLower()+nStochCol;
			stochdcup = smiOsi1->getColUpper()+nStochCol;
			stochdobj = smiOsi1->getObjCoefficients()+nStochCol;
			
			// get matrix
			const CoinPackedMatrix *stochmat = smiOsi1->getMatrixByRow();
			int t;
			for (t=1;t<3;t++)
			{
				double elt1,elt2;
				int ic;
				int colOff = core->getColStart(1);
				for(ii=core->getColStart(t);ii<core->getColStart(t+1);ii++)
				{
					ic = core->getColExternalIndex(ii);
					elt1 = stochdclo[ii-colOff];
					elt2 = dclo[ic];
					assert(elt1==elt2);
					elt1 = stochdcup[ii-colOff];
					elt2 = dcup[ic];
					assert(elt1==elt2);
					elt1 = stochdobj[ii-colOff];
					elt2 = dobj[ic];
					assert(fabs(elt1 - (elt2*dp/totalProb)) < 1.0e-8);
				}
				int ir,rowOff;
				rowOff = core->getRowStart(1);
				for(ii=core->getRowStart(t);ii<core->getRowStart(t+1);ii++)
				{
					
					ir = core->getRowExternalIndex(ii);
					
					assert(stochdrlo[ii-rowOff]==drlo[ir]);
					assert(stochdrup[ii-rowOff]==drup[ir]);
					
					const CoinPackedVector row1 = stochmat->getVector(ii);
					const CoinPackedVector row2 = origmat->getVector(ir);
					assert(row1.getNumElements() == row2.getNumElements());
					const int *indx = row1.getIndices();
					const double *els = row1.getElements();
					for (int j=0; j<row1.getNumElements(); j++)
					{
						elt1 = els[j];
						ic = core->getColExternalIndex(indx[j]);
						elt2 = row2[ic];
						assert(elt1==elt2);
					}
				}		
			}
			
			nStochCol = smiOsi1->getNumCols();
			nStochRow = smiOsi1->getNumRows();
			
			printf(" *** Successfully tested problem with scenario %d.\n",is);
		}	

	}

	delete incr;
	free( drlo) ;
	free( drup) ;
	free( rstg) ;
	free( dclo) ;
	free( dcup) ;
	free( dobj) ;
	free( cstg) ;
}

	
	// solve with decomp solver
	
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

//			fprintf(fp,"probability \t %16f \n",scenprob);
			// getColStart returns the starting index of node in OSI model
			for(int j=node->getColStart(); j<node->getColStart()+node->getNumCols(); ++j)
			{
				// getCoreColIndex returns the corresponding Core index
				// in the original (user's) ordering
				scenSum += dobj[node->getCoreColIndex(j)]*dsoln[j];	
//				fprintf(fp,"solution %16f\t objective %16f\n",dsoln[j],dobj[node->getCoreColIndex(j)]);
				

			}			
			// get parent of node
			node = node->getParent();
		}
		objSum += scenSum*scenprob;
	}

//	fclose(fp);
	assert(fabs(smiOsi->getObjValue()-objSum) < 0.01);

	printf(" *** Completed all tests for SmiScnModel.\n");
        delete smiModel;
}

void ModelBug()
{

	OsiClpSolverInterface osi;	
	double INF=osi.getInfinity();

/*
NAME    BUG
ROWS
  N  obj
  G  C0
  G  C1
  G  C2
  G  C3
*/
	int nCoreRows=4;
	double dCoreRup[] = { INF, INF, INF, INF };
/*
COLUMNS
   x01   obj   1
   x01   C3    1
   x01   C1    1
   x01   C0    1
   x02   obj   1
   x02   C2    1
   x02   C1    1
   x02   C0    1
   x03   obj   1
   x03   C3    1
   x03   C2    1
   x03   C0    1
   x04   obj   0.5
   x04   C3    1
   x04   C1    1
   x05   obj   0.5
   x05   C2    1
   x05   C1    1
   x06   obj   0.5
   x06   C3    1
   x06   C2    1
*/
	int nCoreCols=6;
	double dCoreClo[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	double dCoreCup[] = {INF, INF, INF, INF, INF, INF};
	double dCoreObj[] = {1.0, 1.0, 1.0, 0.5, 0.5, 0.5};

	//int nCoreElts=15;
	int iCoreColStarts[] = {0,
						  3,
						  6,
						  9,
						  11,
						  13,
						  15};
	int iCoreRowIndice[] = {3,1,0,
						  2,1,0,
						  3,2,0,
						  3,1,
						  2,1,
						  3,2};
	double dCoreMatEntries[] = {1.0, 1.0, 1.0,
							  1.0, 1.0, 1.0,
							  1.0, 1.0, 1.0,
							  1.0, 1.0,
							  1.0, 1.0,
							  1.0, 1.0 };						

/*
RHS
  RHS    C0    0
  RHS    C1    1
  RHS    C2    1
  RHS    C3    1
ENDATA
*/
	double dCoreRlo[] = { 0.0, 1.0, 1.0, 1.0 };
/*
-----------------------------------------------------------
bug.time:

TIME          BUG
PERIODS       LP
     x01       C0                      STG01
     x04       C1                      STG02 
ENDATA 
-----------------------------------------------------------
*/
	int nCoreStages = 2;
	int iColStages[] = {0,0,0,1,1,1};
	int iRowStages[] = {0,1,1,1};

/*
----------------------------------------------------------
bug.stoch file:

NAME          BUG
SCENARIOS     DISCRETE                REPLACE
  SC SCEN01    ROOT           0.500    STG02
     RHS       C1             1.000
     RHS       C2             1.000
     RHS       C3             0.000
  SC SCEN02    ROOT           0.500    STG02
     RHS       C1             0.000
     RHS       C2             1.000
     RHS       C3             0.000 
ENDATA
*/

	//int nScenarios = 2;
	int iBranchStage[] = {1,1};

	int iAncestorScn[] = {0,0};
	double dProbScn[] = {0.5, 0.5};
	
	double drlo0[] = {1.0, 1.0, 0.0 };
	int indices[]  = {1, 2, 3};
	CoinPackedVector *rlo0 = new CoinPackedVector(3, indices,drlo0);
	double drlo1[] = {0.0, 1.0, 0.0 };
	CoinPackedVector *rlo1 = new CoinPackedVector(3, indices,drlo1);

	// generate Core model
	
	osi.loadProblem(nCoreCols, nCoreRows,iCoreColStarts,iCoreRowIndice,dCoreMatEntries,dCoreClo,dCoreCup,dCoreObj,
								dCoreRlo,dCoreRup);
	SmiCoreData *osiCore = new SmiCoreData(&osi,nCoreStages,iColStages,iRowStages);

	// initialize SmiScnModel
	SmiScnModel *smiModel = new SmiScnModel();

	// Add Scenarios
	int	is = smiModel->generateScenario(osiCore,NULL,NULL,NULL,NULL,
										rlo0,NULL,iBranchStage[0],iAncestorScn[0],dProbScn[0]);
	
	is = smiModel->generateScenario(osiCore,NULL,NULL,NULL,NULL,
										rlo1,NULL,iBranchStage[1],iAncestorScn[1],dProbScn[1]);

	// Set Solver
	OsiClpSolverInterface osiClp;
	smiModel->setOsiSolverHandle(osiClp);
	
	// Load Scenarios into Deterministic Equivalent
	smiModel->loadOsiSolverData();

	// Get Det Equiv OSI
	OsiSolverInterface *osiStoch = smiModel->getOsiSolverInterface();

	// write MPS file
	osiStoch->writeMps("bug_gen");
	
	// Solve
	osiStoch->initialSolve();

			// print results
		printf("Solved stochastic program %s\n", "BUG");
		printf("Number of rows: %d\n",osiStoch->getNumRows());
		printf("Number of cols: %d\n",osiStoch->getNumCols());
		printf("Optimal value: %g\n",osiStoch->getObjValue());
		
		assert(osiStoch->getObjValue()== 0.5);

}

void SmpsBug()
{
		SmiScnModel smi;

		std::string dataDir="../../Data/Stochastic";
		
		// read SMPS model from files
		//	<name>.core, <name>.time, and <name>.stoch
		// the argument myCombineRule overrides the combine rule specified in the Stoch file
		smi.readSmps((dataDir+"/bug").c_str());		

		// generate OSI solver object
		// 	here we use OsiClp
		OsiClpSolverInterface *clp = new OsiClpSolverInterface();

		// set solver object for SmiScnModel
		smi.setOsiSolverHandle(*clp);	

		// load solver data
		// 	this step generates the deterministic equivalent 
		//	and returns an OsiSolver object 
		OsiSolverInterface *osiStoch = smi.loadOsiSolverData();

		// solve
		osiStoch->initialSolve();		

		// print results
		printf("Solved stochastic program Bug\n");
		printf("Number of rows: %d\n",osiStoch->getNumRows());
		printf("Number of cols: %d\n",osiStoch->getNumCols());
		printf("Optimal value: %g\n",osiStoch->getObjValue());		
		assert(osiStoch->getObjValue()== 0.5);

}	

void Smps20()
{
		SmiScnModel smi;

		std::string dataDir="data";
		
		// read SMPS model from files
		//	<name>.core, <name>.time, and <name>.stoch
		// the argument myCombineRule overrides the combine rule specified in the Stoch file
		smi.readSmps((dataDir+"/20").c_str());		

		// generate OSI solver object
		// 	here we use OsiClp
		OsiClpSolverInterface *clp = new OsiClpSolverInterface();

		// set solver object for SmiScnModel
		smi.setOsiSolverHandle(*clp);	

		// load solver data
		// 	this step generates the deterministic equivalent 
		//	and returns an OsiSolver object 
		OsiSolverInterface *osiStoch = smi.loadOsiSolverData();

		// solve
		osiStoch->initialSolve();		

		// print results
		printf("Solved stochastic program 20\n");
		printf("Number of rows: %d\n",osiStoch->getNumRows());
		printf("Number of cols: %d\n",osiStoch->getNumCols());
		printf("Optimal value: %g\n",osiStoch->getObjValue());		
		//assert(osiStoch->getObjValue()== 0.5);

}	

int main()
{
  
	testingMessage( "Testing SmiTreeNode \n");
	SmiTreeNodeUnitTest();

	testingMessage( "Testing SmiScenarioTree\n" );
	SmiScenarioTreeUnitTest();

	testingMessage( "Testing SmiScnSmpsIO\n" );
	SmiScnSmpsIOUnitTest();

	testingMessage( "Testing base data structures for SmiScnModel\n");
	SmiScnModelScenarioUnitTest();

	testingMessage( "Testing SmiScnModel Discrete Distribution\n" );
	SmiScnModelDiscreteUnitTest();

	testingMessage("Model generation for simple model Bug");
	ModelBug();

	testingMessage("Read SMPS version of simple model Bug");
	SmpsBug();


	testingMessage( "*** Done! *** \n");
	

  return 0;
}
