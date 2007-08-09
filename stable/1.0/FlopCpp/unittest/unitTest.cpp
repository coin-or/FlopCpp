#include "unitTest.hpp"
#include <iostream>

#include "TestBed.hpp"
#include "TestItem.hpp"

int main() {
    {
        TestBed mainBed;
        TestItem::setBed(&mainBed);
        
        bool bSuccess=true;
        bSuccess = indexTest();
        bSuccess = bSuccess && setTest();
        bSuccess = bSuccess && constantTest();
        bSuccess = bSuccess && booleanTest();

        /* alternative if you don't like the TestItem TestBed arrangement.
        if(bSuccess)
        std::cout<<"PASSED"<<std::endl;
        else
        std::cout<<"FAILED!"<<std::endl;
        */
    }
    return 0;
}
