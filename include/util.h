#ifndef UTIL
#define UTIL
#include "math.h"

class Topaz;
class Task;
typedef void (*fxnptr)(Task *, Task *);
typedef bool (*lrnptr)(Task *, Task *);

float rand_norm();

typedef enum TYPE_ID_ENUM{
	FLOAT=0,
	DOUBLE=1,
	CHAR=2,
	INT=3,
	UNKNOWN=4
} type_id;

#define isNaN(f) (isnan(f))
#define isInf(f) (f == INFINITY)
#define isNegInf(f) (f==-INFINITY)
#define isBad(f) (isNaN(f) || isInf(f) || isNegInf(f))

#define CAST(expr, tid) (tid == FLOAT ? (float) (expr) : ( tid == DOUBLE ? (double) (expr) : ( tid == INT ? (int) (expr): (char) (expr))) )
#define PCAST(expr, tid) (tid == FLOAT ? (float*) (expr) : ( tid == DOUBLE ? (double*) (expr) : ( tid == INT ? (int*) (expr): (char*) (expr))) )
#define SIZEOF(tid) (tid == FLOAT ? sizeof(float) : ( tid == DOUBLE ? sizeof(double) : ( tid == INT ? sizeof(int): sizeof(char))) )
#define PARAMETRIC(mcro, typ) switch(typ){ \
									case FLOAT: mcro(float); break; \
									case DOUBLE: mcro(double); break; \
									case INT: mcro(int); break; \
									case CHAR: mcro(char); break; \
									default: mcro(void); break; \
								}
								
#ifndef NULL
#define NULL 0
#endif


#endif
