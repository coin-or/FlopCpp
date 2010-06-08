#ifndef _MP_shared_implementation_details_hpp_
#define _MP_shared_implementation_details_hpp_

#include <iostream>

#include <boost/shared_ptr.hpp>
#include <boost/random.hpp>


namespace flopc {

    typedef boost::mt19937 base_generator_type; //Set RNG
    typedef boost::variate_generator<base_generator_type&, boost::uniform_01<double> > uniform_distr_gen;

    // Stores Uniform01Generator, hides implementation from .hpp
    struct Uniform01Generator {
        boost::shared_ptr<base_generator_type> rng;
        boost::shared_ptr<uniform_distr_gen> uniform;
        unsigned int seed;

        Uniform01Generator();
    };
}

#endif 

