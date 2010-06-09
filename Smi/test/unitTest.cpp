// Copyright (C) 2003, International Business Machines
// Corporation and others.  All Rights Reserved.


#include <string>

#define SMI_TEST_DATA_DIR  ""

#include "SmiScnModel.hpp"
#include "OsiClpSolverInterface.hpp"

//####################################################################

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

void myAssert(const char * file, const int line, bool c )
{
	if(!c)
	{
		char l[10];
		sprintf(l,"%d",line);
		string e("[Smi] Error thrown from file ");
		e+=file;
		e+=", line ";
		e+=l;
		cout << e << endl;
		exit(1);
	}
}

void DecompUnitTest()
{


	OsiClpSolverInterface osi;
	double INF=osi.getInfinity();

	/* Problem Statement:
	 *
	 *    minimize x01 + x02 + x03 + 1/2 E[ x04 + x05 + x06 ]
	 *
	 *    subject to:
	 *      xN =>  0
	 *      0  <=  x01 + x02 + x03
	 *      C1 <=  x01 + x02 +              x04 + x05
	 *      C2 <=      + x02 + x03                x05 + x06
	 *      C3 <=  x01       + x03          x04 +     + x06
	 *
	 *    (C1, C2, C3) = ( 1, 1, 0) wp 1/2
	 *    (C1, C2, C3) = ( 0, 1, 0) wp 1/2
	 */


	int nCoreRows=4;
	double dCoreRup[] = { INF, INF, INF, INF };

	int nCoreCols=6;
	double dCoreClo[] = {0.0, 0.0, 0.0, 0.0, 0.0, 0.0};
	double dCoreCup[] = {INF, INF, INF, INF, INF, INF};
	double dCoreObj[] = {1.0, 1.0, 1.0, 0.5, 0.5, 0.5};

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
	double dCoreRlo[] = { 0.0, 1.0, 1.0, 1.0 };

	int nCoreStages = 2;
	int iColStages[] = {0,0,0,1,1,1};
	int iRowStages[] = {0,1,1,1};

	int iBranchStage[] = {1,1};

	int iAncestorScn[] = {0,0};
	double dProbScn[] = {0.5, 0.5};

	double drlo0[] = {1.0, 1.0, 0.0 };
	int indices[]  = {1, 2, 3};
	CoinPackedVector *rlo0 = new CoinPackedVector(3, indices,drlo0);
	double drlo1[] = {0.0, 1.0, 0.0 };
	CoinPackedVector *rlo1 = new CoinPackedVector(3, indices,drlo1);

	// generate Core model

	osi.loadProblem(nCoreCols,
		nCoreRows,iCoreColStarts,iCoreRowIndice,dCoreMatEntries,dCoreClo,
		dCoreCup,dCoreObj,
		dCoreRlo,dCoreRup);
	SmiCoreData *osiCore = new
		SmiCoreData(&osi,nCoreStages,iColStages,iRowStages);

	// initialize SmiScnModel
	SmiScnModel *smiModel = new SmiScnModel();

	// Add Scenarios
	int	is = smiModel->generateScenario(osiCore,NULL,NULL,NULL,NULL,
		rlo0,NULL,iBranchStage[0],
		iAncestorScn[0],dProbScn[0]);

	is = smiModel->generateScenario(osiCore,NULL,NULL,NULL,NULL,
		rlo1,NULL,iBranchStage[1],
		iAncestorScn[1],dProbScn[1]);


	/*
	 *  set up vector of submodels, one for each scenario
	 */
	vector<SmiScnModel *> submodel;
	for( int jj=0; jj<smiModel->getNumScenarios(); jj++)
	{
		submodel.push_back(new SmiScnModel());
		submodel[jj]->setOsiSolverHandle(new OsiClpSolverInterface());
	}


	// add root node to submodel[0]
	myAssert(__FILE__,__LINE__,1 == submodel[0]->addNodeToSubmodel(smiModel->getRootNode()) );

	// add leaf node to submodel[0]
	myAssert(__FILE__,__LINE__,1 == submodel[0]->addNodeToSubmodel(smiModel->getLeafNode(0)) );
	submodel[is]->setModelProb(1.0);

	vector<SmiScnNode *> vecSub0 = submodel[0]->smiTree_.wholeTree();
	submodel[1]->setModelProb(1.0);

	myAssert(__FILE__,__LINE__,    !vecSub0[0]->isVirtualNode() );
	myAssert(__FILE__,__LINE__,     vecSub0[0]->getNumCols() == 3 );
	myAssert(__FILE__,__LINE__,     vecSub0[0]->getNumRows() == 1 );
	myAssert(__FILE__,__LINE__,     vecSub0[0]->getProb() == 1.0 );

	myAssert(__FILE__,__LINE__,    !vecSub0[1]->isVirtualNode() );
	myAssert(__FILE__,__LINE__,     vecSub0[1]->getNumCols() == 3 );
	myAssert(__FILE__,__LINE__,     vecSub0[1]->getNumRows() == 3 );
	myAssert(__FILE__,__LINE__,     vecSub0[1]->getProb() == 0.5 );

	// add leaf node to submodel[1]
	myAssert(__FILE__,__LINE__,2 == submodel[1]->addNodeToSubmodel(smiModel->getLeafNode(1)) );
	submodel[1]->setModelProb(1.0);

	vector<SmiScnNode *> vecSub1 = submodel[1]->smiTree_.wholeTree();

	myAssert(__FILE__,__LINE__,     vecSub1[0]->isVirtualNode() );
	myAssert(__FILE__,__LINE__,     vecSub1[0]->getNumCols() == 3 );

	myAssert(__FILE__,__LINE__,    !vecSub1[1]->isVirtualNode() );
	myAssert(__FILE__,__LINE__,     vecSub1[1]->getNumCols() == 3 );
	myAssert(__FILE__,__LINE__,     vecSub1[1]->getNumRows() == 3 );
	myAssert(__FILE__,__LINE__,     vecSub1[1]->getProb() == 0.5 );

	/*
	 * set up vector of OSI models, one for each submodel
	 */
	vector<OsiSolverInterface *> osiSubmodel;
	osiSubmodel.reserve(submodel.size());

	//myAssert(__FILE__,__LINE__,     osiSubmodel[0]=submodel[0]->loadOsiSolverData());
	//myAssert(__FILE__,__LINE__,     osiSubmodel[1]=submodel[1]->loadOsiSolverData());
	osiSubmodel.push_back(submodel[0]->loadOsiSolverData());
	osiSubmodel.push_back(submodel[1]->loadOsiSolverData());


	myAssert(__FILE__,__LINE__,    !vecSub0[0]->isVirtualNode() );
	myAssert(__FILE__,__LINE__, vecSub0[0]->getNumCols() == 3 );
	myAssert(__FILE__,__LINE__, vecSub0[0]->getNumRows() == 1 );
	myAssert(__FILE__,__LINE__, vecSub0[0]->getColStart() == 0 );
	myAssert(__FILE__,__LINE__,     vecSub0[0]->getRowStart() == 0 );
	myAssert(__FILE__,__LINE__,     vecSub0[0]->getProb() == 1.0 );

	myAssert(__FILE__,__LINE__,    !vecSub0[1]->isVirtualNode() );
	myAssert(__FILE__,__LINE__,     vecSub0[1]->getNumCols() == 3 );
	myAssert(__FILE__,__LINE__,     vecSub0[1]->getNumRows() == 3 );
	myAssert(__FILE__,__LINE__,     vecSub0[1]->getColStart() == 3 );
	myAssert(__FILE__,__LINE__,     vecSub0[1]->getRowStart() == 1 );
	myAssert(__FILE__,__LINE__,     vecSub0[1]->getProb() == 0.5 );



	myAssert(__FILE__,__LINE__,     vecSub1[0]->isVirtualNode() );
	myAssert(__FILE__,__LINE__,     vecSub1[0]->getNumCols() == 3 );

	myAssert(__FILE__,__LINE__,    !vecSub1[1]->isVirtualNode() );
	myAssert(__FILE__,__LINE__,     vecSub1[1]->getNumCols() == 3 );
	myAssert(__FILE__,__LINE__,     vecSub1[1]->getNumRows() == 3 );
	myAssert(__FILE__,__LINE__,     vecSub1[1]->getColStart() == 3 );
	myAssert(__FILE__,__LINE__,     vecSub1[1]->getRowStart() == 0 );
	myAssert(__FILE__,__LINE__,     vecSub1[1]->getProb() == 0.5 );

	 /* Submodels
	 *
	 * 1:   dobj = [1, 1, 1, 0.25, 0.25, 0.25]
	 *      drlo = [0, 1, 1, 0 ],     drup = [inf, inf, inf, inf]
	 *      dclo = [0,0,0,0,0,0],     dcup = [inf,inf,inf,inf,inf,inf]
	 *      matrix = [ 1 1 1 0 0 0
	 *                 1 1 0 1 1 0
	 *                 0 1 1 0 1 1
	 *                 1 0 1 1 0 1 ]
	 *      nrows = 4
	 *      ncols = 6
	 *      coffset = 0
	 *      roffset = 0
	 *
	 * 2:   dobj = [0, 0, 0, 0.25, 0.25, 0.25]
	 *      drlo = [0, 1, 0],           drup = [inf, inf, inf]
	 *      dclo = [inf,inf,inf,0,0,0], dcup = [inf, inf, inf, inf, inf, inf]
	 *      matrix = [ 1 1 0 1 1 0
	 *                 0 1 1 0 1 1
	 *                 1 0 1 1 0 1 ]
	 *      nrows = 3
	 *      ncols = 6
	 *      coffset = 3
	 *      roffset = 0
	 */

	myAssert(__FILE__,__LINE__,osiSubmodel[0]->getNumCols()==6);
	myAssert(__FILE__,__LINE__,osiSubmodel[0]->getNumRows()==4);

	myAssert(__FILE__,__LINE__,osiSubmodel[1]->getNumCols()==6);
	myAssert(__FILE__,__LINE__,osiSubmodel[1]->getNumRows()==3);

	delete smiModel;
	delete osiCore;

	delete rlo0;
	delete rlo1;
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
	int is12 = s1.addPathtoLeaf(node->scenario(),node->depth(),ppath5,
		node->depth()+1);
	s1.setChildLabels(node,label5);

	node = s1.find(label6);
	int is13 = s1.addPathtoLeaf(node->scenario(),node->depth(),ppath6,
		node->depth()+1);
	s1.setChildLabels(node,label6);

	node = s1.find(label7);
	int is14 = s1.addPathtoLeaf(node->scenario(),node->depth(),ppath7,
		node->depth()+1);
	s1.setChildLabels(node,label7);

	node = s1.find(label8);
	int is15 = s1.addPathtoLeaf(node->scenario(),node->depth(),ppath8,
		node->depth()+1);
	s1.setChildLabels(node,label8);



	vector<int *> vec1 = s.getScenario(is1);
	myAssert(__FILE__,__LINE__, vec1[0] == i1 );
	myAssert(__FILE__,__LINE__, vec1[1] == i2 );
	myAssert(__FILE__,__LINE__, vec1[2] == i4 );

	vector<int *> vec11 = s1.getScenario(is11);
	myAssert(__FILE__,__LINE__, vec11[0] == ii1 );
	myAssert(__FILE__,__LINE__, vec11[1] == ii2 );
	myAssert(__FILE__,__LINE__, vec11[2] == ii4 );

	vector<int *>::iterator vbeg = s.scenBegin(is1);
	vector<int *>::iterator vend = s.scenEnd(is1);
	myAssert(__FILE__,__LINE__, *vbeg++ == i1 );
	myAssert(__FILE__,__LINE__, *vbeg++ == i2 );
	myAssert(__FILE__,__LINE__, *vbeg++ == i4 );
	myAssert(__FILE__,__LINE__, vbeg == vend );

	vector<int *>::iterator vbeg1 = s1.scenBegin(is11);
	vector<int *>::iterator vend1 = s1.scenEnd(is11);
	myAssert(__FILE__,__LINE__, *vbeg1++ == ii1 );
	myAssert(__FILE__,__LINE__, *vbeg1++ == ii2 );
	myAssert(__FILE__,__LINE__, *vbeg1++ == ii4 );
	myAssert(__FILE__,__LINE__, vbeg1 == vend1 );


	vector<int *> vec2 = s.getScenario(is2);
	myAssert(__FILE__,__LINE__, vec2[0] == i1 );
	myAssert(__FILE__,__LINE__, vec2[1] == i2 );
	myAssert(__FILE__,__LINE__, vec2[2] == i5 );


	vector<int *> vec12 = s1.getScenario(is12);
	myAssert(__FILE__,__LINE__, vec12[0] == ii1 );
	myAssert(__FILE__,__LINE__, vec12[1] == ii2 );
	myAssert(__FILE__,__LINE__, vec12[2] == ii5 );

	vector<int *> vec3 = s.getScenario(is3);
	myAssert(__FILE__,__LINE__, vec3[0] == i1 );
	myAssert(__FILE__,__LINE__, vec3[1] == i3 );
	myAssert(__FILE__,__LINE__, vec3[2] == i6 );

	vector<int *> vec13 = s1.getScenario(is13);
	myAssert(__FILE__,__LINE__, vec13[0] == ii1 );
	myAssert(__FILE__,__LINE__, vec13[1] == ii3 );
	myAssert(__FILE__,__LINE__, vec13[2] == ii6 );

	vector<int *> vec4 = s.getScenario(is4);
	myAssert(__FILE__,__LINE__, vec4[0] == i1 );
	myAssert(__FILE__,__LINE__, vec4[1] == i3 );
	myAssert(__FILE__,__LINE__, vec4[2] == i7 );


	vector<int *> vec14 = s1.getScenario(is14);
	myAssert(__FILE__,__LINE__, vec14[0] == ii1 );
	myAssert(__FILE__,__LINE__, vec14[1] == ii3 );
	myAssert(__FILE__,__LINE__, vec14[2] == ii7 );

	vector<int *> vec5 = s.getScenario(is5);
	myAssert(__FILE__,__LINE__, vec5[0] == i1 );
	myAssert(__FILE__,__LINE__, vec5[1] == i3 );
	myAssert(__FILE__,__LINE__, vec5[2] == i8 );


	vector<int *> vec15 = s1.getScenario(is15);
	myAssert(__FILE__,__LINE__, vec15[0] == ii1 );
	myAssert(__FILE__,__LINE__, vec15[1] == ii3 );
	myAssert(__FILE__,__LINE__, vec15[2] == ii8 );

	vector<int*>::iterator i;
	i=s.treeBegin();
	myAssert(__FILE__,__LINE__,*i==i1);
	i++;
	myAssert(__FILE__,__LINE__,*i==i2);
	i = s.treeEnd();
	i--;
	myAssert(__FILE__,__LINE__,*i==i8);

	vector<int*>::iterator ii;
	ii=s1.treeBegin();
	myAssert(__FILE__,__LINE__,*ii==ii1);
	ii++;
	myAssert(__FILE__,__LINE__,*ii==ii2);
	ii = s1.treeEnd();
	ii--;
	myAssert(__FILE__,__LINE__,*ii==ii8);

	vbeg = s.scenBegin(is1);
	vend = s.scenEnd(is1);
	myAssert(__FILE__,__LINE__, *vbeg++ == i1 );
	myAssert(__FILE__,__LINE__, *vbeg++ == i2 );
	myAssert(__FILE__,__LINE__, *vbeg++ == i4 );
	myAssert(__FILE__,__LINE__, vbeg == vend );

	vbeg1 = s1.scenBegin(is11);
	vend1 = s1.scenEnd(is11);
	myAssert(__FILE__,__LINE__, *vbeg1++ == ii1 );
	myAssert(__FILE__,__LINE__, *vbeg1++ == ii2 );
	myAssert(__FILE__,__LINE__, *vbeg1++ == ii4 );
	myAssert(__FILE__,__LINE__, vbeg1 == vend1 );



	delete i1;
	delete i2;
	delete i3;
	delete i4;
	delete i5;
	delete i6;
	delete i7;
	delete i8;

	delete ii1;
	delete ii2;
	delete ii3;
	delete ii4;
	delete ii5;
	delete ii6;
	delete ii7;
	delete ii8;
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

	myAssert(__FILE__,__LINE__, n1->depth() == 0 );
	myAssert(__FILE__,__LINE__, n2->depth() == 1 );
	myAssert(__FILE__,__LINE__, n3->depth() == 1 );
	myAssert(__FILE__,__LINE__, n4->depth() == 2 );
	myAssert(__FILE__,__LINE__, n5->depth() == 2 );
	myAssert(__FILE__,__LINE__, n6->depth() == 2 );
	myAssert(__FILE__,__LINE__, n7->depth() == 2 );
	myAssert(__FILE__,__LINE__, n8->depth() == 2 );

	// parents point to last children
	myAssert(__FILE__,__LINE__, n1->getChild() == n3 );
	myAssert(__FILE__,__LINE__, n3->getChild() == n8 );

	// siblings
	myAssert(__FILE__,__LINE__, n3->hasSibling() );
	myAssert(__FILE__,__LINE__, n3->getSibling() == n2 );
	myAssert(__FILE__,__LINE__, !n2->hasSibling() );

	// sibling and cousin pointers are a linked list
	// for same level nodes
	myAssert(__FILE__,__LINE__, n8->hasSibling() );
	myAssert(__FILE__,__LINE__, n8->getSibling() == n7 );
	myAssert(__FILE__,__LINE__, n7->hasSibling() );
	myAssert(__FILE__,__LINE__, n7->getSibling() == n6 );
	myAssert(__FILE__,__LINE__, !n6->hasSibling());

	myAssert(__FILE__,__LINE__, n6->getParent()->getSibling()  == n2
		);
	myAssert(__FILE__,__LINE__, n2->getChild() == n5 );
	myAssert(__FILE__,__LINE__, n5->hasSibling() );
	myAssert(__FILE__,__LINE__, n5->getSibling() == n4 );
	// last element of same level list
	myAssert(__FILE__,__LINE__, !n4->hasSibling());

	vector<SmiTreeNode<int *> *> *vec1 = n1->getChildren();
	assert ((*vec1)[0] == n2 );
	assert ((*vec1)[1] == n3 );
	delete vec1;

	delete i1;
	delete i2;
	delete i3;
	delete i4;
	delete i5;
	delete i6;
	delete i7;
	delete i8;

	delete n1;
}




void SmiScnSmpsIOUnitTestReplace()
{

	std::string dataDir=SMI_TEST_DATA_DIR;
	int nrows, ncols;

	// test SMPS files app0110R
	SmiScnModel smi;
	myAssert(__FILE__,__LINE__,-1!=smi.readSmps((dataDir+"/app0110R").c_str()));

	OsiClpSolverInterface *clp = new OsiClpSolverInterface();
	smi.setOsiSolverHandle(*clp);
	OsiSolverInterface *osiStoch = smi.loadOsiSolverData();

	nrows = osiStoch->getNumRows();
	ncols = osiStoch->getNumCols();
	myAssert(__FILE__,__LINE__,nrows==129);
	myAssert(__FILE__,__LINE__,ncols==268);

	osiStoch->initialSolve();
	myAssert(__FILE__,__LINE__,fabs(osiStoch->getObjValue()-
		44.66666) < 0.0001);
	printf(" *** Successfully tested SMPS interfaces on app0110	with Replace option.\n");

	delete clp;

}
void SmiScnSmpsIOUnitTestAdd()
{

	std::string dataDir=SMI_TEST_DATA_DIR;
	int nrows, ncols;

	// test SMPS files app0110 -- ADD scenario values
	SmiScnModel smi;
	myAssert(__FILE__,__LINE__,-1!=smi.readSmps((dataDir+"/app0110").c_str()));
	OsiClpSolverInterface *clp = new OsiClpSolverInterface();
	smi.setOsiSolverHandle(*clp);
	OsiSolverInterface *osiStoch = smi.loadOsiSolverData();

	nrows = osiStoch->getNumRows();
	ncols = osiStoch->getNumCols();
	myAssert(__FILE__,__LINE__,nrows==129);
	myAssert(__FILE__,__LINE__,ncols==268);

	osiStoch->initialSolve();
	myAssert(__FILE__,__LINE__,fabs(osiStoch->getObjValue()-
		44.66666) < 0.0001);
	printf(" *** Successfully tested SMPS interfaces on app0110	with Add option.\n");

	delete clp;

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
	double *dobj,cdobj[]={ 18.0, 21.0, 18.0, 16.0, 10.0, 15.0, 16.0,
		14.0, 9.0,
		10.0,  9.0,  6.0, 17.0, 16.0, 17.0, 15.0, 10.0, 0.0,
		13.0,  0.0, 13.0,  0.0,  7.0,  0.0,  7.0,  0.0, 1.0 };

	/* Column bounds */
	double *dclo,cdclo[]={ 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
		0.0,  0.0,
		0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
		0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0 };

	double *dcup,cdcup[]={ INF,  INF,  INF,  INF,  INF,  INF,  INF,
		INF,  INF,
		INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,
		INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF };

	/* Row bounds */
	double *drlo,cdrlo[]={ -INF, -INF, -INF, -INF,  0.0, 4.0, 0.0,
		8.0, 10.0 };
	double *drup,cdrup[]={ 10.0, 19.0, 25.0, 15.0,  0.0, 7.0, 0.0,
		8.0, 90.0 };

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

	for (ii=0;ii<nindp;ii++) ns *= nsamp[ii];     /* Compute number of
												  scenarios */
	// initialize SmiModel
	SmiScnModel *smiModel = new SmiScnModel();
	smiModel->setOsiSolverHandle(*osiClp1);

	/* scramble LP entries -- only to test sorting functionality!! */
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
	CoinPackedMatrix *origmat = new CoinPackedMatrix(false,mrow,mcol,
		dels,nels);
	int corenels = nels - 4;


	// set core model using Osi interface
	OsiClpSolverInterface ocsi;
	ocsi.loadProblem(CoinPackedMatrix(
		1,mrow,mcol,dels,corenels),dclo,dcup,dobj,drlo,drup);

	OsiSolverInterface *ohoh= ocsi.clone();

	// test Core Model
	SmiCoreData *osiCore = new SmiCoreData(ohoh,3,cstg,rstg);

	myAssert(__FILE__,__LINE__,osiCore->getNumCols(0) ==
		n_first_stg_cols);
	myAssert(__FILE__,__LINE__,osiCore->getNumCols(1) ==
		n_second_stg_cols);
	myAssert(__FILE__,__LINE__,osiCore->getNumCols(2) == ncol -
		n_first_stg_cols - n_second_stg_cols);

	myAssert(__FILE__,__LINE__,osiCore->getNumRows(0) ==
		n_first_stg_rows);
	myAssert(__FILE__,__LINE__,osiCore->getNumRows(1) ==
		n_second_stg_rows);
	myAssert(__FILE__,__LINE__,osiCore->getNumRows(2) == nrow -
		n_first_stg_rows - n_second_stg_rows);

	myAssert(__FILE__,__LINE__,osiCore->getColStart(0) == 0);
	myAssert(__FILE__,__LINE__,osiCore->getColStart(1) ==
		n_first_stg_cols );
	myAssert(__FILE__,__LINE__,osiCore->getColStart(2) ==
		n_first_stg_cols + n_second_stg_cols);
	myAssert(__FILE__,__LINE__,osiCore->getColStart(3) == ncol);

	myAssert(__FILE__,__LINE__,osiCore->getRowStart(0) == 0);
	myAssert(__FILE__,__LINE__,osiCore->getRowStart(1) ==
		n_first_stg_rows );
	myAssert(__FILE__,__LINE__,osiCore->getRowStart(2) ==
		n_first_stg_rows + n_second_stg_rows);
	myAssert(__FILE__,__LINE__,osiCore->getRowStart(3) == nrow);

	for (ii = 0; ii < n_first_stg_cols ; ii++)
		myAssert(__FILE__,__LINE__,cstg[osiCore->getColExternalIndex(ii)] == 0);
	for (ii = n_first_stg_cols; ii < n_first_stg_cols + n_second_stg_cols ; ii++)
		myAssert(__FILE__,__LINE__,cstg[osiCore->getColExternalIndex(ii)] == 1);
	for (ii = n_first_stg_cols + n_second_stg_cols; ii < ncol ; ii++)
		myAssert(__FILE__,__LINE__,cstg[osiCore->getColExternalIndex(ii)] == 2);

	for (ii = 0; ii < n_first_stg_rows ; ii++)
		myAssert(__FILE__,__LINE__,rstg[osiCore->getRowExternalIndex(ii)] == 0);
	for (ii = n_first_stg_rows; ii < n_first_stg_rows +	n_second_stg_rows ; ii++)
		myAssert(__FILE__,__LINE__,rstg[osiCore->getRowExternalIndex(ii)] == 1);
	for (ii = n_first_stg_rows + n_second_stg_rows; ii < nrow ; ii++)
		myAssert(__FILE__,__LINE__,rstg[osiCore->getRowExternalIndex(ii)] == 2);

	const CoinPackedMatrix *origCore = ohoh->getMatrixByRow();
	int t;
	for ( t=0;t<3;t++)
	{
		SmiNodeData *n=osiCore->getNode(t);
		CoinPackedVector cpvdrlo(n->getRowLowerLength(),n->getRowLowerIndices(),n->getRowLowerElements());
		CoinPackedVector cpvdrup(n->getRowUpperLength(),n->getRowUpperIndices(),n->getRowUpperElements());
		CoinPackedVector cpvdclo(n->getColLowerLength(),n->getColLowerIndices(),n->getColLowerElements());
		CoinPackedVector cpvdcup(n->getColUpperLength(),n->getColUpperIndices(),n->getColUpperElements());
		CoinPackedVector cpvdobj(n->getObjectiveLength(),n->getObjectiveIndices(),n->getObjectiveElements());


		double *core_drlo = new double[osiCore->getNumRows(t)];
		osiCore->getNode(t)->copyRowLower(core_drlo);

		double elt1,elt2;
		int ic;
		for(ii=osiCore->getColStart(t);ii<osiCore->getColStart(t+1);ii++)
		{
			ic = osiCore->getColExternalIndex(ii);
			elt1 = cpvdclo[ii];
			elt2 = dclo[ic];
			myAssert(__FILE__,__LINE__,elt1==elt2);
			elt1 = cpvdcup[ii];
			elt2 = dcup[ic];
			myAssert(__FILE__,__LINE__,elt1==elt2);
			elt1 = cpvdobj[ii];
			elt2 = dobj[ic];
			myAssert(__FILE__,__LINE__,elt1==elt2);
		}
		for(ii=osiCore->getRowStart(t);ii<osiCore->getRowStart(t+1);ii++)
		{
			myAssert(__FILE__,__LINE__,cpvdrlo[ii]==drlo[osiCore->getRowExternalIndex(ii)]);
			myAssert(__FILE__,__LINE__,cpvdrup[ii]==drup[osiCore->getRowExternalIndex(ii)]);
			myAssert(__FILE__,__LINE__,core_drlo[ii-osiCore->getRowStart(t)] ==drlo[osiCore->getRowExternalIndex(ii)]);

			CoinPackedVector row1(n->getRowLength(ii),n->getRowIndices(ii),n->getRowElements(ii));
			const CoinPackedVector row2 =origCore->getVector(osiCore->getRowExternalIndex(ii));
			myAssert(__FILE__,__LINE__,row1.getNumElements() == row2.getNumElements());
			int *indx = row1.getIndices();
			double *els = row1.getElements();
			for (int j=0; j<row1.getNumElements(); j++)
			{
				elt1 = els[j];
				ic = osiCore->getColExternalIndex(indx[j]);
				elt2 = row2[ic];
				myAssert(__FILE__,__LINE__,elt1==elt2);
			}
		}
		delete[] core_drlo;
	}

	delete ohoh; ohoh = NULL;

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


	myAssert(__FILE__,__LINE__,smiModel->getNumScenarios()==1);

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
			myAssert(__FILE__,__LINE__,elt1==elt2);
			elt1 = stochdcup[ii];
			elt2 = dcup[ic];
			myAssert(__FILE__,__LINE__,elt1==elt2);
			elt1 = stochdobj[ii];
			elt2 = dobj[ic];
			myAssert(__FILE__,__LINE__,elt1==elt2);
		}
		int ir;
		for(ii=osiCore->getRowStart(t);ii<osiCore->getRowStart(t+1);ii++)
		{

			ir = osiCore->getRowExternalIndex(ii);

			myAssert(__FILE__,__LINE__,stochdrlo[ii]==drlo[ir]);
			myAssert(__FILE__,__LINE__,stochdrup[ii]==drup[ir]);

			const CoinPackedVector row1 = stochmat->getVector(ii);
			const CoinPackedVector row2 = origmat->getVector(ir);
			myAssert(__FILE__,__LINE__,row1.getNumElements() == row2.getNumElements());
			const int *indx = row1.getIndices();
			const double *els = row1.getElements();
			for (int j=0; j<row1.getNumElements(); j++)
			{
				elt1 = els[j];
				ic = osiCore->getColExternalIndex(indx[j]);
				elt2 = row2[ic];
				myAssert(__FILE__,__LINE__,elt1==elt2);
			}
		}
	}

	printf(" *** Successfully tested problem with scenario %d.\n",is);


	/***** ...main loop to generate scenarios from discrete random
	variables
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
					myAssert(__FILE__,__LINE__,elt1==elt2);
					elt1 = stochdcup[ii-colOff];
					elt2 = dcup[ic];
					myAssert(__FILE__,__LINE__,elt1==elt2);
					elt1 = stochdobj[ii-colOff];
					elt2 = dobj[ic];
					myAssert(__FILE__,__LINE__,fabs(elt1 - (elt2*dp/totalProb)) < 1.0e-8);
				}
				int ir,rowOff;
				rowOff = osiCore->getRowStart(1);
				for(ii=osiCore->getRowStart(t);ii<osiCore->getRowStart(t+1);ii++)
				{

					ir = osiCore->getRowExternalIndex(ii);

					myAssert(__FILE__,__LINE__,stochdrlo[ii-rowOff]==drlo[ir]);
					myAssert(__FILE__,__LINE__,stochdrup[ii-rowOff]==drup[ir]);

					const CoinPackedVector row1 = stochmat->getVector(ii);
					const CoinPackedVector row2 = origmat->getVector(ir);
					myAssert(__FILE__,__LINE__,row1.getNumElements()== row2.getNumElements());
					const int *indx = row1.getIndices();
					const double *els = row1.getElements();
					for (int j=0; j<row1.getNumElements(); j++)
					{
						elt1 = els[j];
						ic = osiCore->getColExternalIndex(indx[j]);
						elt2 = row2[ic];
						myAssert(__FILE__,__LINE__,elt1==elt2);
					}
				}
			}

			nStochCol = smiOsi1->getNumCols();
			nStochRow = smiOsi1->getNumRows();

			printf(" *** Successfully tested problem with scenario %d.\n",is);
		}
	}

	myAssert(__FILE__,__LINE__,ns==smiModel->getNumScenarios());

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
	myAssert(__FILE__,__LINE__,fabs(smiOsi->getObjValue()-1566.042)<0.01);

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
			for(int j=node->getColStart();j<node->getColStart()+node->getNumCols(); ++j)
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


	myAssert(__FILE__,__LINE__,fabs(smiOsi->getObjValue()-objSum) <	0.01);
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

	delete osiClp1;
	delete osiCore;
	delete origmat;
	delete cpm_mat;
}



//forward declarations
void replaceFirstWithSecond(CoinPackedVector &dfirst, const
							CoinPackedVector &dsecond);
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
	double *dobj,cdobj[]={ 18.0, 21.0, 18.0, 16.0, 10.0, 15.0, 16.0,
		14.0, 9.0,
		10.0,  9.0,  6.0, 17.0, 16.0, 17.0, 15.0, 10.0, 0.0,
		13.0,  0.0, 13.0,  0.0,  7.0,  0.0,  7.0,  0.0, 1.0 };

	/* Column bounds */
	double *dclo,cdclo[]={ 0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
		0.0,  0.0,
		0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,
		0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0,  0.0 };


	double *dcup,cdcup[]={ INF,  INF,  INF,  INF,  INF,  INF,  INF,
		INF,  INF,
		INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,
		INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF,  INF };

	/* Row bounds */
	double *drlo,cdrlo[]={ -INF, -INF, -INF, -INF,  0.0, 4.0, 0.0,
		8.0, 10.0 };
	double *drup,cdrup[]={ 10.0, 19.0, 25.0, 15.0,  0.0, 7.0, 0.0,
		8.0, 90.0 };

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
	ocsi.loadProblem(CoinPackedMatrix(1,mrow,mcol,dels,corenels),dclo,dcup,dobj,drlo,drup);

	// core model with 3 stages
	SmiCoreData *smiCore = new SmiCoreData(&ocsi,3,cstg,rstg);

	// Coin structure for scenario "updates" to core matrix
	// ..row-ordered
	CoinPackedMatrix *cpm_mat = new CoinPackedMatrix(false,mrow+corenels,mcol+corenels,dels+corenels,nels-corenels);



	// Create discrete distribution
	SmiDiscreteDistribution *smiDD = new SmiDiscreteDistribution(
		smiCore);

	int index=0;
	for (jj=0;jj<nindp;jj++)
	{
		SmiDiscreteRV *smiRV = new SmiDiscreteRV(1);
		for (ii=0;ii<nsamp[jj];ii++)
		{
			CoinPackedVector empty_vec;
			CoinPackedVector cpv_rlo ;
			CoinPackedVector cpv_rup ;
			cpv_rlo.insert(irow[n_first_stg_rows + jj], demand[index+
				ii]);
			cpv_rup.insert(irow[n_first_stg_rows + jj], demand[index+
				ii]);
			smiRV->addEvent(*cpm_mat,empty_vec,empty_vec,empty_vec,
				cpv_rlo,cpv_rup,dprobs[index+ii]);
			cpv_rlo.clear();
			cpv_rup.clear();
		}
		myAssert(__FILE__,__LINE__,static_cast<int>(smiRV->
			getNumEvents())==nsamp[jj]);
		for (ii=0;ii<nsamp[jj];ii++)
		{
			myAssert(__FILE__,__LINE__,smiRV->getEventColLower(ii).
				getNumElements()==0);
			myAssert(__FILE__,__LINE__,smiRV->getEventColUpper(ii).
				getNumElements()==0);
			myAssert(__FILE__,__LINE__,smiRV->getEventObjective(ii).
				getNumElements()==0);
			myAssert(__FILE__,__LINE__,smiRV->getEventMatrix(ii).
				getNumElements()==cpm_mat->getNumElements());
			myAssert(__FILE__,__LINE__,smiRV->getEventRowLower(ii).
				getElements()[0] == demand[index+ii]);
			myAssert(__FILE__,__LINE__,smiRV->getEventRowLower(ii).
				getIndices()[0] == irow[n_first_stg_rows + jj]);
			myAssert(__FILE__,__LINE__,smiRV->getEventRowUpper(ii).
				getElements()[0] == demand[index+ii]);
			//printf("event prob %g\n",smiRV->getEventProb(ii));
			myAssert(__FILE__,__LINE__,fabs(smiRV->getEventProb(ii) -
				dprobs[index+ii]) < 0.0000001);
		}
		smiDD->addDiscreteRV(smiRV);
		index+=nsamp[jj];
	}

	myAssert(__FILE__,__LINE__,smiDD->getNumRV() == nindp);

	// this is cut-pasted from
	smiModel->processDiscreteDistributionIntoScenarios(smiDD);

	if (0)
	{
		SmiCoreData *core=smiDD->getCore();

		int nindp = smiDD->getNumRV();
		myAssert(__FILE__,__LINE__,nindp > 0);

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
			drlo[irow[n_first_stg_rows + jj]] = demand[ index + indx[jj]
			];
			drup[irow[n_first_stg_rows + jj]] = demand[ index + indx[jj]
			];
			index += nsamp[jj];
			//AJK -debug end


			cpv_dclo.append(smiRV->getEventColLower(indx[jj]));

			cpv_dcup.append(smiRV->getEventColUpper(indx[jj]));

			cpv_dobj.append(smiRV->getEventObjective(indx[jj]));

			cpv_drlo.append(smiRV->getEventRowLower(indx[jj]));

			cpv_drup.append(smiRV->getEventRowUpper(indx[jj]));


			CoinPackedMatrix m = smiRV->getEventMatrix(indx[jj]);
			myAssert(__FILE__,__LINE__,!m.isColOrdered());
			if (matrix.getNumElements())
			{
				for (int i=0; i<m.getNumRows(); ++i)
				{
					CoinPackedVector row=m.getVector(i);
					CoinPackedVector rrow=matrix.getVector(i);
					for (int j=m.getVectorFirst(i); j<m.getVectorLast(j); ++j)
					{

						myAssert(__FILE__,__LINE__,rrow[j] == 0.0);//tests duplicate index
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
		int	is = smiModel->generateScenario(core,&matrix,&cpv_dclo,&
			cpv_dcup,&cpv_dobj,

			&cpv_drlo,&cpv_drup,branch,anc,dp)
			;

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
				myAssert(__FILE__,__LINE__,elt1==elt2);
				elt1 = stochdcup[ii];
				elt2 = dcup[ic];
				myAssert(__FILE__,__LINE__,elt1==elt2);
				elt1 = stochdobj[ii];
				elt2 = dobj[ic];
				myAssert(__FILE__,__LINE__,elt1==elt2);
			}
			int ir;
			for(ii=core->getRowStart(t);ii<core->getRowStart(t+1);ii++)
			{

				ir = core->getRowExternalIndex(ii);

				myAssert(__FILE__,__LINE__,stochdrlo[ii]==drlo[ir]);
				myAssert(__FILE__,__LINE__,stochdrup[ii]==drup[ir]);

				const CoinPackedVector row1 = stochmat->getVector(ii);
				const CoinPackedVector row2 = origmat->getVector(ir);
				myAssert(__FILE__,__LINE__,row1.getNumElements() == row2.
					getNumElements());
				const int *indx = row1.getIndices();
				const double *els = row1.getElements();
				for (int j=0; j<row1.getNumElements(); j++)
				{
					elt1 = els[j];
					ic = core->getColExternalIndex(indx[j]);
					elt2 = row2[ic];
					myAssert(__FILE__,__LINE__,elt1==elt2);
				}
			}
		}

		printf(" *** Successfully tested problem with scenario %d.\n",is);



		SmiTreeNode<SmiScnNode *> *root = smiModel->smiTree_.getRoot();
		smiModel->smiTree_.setChildLabels(root,label);

		/* sample space increment initialized to 1 */
		int *incr = (int *) malloc( nindp*sizeof(int) );
		for (jj=0;jj<nindp;jj++) incr[jj] = 1;

		/***** ...main loop to generate scenarios from discrete random
		variables
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

				label[iStage] *= nEvents;
				label[iStage] += indx[jjj];

				drlo[irow[n_first_stg_rows + jjj]] = demand[ indx[jjj]+
					index ];
				drup[irow[n_first_stg_rows + jjj]] = demand[ indx[jjj]+
					index ];
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
			myAssert(__FILE__,__LINE__,!m.isColOrdered());
			if (matrix.getNumElements())
			{
				for (int i=0; i<m.getNumRows(); ++i)
				{
					CoinPackedVector row=m.getVector(i);
					CoinPackedVector rrow=matrix.getVector(i);
					for (int j=m.getVectorFirst(i); j<m.getVectorLast(j);
						++j)
					{
						matrix.modifyCoefficient(i,j,row[j],true);
					}
				}
			}
			else
				matrix = m;

			// find ancestor node
			SmiTreeNode<SmiScnNode *> *tnode = smiModel->smiTree_.find(
				label);

			// add scenario
			anc = tnode->scenario();
			myAssert(__FILE__,__LINE__,anc==0);
			branch = tnode->depth();
			myAssert(__FILE__,__LINE__,branch==1);
			is = smiModel->generateScenario(core,&matrix,&cpv_dclo,&
				cpv_dcup,&cpv_dobj,
				&cpv_drlo,&cpv_drup,branch,anc,dp)
				;

			smiModel->smiTree_.setChildLabels(tnode,label);

			if (is < 3)
			{
				// test scenario

				// load problem data into OsiSolver
				smiModel->loadOsiSolverData();
				// get Osi pointer
				OsiSolverInterface *smiOsi1 = smiModel->
					getOsiSolverInterface();

				totalProb += dp;

				// get arrays
				stochdrlo = smiOsi1->getRowLower()+nStochRow;
				stochdrup = smiOsi1->getRowUpper()+nStochRow;
				stochdclo = smiOsi1->getColLower()+nStochCol;
				stochdcup = smiOsi1->getColUpper()+nStochCol;
				stochdobj = smiOsi1->getObjCoefficients()+nStochCol;

				// get matrix
				const CoinPackedMatrix *stochmat = smiOsi1->getMatrixByRow
					();
				int t;
				for (t=1;t<3;t++)
				{
					double elt1,elt2;
					int ic;
					int colOff = core->getColStart(1);
					for(ii=core->getColStart(t);ii<core->getColStart(t+1);
						ii++)
					{
						ic = core->getColExternalIndex(ii);
						elt1 = stochdclo[ii-colOff];
						elt2 = dclo[ic];
						myAssert(__FILE__,__LINE__,elt1==elt2);
						elt1 = stochdcup[ii-colOff];
						elt2 = dcup[ic];
						myAssert(__FILE__,__LINE__,elt1==elt2);
						elt1 = stochdobj[ii-colOff];
						elt2 = dobj[ic];
						myAssert(__FILE__,__LINE__,fabs(elt1 - (elt2*dp/
							totalProb)) < 1.0e-8);
					}
					int ir,rowOff;
					rowOff = core->getRowStart(1);
					for(ii=core->getRowStart(t);ii<core->getRowStart(t+1);
						ii++)
					{

						ir = core->getRowExternalIndex(ii);

						myAssert(__FILE__,__LINE__,stochdrlo[ii-rowOff]==
							drlo[ir]);
						myAssert(__FILE__,__LINE__,stochdrup[ii-rowOff]==
							drup[ir]);

						const CoinPackedVector row1 = stochmat->getVector(
							ii);
						const CoinPackedVector row2 = origmat->getVector(
							ir);
						myAssert(__FILE__,__LINE__,row1.getNumElements()
							== row2.getNumElements());
						const int *indx = row1.getIndices();
						const double *els = row1.getElements();
						for (int j=0; j<row1.getNumElements(); j++)
						{
							elt1 = els[j];
							ic = core->getColExternalIndex(indx[j]);
							elt2 = row2[ic];
							myAssert(__FILE__,__LINE__,elt1==elt2);
						}
					}
				}

				nStochCol = smiOsi1->getNumCols();
				nStochRow = smiOsi1->getNumRows();

				printf(" *** Successfully tested problem with scenario %d.\n",is);
			}

		}

		free( incr) ;
		free( drlo) ;
		free( drup) ;
		free( rstg) ;
		free( dclo) ;
		free( dcup) ;
		free( cstg) ;

	} else { // if (0)
		free( drlo) ;
		free( drup) ;
		free( rstg) ;
		free( dclo) ;
		free( dcup) ;
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
	myAssert(__FILE__,__LINE__,fabs(smiOsi->getObjValue()-1566.042)<
		0.01);

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
			for(int j=node->getColStart();
				j<node->getColStart()+node->getNumCols(); ++j)
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
	myAssert(__FILE__,__LINE__,fabs(smiOsi->getObjValue()-objSum) <	0.01);

	printf(" *** Completed all tests for SmiScnModel.\n");
	delete smiModel;
	delete osiClp1;
	delete smiCore;
	delete cpm_mat;
	delete smiDD;
	delete origmat;

	free( dobj) ;
	free( mrow) ;
	free( mcol) ;
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

	osi.loadProblem(nCoreCols,
		nCoreRows,iCoreColStarts,iCoreRowIndice,dCoreMatEntries,dCoreClo,
		dCoreCup,dCoreObj,
		dCoreRlo,dCoreRup);
	SmiCoreData *osiCore = new
		SmiCoreData(&osi,nCoreStages,iColStages,iRowStages);

	// initialize SmiScnModel
	SmiScnModel *smiModel = new SmiScnModel();

	// Add Scenarios
	int	is = smiModel->generateScenario(osiCore,NULL,NULL,NULL,NULL,
		rlo0,NULL,iBranchStage[0],
		iAncestorScn[0],dProbScn[0]);

	is = smiModel->generateScenario(osiCore,NULL,NULL,NULL,NULL,
		rlo1,NULL,iBranchStage[1],
		iAncestorScn[1],dProbScn[1]);

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

	myAssert(__FILE__,__LINE__,osiStoch->getObjValue()== 0.5);

	delete smiModel;
	//delete osiCore;

	delete rlo0;
	delete rlo1;
}

void SmpsBug()
{

	SmiScnModel smi;

	std::string dataDir=SMI_TEST_DATA_DIR;
	dataDir += "bug";

	// read SMPS model from files
	// <name>.core, <name>.time, and <name>.stoch
	// the argument myCombineRule overrides the combine rule specified in the Stoch file
	myAssert(__FILE__,__LINE__,-1!=smi.readSmps(dataDir.c_str()));

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
	myAssert(__FILE__,__LINE__,osiStoch->getObjValue()== 0.5);

	delete clp;
}



int main()
{

	//testingMessage( "Testing SmiTreeNode \n");
	//SmiTreeNodeUnitTest();

	//testingMessage( "Testing SmiScenarioTree\n" );
	//SmiScenarioTreeUnitTest();

	//testingMessage( "Testing SmiScnSmpsIO Replace\n" );
	////SmiScnSmpsIOUnitTestReplace();

	//testingMessage( "Testing SmiScnSmpsIO Add\n" );
	////SmiScnSmpsIOUnitTestAdd();

	//testingMessage( "Testing base data structures for SmiScnModel\n");
 //   //SmiScnModelScenarioUnitTest();

	//testingMessage( "Testing SmiScnModel Discrete Distribution\n" );
	////SmiScnModelDiscreteUnitTest();

	//testingMessage("Model generation for simple model Bug");
	ModelBug();

	testingMessage("Read SMPS version of simple model Bug");
	SmpsBug();

	//testingMessage("Unit test for decomposition.");
	//DecompUnitTest();

	SmiCoreCombineReplace::ClearInstance();
	SmiCoreCombineAdd::ClearInstance();

    testingMessage( "*** Done! *** \n");


	return 0;
}

