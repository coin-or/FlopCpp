// ******************** FlopCpp **********************************************
// File: MP_data.hpp
// $Id$
// Author: Tim Helge Hultberg (thh@mat.ua.pt)
// Copyright (C) 2003 Tim Helge Hultberg
// All Rights Reserved.
// ****************************************************************************

#ifndef _MP_data_hpp_
#define _MP_data_hpp_

#include <vector>
#include "MP_index.hpp"    
#include "MP_set.hpp"      
#include "MP_constant.hpp" 
#include "MP_boolean.hpp" 

namespace flopc {

  class MP_data;

  /** @brief Reference to a set of data
      @ingroup INTERNAL_USE
      @note FOR INTERNAL USE: This is not normally used directly by the
      calling code.
  */
  class DataRef : public Constant_base, public Functor {
  public:
    DataRef(MP_data* d, 
            const MP_index_exp& i1,
            const MP_index_exp& i2,
            const MP_index_exp& i3,
            const MP_index_exp& i4,
            const MP_index_exp& i5,
            int s = 0) : 
      D(d),I1(i1),I2(i2),I3(i3),I4(i4),I5(i5),C(0),stochastic(s) {}

    ~DataRef() {} 
    DataRef& such_that(const MP_boolean& b);
    double evaluate() const;
    int getStage() const;
    const DataRef& operator=(const DataRef& r); 
    const DataRef& operator=(const Constant& c);
    void evaluate_lhs(double v) const;
    void operator()() const;
    DataRef& probability(double p) { return *this; }
  private:
    MP_data* D;
    MP_index_exp I1,I2,I3,I4,I5;
    Constant C;
    int stochastic;
    MP_boolean B;
  };

  /** @brief Input data set.
      @ingroup PublicInterface
      This is one of the main public interface classes.  
      It is normally directly constructed given a set of indices (domain)
      over which it is valid. If the data is not bound at construction,
      either the value() or initialize() method must be called
      which (deep) copies in the actual data.  
      If one wishes to refer to external data instead rather than
      doing a deep copy, use the constructor which takes the value pointer
      as an argument.  This copies the original data pointer value 
      (rather than a deep copy).
      <br> This is used for construction of :
      @li objective coefficients
      @li constraint coefficients
      @li 'right hand sides'
  */
  class MP_data : public RowMajor, public Functor , public Named {
    friend class MP_variable;
    friend class DisplayData;
    friend class DataRef;
    friend class MP_model;
  public:
    void operator()() const;
    /// similar to value() but copies the same value to all entries.
    void initialize(double d) {
      for (int i=0; i<size(); i++) {
        v[i] = d;
      }
    }
    /** Constructs the MP_data object, and allocates space for data, but
        does not initialize the data.
    */
    MP_data(const MP_set_base &s1 = MP_set::getEmpty(), 
            const MP_set_base &s2 = MP_set::getEmpty(), 
            const MP_set_base &s3 = MP_set::getEmpty(),
            const MP_set_base &s4 = MP_set::getEmpty(), 
            const MP_set_base &s5 = MP_set::getEmpty()) :
      RowMajor(s1.size(),s2.size(),s3.size(),s4.size(),s5.size()),
      S1(s1),S2(s2),S3(s3),S4(s4),S5(s5),
      v(new double[size()]), manageData(true) 
      {
        initialize(0); 
      }

    /** Construct the object, and uses the data in the original array
        (shallow copy)
    */
    MP_data(double* value,
            const MP_set_base &s1 = MP_set::getEmpty(), 
            const MP_set_base &s2 = MP_set::getEmpty(), 
            const MP_set_base &s3 = MP_set::getEmpty(),
            const MP_set_base &s4 = MP_set::getEmpty(), 
            const MP_set_base &s5 = MP_set::getEmpty()) :
      RowMajor(s1.size(),s2.size(),s3.size(),s4.size(),s5.size()),
      S1(s1),S2(s2),S3(s3),S4(s4),S5(s5),
      v(value), manageData(false) 
      { }

    ~MP_data() {
      if (manageData == true) delete[] v;
    }
        
    /// Used to bind and deep copy data into the MP_data data structure.
    void value(const double* d) {
      for (int i=0; i<size(); i++) {
        v[i] = d[i];
      }
    }

    /// @todo purpose?
    operator double() {
      return operator()(0);
    }
    
    /** Looks up the data based on the index values passed in.
        @note this is used internally, but may also be useful for spot
        checking data or in other expressions.
    */
    double& operator()(int i1, int i2=0, int i3=0, int i4=0, int i5=0) {
      i1 = S1.check(i1);
      i2 = S2.check(i2);
      i3 = S3.check(i3);
      i4 = S4.check(i4);
      i5 = S5.check(i5);
      int i = f(i1,i2,i3,i4,i5);
      if (i == outOfBound) {
        outOfBoundData = 0;
        return outOfBoundData;
      } else {
        return v[i];
      }
    }
    
    /** returns a DataRef which refers into the MP_data.  
        @note For internal use.
        @todo can this be private?
    */
    DataRef& operator() (
      const MP_index_exp& i1 = MP_index_exp::getEmpty(),
      const MP_index_exp& i2 = MP_index_exp::getEmpty(),
      const MP_index_exp& i3 = MP_index_exp::getEmpty(),
      const MP_index_exp& i4 = MP_index_exp::getEmpty(),
      const MP_index_exp& i5 = MP_index_exp::getEmpty()
      ) {
      myrefs.push_back(new DataRef(this, i1, i2, i3, i4, i5));
      return *myrefs.back();
    }
    

    /// For displaying data in a human readable format.
    void display(string s = "");
  protected:
    vector<DataRef*> myrefs;
  private:
    MP_data(const MP_data&); // Forbid copy constructor
    MP_data& operator=(const MP_data&); // Forbid assignment

    static double outOfBoundData;

    MP_index i1,i2,i3,i4,i5;
    const MP_set_base &S1,&S2,&S3,&S4,&S5;
    double* v;
    bool manageData;
  };

  class MP_stochastic_data : public MP_data {
  public:
    MP_stochastic_data(const MP_set_base &s1 = MP_set::getEmpty(), 
                       const MP_set_base &s2 = MP_set::getEmpty(), 
                       const MP_set_base &s3 = MP_set::getEmpty(),
                       const MP_set_base &s4 = MP_set::getEmpty(), 
                       const MP_set_base &s5 = MP_set::getEmpty()) :
      MP_data(s1,s2,s3,s4,s5) {}

    DataRef& operator() (
      const MP_index_exp& i1 = MP_index_exp::getEmpty(),
      const MP_index_exp& i2 = MP_index_exp::getEmpty(),
      const MP_index_exp& i3 = MP_index_exp::getEmpty(),
      const MP_index_exp& i4 = MP_index_exp::getEmpty(),
      const MP_index_exp& i5 = MP_index_exp::getEmpty()
      ) {
      myrefs.push_back(new DataRef(this, i1, i2, i3, i4, i5, 1));
      return *myrefs.back();
    }
  };

} // End of namespace flopc
#endif
