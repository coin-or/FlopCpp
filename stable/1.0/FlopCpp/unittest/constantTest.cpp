#include "unitTest.hpp"
#include "MP_constant.hpp"
#include "MP_index.hpp"
#include "MP_set.hpp"
#include "MP_data.hpp"
#include "TestItem.hpp"
#include <iostream>

bool constantTest() {
    TestItem *ti = new TestItem("flopc:constantTest");
    // test absolute value.
    if(flopc::abs(-3)->evaluate()!=3.0)
    {ti->failItem(__SPOT__); return false;}
    if(flopc::abs(3)->evaluate()!=3.0)
    {ti->failItem(__SPOT__); return false;}

    // test positive values (or zero)
    if(flopc::pos(3)->evaluate()!=3.0)
    {ti->failItem(__SPOT__); return false;}

    // test ceil
    if(flopc::ceil(2.9)->evaluate()!=3.0)
    {ti->failItem(__SPOT__); return false;}

    if(flopc::ceil(3.1)->evaluate()!=4.0)
    {ti->failItem(__SPOT__); return false;}
    
    // test floor
    if(flopc::floor(2.9)->evaluate()!=2.0)
    {ti->failItem(__SPOT__); return false;}

    if(flopc::floor(3.1)->evaluate()!=3.0)
    {ti->failItem(__SPOT__); return false;}

    // test minimum
    if(flopc::minimum(flopc::ceil(3.1),flopc::floor(5.1))->evaluate()!=4.0)
    {ti->failItem(__SPOT__); return false;}

    if(flopc::minimum(flopc::floor(5.1),flopc::ceil(3.1))->evaluate()!=4.0)
    {ti->failItem(__SPOT__); return false;}

    // test maximum
    if(flopc::maximum(flopc::ceil(3.1),flopc::floor(5.1))->evaluate()!=5.0)
    {ti->failItem(__SPOT__); return false;}

    if(flopc::maximum(flopc::floor(5.1),flopc::ceil(3.1))->evaluate()!=5.0)
    {ti->failItem(__SPOT__); return false;}

    // operator+
    if((flopc::floor(3.3)+flopc::Constant(3))->evaluate()!=6.0)
    {ti->failItem(__SPOT__); return false;}

    // operator- (with constants)
    if((flopc::floor(4.3)-flopc::Constant(3))->evaluate()!=1.0)
    {ti->failItem(__SPOT__); return false;}

    // operator- (with index values)
    flopc::MP_index i;
    flopc::MP_index j;
    std::cout<<"This isn't really sufficient testing against zero..but it's what I can do."<<std::endl;
    if((i-j)->evaluate()!=0.0)
    {ti->failItem(__SPOT__); return false;}

    // operator* (with constants)
    if((flopc::floor(4.3)*flopc::Constant(3))->evaluate()!=12.0)
    {ti->failItem(__SPOT__); return false;}

    // operator/ (with constants)
    if((flopc::floor(4.3)/flopc::Constant(3))->evaluate()!=4.0/3.0)
    {ti->failItem(__SPOT__); return false;}

    // maximum over a set.  In this case, the set is {0,1,2}, so i+1 = {1,2,3}, so the max is 3.
    // also testing construction of a constant from an index expression.
    flopc::MP_set mySet(3);
    if((flopc::maximum(mySet(i),flopc::abs(i+1)))->evaluate()!=3.0)
    {ti->failItem(__SPOT__); return false;}

    // minimum over a set.  In this case, the set is {0,1,2}, so i+1 = {1,2,3}, so the min is 1.
    // also testing construction of a constant from an index expression.
    if((flopc::minimum(mySet(i),flopc::abs(i+1)))->evaluate()!=1.0)
    {ti->failItem(__SPOT__); return false;}

    // sum over a set.  In this case, the set is {0,1,2}, so i+1 = {1,2,3}, so the sum is 6.
    // also testing construction of a constant from an index expression.
    if((flopc::sum(mySet(i),flopc::abs(i+1)))->evaluate()!=6.0)
    {ti->failItem(__SPOT__); return false;}

    // product over a set.  In this case, the set is {0,1,2}, so i+1 = {1,2,3}, so the product is 6.
    // also testing construction of a constant from an index expression.
    if((flopc::product(mySet(i),flopc::abs(i+1)))->evaluate()!=6.0)
    {ti->failItem(__SPOT__); return false;}

    // test construction using a DataRef;
    flopc::MP_data myData(mySet);
    double d[] = { 1.0,2.0,3.0};
    myData.value(d);
    if(flopc::abs(myData(1))->evaluate()!=2.0)
    {ti->failItem(__SPOT__); return false;}

    ti->passItem();
    return true;
}
