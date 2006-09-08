#ifndef TESTBED_H
#define TESTBED_H

#include <iostream>
#include <set>
#include "PLess.h"
#include <TestItem.h>

class TestBed : public std::set < TestItem * , PLess< TestItem * > >
{
public:
	TestBed(){ std::cout<<"Test begin"<<std::endl;}
	~TestBed(){ std::cout<<"Test end\nresults:\n"<<*this<<std::endl;}
  	friend std::ostream &operator<<(std::ostream &os, const TestBed &bed);
};
#endif
