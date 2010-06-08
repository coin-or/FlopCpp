// ******************** FlopCpp **********************************************
// File: MP_utilities.cpp
//****************************************************************************

#include <cmath>

#include "MP_utilities.hpp"
namespace flopc{

    // Returns true in the case of equality
    bool compareDouble(double x, double y, double absoluteTolerance, double relativeTolerance){
        if ((std::fabs(x-y) <= absoluteTolerance) || std::fabs(x-y) <= relativeTolerance*(std::max(std::fabs(x),std::fabs(y))))
            return true;
        return false;
    }

    int round(double x){
        double temp;
        if (std::modf(x,&temp)>=0.5){
            if (x>=0){
                return int(std::ceil(x));
            }
            else{
                return int(std::floor(x));
            }
        }
        else{
            if (x<0){
                return int(std::ceil(x));
            }
            else{
                return int(std::floor(x));
            }
        }
    }

    double diffclock(clock_t clock1,clock_t clock2)
    {
        double diffticks=clock1-clock2;
        double diffms=(diffticks)/(CLOCKS_PER_SEC/1000);
        return diffms;
    }

}
