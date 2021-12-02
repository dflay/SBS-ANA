#ifndef UTIL_RUN_H
#define UTIL_RUN_H

// a simple struct for CODA run info 

typedef struct codaRun {
   std::string info;
   int stream;
   int segmentBegin;
   int segmentEnd;
   int runNumber;
   // constructor 
   codaRun(): 
      info("NONE"),stream(0),segmentBegin(0),segmentEnd(0),runNumber(0)
   {}
} codaRun_t; 

#endif 
