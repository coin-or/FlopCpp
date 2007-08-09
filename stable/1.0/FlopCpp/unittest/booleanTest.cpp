#include "TestItem.hpp"
#include "MP_index.hpp"
#include "MP_set.hpp"
#include "MP_boolean.hpp"
#include "MP_constant.hpp"

bool booleanTest()
{
    TestItem *ti = new TestItem("flopc:booleanTest");
    
    flopc::MP_set mySet(3);
    flopc::MP_index i;
    // test alltrue.  It's always true.  even if the expression is not.
    if(alltrue(mySet(i), i>(-1))->evaluate()!=true)
    {ti->failItem(__SPOT__);return false;}
    if(alltrue(mySet(i), i>(0))->evaluate()!=true)
    {ti->failItem(__SPOT__);return false;}

    // test simple construction from a constant and operator&&
    flopc::MP_boolean b1(flopc::Constant(3)<=flopc::Constant(4));
    flopc::MP_boolean b2(flopc::Constant(4)>=flopc::Constant(3));
    if((b1&&b2)->evaluate()!=true)
    {ti->failItem(__SPOT__);return false;}

    // test simple construction from a constant and operator||
    flopc::MP_boolean b11(flopc::Constant(3)<=flopc::Constant(4));
    flopc::MP_boolean b12(flopc::Constant(4)<=flopc::Constant(3));
    if((b11||b12)->evaluate()!=true)
    {ti->failItem(__SPOT__);return false;}

    // test operator<
    flopc::MP_boolean b3(flopc::Constant(3)<flopc::Constant(3));
    if(b3->evaluate()==true)
    {ti->failItem(__SPOT__);return false;}

    flopc::MP_boolean b4(flopc::Constant(2)<flopc::Constant(3));
    if(b4->evaluate()!=true)
    {ti->failItem(__SPOT__);return false;}

    // test operator>
    flopc::MP_boolean b5(flopc::Constant(3)>flopc::Constant(3));
    if(b5->evaluate()==true)
    {ti->failItem(__SPOT__);return false;}

    flopc::MP_boolean b6(flopc::Constant(3)>flopc::Constant(2));
    if(b6->evaluate()!=true)
    {ti->failItem(__SPOT__);return false;}

    // test operator==
    flopc::MP_boolean b7(flopc::Constant(3)==flopc::Constant(3));
    if(b7->evaluate()!=true)
    {ti->failItem(__SPOT__);return false;}

    flopc::MP_boolean b8(flopc::Constant(3)==flopc::Constant(2));
    if(b8->evaluate()==true)
    {ti->failItem(__SPOT__);return false;}

    // test operator!=
    flopc::MP_boolean b9(flopc::Constant(3)!=flopc::Constant(3));
    if(b9->evaluate()==true)
    {ti->failItem(__SPOT__);return false;}

    flopc::MP_boolean b10(flopc::Constant(3)!=flopc::Constant(2));
    if(b10->evaluate()!=true)
    {ti->failItem(__SPOT__);return false;}

    // test direct construction
    if(flopc::MP_boolean(true)->evaluate()!=true)
    {ti->failItem(__SPOT__);return false;}
    if(flopc::MP_boolean(false)->evaluate()!=false)
    {ti->failItem(__SPOT__);return false;}

    // test construction from a Constant .
    flopc::MP_boolean b14(flopc::Constant(3));
    if(b14->evaluate()!=true)
    {ti->failItem(__SPOT__);return false;}

    if(flopc::MP_boolean(flopc::Constant(0))->evaluate()!=false)
    {ti->failItem(__SPOT__);return false;}



    /** @TODO finish out these tests.
    MP_boolean operator<=(const MP_index_exp& e1, const MP_index_exp& e2) {
	return new Boolean_lessEq(e1, e2);
    } 
    MP_boolean operator<(const MP_index_exp& e1, const MP_index_exp& e2) {
	return new Boolean_less(e1, e2);
    }
    MP_boolean operator>=(const MP_index_exp& e1, const MP_index_exp& e2) {
	return new Boolean_greaterEq(e1, e2);
    }
    MP_boolean operator>(const MP_index_exp& e1, const MP_index_exp& e2) {
	return new Boolean_greater(e1, e2);
    }
    MP_boolean operator==(const MP_index_exp& e1, const MP_index_exp& e2) {
	return new Boolean_equal(e1, e2);
    }
    MP_boolean operator!=(const MP_index_exp& e1, const MP_index_exp& e2) {
	return new Boolean_not_equal(e1, e2);
    }

    MP_boolean::MP_boolean(SUBSETREF& c) : 
        Handle<Boolean_base*>(new Boolean_SUBSETREF(c)) {}

    */
    ti->passItem();
    return true;
}
