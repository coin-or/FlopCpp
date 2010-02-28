// ******************** FlopCpp **********************************************
// File: MP_set.cpp
//****************************************************************************

#include "MP_set.hpp"
#include "MP_model.hpp"
#include <sstream>
using namespace flopc;

namespace flopc {
  MP_set &MP_set::getEmpty() {
    static MP_set Empty(1);
    return Empty;
  }
  
}
