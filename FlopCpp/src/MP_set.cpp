// ******************** FlopCpp **********************************************
// File: MP_set.cpp
//****************************************************************************

#include "MP_set.hpp"
#include "MP_model.hpp"
#include <sstream>
#include <iostream>
using namespace flopc;

using std::cout;
using std::endl;

namespace flopc {

	//See item 26 More Effective C++
	MP_set &MP_set::getEmpty() {
	  //Make it real static and const (Singleton)
	  static MP_set Empty(1);
	  return Empty; // Does this work?
	 }
  

  MP_stage::MP_stage( int i /*= 0*/ ) : MP_set(i)
	  {
	  //MP_model contains a MP_stage and a MP_scenario_set set, initalized to 0.
	  //If a new one is created we only need to add it to the current model
	  
	  //During first initialization, currentModel pointer is zero. Under this circumstances,
	  //stage gets initalized with 0 by MP_model constructor and belongs to the current model
      this->setName(MP_model::stageString); //Assign stageString to MP_stage set to identify it later on.
	  if (MP_model::getCurrentModel() != 0)
		MP_model::getCurrentModel()->setStage(*this);
	  }

  MP_stage::~MP_stage() { }

  MP_scenario_set::MP_scenario_set( int i /*= 0*/ ):MP_set(i)
	  {
	  //During first initialization, currentModel pointer is zero. Under this circumstances,
	  //stage gets initalized with 0 by MP_model constructor and belongs to the current model
	  if (MP_model::getCurrentModel() != 0)
		  MP_model::getCurrentModel()->setScenSet(*this);
	  }

    MP_scenario_set::~MP_scenario_set() { }

  flopc::MP_domain SUBSETREF::getDomain( MP_set* s ) const
  {
      return MP_domain::getEmpty();
  }

  int SUBSETREF::evaluate() const
  {
      return 0;
  }

  MP_index* SUBSETREF::getIndex() const
  {
      return 0;
  }

  MP_index_base* SUBSETREF::insertIndexExpr( const MP_index_exp& expr)
  {
      throw not_implemented_error();
      return getIndex(); //TODO: We have to redo this eventually?
  }

  MP_index_base* SUBSETREF::deepCopy() const
  {
      return getIndex();
  }

}
