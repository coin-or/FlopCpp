// $Id$
#ifndef _Timer_hpp_
#define _Timer_hpp_

#include <sys/time.h> 
#include <unistd.h>                                                      

// A stopwatch. Returns time in seconds.

class Timer {
 public:
  Timer() {};
  
  void start() {
    gettimeofday(&startTime,0);
  }

  void stop() {
    gettimeofday(&endTime,0);
  }

  double time() const {
    long seconds = endTime.tv_sec - startTime.tv_sec;
    long microseconds = endTime.tv_usec - startTime.tv_usec;
    return seconds + microseconds/1000000.0;
  }

 private:
  timeval startTime, endTime;  
};

#endif
