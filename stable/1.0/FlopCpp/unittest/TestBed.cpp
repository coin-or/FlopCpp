#include "TestBed.hpp"
#include <iostream>

std::ostream &operator<<(std::ostream &os, const TestBed &testBed) {
    int passed=0,failed=0;
    os<<"TestBed Detail:\n--------------\n";
    for(TestBed::const_iterator itemIt = testBed.begin();
        itemIt != testBed.end(); itemIt++) {
        os<<*(*itemIt)<<'\n';
        if((*itemIt)->passed())
            passed++;
        else
            failed++;
    }
    os<<"\nSummary:\n-----------\n"<<passed<<" Passed\n";
    os<<failed<<" Failed\n----------\n"<<passed+failed<<" Tested\n";
    if(failed)
	os<<"FAILED";
    else
	os<<"PASSED";
    return os;
}
