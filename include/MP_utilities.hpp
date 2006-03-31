// ******************** flopc++ **********************************************
// File: MP_utilities.hpp
// $Id$
// Author: Tim Helge Hultberg (thh@mat.ua.pt)
// Copyright (C) 2003 Tim Helge Hultberg
// All Rights Reserved.
//****************************************************************************

#ifndef _MP_utilities_hpp_
#define _MP_utilities_hpp_

#include <string>
using std::string;

#include <vector>
using std::vector;

namespace flopc {

    class Functor {
    public:
	virtual void operator()() const = 0;
    protected:
	Functor() {}
	virtual ~Functor() {}
    private:
	Functor(const Functor&);
	Functor& operator=(const Functor&);
    };	

    template<int nbr, class T>
    vector<T> makeVector(T i1, T i2=0, T i3=0, T i4=0, T i5=0) {
	vector<T> S(nbr);
	S[0] = i1; 
	if (nbr==1) return S;
	S[1] = i2;
	if (nbr==2) return S;
	S[2] = i3;
	if (nbr==3) return S;
	S[3] = i4;
	if (nbr==4) return S;
	S[4] = i5; 
	return S;
    }

    inline int mod(int a, int b) {
	int t = a % b;
	return  (t>=0) ? t : t+b;
    }

    const int outOfBound = -2;    

    class RowMajor {
    public:
	int size() const { return size_; }
    protected:
	RowMajor(int s1, int s2, int s3, int s4, int s5) :
	    size1(s1), size2(s2), size3(s3), size4(s4), size5(s5),
	    size_(s1*s2*s3*s4*s5) {}
	int f(int i1=0, int i2=0, int i3=0, int i4=0, int i5=0) const {
	    if ( i1==outOfBound || i2==outOfBound || i3==outOfBound ||
		 i4==outOfBound || i5==outOfBound ) {
		return outOfBound;
	    } else {
		int i = i1;
		i *= size2; i += i2;  i *= size3; i += i3;
		i *= size4; i += i4;  i *= size5; i += i5;
		return i;
	    } 
	}
	int size1,size2,size3,size4,size5,size_;
    };

    class Named {
    public:
	string getName() const { return name; }
	void setName(const string& n) { name = n; }
    private:
	string name;
    };

//     template<class T> class Handle {
//     public:
// 	T operator->() const {
// 	    return root;
// 	}
//       // protected:
// 	Handle(T r) : root(r) {
// 	  if (root!=0) {
// 	    root->count++;
// 	  }
// 	}
// 	Handle(const Handle& h) : root(h.root) {
// 	  if (root!=0) {
// 	    root->count++;
// 	  }
// 	}
// 	const Handle& operator=(const Handle& h) {
// 	  if (h.root!=0) {
// 	    h.root->count++;
// 	  }
// 	  if (--(root->count) == 0) {
// 	    delete root;
// 	  }
// 	  root = h.root;
// 	  return *this;
// 	}

// 	~Handle() {
// 	    if (--(root->count) == 0) {
// 		delete root;
// 	    }
// 	}
// 	T root;
//     };

    template<class T> class Handle {
    public:
	T operator->() const {
	    return root;
	}
    public:
      Handle(T r) : root(r) {
	if (root!=0) {
	  root->count++;
	}
      }
      Handle(const Handle& h) : root(h.root) {
	if (root!=0) {
	  root->count++;
	}
      }
      const Handle& operator=(const Handle& h) {
	if (root!=h.root) {
	  if ((root!=0) && (--(root->count)==0)) {
	    delete root;
	  }
	  root = h.root;
	  if (root!=0) {
	    root->count++;
	  }
	}
	return *this;
      }
      virtual ~Handle() {
	if ((root!=0) && (--(root->count)==0)) {
	  delete root;
	}
      }
      T root;
    };

} // End of namespace flopc
#endif
