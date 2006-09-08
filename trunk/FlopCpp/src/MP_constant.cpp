// ******************** FlopCpp **********************************************
// File: MP_constant.cpp
// $Id$
// Author: Tim Helge Hultberg (thh@mat.ua.pt)
// Copyright (C) 2003 Tim Helge Hultberg
// All Rights Reserved.
//****************************************************************************

#include <float.h>
#include <cmath>
#include <sstream>
#include "MP_constant.hpp"
#include "MP_data.hpp"
#include "MP_domain.hpp"
#include "MP_index.hpp"

namespace flopc {

    class Constant_index : public Constant_base {
	friend class Constant;
    private:
	Constant_index(const MP_index_exp& i) : I(i) {};
	double evaluate() const {
	    return I->evaluate(); 
	}
	const MP_index_exp I;
    };

    class Constant_double : public Constant_base {
	friend class Constant;
    private:
	Constant_double(double d) : D(d) {}
	double evaluate() const { 
	    return D; 
	} 
	double D;
    };

    class Constant_abs : public Constant_base {
	friend Constant abs(const Constant& c);
    private:
	Constant_abs(const Constant& c) : C(c) {}
	double evaluate() const {
	    return fabs(C->evaluate());
	}    
	Constant C;
    };
    /** @brief  constructs a constant from the absolute value of a constant.
    @ingroup PublicInterface
    */
    Constant abs(const Constant& c) {
	return new Constant_abs(c);
    }

    class Constant_pos : public Constant_base {
	friend Constant pos(const Constant& c);
    private:
	Constant_pos(const Constant& c) : C(c) {}
	double evaluate() const {
	    double temp = C->evaluate();
	    if (temp>0) {
		return temp;
	    } else {
		return 0.0;
	    }
	}    
	Constant C;
    };
    /** @brief  constructs a constant from the value if positive, or zero if negative.
    @ingroup PublicInterface
    */
    Constant pos(const Constant& c) {
	return new Constant_pos(c);
    }

    class Constant_ceil : public Constant_base {
	friend Constant ceil(const Constant& c);
    private:
	Constant_ceil(const Constant& c) : C(c) {}
	double evaluate() const {
	    return std::ceil(C->evaluate());
	}
	Constant C;
    };
    /** @brief  ceil rounds up a constant and returns a constant.
    @ingroup PublicInterface
    */
    Constant ceil(const Constant& c) {
	return new Constant_ceil(c);
    }

    /** @brief  floor rounds down a constant and returns a constant.
    @ingroup PublicInterface
    */
    class Constant_floor : public Constant_base {
	friend Constant floor(const Constant& c);
    private:
	Constant_floor(const Constant& c) : C(c) {}
	double evaluate() const {
	    return std::floor(C->evaluate());
	}
	Constant C;
    };
    Constant floor(const Constant& c) {
	return new Constant_floor(c);
    }

    class Constant_exp : public Constant_base {
    protected:
	Constant_exp(const Constant& i, const Constant& j) : left(i),right(j) {}
	Constant left, right;
    };

    class Constant_min_2 : public Constant_exp {
	friend Constant minimum(const Constant& a, const Constant& b);
    private:
	Constant_min_2(const Constant& i, const Constant& j) : Constant_exp(i,j) {}
	double evaluate() const {
	    return std::min(left->evaluate(),right->evaluate());
	}
    };

    /** @brief returns a constant which represents the minimum of two constants.
    @ingroup PublicInterface
    */
    Constant minimum(const Constant& a, const Constant& b) {
	return new Constant_min_2(a,b);
    }

    class Constant_max_2 : public Constant_exp {
	friend Constant maximum(const Constant& a, const Constant& b);
    private:
	Constant_max_2(const Constant& i, const Constant& j) : Constant_exp(i,j) {}
	double evaluate() const {
	    return std::max(left->evaluate(),right->evaluate());
	}
    };

    /** @brief returns a constant which represents the maximum of two constants.
    @ingroup PublicInterface
    */
    Constant maximum(const Constant& a, const Constant& b) {
	return new Constant_max_2(a,b);
    }

    class Constant_plus : public Constant_exp {
	friend Constant operator+(const Constant& a, const Constant& b);
	friend Constant operator+(MP_index& a, MP_index& b);
    private:
	Constant_plus(const Constant& i, const Constant& j) : Constant_exp(i,j) {}
	double evaluate() const {
	    return left->evaluate()+right->evaluate();
	}
    };

    /** @brief   returns a constant which represents the sum of two Constant objects
    @ingroup PublicInterface
    */

    Constant operator+(const Constant& a, const Constant& b) {
	return new Constant_plus(a,b);
    }
    /** @brief   returns a constant which represents the sum of two MP_index values.
    @ingroup PublicInterface
    */
    Constant operator+(MP_index& a, MP_index& b) {
	return new Constant_plus(Constant(a),Constant(b));
    }

    class Constant_minus : public Constant_exp {
	friend Constant operator-(const Constant& a, const Constant& b);
	friend Constant operator-(MP_index& a, MP_index& b);
    private:
	Constant_minus(const Constant& i, const Constant& j): Constant_exp(i,j) {};
	double evaluate() const {
	    return left->evaluate()-right->evaluate(); 
	}
    };

    /** @brief   returns a constant which represents the difference between two Constant objects
    @ingroup PublicInterface
    */
    Constant operator-(const Constant& a, const Constant& b) {
	return new Constant_minus(a,b);
    }

    /** @brief  returns a constant which represents the difference between two MP_index objects
    @ingroup PublicInterface
    */
    Constant operator-(MP_index& a, MP_index& b) {
	return new Constant_minus(Constant(a),Constant(b));
    }

    class Constant_mult : public Constant_exp {
	friend Constant operator*(const Constant& a, const Constant& b);
    private:
	Constant_mult(const Constant& i, const Constant& j) : Constant_exp(i,j) {};
	double evaluate() const {
	    return left->evaluate()*right->evaluate(); 
	}
    };

    /** @brief  returns a constant which represents the product of two Constant objects
    @ingroup PublicInterface
    */
    Constant operator*(const Constant& a, const Constant& b) {
	return new Constant_mult(a,b);
    }

    class Constant_div : public Constant_exp {
	friend Constant operator/(const Constant& a, const Constant& b); 
    private:
	Constant_div(const Constant& i, const Constant& j) : Constant_exp(i,j) {};
	double evaluate() const {
	    return left->evaluate()/right->evaluate(); 
	}
    };

    /** @brief  returns a constant which represents the division of a Constant object by another.
    @ingroup PublicInterface
    */
    Constant operator/(const Constant& a, const Constant& b) {
	return new Constant_div(a,b);
    }
   
    class Constant_max : public Constant_base, public Functor {
	friend Constant maximum(const MP_domain& i, const Constant& e);
    private:
	Constant_max(const MP_domain& i, const Constant& e) : d(i), exp(e) {};
	void operator()() const {
	    double temp = exp->evaluate();
	    if (temp > the_max) {
		the_max = temp;
	    }
	}
	double evaluate() const {    
	    the_max = DBL_MIN;
	    d.Forall(this);
	    return the_max;
	}
    
	MP_domain d;
	Constant exp;
	mutable double the_max;
    };

    class Constant_min : public Constant_base, public Functor {
	friend Constant minimum(const MP_domain& i, const Constant& e);
    private:
	Constant_min(const MP_domain& i, const Constant& e) : d(i), exp(e) {};
	void operator()() const {
	    double temp = exp->evaluate();
	    if (temp < the_min) {
		the_min = temp;
	    }
	}
	double evaluate() const {    
	    the_min = DBL_MAX;
	    d.Forall(this);
	    return the_min;
	}

	MP_domain d;
	Constant exp;
	mutable double the_min;
    };

    class Constant_sum : public Constant_base, public Functor {
	friend Constant sum(const MP_domain& i, const Constant& e);
    private:
	Constant_sum(const MP_domain& i, const Constant& e) : d(i), exp(e) {};
	void operator()() const {
	    the_sum += exp->evaluate();
	}
	double evaluate() const {  
	    the_sum = 0;
	    d.Forall(this);
	    return the_sum;
	}

	MP_domain d;
	Constant exp;
	mutable double the_sum;
    };

    class Constant_product : public Constant_base, public Functor {
	friend Constant product(const MP_domain& i, const Constant& e);
    private:
	Constant_product(const MP_domain& i, const Constant& e) : d(i), exp(e) {};
	void operator()() const {
	    the_product *= exp->evaluate();
	}
	double evaluate() const {  
	    the_product = 1;
	    d.Forall(this);
	    return the_product;
	}

	MP_domain d;
	Constant exp;
	mutable double the_product;
    };

    /** @brief  Returns a constant which represents the maximum value of the constant value across the domain.
    @ingroup PublicInterface
    */
    Constant maximum(const MP_domain& i, const Constant& e) {
	return new Constant_max(i,e);
    }
    /** @brief  Returns a constant which represents the minimum value of the constant value across the domain.
    @ingroup PublicInterface
    */
    Constant minimum(const MP_domain& i, const Constant& e) {
	return new Constant_min(i,e);
    }
    /** @brief  Returns a constant which represents the sum of the constant value across the domain.
    @ingroup PublicInterface
    */
    Constant sum(const MP_domain& i, const Constant& e) {
	return new Constant_sum(i,e);
    }
    /** @brief  Returns a constant which represents the product of the constant value across the domain.
    @ingroup PublicInterface
    */
    Constant product(const MP_domain& i, const Constant& e) {
	return new Constant_product(i,e);
    }

    /** @brief  Constructs a Constant from a DataRef
    @ingroup PublicInterface
    */
    Constant::Constant(const DataRef& d) : 
	Handle<Constant_base*>(const_cast<DataRef*>(&d)) {}

    /** @brief  Constructs a Constant from an index expression
    @ingroup PublicInterface
    */
    Constant::Constant(const MP_index_exp& i) :
	Handle<Constant_base*>(new Constant_index(i)){}

    /** @brief  Constructs a Constant from a double constant
    @ingroup PublicInterface
    */
    Constant::Constant(double d) :
	Handle<Constant_base*>(new Constant_double(d)) {}

    /** @brief  Constructs a Constant from an integer constant
    @ingroup PublicInterface
    */
    Constant::Constant(int d) :
	Handle<Constant_base*>(new Constant_double(d)) {}
 
} // End of namespace flopc

using namespace flopc;

