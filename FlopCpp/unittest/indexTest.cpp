#include "MP_index.hpp"
#include <iostream>
#include "MP_domain.hpp"
#include "MP_set.hpp"
#include "TestItem.hpp"

using namespace flopc;
bool indexTest() {
    TestItem *ti = new TestItem("flopc:indexTest");
    MP_index idx;
    if(idx.isInstantiated())
    {ti->failItem(__SPOT__); return false;}
    idx.instantiate();
    if(!idx.isInstantiated())
    {ti->failItem(__SPOT__); return false;}
    idx.unInstantiate();
    if(idx.isInstantiated())
    {ti->failItem(__SPOT__); return false;}
    idx.assign(3);
    if(idx.evaluate()!=3)
    {ti->failItem(__SPOT__); return false;}

    MP_index *pIdx = idx.getIndex();
    if(pIdx!=&idx)
    { ti->failItem(__SPOT__); return false;}
    
    MP_index &any=MP_index::Any;
    MP_index &any2=MP_index::Any;
    if(&any!=&any2)
    {ti->failItem(__SPOT__);  return false; }

    MP_index &empty=MP_index::getEmpty();
    MP_index &empty2=MP_index::getEmpty();
    if(&empty!=&empty2)
    { ti->failItem(__SPOT__); return false; }
    
    // test constant construction through add/subtr indices.
    Constant constant1 = idx+5;
    std::cout<<"How tell if this is successful?"<<std::endl;
    Constant constant2 = idx-5;
    std::cout<<"How tell if this is successful?"<<std::endl;

    MP_set mySet(3);
    MP_domain dom1 = idx.getDomain(&mySet);
    if(dom1.size()!=3)
    {ti->failItem(__SPOT__);  return false;}

    std::cout<<"Is there a way to test the correctness of an MP_index_exp directly?"<<std::endl;
    // construction by addition to a Constant object.
    Constant myConstant(3);
    MP_index_exp ie1=idx+myConstant;
    // construction of an index expression through addition to an integer.
    MP_index_exp ie2 = idx+3;
    // construction of an index expression through subtraction of an integer.
    MP_index_exp ie3 = idx-1;
    // construction of an index expression through subtraction of a Constant.
    ///NB MP_index_exp ie4 = idx-myConstant;
    // construction of an index expression through subtraction of a Constant.
    MP_index_exp ie5 = idx*5;
    // construction of an index expression from an integer.
    MP_index_exp ie6(3);
    // construction of an index expression from a Constant.
    MP_index_exp ie7(Constant(4));
    // construction of an index expression from an index.
    MP_index_exp ie8(idx);
    // construction of an index expression from an index.
    MP_index_exp ie9(ie8);
    ti->passItem();
    return true;
}
