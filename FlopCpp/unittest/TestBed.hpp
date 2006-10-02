#ifndef _TESTBED_HPP_
#define _TESTBED_HPP_

#include <iostream>
#include <set>

#include "TestItem.hpp"

template <class T>
struct PLess : std::binary_function<T, T, bool> {
    bool operator()(const T& x, const T& y) const { return *x < *y; }
};

class TestBed : public std::set < TestItem * , PLess< TestItem * > > {
public:
	TestBed(){ std::cout<<"Test begin"<<std::endl;}
	~TestBed(){ std::cout<<"Test end\nresults:\n"<<*this<<std::endl;}
  	friend std::ostream &operator<<(std::ostream &os, const TestBed &bed);
};
#endif
