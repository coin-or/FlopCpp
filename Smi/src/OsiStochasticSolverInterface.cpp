#include "OsiSolverInterface.hpp"
#include "SmiScnModel.hpp"

#include "OsiStochasticSolverInterface.hpp"




OsiStochasticSolverInterface::OsiStochasticSolverInterface( SmiScnModel* smiModel ): smiModel(smiModel) {}

OsiStochasticSolverInterface::~OsiStochasticSolverInterface(){
    //if (smiModel != 0){
    //    delete smiModel;
    //    smiModel = 0;
    //}
}

void OsiStochasticSolverInterface::initialSolve()
{//Default implementation: Solve deterministic equivalent

}

OsiStochasticDetEqSolverInterface::OsiStochasticDetEqSolverInterface( SmiScnModel* smiModel )
: OsiStochasticSolverInterface(smiModel)
{

}

OsiStochasticDetEqSolverInterface::~OsiStochasticDetEqSolverInterface()
{

}

void OsiStochasticDetEqSolverInterface::initialSolve()
{
    OsiSolverInterface* solver = smiModel->loadOsiSolverData();
    solver->initialSolve();
}

