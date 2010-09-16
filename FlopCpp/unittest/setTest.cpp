#include "MP_set.hpp"
#include "TestItem.hpp"

using namespace flopc;

class MyBooleanTest: public MP_boolean {
public:
    MyBooleanTest(MP_set &s)
        :setRef(s) { }
    virtual bool evaluate() const {
        return setRef.size()==2;
    }
private:
    MP_set &setRef;
};

bool setTest() {
    TestItem *ti = new TestItem("flopc:setTest");
    // test construction.
    MP_set set1;
    MP_set set2(2);
    MP_set set3(3);
    if(set1.size()!=0)
    {ti->failItem(__SPOT__); return false; }
    if(set2.size()!=2)
    {ti->failItem(__SPOT__); return false; }

    // conversion to a domain
    MP_domain d = set1;
    if(d.size()!=0)
    {ti->failItem(__SPOT__); return false; }
    // convert to a domain but only a particular index setting.
    MP_domain d2 = set2(2);
    if(d2.size()!=1)
    {ti->failItem(__SPOT__); return false; }
    MP_index i;
    // convert to a domain with a "such_that" clause.
    MP_domain d3 = set2.such_that(i<=3);
    if(d3.size()!=2) {
        ti->failItem(__SPOT__); 
        return false;
    }
    
    MP_set setCyclic(7); // kinda like week days.
    if(setCyclic.size()!=7)
    {ti->failItem(__SPOT__); return false; }
    enum { A,B,C,ABCSIZE } ;

    MP_set setFromEnum(ABCSIZE);
    if(setFromEnum.size()!=3)
    {ti->failItem(__SPOT__); return false; }
    ti->passItem();
    return true;
}
