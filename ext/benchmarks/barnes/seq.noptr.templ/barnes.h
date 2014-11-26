#ifndef BARNES_H
#define BARNES_H

#include <stdio.h>
#include <string.h>
#include <math.h>

#include <iostream>
#include <fstream>
#include "barnes.h"
const int  MAX_NAME = 80;
const int NDIM   = 3;

typedef struct VECTOR_T {
	double val[NDIM];
} vector_t;


void in_vector(std::ifstream& f, double *v);
void tvecClr(double *v);
void tvecAdd(double *v1, double *v2);
void tvecSub(double *v1, double *v2);
void tvecLoad(double *v1, double *v2);
void tvecStore(double *v1, double *v2);
void tvecDiv(double *v1, double *v2);
void tvecProd(double *v1, double *v2);
void tvecScale(double *v, double s);

double tvecDotProd(double *v1,double *v2);
void tvecCrossProd(double *v, double *u, double *w);
void tvecMin(double *v1, double  *v2);
double tvecNorm1(double *v);
void vecCons(vector_t * vc);
void vecPrint(vector_t * v);
void vecClr(vector_t * vc);
void vecSetUnit(vector_t * vc);
void vecSub(vector_t * vc, double *v);
void vecProd(vector_t * vc, double *v);
void vecScale(vector_t * vc,double s);
double vecGetVal(vector_t * vc,int idx);
void vecFieldAdd(vector_t * vc,int idx,double v);
void vecFieldClr(vector_t * vc,int idx);
void vecFieldSet(vector_t * vc,int idx, double v);
void vecLoad(vector_t * vc,double *v);
void PRTV(vector_t * vc,char *name);
void vecMin(vector_t * vc,double *v);
void vecMax(vector_t * vc,double *v);
double vecNorm1(vector_t * vc);
double vecSqNorm2(vector_t * vc);
void vecAdd(vector_t * vc, double *v);
void vecStore(vector_t * vc, double *v);


typedef struct MATRIX{
	double val[NDIM][NDIM];
} matrix_t;


void matClr(matrix_t * mmat);
void matCons(matrix_t * mmat);
void matDes(matrix_t * mmat);
double  matGetVal(matrix_t * mmat, int row, int col);
void    matSetId(matrix_t * mmat);
void    matSet(matrix_t * mmat, matrix_t *m);
void    matTrans(matrix_t * mmat);
void    matAdd(matrix_t * mmat, matrix_t *m);
void    matSub(matrix_t * mmat, matrix_t *q, matrix_t *r);
void    matMult(matrix_t * mmat, matrix_t *q, matrix_t *r);
void    matScale(matrix_t * mmat, matrix_t *q, double f);
double  matTrace(matrix_t * mmat);
void    matVectMult(matrix_t * mmat, double *v, double *u);
void    matOutProd(matrix_t * mmat, double *v, double *u);
void    matPrint(matrix_t * mmat);


const int LEAFMAXBODIES = 10;
const int NSUB = 8; 			// (1 << NDIM)
const int IMAX	= 1073741824;		// (1 << MAXLEVEL)

const int CELL = 01;
const int LEAF = 02;
const int BODY = 04;

typedef struct NODE_REF_T {char type; int offset;} node_ref;
const node_ref REF_NULL = {-1,-1};

inline int isnull(node_ref t){return (REF_NULL.type == t.type && REF_NULL.offset == t.offset);}
inline int isequals(node_ref t1, node_ref t2){return (t1.type==t2.type && t1.offset==t2.offset);}
inline node_ref cellptr(int offset){node_ref r; r.type = CELL; r.offset = offset; return r;}
inline node_ref leafptr(int offset){node_ref r; r.type = LEAF; r.offset = offset; return r;}
inline node_ref bodyptr(int offset){node_ref r; r.type = BODY; r.offset = offset; return r;}
typedef struct NODE_T{
	int type;
	double mass;
	vector_t pos;
	int level;
	int child_num;
	node_ref parent;
} node_t;

void node_cons(node_t * n);
void node_adjLevel(node_t * me, node_t * parent);

typedef struct BODY_T{
   //body 
   node_t node;
   vector_t vel;          /* velocity of body                             */
   vector_t acc;          /* acceleration of body                         */
   double phi;          /* potential at body                            */
   vector_t tmp;          /* Skratch vector		                */
} body_t;

void body_cons(body_t * l);
void body_clear(body_t * l);
void body_swapAcc(body_t * l);
void body_startVel(body_t * l, double f);
void body_advance(body_t * l, double hts, double ts);

typedef struct LEAF_T{
	node_t node;
	int num_bodies;
	node_ref bodyp[LEAFMAXBODIES];
} leaf_t;

void leaf_cons(leaf_t * l);
void leaf_clear(leaf_t * l);
int  leaf_full(leaf_t * l);
typedef struct CELL_T{
	node_t node;
	node_ref subp[NSUB]; 
} cell_t;

void cell_cons(cell_t * l);
void cell_clear(cell_t * l);

typedef struct CELLSET_T  {
	int numcell;
	int maxcell;
	cell_t* celltab;
} cellset_t;

void cellset_cons(cellset_t * c, int siz);
void cellset_reset(cellset_t * c);
cell_t * cellset_get(cellset_t * c, node_ref idx);
node_ref cellset_new(cellset_t * c);
node_ref cellset_make(cellset_t * c, node_ref parent);

typedef struct LEAFSET_T {
	int numleaf;
	int maxleaf;
	leaf_t* leaftab;
} leafset_t;

void leafset_cons(leafset_t * c, int siz);
void leafset_reset(leafset_t * c);
leaf_t * leafset_get(leafset_t * c, node_ref idx);
node_ref leafset_new(leafset_t * c);
node_ref leafset_make(leafset_t * s, cellset_t * s2, node_ref parent);

typedef struct BODYSET_T{
	int numbody;
	int maxbody;
	body_t* bodytab;
} bodyset_t;

void bodyset_cons(bodyset_t * c, int siz);
body_t * bodyset_get(bodyset_t * c, node_ref i);
body_t * bodyset_newbody(bodyset_t * c);
// ----------- FILE: simulation.h ----------
typedef struct PARMS_T  {
char    headline[MAX_NAME];/* message describing calculation */
char    infile[MAX_NAME];  /* file name for snapshot input */
char    outfile[MAX_NAME]; /* file name for snapshot output */
double  dtime;          /* timestep for leapfrog integrator */
double  dtout;          /* time between data outputs */
double  tstop;          /* time to stop calculation */
double  fcells;         /* ratio of cells/leaves allocated */
double  fleaves;        /* ratio of leaves/bodies allocated */
double  tol;            /* accuracy parameter: 0.0 => exact */
double  tolsq;          /* square of previous */
double  eps;            /* potential softening parameter */
double  epssq;          /* square of previous */
double  dthf;           /* half time step */
char    *defaults[12];  /* pairs of name = value */
} parms_t;
// --- PARMS CODE ------------------------------------------------------------
void parms_cons(parms_t * p);
void parms_setparms(parms_t * p, char **fnames, double *args);
void parms_loadInfile(parms_t * p);
void parms_loadParms(parms_t * p);
int parms_getiparam(parms_t * p, const char *name);
double parms_getdparam(parms_t * p, const char *name);

typedef struct NBODYSYSTEM_T{
	int nbody;
	node_ref BH_tree;
	cellset_t cells;
	leafset_t leaves;
	bodyset_t bodies;

	double mtot;
	vector_t etot;
	matrix_t keten;
	matrix_t peten;
	vector_t cmphasePos;
	vector_t cmphaseVel;
	vector_t amvec;

	vector_t mincorner;
	vector_t maxcorner;

	double size;
	double tnow;
	double tout;

	int nstep;
	int n2bcalc;
	int nbccalc;
	int computestart;
	int computeend;
	int tracktime;
	int partitiontime;
	int treebuildtime;
	int forcecalctime;
	int updatetime;
} nbody_t;
// ----------------------------------------------------------------------------
void nbody_init(nbody_t * nb);
void nbody_output(nbody_t * nb, parms_t *sim);
void nbody_clearTiming(nbody_t * nb);
void nbody_initOutput(nbody_t * nb, parms_t *p);
void nbody_outputTiming(nbody_t * nb);
void nbody_startrun(nbody_t * n, parms_t *p);
void nbody_stepsystem(nbody_t * n, parms_t *p);
void nbody_setbound(nbody_t * nb);
void nbody_clearStats(nbody_t * n);
void nbody_outputEnergy(nbody_t * n);
void nbody_loadData(nbody_t * n, char *name);
void nbody_loadTestData(nbody_t * n);
void nbody_maketree(nbody_t * n);
//debugging
void print_node(nbody_t * n, node_ref parent_ref, int depth);


void body_walksub(node_ref cbody, node_ref head, double tolsq, double dsq, double epssq, nbody_t * nb);


#endif
