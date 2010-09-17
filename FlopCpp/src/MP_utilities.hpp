// ******************** FlopCpp **********************************************
// File: MP_utilities.hpp
// ****************************************************************************

#ifndef _MP_utilities_hpp_
#define _MP_utilities_hpp_

#include <cassert>
#include <string>
#include <ctime>
using std::string;

#include <vector>
using std::vector;

// TODO: Remove these headers together with the static int in the Handle class
//#include <iostream>
//#include <typeinfo>
//

#include <boost/exception/all.hpp>
#include <glog/logging.h>


namespace flopc {

    //Define Exception hierarchy
    //TODO: Remove eventually..
    struct exception_base: virtual std::exception, virtual boost::exception { };
    struct io_error: virtual exception_base { };
    struct not_implemented_error: virtual exception_base { };
    struct invalid_argument_exception: virtual exception_base { };
    struct not_initialized_exception: virtual exception_base { };

    //These values are default values for the comparison function compareDouble and can be changed if needed.
    static double absoluteTolerance = 0.0001;
    static double relativeTolerance = 0.0001;

    bool compareDouble(double x, double y, double absoluteTolerance = flopc::absoluteTolerance, double relativeTolerance = flopc::relativeTolerance);
    int round(double x);

    // Returns time in miliseconds between two clock events.
    double diffclock(clock_t clock1,clock_t clock2);


    /** @file This file contains several different useful utilities which are
    used to simplify use of FlopC++.
    @ingroup PublicInterface
    */

    /** @brief Function object.  Often used
    @ingroup INTERNAL_USE
    @note is the base class for passing a function object around.
    */
    class Functor {
    public:
        virtual void operator()() const = 0;
    
        Functor() {}
        virtual ~Functor() {}
    protected:
        Functor(const Functor&); //copy constructor
    private:
        //disable assignment operator
        Functor& operator=(const Functor&);
    };    

    /** This template makes a vector of appropriate size out of the
    variable number of arguments.
    @ingroup INTERNAL_USE
    */
    template<int nbr, class T>
    vector<T> makeVector(T i1, T i2=0, T i3=0, T i4=0, T i5=0) {
        assert(nbr<6);
        vector<T> S;
        S.reserve(nbr);
        S.push_back(i1); 
        if (nbr==1) return S;
        S.push_back(i2);
        if (nbr==2) return S;
        S.push_back(i3);
        if (nbr==3) return S;
        S.push_back(i4);
        if (nbr==4) return S;
        S.push_back(i5);
        return S;
    }

    /// return the strictly positive modulus of two integers
    inline int mod(int a, int b) {
        int t = a % b;
        return  (t>=0) ? t : t+b;
    }

    /// Distinct return value on conditions where an index goes out of bounds.
    const int getZero = -5;
    const int outOfBound = -4;    
    const int lowerBound = -3;   
    const int upperBound = -2;   

    struct IndexKeeper {
        int i1;
        int i2;
        int i3;
        int i4;
        int i5;
    };

    class MP_modelTest;

    /** Utility class to flatten multidimensional information into single
    dimentional offset information.
    @ingroup INTERNAL_USE
    */
    class RowMajor {
        friend class MP_modelTest;
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
             IndexKeeper getIndices(int k) {
                 // Compute set of indices which are the basis for the result k
                 //if (k size5;
                 IndexKeeper tempKeeper;
                 tempKeeper.i1 = k/(size5*size4*size3*size2);
                 tempKeeper.i2 = k/(size5*size4*size3) % size2;
                 tempKeeper.i3 = k/(size5*size4) % size3;
                 tempKeeper.i4 = k/(size5) % size4;
                 tempKeeper.i5 = k % size5;
                 if (tempKeeper.i1 > size1)
                     tempKeeper.i1 = outOfBound;
                 return tempKeeper;
             }
             int size1,size2,size3,size4,size5,size_;
    };

    

    /** @brief Utility interface class for adding a string name onto a
    structure.
    @ingroup INTERNAL_USE
    */
    class Named {
    public:
        string getName() const { return name; }
        void setName(const string& n) { name = n; }

    private:
        string name;
    };

    /** @brief Utility for doing reference counted pointers.
    @ingroup INTERNAL_USE
    */
    template<class T> class Handle {
    public:

        //static int counter;

        const T &operator->() const {
            return root;
        }
        Handle(const T &r) : root(r) {
            increment();
            //counter++;
            //std::cout << "one more handle" << typeid(T).name() << " " << counter << std::endl;
        }
        Handle(const Handle& h) : root(h.root) {
            increment();
            //counter++;
            //std::cout << "one more handle" << typeid(T).name() << " " << counter << std::endl;
        }
        const Handle& operator=(const Handle& h) {
            if (root != h.root) {
                decrement();
                root = h.root;
                increment();
            }
            return *this;
        }
        bool isDefined() const {
            return root != 0;
        }
        bool operator==(const Handle& h) const{
            return root == h.operator->();
        }
        bool operator!=(const Handle& h) const{
            return !operator==(h);
        }
        ~Handle() {
            decrement();
            //counter--;
            //std::cout << "current amount of handles for " << typeid(T).name() << " " << counter << std::endl;
        }
    protected:
        void increment() {
            if(root != 0) {
                ++(root->count);
            }
        }
        void decrement() {
            if(root != 0) {
                if(root->count == 1) {
                    delete root;
                    root = 0;
                } else {
                    assert(root->count > 0 );
                    --(root->count);
                }
            }
        }
    private:
        Handle() : root(0) {}
        T root;
    };

    //template<class T> int Handle<T>::counter = 0;

    

} // End of namespace flopc
#endif
