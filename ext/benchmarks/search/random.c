/*
 * Copyright (c) 1983 Regents of the University of California.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms are permitted
 * provided that the above copyright notice and this paragraph are
 * duplicated in all such forms and that any documentation,
 * advertising materials, and other materials related to such
 * distribution and use acknowledge that the software was developed
 * by the University of California, Berkeley.  The name of the
 * University may not be used to endorse or promote products derived
 * from this software without specific prior written permission.
 * THIS SOFTWARE IS PROVIDED ``AS IS'' AND WITHOUT ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, WITHOUT LIMITATION, THE IMPLIED
 * WARRANTIES OF MERCHANTIBILITY AND FITNESS FOR A PARTICULAR PURPOSE.
 */



/* $Source: /os/master/srctree/usr/src/lib/libc/bsd/gen/RCS/random.c,v $ */

/* $Id: random.c,v 4.0.1.0.1.0 92/04/16 11:28:32 lfsh Exp $ */

/*
 *$Log:	random.c,v $
 *Revision 4.0.1.0.1.0  92/04/16  11:28:32  lfsh
 *DASH modbranch
 *
 *Revision 4.0.1.0  92/04/16  11:28:31  lfsh
 *DASH masterbranch
 *
 *Revision 4.0  92/04/16  11:28:30  lfsh
 *SVR4 master
 *
 */

#include <stdio.h>
#include <stdlib.h>

/*
 * random.c:
 * An improved random number generation package.  In addition to the standard
 * rand()/srand() like interface, this package also has a special state info
 * interface.  The initstate() routine is called with a seed, an array of
 * bytes, and a count of how many bytes are being passed in; this array is then
 * initialized to contain information for random number generation with that
 * much state information.  Good sizes for the amount of state information are
 * 32, 64, 128, and 256 bytes.  The state can be switched by calling the
 * setstate() routine with the same array as was initiallized with initstate().
 * By default, the package runs with 128 bytes of state information and
 * generates far better random numbers than a linear congruential generator.
 * If the amount of state information is less than 32 bytes, a simple linear
 * congruential R.N.G. is used.
 * Internally, the state information is treated as an array of longs; the
 * zeroeth element of the array is the type of R.N.G. being used (small
 * integer); the remainder of the array is the state information for the
 * R.N.G.  Thus, 32 bytes of state information will give 7 longs worth of
 * state information, which will allow a degree seven polynomial.  (Note: the
 * zeroeth word of state information also has some other information stored
 * in it -- see setstate() for details).
 * The random number generation technique is a linear feedback shift register
 * approach, employing trinomials (since there are fewer terms to sum up that
 * way).  In this approach, the least significant bit of all the numbers in
 * the state table will act as a linear feedback shift register, and will have
 * period 2^deg - 1 (where deg is the degree of the polynomial being used,
 * assuming that the polynomial is irreducible and primitive).  The higher
 * order bits will have longer periods, since their values are also influenced
 * by pseudo-random carries out of the lower bits.  The total period of the
 * generator is approximately deg*(2**deg - 1); thus doubling the amount of
 * state information has a vast influence on the period of the generator.
 * Note: the deg*(2**deg - 1) is an approximation only good for large deg,
 * when the period of the shift register is the dominant factor.  With deg
 * equal to seven, the period is actually much longer than the 7*(2**7 - 1)
 * predicted by this formula.
 */


#define		TYPE_3		3		/* x**31 + x**3 + 1 */
#define		BREAK_3		128
#define		DEG_3		31
#define		SEP_3		3


/*
 * Initially, everything is set up as if from :
 *		initstate( 1, &randtbl, 128 );
 * Note that this initialization takes advantage of the fact that random()
 * advances the front and rear pointers 10*rand_deg times, and hence the
 * rear pointer which starts at 0 will also end up at zero; thus the zeroeth
 * element of the state information, which contains info about the current
 * position of the rear pointer is just
 *	MAX_TYPES*(rptr - state) + TYPE_3 == TYPE_3.
 */

#include "pthread.h"

typedef struct s_random_t *random_t;
typedef struct s_random_t {
  long randtbl[DEG_3 + 1];
  long *state;
  int             rand_type;
  int             rand_deg;
  int             rand_sep;
  long            *end_ptr;
  long *fptr;
  long *rptr;
  pthread_mutex_t mut;
} struct_random_t;

random_t make_random(x)
unsigned x;
{
  random_t r;
  r = (random_t) malloc(sizeof(struct_random_t));
  r->state = &r->randtbl[1];
  r->rand_type = TYPE_3;
  r->rand_deg = DEG_3;
  r->rand_sep = SEP_3;
  r->end_ptr = &r->randtbl[ DEG_3 + 1 ];
  r->fptr = &r->randtbl[ SEP_3 + 1 ];
  r->rptr = &r->randtbl[ 1 ];
  pthread_mutex_init(&r->mut, 0);
  seed_random(x, r);

  return(r);
}

destroy_random(r)
random_t r;
{
   pthread_mutex_destroy(&r->mut);
   free(r);
}

/*
 * srandom:
 * Initialize the random number generator based on the given seed.  If the
 * type is the trivial no-state-information type, just remember the seed.
 * Otherwise, initializes state[] based on the given "seed" via a linear
 * congruential generator.  Then, the pointers are set to known locations
 * that are exactly rand_sep places apart.  Lastly, it cycles the state
 * information a given number of times to get rid of any initial dependencies
 * introduced by the L.C.R.N.G.
 * Note that the initialization of randtbl[] for default usage relies on
 * values produced by this routine.
 */

long next_random( random_t s);


seed_random(x, s)
    unsigned		x;
    random_t s;
{
    	register  int		i, j;

	    j = 1;
	    s->state[ 0 ] = x;
	    for( i = 1; i < s->rand_deg; i++ )  {
		s->state[i] = 1103515245*s->state[i - 1] + 12345;
	    }
	    s->fptr = &s->state[ s->rand_sep ];
	    s->rptr = &s->state[ 0 ];
	    for( i = 0; i < 10*s->rand_deg; i++ )  next_random(s);
}



/*
 * random:
 * If we are using the trivial TYPE_0 R.N.G., just do the old linear
 * congruential bit.  Otherwise, we do our fancy trinomial stuff, which is the
 * same in all ther other cases due to all the global variables that have been
 * set up.  The basic operation is to add the number at the rear pointer into
 * the one at the front pointer.  Then both pointers are advanced to the next
 * location cyclically in the table.  The value returned is the sum generated,
 * reduced to 31 bits by throwing away the "least random" low bit.
 * Note: the code takes advantage of the fact that both the front and
 * rear pointers can't wrap on the same call by not testing the rear
 * pointer if the front one has wrapped.
 * Returns a 31-bit random number.
 */

long
next_random(s)
random_t s;
{
	long		i;
	    pthread_mutex_lock(&s->mut);
	    *s->fptr += *s->rptr;
	    i = (*s->fptr >> 1)&0x7fffffff;	/* chucking least random bit */
	    if(  ++s->fptr  >=  s->end_ptr  )  {
		s->fptr = s->state;
		++s->rptr;
	    }
	    else  {
		if(  ++s->rptr  >=  s->end_ptr  )  s->rptr = s->state;
	    }
	    pthread_mutex_unlock(&s->mut);
	return( i );
}

#include "myrandom.h"

#define PAD_SIZE 64
#ifdef _MYPAR_LIB_
inline int isParallelSectionWrapper() {return 0 != parlib_runtime_parallel_mode;}
inline int getThreadNumWrapper() {return parlib_runtime_thread_num;}
inline int getCurrentThreadIdWrapper() {return parlib_runtime_this_thread_id;}
#else
#include <omp.h>
inline int isParallelSectionWrapper() {return omp_in_parallel();}
inline int getThreadNumWrapper() {return omp_get_max_threads();}
inline int getCurrentThreadIdWrapper() {return omp_get_thread_num();}
#endif


#include <assert.h>

typedef struct s_pad_random_t {
   char pad0[PAD_SIZE];
   random_t random;
   char pad1[PAD_SIZE];
} s_pad_random_t;

#define MAXRAND 32

s_pad_random_t RANDOMS[MAXRAND];

void srand_mr(unsigned seed) {

   unsigned tid = getCurrentThreadIdWrapper();
   assert (tid < MAXRAND);
   random_t rand = RANDOMS[tid].random;
   if (rand == 0) {
      rand = make_random(seed);
      RANDOMS[tid].random = rand;
   } else {
      seed_random(seed, rand);
   }


}

long int rand_mr() {

   unsigned tid = getCurrentThreadIdWrapper();
   random_t rand = RANDOMS[tid].random;
   return next_random(rand);

}


//
//#define PAD_SIZE 64
//
//struct pad_drand48_data {
//   char p0[PAD_SIZE];
//   struct drand48_data buf;
//   char p1[PAD_SIZE];
//};
//
//static struct pad_drand48_data priv_seq_buffer[32];
//
//
//// definition of parallel routines:
//
//
//#ifdef _MYPAR_LIB_
//inline int isParallelSectionWrapper() {return 0 != parlib_runtime_parallel_mode;}
//inline int getThreadNumWrapper() {return parlib_runtime_thread_num;}
//inline int getCurrentThreadIdWrapper() {return parlib_runtime_this_thread_id;}
//#else
//#include <omp.h>
//inline int isParallelSectionWrapper() {return omp_in_parallel();}
//inline int getThreadNumWrapper() {return omp_get_max_threads();}
//inline int getCurrentThreadIdWrapper() {return omp_get_thread_num();}
//#endif
//
//
//void srand_par(unsigned seed) {
//   int tid = getCurrentThreadIdWrapper();
//   srand48_r(seed, &priv_seq_buffer[tid].buf);
//}
//
//long int rand_par() {
//  long int result;
//  int tid = getCurrentThreadIdWrapper();
//  lrand48_r(&priv_seq_buffer[tid].buf, &result);
//  return result;
//}
//
//void srand_seq(long seed) {
//  srand48(seed);
//}
//
//long int rand_seq() {
//  long int result;
//  result = lrand48();
//  return result;
//}
//
//
///// THE HEADER INTERFACE:
//#ifdef _USE_PARALLEL_
//void srand_mr(unsigned seed) { srand_par(seed); }
//
//long int rand_mr() { return rand_par(); }
//
//#else
//
//int srand_is_first;
//#include <stdio.h>
//
//void srand_mr(unsigned seed) {
//   if (!srand_is_first) {
//      srand_is_first = 1;
//      printf ("search warning: this is sequential random number generator");
//   }
//   srand_seq(seed);
//}
//
//long int rand_mr() { return rand_seq(); }
//
//
//#endif
