#ifndef _huang_hpp_
#define _huang_hpp_

#include "flopc.hpp"
using namespace flopc;

class model {
public:

  MP_set S1, S2; 
  MP_variable * Y;
  MP_model * mm; 

  model(int numS1TimeRange,
	int numS2
	);

  ~model();

  void f1();
  void f2();
}; 
#endif
