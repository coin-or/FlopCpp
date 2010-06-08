#include "MP_shared_implementation_details.hpp"

//Windows. Construct necessary as VisualC++ include stdlib.h before I can set _CRT_RAND_S
#ifdef _MSC_VER
#ifndef _CRT_RAND_S
#define _CRT_RAND_S
#endif 
#include <stdlib.h>
#else
#include <stdio.h>
#include <fcntl.h>
#endif
//Linux: Call /dev/urandom to generate a seed

using namespace flopc;

Uniform01Generator::Uniform01Generator() : rng(),uniform(),seed(0) {
            #ifdef _MSC_VER
            //Windows
            errno_t err;
            err = rand_s(&seed);
            if (err != 0){
                std::cerr << "Error occured during seed generation: " << err << std::endl;
                // We need to abort here TODO
            }
            #endif
            //So we are not compiling using windows compiler, so we have /dev/urandom
            #ifndef _MSC_VER
            //Copied from Good Practice RNG.pdf (google for it)
            int fn;
            fn = open("/dev/urandom", O_RDONLY);
            if (fn == -1)
                exit(-1); /* Failed! */
            if (read(fn, &seed, 4) != 4)
                exit(-1); /* Failed! */
            close(fn);
            //Without random numbers this does not make much sense
            #endif

            std::cout << "Seed is: " << seed << std::endl;
        }
