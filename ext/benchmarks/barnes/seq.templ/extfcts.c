#include <stdio.h>
#include <sys/types.h>
#include <sys/time.h>
#include <sys/times.h>
#include <sys/param.h>

#define GRANULARITY_TIME 1000
#define GRANULARITY_MASK 0xfffff
#define TICKS_PER_SECOND_TIME GRANULARITY_TIME
 
void get_ticks_time(int *ret) { 
  *ret = TICKS_PER_SECOND_TIME;
}

void get_time(int *ret) /* miliseconds resolution */
{
  double t;
  struct timeval _tp;
  struct timezone _tzp;
  gettimeofday(&_tp,&_tzp);
  t = _tp.tv_sec & GRANULARITY_MASK;
  t = t*GRANULARITY_TIME;
  t += (_tp.tv_usec) / (1000000 / GRANULARITY_TIME);
  *ret = (int)t;
}

int matchname(char *bind, char *name) {
   char *bp, *np;
   bp = bind; np = name;
   while (*bp == *np) { bp++; np++; }
   return ((*bp == '=') && (*np == '\0'));
}

int scanbind(char **bvec, char *name) {
  int i;

  for (i = 0; bvec[i] != NULL; i++)
    if (matchname(bvec[i], name))
      return i;
  return -1;
}

char *extrvalue(char *arg) { /* string of the form "name=value" */
   char *ap;
   ap = arg;
   while (*ap != '\0'){
     if(*ap++ == '=')
       return ((char *) ap);
   }
   return (NULL);
}

const int       MULT    = 1103515245;
const int       ADD     = 12345;
const int       MASK    = 0x7FFFFFFF;
const double    TWOTO31 = 2147483648.0;

static int A = 1;
static int B = 0;
static int randx = 1;
static int lastrand;   /* the last random number */

double prand() {
   lastrand = randx;
   randx = (A*randx+B) & MASK;
   return((double)lastrand/TWOTO31);
}

double xrand(double xl, double xh) { return (xl + (xh - xl) * prand()); }

void pranset(int seed) {
   A = 1; B = 0;
   randx = (A*seed+B) & MASK;
   A = (MULT * A) & MASK;
   B = (MULT*B + ADD) & MASK;
}
