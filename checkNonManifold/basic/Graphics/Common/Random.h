#ifndef _RANDOM_
#define _RANDOM_

#include <sys/types.h>
#include <time.h>
#include <stdlib.h>

#ifdef WIN32
#define srand48 srand
static double drand48 (void)
{
	return rand() / (float)RAND_MAX;
}
#endif


#define IA 16807
#define IM 2147483647
#define AM (1.0/IM)
#define IQ 127773
#define IR 2836
#define MASK 123459876
  
static double RandomDouble()
{
  static long idum = 1;
  long k;
  double ans;
    
  idum ^= MASK;
  k=(idum)/IQ;
  idum=IA*(idum-k*IQ)-IR*k;
  if (idum < 0) idum += IM;
  ans=AM*(idum);
  idum ^= MASK;
  return ans;
}  
#undef IA
#undef IM
#undef AM
#undef IQ
#undef IR
#undef MASK

class Random 
{
private:
  static long TimeSeed(void)
    { return (long) time(NULL); }
  
public:
  Random(unsigned long initSeed = TimeSeed())
    { srand48(initSeed); }
  ~Random(void) {}

  void setSeed(unsigned long newSeed = TimeSeed())
    { srand48(newSeed); }

  double operator()()
    // return a random number [0.0,1.0)
    { return drand48(); }

  double operator()(double fact)
    // return a random number [0.0,fact)
    { return fact*drand48(); }
};

#endif
