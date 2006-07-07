// $Id$
#ifndef _Timer_hpp_
#define _Timer_hpp_
#ifdef _MSC_VER
#include <math.h>
#include <windows.h>
#include <Psapi.h.>
#include <stdlib.h>


double Filetime2Seconds(FILETIME ft)
{
	const double clk_tck = .01e+9;
    const double twoToThirtyTwo = pow(2.0,32.0);
	return (((double(ft.dwHighDateTime) *twoToThirtyTwo) + ft.dwLowDateTime)
            / clk_tck);
}


#else
#include <sys/time.h> 
#include <unistd.h>                                                      
#endif

// A stopwatch. Returns time in seconds.

class Timer {
 public:
  Timer() {};
#ifdef _MSC_VER
typedef double timeval;
  void gettimeofday(timeval *tv,int *junk)
  {

	BOOL bStatus;
	FILETIME ftCreationTime;
	FILETIME ftExitTime;
	FILETIME ftKernelTime;
	FILETIME ftUserTime;
	HANDLE hProcess;

	hProcess = GetCurrentProcess();
	bStatus = GetProcessTimes(hProcess, 
			&ftCreationTime, &ftExitTime, 
			&ftKernelTime, &ftUserTime);

	double dUserTime = Filetime2Seconds(ftUserTime);
	double dSystemTime = Filetime2Seconds(ftKernelTime);
	// not sure how timeofday is useful on multiprocessor machines, gonna get CPU consumed to this point
	// including system and user time.  I think I like "times" better with user/system time.
	*tv= dUserTime+dSystemTime;
  }

#endif
  void start() {
    gettimeofday(&startTime,0);
  }

  void stop() {
    gettimeofday(&endTime,0);
  }

  double time() const {
#ifdef _MSC_VER
	  return endTime-startTime;
#else
    long seconds = endTime.tv_sec - startTime.tv_sec;
    long microseconds = endTime.tv_usec - startTime.tv_usec;
    return seconds + microseconds/1000000.0;
#endif

  }

 private:
  timeval startTime, endTime;  
};

#endif
