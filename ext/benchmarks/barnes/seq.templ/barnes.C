/******************************************************************************/
/*                                                                            */
/*  Copyright (c) 1996 University of California at Santa Barbara              */
/*                                                                            */
/*  All rights reserved.                                                      */
/*                                                                            */
/*  Permission is given to use, copy, and modify this software for any        */
/*  non-commercial purpose as long as this copyright notice is not removed    */
/*  All other uses, including redistribution in whole or in part, are         */
/*  forbidden without prior written permission.                               */
/*                                                                            */
/*  This software is provided with absolutely no warranty and no support.     */
/*                                                                            */
/*  For further information contact: pedro@cs.ucsb.edu or martin@cs.ucsb.edu  */
/*                                                                            */
/******************************************************************************/

#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <limits.h>

#define CHECK
using namespace std;

// const int NULL = 0;
// const int  FALSE = 0;
// const int  TRUE  = 1;

const double    PI = 3.14159265358979323846;
// const double    TWO_PI = 6.28318530717958647693;
// const double    FOUR_PI = 12.56637061435917295385;
// const double    HALF_PI = 1.57079632679489661923;
// const double    FRTHRD_PI = 4.18879020478639098462;

const int  MAX_NAME = 80;

extern "C" void reset_stat();
extern "C" void print_stat();

extern "C" void         get_time(int *r);
extern "C" void         get_ticks_time(int *r);

extern "C" double       prand();
extern "C" double       xrand(double xl, double xh);
extern "C" void         pranset(int seed);

extern "C" int          scanbind(char **bvec, char *name);
extern "C" int          matchname(char *bind, char *name);
extern "C" char*        extrvalue(char *arg);

const int NDIM   = 3;

inline void in_vector(ifstream& f, double *v){
  double x1, x2, x3;
  f >> x1 >> x2 >> x3;
  v[0] = x1; v[1] = x2; v[2] = x3;
}

inline void tvecClr(double *v) { int i; for(i=0; i < NDIM; i++) v[i] = 0.0; }

inline void tvecAdd(double *v1, double *v2) { int i;
for(i=0; i < NDIM; i++) v1[i] += v2[i]; }

inline void tvecSub(double *v1, double *v2) { int i;
for(i=0; i < NDIM; i++) v1[i] -= v2[i]; }

inline void tvecLoad(double *v1, double *v2) { int i;
for(i=0; i < NDIM; i++) v1[i] = v2[i]; }

inline void tvecStore(double *v1, double *v2) { int i;
for(i=0; i < NDIM; i++) v2[i] = v1[i]; }

inline void tvecDiv(double *v1, double *v2) { int i;
for(i=0; i < NDIM; i++) v1[i] /= v2[i]; }

inline void tvecProd(double *v1, double *v2) { int i;
for(i=0; i < NDIM; i++) v1[i] *= v2[i]; }

inline void tvecScale(double *v, double s) { int i;
for(i=0; i < NDIM; i++) v[i] *= s; }

inline double tvecDotProd(double *v1,double *v2) { int i; double d = 0.0;
  for(i=0; i < NDIM; i++) d += (v1[i] * v2[i]); return d; }

inline void tvecCrossProd(double *v, double *u, double *w){
  v[0] = u[1]*w[2] - u[2]*w[1];
  v[1] = u[2]*w[0] - u[0]*w[2];
  v[2] = u[0]*w[1] - u[1]*w[0];
}

inline void tvecMin(double *v1, double  *v2){ int i;
  for(i=0; i < NDIM; i++) if(v1[i] > v2[i]) v1[i] = v2[i]; }

inline double tvecNorm1(double *v){ int i; double d;
d = 0.0; for(i=0; i < NDIM; i++) d += v[i]; return d; }

class vector {
public:
  double val[NDIM];
  vector(){}
  ~vector(){}
void vecPrint() { printf("(%8.4f %8.4f %8.4f) ",val[0],val[1],val[2]); }
void vecClr() 	{ int i; for(i=0; i < NDIM; i++) val[i] = 0.0; }
void vecSetUnit()      { int i; for(i=0; i < NDIM; i++) val[i] = 1.0; }
void vecSub(double *v) { int i; for(i=0; i < NDIM; i++) val[i] -= v[i]; }
void vecProd(double *v){ int i; for(i=0; i < NDIM; i++) val[i] *= v[i]; }
void vecScale(double s){ int i; for(i=0; i < NDIM; i++) val[i] *= s; }
double vecGetVal(int idx)		{ return val[idx]; }
void vecFieldAdd(int idx,double v)	{ val[idx] += v; }
void vecFieldClr(int idx)		{ val[idx] = 0.0; }
void vecFieldSet(int idx, double v)	{ val[idx] = v; }
void vecLoad(double *v) { int i; for(i=0; i < NDIM; i++) val[i] = v[i]; }
void PRTV(char *name)   { cout << name <<"= ["<<val[0]<<" "<<val[1]<<" "<<val[2]<<"] "; }

void vecMin(double *v){ int i;
  for(i=0; i < NDIM; i++)
    if(val[i] > v[i])
      val[i] = v[i];
}
void vecMax(double *v){ int i;
  for(i=0; i < NDIM; i++)
    if(val[i] < v[i])
      val[i] = v[i];
}

double vecNorm1(){
  int i; double d=0.0;
  for(i=0; i < NDIM; i++)
    d += val[i];
  return d;
}

double vecSqNorm2(){
  int i; double d=0.0;
  for(i=0; i < NDIM; i++)
    d += (val[i]*val[i]);
  return d;
}

void vecAdd(double *v);

void vecStore(double *v);

};


void vector::vecAdd(double *v){
  int i;
  for(i=0; i < NDIM; i++){
    val[i] += v[i];
  }
}

void vector::vecStore(double *v){
  int i;
  for(i=0; i < NDIM; i++){
     v[i] = val[i];
  }
}

class matrix {
  double val[NDIM][NDIM];
public:
  matrix()      { matClr(); }
  ~matrix()     { matClr(); }
double  matGetVal(int row, int col) { return val[row][col]; }

void    matClr()        {
          int i, j;
          for(i = 0; i < NDIM; i++)
            for(j = 0; j < NDIM; j++)
              val[i][j] = 0.0;
        }

void    matSetId()      {
          int i;
          matClr();
          for(i=0; i < NDIM; i++)
            val[i][i] = 1.0;
        }

void    matSet(matrix *m) {
          int i, j;
          for(i = 0; i < NDIM; i++)
            for(j = 0; j < NDIM; j++)
              val[i][j] = m->matGetVal(i,j);
        }

void    matTrans()      {
          int i, j;
          for(i=0; i < NDIM; i++)
            for(j=0; j < NDIM; j++)
              val[i][j] = val[j][i];
        }

void    matAdd(matrix *m) {
          int i, j;
          for(i = 0; i < NDIM; i++)
            for(j = 0; j < NDIM; j++)
              val[i][j] += m->matGetVal(i,j);
        }

void    matSub(matrix *q, matrix *r) {
          int i, j;
          for(i = 0; i < NDIM; i++)
            for(j = 0; j < NDIM; j++)
              val[i][j] = q->matGetVal(i,j) - r->matGetVal(i,j);
        }

void    matMult(matrix *q, matrix *r) {
          int i, j, k;
          for(i=0; i < NDIM; i++)
            for(j=0; j < NDIM; j++) {
              val[i][j] = 0.0;
              for(k = 0; k < NDIM; k++)
                val[i][j] += q->matGetVal(i,k) * r->matGetVal(k,j);
            }
        }

void    matScale(matrix *q, double f){
          int i, j;
          for(i = 0; i < NDIM; i++)
            for(j = 0; j < NDIM; j++)
              val[i][j] = q->matGetVal(i,j) * f;
        }

double  matTrace()      { int i; double s;
          s = 0.0;
          for(i=0; i < NDIM; i++)
            s += val[i][i];
          return s;
        }

void    matVectMult(double *v, double *u) {
          int i, j;
          for(i=0; i < NDIM; i++) {
            v[i] = 0.0;
            for(j=0; j < NDIM; j++)
              v[i] += val[i][j] * u[j];
          }
        }

void    matOutProd(double *v, double *u){
          int i, j;
          for(i=0; i < NDIM; i++)
            for(j=0; j < NDIM; j++)
              val[i][j] = v[i] * u[j];
        }

void    matPrint(){ int i, j;
          cout << "[";
          for(i=0; i < NDIM; i++){
            cout << "(";
            for(j=0; j < NDIM; j++)
              cout << val[i][j] << " ";
            cout << ")";
          }
          cout << "]";
        }
};

// ----------- FILE: body.h ----------
// BH-tree capacity constants
const int LEAFMAXBODIES = 10;
const int NSUB = 8; 			// (1 << NDIM)
const int IMAX	= 1073741824;		// (1 << MAXLEVEL)

const int CELL = 01;
const int LEAF = 02;
const int BODY = 04;

class node {	// inherited by all CELL, LEAF and BODY classes
public:
   int type;
   double mass;      /* total mass of node */
   vector pos;       /* position of node */
   int level;
   node *parent;     /* ptr to parent of this node in tree */
   int child_num;    /* Index that this node should be put at in parent cell */
	node()		{ type = 0; mass = 0.0; pos.vecClr(); level = 0; parent = NULL; child_num =0;}
	~node()		{ }
int 	Type()		{ return type; }
double	Mass()		{ return mass; }
vector  *Pos()  	{ return &pos; }
int	Level()  	{ return level; }
node 	*Parent()  	{ return parent; }
int 	ChildNum()  	{ return child_num; }
void    setChildNum(int n) { child_num = n; }
void	setKind(int t)	{ type = t; }
void	setMass(double m) { mass = m; }
void	setLevel(int d)	{ level =d; }
void	setParent(node *p) { parent = p; }
void	PosvecStore(double *p) { pos.vecStore(p); }
void	PosvecLoad(double *p) { pos.vecLoad(p); }
void	PosvecAdd(double *p) { pos.vecAdd(p); }
void	clear()		{ mass= 0.0; pos.vecClr(); level = 0;
			  parent = NULL; child_num =0; }
void	adjLevel(node *p) {
			  if (p == NULL) level = (IMAX >> 1);
			  else level = (p->Level() >> 1);
			}
int	subindex(int *x, int lev);
void	computecofm();
};

// ----------------------------------------------------------------------------
// BODY: data structure used to represent particles.
// ----------------------------------------------------------------------------
class body : public node {

   double  subdiv(node *p, double *ai);
public:
   vector vel;          /* velocity of body                             */
   vector acc;          /* acceleration of body                         */
   double phi;          /* potential at body                            */
   vector tmp;          /* Skratch vector		                */
        body()		{ type = BODY; vel.vecClr(); acc.vecClr(); tmp.vecClr(); phi = 0.0; }
        ~body()         { }
vector  *Vel()          { return &vel; }
vector  *Acc()          { return &acc; }
double  Phi()           { return phi; }
vector  *Tmp()          { return &tmp; }
void 	BodyClear()	{ node::clear();
			  vel.vecClr(); acc.vecClr(); tmp.vecClr();
			  phi = 0.0;
			}
void	VelvecAdd(double *p)	{ vel.vecAdd(p); }
void	VelvecStore(double *p)	{ vel.vecStore(p); }
void	VelvecLoad(double *p)	{ vel.vecLoad(p); }
void	AccvecStore(double *p)	{ acc.vecStore(p); }

double  computeInter(node *p, double epsSq, double *res);
void	openCell(node *n, double tolsq, double dsq, double epssq);
void	openLeaf(node *n, double epssq, int iter);
void 	gravsub(node *p, double epssq);
void 	updatePhi(double inc);
void    fastgravsub(node *p, double *ai, double drsq, double epssq);
void   	walksub(node *n, double tolsq, double dsq, double epssq);
void	advance(double hts, double ts);
void	swapAcc();
void 	startVel(double f);
#ifdef CHECK
void dump();
#endif
};

// ----------------------------------------------------------------------------
// LEAF: structure used to represent leaf nodes of tree.
// ----------------------------------------------------------------------------
class cell;
class leaf : public node {
   int num_bodies;
   body *bodyp[LEAFMAXBODIES];    // bodies of leaf
public:
        leaf()          { type = LEAF; }
        ~leaf()         { }
body    *Bodyp(int idx) { return bodyp[idx]; }
void    setBodyp(int idx, body *p) { bodyp[idx] = p; }
int     NumBody()       { return num_bodies; }
void    setNumBody(int n) { num_bodies = n; }
int     full()          { return (num_bodies == LEAFMAXBODIES); }
void    LeafClear()         { int i;
                          node::clear();
                          num_bodies = 0;
                          for (i = 0; i < LEAFMAXBODIES; i++)
                            bodyp[i] = NULL;
                        }

void    Subdivide(cell *ce, int idx);
void    LeafAddBody(body *bp);
};

// ----------------------------------------------------------------------------
// CELL: structure used to represent internal nodes of tree.
// ----------------------------------------------------------------------------
class cell : public node {
   node *subp[NSUB];    // descendents of cell
public:
        cell()          { type = CELL; }
        ~cell()         { }
node    *Subp(int idx)  { return subp[idx]; }
void    setSubp(int idx, node *p)  { subp[idx] = p; }
void    CellClear()         { int i;
                          node::clear();
                          for (i = 0; i < NSUB; i++)
                            subp[i] = NULL;
                        }
void CellAddBody(body *p, int *coords);
};

// ----------------------------------------------------------------------------
// CELLSET: set of cells.
// ----------------------------------------------------------------------------
class cellset {
  int	numcell;       	/* no. cells  used in celltab 	*/
  int	maxcell;       	/* max no. of cells  allocated 	*/
  cell	*celltab;	/* array of cells allocated 	*/
public:
  cellset(int max)	{ int i;
			  celltab = new cell[max]; maxcell=max; numcell=0;
			  for(i=0; i < maxcell; i++)
			    celltab[i].clear();
			}
  ~cellset()		{ delete[] celltab; maxcell=0; numcell=0; }
void	CellSetclear()	{ int i;
			  for(i=0; i < maxcell; i++)
			    celltab[i].clear();
			}
void 	CellSetReset() 	{ numcell = 0; }
 cell	*newcell();
 cell	*makecell(node *parent);
 cell	*getcell(int idx) { return &celltab[idx]; }
};

// ----------------------------------------------------------------------------
// LEAFSET: set of leaves.
// ----------------------------------------------------------------------------
class leafset {
  int	numleaf;       	/* no. leaves used in leafctab 	*/
  int	maxleaf;       	/* max no. of leaves allocated 	*/
  leaf	*leaftab;	/* array of leaves allocated 	*/
public:
  leafset(int max)	{ leaftab = new leaf[max]; maxleaf=max; numleaf=0; LeafSetClear(); }
  ~leafset()		{ delete[] leaftab; maxleaf=0; numleaf=0;}
void	LeafSetClear()	{ int i; for(i=0; i < maxleaf; i++)
			    leaftab[i].clear();
			}
void 	LeafSetReset() 	{ numleaf = 0; }
leaf* newleaf();
leaf* makeleaf(node *parent);
leaf *getleaf(int idx)	{ return &leaftab[idx]; }
// void  add_body(body *bp);
};

// ----------------------------------------------------------------------------
// BODYSET: set of particle.
// ----------------------------------------------------------------------------
class bodyset {
  int   numbody;        /* no. bodies used in bodytab   */
  int   maxbody;        /* max no. of bodies allocated  */
  body  *bodytab;       /* array of bodies   allocated  */
public:
  bodyset(int m)      { int i; bodytab = new body[m]; maxbody=m; numbody=0; for(i=0; i < maxbody; i++) bodytab[i].clear(); }
  ~bodyset()            { delete[] bodytab; maxbody=0; numbody=0;}
void  BodySetClear()    { int i; for(i=0; i < maxbody; i++)
                            bodytab[i].clear();
                        }
void  BodySetReset()         { numbody = 0; }
body* getbody(int idx)  { return &bodytab[idx]; }
body* newbody();
  int size() { return numbody; }
};

class parms;
class NBodySystem {
  int nbody;
  cell* BH_tree;
  cellset* cells;
  leafset* leaves;
  bodyset* bodies;

  double mtot;
  vector *etot;
  matrix *keten;
  matrix *peten;
  vector *cmphasePos;
  vector *cmphaseVel;
  vector *amvec;

  vector *mincorner;
  vector *maxcorner;

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
  void pickshell(double *v, double rad);
public:
  NBodySystem() {  nbody = 0; cells = NULL; leaves = NULL; bodies = NULL; }
  ~NBodySystem() { delete cells; delete leaves; delete bodies; nbody = 0; }
  void init();
  void intcoord(int *xp, double *rp);
  void updateInter(int t) { if(t == BODY) n2bcalc++; else nbccalc++; }
bodyset *getbodies()    { return bodies; }
leafset *getleaves()    { return leaves; }
cellset *getcells()     { return cells; }
  void loadData(char *name);
  void loadTestData();
  void clearStats();
  void updateStats();
  void outputStats();
  void updateEnergy();
  void outputEnergy();
  void clearTiming();
  void outputTiming();
  void initOutput(parms *p);
  void output(parms *p);
  void startrun(parms *p);
  void stepsystem(parms *p);
  void setbound();
  void maketree();
  void SwapAccs();
  void Advance(double hts, double ts);
  void ComputeAccels(double tolerance, double eps);
  void StartVels(double halftimestep);
  void computeForces(double tol, double hts, double eps);
#ifdef CHECK
void dump();
#endif
};

// ----------- FILE: simulation.h ----------
class parms {
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

public:
        parms();
        ~parms(){ }
  int     getiparam(const char *name);
  double  getdparam(const char *name);
  void    getparam(const char *name);

  void    setparms(char **fnames, double *args);
  void    loadInfile();
  void    loadParms();
char    *getheadline()  { return headline; }
char    *getinfile()    { return infile; }
char    *getoutfile()   { return outfile; }
double  getdtime()      { return dtime; }
double  getdtout()      { return dtout; }
double  gettstop()      { return tstop; }
double  getfcells()     { return fcells; }
double  getfleaves()    { return fleaves; }
double  gettol()        { return tol; }
double  gettolsq()      { return tolsq; }
double  geteps()        { return eps; }
double  getepssq()      { return epssq; }
double  getdthf()       { return dthf; }
};

// --- Global variables ---------------------------------------------------
NBodySystem Nbody;
parms simparms;
char    buf[128];

// ### CODE BELOW  ###################################################
// ### CODE BELOW  ###################################################
// ### CODE BELOW  ###################################################


#ifdef CHECK
// ----------------------------------------------------------------------------
void NBodySystem::dump(){
  int i;
  body *p;
  printf ("## mass\t (position)\t (velocity)\t (acceleration)\n");
  for (i = 0; i < nbody ; i++){
    p = bodies->getbody(i);
    p->dump();
  }
}
#endif

#ifdef CHECK
void body::dump(){
 printf("%8.4f ",mass);
 pos.vecPrint();
 vel.vecPrint();
 acc.vecPrint();
 printf("\n");
}
#endif



// ---------------------------------------------------------------------------
void NBodySystem::init() {
  etot = new vector;
  keten = new matrix;
  peten = new matrix;
  cmphasePos = new vector;
  cmphaseVel = new vector;
  amvec = new vector;
  mincorner = new vector;
  maxcorner = new vector;
}

// ---------------------------------------------------------------------------
void NBodySystem::clearStats(){
  nstep = 0;
  n2bcalc = 0;
  nbccalc = 0;
}

// --- NBodySystem ---------------------------------------------------
void NBodySystem::intcoord(int *xp, double *rp){
  int k;
  double xsc, v[NDIM];

  mincorner->vecStore(v);
  for(k = 0; k < NDIM; k++) {
    xsc = (rp[k] - v[k]) / size;
      xp[k] = (int)(floor(IMAX * xsc));
  }
}

// ---------------------------------------------------------------------------
void NBodySystem::updateStats(){
  int nttot, nbavg, ncavg;
  nttot = n2bcalc + nbccalc;
  nbavg = (int) ((double) n2bcalc / (double) nbody);
  ncavg = (int) ((double) nbccalc / (double) nbody);
}

void NBodySystem::outputStats(){
  cout << "\n\nINTERACTIONS\tTOTAL\tITER\tAVG\n";
  cout << "body-body\t\t"<< n2bcalc << "\t" << n2bcalc/nstep<< "\t" <<
    (int)sqrt((double)(n2bcalc/nstep)) << "\n";
  cout << "body-cell\t\t"<< nbccalc << "\t" << nbccalc/nstep << "\t" <<
    (int)sqrt((double)(nbccalc/nstep)) << " \n";;
}

// ----------------------------------------------------------------------------
void NBodySystem::updateEnergy(){
  int k;
  matrix m1;
  double v0[3], v1[3], v2[3], v3[3], v4[3], m, velsq;
  body *p;

  mtot = 0.0;
  etot->vecClr();
  keten->matClr();
  peten->matClr();
  cmphasePos->vecClr();
  cmphaseVel->vecClr();
  amvec->vecClr();

  tvecClr(v4);
  for(k = 0; k < nbody; k++) {
    p = bodies->getbody(k);
    m = p->Mass();
    mtot += m;
    p->VelvecStore(v1);
    velsq = tvecDotProd(v1,v1);
    v4[1] += 0.5 * m * velsq;
    v4[2] += 0.5 * m * p->Phi();
    p->PosvecStore(v1);
    p->VelvecStore(v2);
    p->AccvecStore(v3);
    tvecLoad(v0,v2);
    tvecScale(v0,0.5*m);
    m1.matOutProd(v0,v2);
    keten->matAdd(&m1);
    tvecLoad(v0,v1);
    tvecScale(v0,m);
    m1.matOutProd(v0,v3);
    peten->matAdd(&m1);
    tvecLoad(v0,v1);
    tvecScale(v0,m);
    cmphasePos->vecAdd(v0);
    tvecLoad(v0,v2);
    tvecScale(v0,m);
    cmphaseVel->vecAdd(v0);
    tvecCrossProd(v0,v1,v2);
    tvecScale(v0,m);
    amvec->vecAdd(v0);
  }
   v4[0] = v4[1] + v4[2];
   etot->vecLoad(v4);

   if (mtot != 0.0){
      cmphasePos->vecScale(1.0/mtot);
      cmphaseVel->vecScale(1.0/mtot);
   }
}


// ----------------------------------------------------------------------------
void NBodySystem::outputEnergy(){
  cout << "\n\nENERGY TOTALS\n";
  cout << " MASS:        "<< mtot << "\n";
  cout << " ENERGY:     "; etot->vecPrint(); cout << "\n";
  cout << " KIN ENERGY: "; keten->matPrint(); cout << "\n";
  cout << " POT ENERGY: "; peten->matPrint(); cout << "\n";
  cout << " AGGREGATE CM POS: "; cmphasePos->vecPrint(); cout << "\n";
  cout << " AGGREGATE CM VEL: "; cmphaseVel->vecPrint(); cout << "\n";
  cout << " ANGULAR MOMENTUM: "; amvec->vecPrint(); cout << "\n\n";
}

// ----------------------------------------------------------------------------
void NBodySystem::output(parms *sim) {
  if( (tout - 0.01 * sim->getdtime()) <= tnow)
    tout += sim->getdtout();
}

// ----------------------------------------------------------------------------
void NBodySystem::clearTiming(){
   tracktime = 0;
   updatetime = 0;
   partitiontime = 0;
   treebuildtime = 0;
   forcecalctime = 0;
}

// ----------------------------------------------------------------------------
void NBodySystem::initOutput(parms *p){
  cout << "\n\t\tOutput: " << p->getheadline() << "\n\n";
  cout << "nbody   dtime   eps   tol   dtout   tstop   fcells\n";
  cout << nbody << "     " << p->getdtime() << "   " << p->geteps()
      << "   " << p->gettol() << "   " << p->getdtout() << "    "
      << p->gettstop() << "   " << p->getfcells() << "\n\n";
}

// ----------------------------------------------------------------------------
void NBodySystem::outputTiming(){
  int ticks;
  double dticks;

  get_ticks_time(&ticks);
  dticks = ticks;
  cout << "COMPUTESTART  = " <<
    (computestart / dticks) << "\n";
  cout << "COMPUTEEND    = " <<
    (computeend / dticks) << "\n";
  cout << "COMPUTETIME   = " <<
    ((computeend - computestart) / dticks) << "\n";
  cout << "TRACKTIME     = " <<
    (tracktime / dticks) << "\n";
  cout << "PARTITIONTIME = " <<
    (partitiontime / dticks) << "\t"
        << ((float)partitiontime)/tracktime << "\n";
  cout << "TREEBUILDTIME = " <<
    (treebuildtime / dticks) << "\t"
        << ((float)treebuildtime)/tracktime << "\n";
  cout << "FORCECALCTIME = " <<
    (forcecalctime / dticks) << "\t"
        << ((float)forcecalctime)/tracktime << "\n";
  cout << "UPDATEPOSTIME = " <<
    (updatetime / dticks) << "\t"
        << ((float)updatetime)/tracktime << "\n";
  cout << "RESTTIME      = " <<
    ((tracktime - partitiontime -
     updatetime - treebuildtime - forcecalctime)
    / dticks) << "\t"
        << ((float)(tracktime-partitiontime- updatetime - treebuildtime-
        forcecalctime))/tracktime<<"\n";
  cout.flush();
}

// ----------------------------------------------------------------------------
void NBodySystem::loadData(char *name){
  int i, ndim;
  body *p;
  double m, tmp[3];
  ifstream instr(name);

  cerr << "Reading input file : " << name << '\n';
  cerr.flush();
  if (!instr)
     cerr << "loadData: cannot find file " << name << '\n';
  instr >> nbody;
  if (nbody < 1)
     cerr << "loadData: nbody = " << nbody << " is absurd\n";
  instr >> ndim;
  if(ndim != NDIM)
    cerr << "inputdata: NDIM = " << NDIM << "ndim = " << ndim << " is absurd\n";
  instr >> tnow;

  for(i = 0; i < nbody; i++){
    p = bodies->getbody(i);
    instr >> m;
    p->setMass(m);
  }
  for(i = 0; i < nbody; i++){
    p = bodies->getbody(i);
    in_vector(instr,tmp);
    p->PosvecLoad(tmp);
  }
  for(i = 0; i < nbody; i++){
    p = bodies->getbody(i);
    in_vector(instr,tmp);
    p->VelvecLoad(tmp);
  }
  instr.close();
}

// ----------------------------------------------------------------------------
void NBodySystem::loadTestData(){
  double rsc, vsc, r, v, x, y, offset, tmp;
  double cmr[NDIM], cmv[NDIM], tmpv[NDIM], tmpv2[NDIM];
  body *p, *cp;
  int halfnbody, i, k;

  tnow = 0.0;
  rsc = 9 * PI / 16;
  vsc = sqrt(1.0 / rsc);

  tvecClr(cmr);
  tvecClr(cmv);

  srand(123);



  halfnbody = nbody / 2;
  if (nbody % 2 != 0) halfnbody++;

  for(k = 0; k < halfnbody; k++) {
    p = bodies->newbody();
    //p->setMass((double)(1.0/nbody));
    //p->setMass(0.1 + 0.9*((double)rand()/INT_MAX));
    //p->setMass(0.5);

    /* reject radii greater than 10 */
    do {
      tmp = xrand(0.0, 0.999);
      r = 1.0 / sqrt(pow(tmp, -2.0/3.0) - 1);
    } while (r > 9.0);

    pickshell(tmpv, rsc * r);
    p->PosvecAdd(tmpv);
    tvecAdd(cmr,tmpv);
    do{
      x = xrand(0.0, 1.0);
      y = xrand(0.0, 0.1);
    } while (y > x*x * pow(1 - x*x, 3.5));
    v = sqrt(2.0) * x / pow(1 + r*r, 0.25);
    pickshell(tmpv, vsc * v);
    p->VelvecAdd(tmpv);
    tvecAdd(cmv,tmpv);
  }

  offset = 4.0;
  for(k = halfnbody; k < nbody; k++) {
    p = bodies->newbody();
    //p->setMass((double)(1.0/nbody));
    //p->setMass(0.1 + 0.9*((double)rand()/INT_MAX));
    //p->setMass(0.5);


    cp = bodies->getbody(k-halfnbody);
    cp->PosvecStore(tmpv);
    tvecClr(tmpv2);
    for(i=0; i < NDIM; i++){
      tmpv2[i] = tmpv[i] + offset;
      tvecAdd(cmr,tmpv2);
    }
    p->PosvecAdd(tmpv2);
    cp->VelvecStore(tmpv);
    tvecClr(tmpv2);
    for(i=0; i < NDIM; i++){
      tmpv2[i] = tmpv[i];
      tvecAdd(cmv,tmpv2);
    }
    p->VelvecAdd(tmpv2);
  }

  tvecScale(cmr, 1.0/nbody);
  tvecScale(cmv, 1.0/nbody);

  for(k=0; k < nbody; k++) {
    p = bodies->getbody(k);
    //p->setMass(0.1 + 0.9*((double)rand()/INT_MAX));
    p->setMass(0.5);

    p->PosvecStore(tmpv);
    tvecSub(tmpv,cmr);
    p->PosvecLoad(tmpv);
    p->VelvecStore(tmpv);
    tvecSub(tmpv,cmv);
    p->VelvecLoad(tmpv);
  }
}

// ----------------------------------------------------------------------------
void NBodySystem::pickshell(double *vec, double rad){
  int k;
  double rsq, rsc;

  do{
    for(k = 0; k < NDIM; k++)
      vec[k] = xrand(-1.0, 1.0);
    rsq = tvecDotProd(vec,vec);
  } while (rsq > 1.0);
  rsc = rad / sqrt(rsq);
  tvecScale(vec,rsc);
}


// ----------------------------------------------------------------------------
void NBodySystem::startrun(parms *p){
  int seed;
  char *name, headbuf[128];

  p->loadInfile();
  name = p->getinfile();
  if(strlen(name) > 1) {
    sprintf(headbuf, "Hack code: input file %s\n", name);
    strcpy(p->getheadline(),headbuf);
  } else {
    nbody = p->getiparam("nbody");
    if(nbody < 1)
     cerr << "startrun: absurd nbody\n";
    seed = p->getiparam("seed");
    strcpy(p->getheadline(),"Hack code: Plummer model");
  }

  p->loadParms();

  // allocation of data structures filing is done later
  cells = new cellset((int)(p->getfcells()*p->getfleaves()*nbody));
  leaves = new leafset((int)(p->getfleaves()*nbody));
  bodies = new bodyset(nbody);

  tout = tnow + p->getdtout();
  pranset(seed);

  name = p->getinfile();
  if(strlen(name) > 1) {
    loadData(name);
  } else {
    loadTestData();
  }

}

void print_node(NBodySystem * n, cell * parent_data, int depth){
	int i=0; int j;
	if(depth == 0)
	printf("-----------------------------\n");
	for(i=0; i < NSUB; i++){
		node * chld = parent_data->Subp(i);
		if(chld != NULL){
			if(chld->Type() == CELL){
				cell * child_data = (cell *) chld;
				for(j=0; j < depth; j++){
					printf(" ");
				}
				printf("[%d] CELL [level=%d, mass=%f, pos=(%f,%f,%f) ] ]\n", i, child_data->Level(),
																	child_data->Mass(),
																   child_data->Pos()->val[0],
																   child_data->Pos()->val[1],
																   child_data->Pos()->val[2]);
				//print_node(n, child_data, depth+1);
			}
			else if(chld->Type() == LEAF){
				leaf * child_data = (leaf *) chld;
				for(j=0; j < depth; j++){
					printf(" ");
				}
				printf("[%d] LEAF: %d NODES [level=%d, mass=%f, pos=(%f,%f,%f) ]\n", i, child_data->NumBody(),child_data->Level(),
																			   child_data->Mass(),
																			   child_data->Pos()->val[0],
																			   child_data->Pos()->val[1],
																			   child_data->Pos()->val[2]);
			}
			else{
				for(j=0; j < depth; j++){
					printf(" ");
				}
				printf("UNKNOWN %d\n", chld->Type());
			}
		}
	}
	if(depth == 0)
	printf("-----------------------------\n");
}

// ----------------------------------------------------------------------------
void NBodySystem::stepsystem(parms *p){
  int i;
  body *bptr;
  double incr, hts, ts, stop, tol, eps;

  int trackstart, trackend;
  int treebuildstart, treebuildend;
  int forcecalcstart, forcecalcend;
  int updatestart, updateend;
  int totalstep = 0;

  incr = p->getdtime();
  hts = p->getdthf();
  ts = p->getdtime();
  stop = p->gettstop();
  tol = p->gettolsq();
  eps = p->getepssq();

  while (tnow < stop + 0.1 * incr){
    get_time(&trackstart);
    leaves->LeafSetReset();
    cells->CellSetReset();
    BH_tree = cells->makecell(NULL);

    get_time(&treebuildstart);

    maketree();
    ((node*)BH_tree)->computecofm();
    //print_node(this, BH_tree, 0);
    
    get_time(&treebuildend);
    treebuildtime += treebuildend - treebuildstart;

    get_time(&forcecalcstart);
    computeForces(tol,hts,eps);
    get_time(&forcecalcend);
    forcecalctime += forcecalcend - forcecalcstart;

    /* advance bodies */
    get_time(&updatestart);
    Advance(hts,ts);
/*
    for(i = 0; i < nbody; i++) {
       bptr = bodies->getbody(i);
       bptr->advance(hts, ts);
    }
*/

    get_time(&updateend);
    updatetime += updateend - updatestart;
    get_time(&trackend);
    tracktime += trackend - trackstart;

    /* compute bounding box */
    setbound();

    nstep++;
    tnow = tnow + incr;
    totalstep++;

    updateEnergy();
    outputEnergy();
	
    if (totalstep == 2) {
      reset_stat();
      clearTiming();
      clearStats();
      get_time(&computestart);
    }
  }
  get_time(&computeend);
  print_stat();
  computeend = computeend - computestart;
  computestart = 0;

}


// --- PARMS CODE ------------------------------------------------------------
parms::parms()  {
  int i;
  // SET DEFAULT PARAMETER VALUES
  strcpy(headline,"");
  strcpy(infile,"");
  strcpy(outfile,"");
  dtime = 0.0;
  dtout = 0.0;
  tstop = 0.0;
  fcells = 0.0;
  fleaves = 0.0;
  tol = 0.0;
  tolsq = 0.0;
  eps = 0.0;
  epssq = 0.0;
  dthf = 0.0;

  for(i = 0; i < 12; i++)
    defaults[i] = (char*)malloc(MAX_NAME);

   /* file names for input/output                                 */
  strcpy(defaults[0],"in=");    /* snapshot of initial conditions  */
  strcpy(defaults[1],"out=");   /* stream of output snapshots      */
  /* params, used if no input specified, to make a Plummer Model   */
  strcpy(defaults[2],"nbody=1024");  /* number of particles to generate */
  strcpy(defaults[3],"seed=123");    /* random number generator seed    */
  /* params to control N-body integration                          */
  strcpy(defaults[4],"dtime=0.025"); /* integration time-step      */
  strcpy(defaults[5],"eps=0.05");    /* usual potential softening  */
  strcpy(defaults[6],"tol=1.0");     /* cell subdivision tolerence */
  strcpy(defaults[7],"fcells=2.0");  /* cell allocation parameter  */
  strcpy(defaults[8],"fleaves=0.5"); /* leaf allocation parameter  */
  strcpy(defaults[9],"tstop=0.075"); /* time to stop integration   */
  strcpy(defaults[10],"dtout=0.25"); /* data-output interval       */
  strcpy(defaults[11],"");
}

void parms::setparms(char **fnames, double *args){
  strcpy(headline,fnames[0]);
  strcpy(infile,fnames[1]);
  strcpy(outfile,fnames[2]);
  dtime = args[0];
  dtout = args[1];
  tstop = args[2];
  fcells = args[3];
  fleaves = args[4];
  tol = args[5];
  tolsq = args[6];
  eps = args[7];
  epssq = args[8];
  dthf = args[9];
}


int parms::getiparam(const char *name){
  getparam(name);
  return (atoi(buf));
}

// ---------------------------------------------------------------------------
double parms::getdparam(const char *name){
   getparam(name);
   return (atof(buf));
}

void parms::getparam(const char *name) {
  int i, leng;
  char *temp;

  buf[0] = '\0';
  if(defaults == NULL){
    cerr << "getparam: called before initparam\n";
  }
  i = scanbind(defaults, (char *) name);
  if(i < 0){
     cerr << "getparam: " << name << " unknown\n";
  }
  temp = extrvalue(defaults[i]);
  fgets(buf, 128, stdin);
  leng = strlen(buf) + 1;
  if(leng <= 1)
    strcpy(buf,temp);
}

// ---------------------------------------------------------------------------
void parms::loadInfile(){
  getparam("in");
  strcpy(infile,buf);
}

// ---------------------------------------------------------------------------
void parms::loadParms(){
  getparam("out");
  strcpy(outfile,buf);
  dtime = getdparam("dtime");
  dthf = 0.5 * dtime;
  eps = getdparam("eps");
  epssq = eps*eps;
  tol = getdparam("tol");
  tolsq = tol*tol;
  fcells = getdparam("fcells");
  fleaves = getdparam("fleaves");
  tstop = getdparam("tstop");
  dtout = getdparam("dtout");
}



// --- CELLSET CODE ------------------------------------------------------------
cell *cellset::newcell(){
  cell *c;
  if(numcell == maxcell){
    cerr << "newcell: More than " << maxcell << " cells; increase fcells\n";
    return NULL;
  }
  c = &celltab[numcell];
  numcell++;
  c->CellClear();
  return c;
}
// ---------------------------------------------------------------------------
cell *cellset::makecell(node *parent) {
  cell *c;
  c = newcell();
  c->setParent(parent);
  if(c == NULL) return NULL;
  c->adjLevel(parent);
  return c;
}


// --- LEAFSET CODE ------------------------------------------------------------
leaf *leafset::newleaf() {
  leaf* p;
  if(numleaf == maxleaf){
    cerr << "newleaf: More than " << maxleaf << " leaves; increase fleaves\n";
    return NULL;
  }
  p = &leaftab[numleaf];
  numleaf++;
  p->LeafClear();
  return p;
}

// ---------------------------------------------------------------------------
leaf *leafset::makeleaf(node *parent){
  leaf* l;
  l = newleaf();
  if(l == NULL) return NULL;
  l->setParent(parent);
  l->adjLevel(parent);
  return (l);
}


// --- BODYSET CODE ------------------------------------------------------------
body* bodyset::newbody(){
  body *p;
  if(numbody == maxbody){
    cerr << "newbody: More than " << maxbody << "body; increase nbody\n";
    return NULL;
  }
  p = &bodytab[numbody];
  numbody++;
  return p;
}

// --- LEAF CODE ------------------------------------------------------------
void leaf::LeafAddBody(body *bp){                  // Assumes enough capacity
  bp->setParent(this);
  bp->setLevel(Level());
  bp->setChildNum(num_bodies);
  bodyp[num_bodies] = bp;
  num_bodies++;
}

void leaf::Subdivide(cell *parent, int idx){
  cell *c;
  leaf *le;
  double tmpv[NDIM];
  int i, index, xp[NDIM], num, Lev;
  body *loctab[LEAFMAXBODIES], *p;

  num = num_bodies;
  for(i=0; i < num; i++) {
    loctab[i] = bodyp[i];
    bodyp[i] = NULL;
  }
  num_bodies = 0;
  c = Nbody.getcells()->makecell(parent);
  c->setChildNum(child_num);
  parent->setSubp(idx,c);
  Lev = level;

  p = loctab[0];
  p->PosvecStore(tmpv);
  Nbody.intcoord(xp,tmpv);
  index = subindex(xp, Lev);
  c->setSubp(index,this);
  child_num = index;
  parent = c;
  level = (Lev >> 1);
  LeafAddBody(p);

  for(i=1; i < num; i++) {
    p = loctab[i];
    p->PosvecStore(tmpv);
    Nbody.intcoord(xp,tmpv);
    index = subindex(xp, Lev);
    le = (leaf*)(c->Subp(index));
    if(le == NULL) {
      le = Nbody.getleaves()->makeleaf(c);
      le->setChildNum(index);
      c->setSubp(index,le);
    }
    le->LeafAddBody(p);
  }
}


// --- CELL CODE ------------------------------------------------------------
void cell::CellAddBody(body *p, int *coords) {
  int Lev, kidIndex;
  node *ptr;
  leaf *le;

  kidIndex = subindex(coords,level);
  ptr = subp[kidIndex];
  Lev = (level >> 1);
  if(Lev != 0){
    if(ptr == NULL) {
      le = Nbody.getleaves()->makeleaf(this);
      le->setChildNum(kidIndex);
      subp[kidIndex] = le;
      le->LeafAddBody(p);
    } else {
      if(ptr->Type() == LEAF){
        le = (leaf*)ptr;
        if(le->full()){
          le->Subdivide(this,kidIndex);
          CellAddBody(p,coords);
        } else {
          le->LeafAddBody(p);
        }
      } else {
        ((cell*)ptr)->CellAddBody(p,coords);
      }
    }
  } else {
    cerr << " *** Error: Not enough levels in tree...(CellAddBody)\n";
  }
}

// ----------------------------------------------------------------------------
int node::subindex(int *x, int l){
  int i, k, yes;

  i = 0;
  yes = 0;
  for(k = 0; k < NDIM; k++) {
    if(((x[k] & l) && !yes) || (!(x[k] & l) && yes)) {
      i += NSUB >> (k + 1);
      yes = 1;
    }
    else yes = 0;
  }
  return (i);
}

void NBodySystem::setbound(){
  int i;
  double tmp1[NDIM], tmp2[NDIM], side;
  body *p;

  mincorner->vecSetUnit();
  mincorner->vecScale(1E99);
  maxcorner->vecSetUnit();
  maxcorner->vecScale(-1E99);
  for(i=0; i < nbody; i++) {
    p = bodies->getbody(i);
    p->PosvecStore(tmp1);
    mincorner->vecMin(tmp1);
    maxcorner->vecMax(tmp1);
  }

  side = 0.0;
  maxcorner->vecStore(tmp1);
  mincorner->vecStore(tmp2);
  tvecSub(tmp1,tmp2);
  for(i=0; i < NDIM; i++)
    if(side < tmp1[i])
      side = tmp1[i];

  for(i=0; i < NDIM; i++)
    tmp1[i] = -side/100000;
  mincorner->vecAdd(tmp1);
  size = 1.00002*side;
  /*
    printf("BOUNDS: \n   size: %f\n   mincorner:(%f,%f,%f)\n   maxcorner:(%f,%f,%f)\n",
			size,
			mincorner->val[0],mincorner->val[1],mincorner->val[2],
			maxcorner->val[0],maxcorner->val[1],maxcorner->val[2]);
 */
}


// ----------------------------------------------------------------------------
// MAKETREE: initialize tree structure for force calculation.
// ----------------------------------------------------------------------------
void NBodySystem::maketree(){
  int i, xp[NDIM];
  double v[NDIM];
  body *p;

  for(i=0; i < nbody; i++){
    p = bodies->getbody(i);
    p->PosvecStore(v);
    intcoord(xp,v);
    BH_tree->CellAddBody(p,xp);
  }
}


void node::computecofm(){
  int i;
  node *nptr;
  body *p;
  leaf *le;
  cell *ce;
  double m;
  double tmpv[NDIM];

  if(type == LEAF){
    le = (leaf*)this;
    for(i=0; i < le->NumBody(); i++) {
      p = le->Bodyp(i);
      m = p->Mass();
      mass += m;
      p->PosvecStore(tmpv);
      tvecScale(tmpv,m);
      pos.vecAdd(tmpv);
    }
    pos.vecStore(tmpv);
    tvecScale(tmpv,1.0/mass);
    pos.vecLoad(tmpv);
  } else {
    ce = (cell*)this;
    for(i=0; i < NSUB; i++){
      nptr = ce->Subp(i);
      if(nptr != NULL){
        nptr->computecofm();
        m = nptr->Mass();
        mass += m;
        nptr->PosvecStore(tmpv);
        tvecScale(tmpv,m);
        pos.vecAdd(tmpv);
      }
    }
    pos.vecStore(tmpv);
    tvecScale(tmpv,1.0/mass);
    pos.vecLoad(tmpv);
  }
}

double body::subdiv(node *p, double *res) {
  int i;
  double drsq, d;

  drsq = 0.0;
  for (i = 0; i < NDIM; i++) {
    d = p->pos.val[i]-pos.val[i];
    drsq += d*d;
    res[i] = d;
  }
  return(drsq);
}

inline double body::computeInter(node *p, double epsSq, double *res){
  int i;
  double drabs, inc, mor3;
  double drsq, d;

  drsq = epsSq;
  for (i = 0; i < NDIM; i++) {
    d = p->pos.val[i]-pos.val[i];
    drsq += d*d;
    res[i] = d;
  }
  drabs = sqrt(drsq);
  inc = p->Mass() / drabs;
  mor3 = inc / drsq;
  tvecScale(res,mor3);
  return inc;
}

void body::updatePhi(double inc) {
  phi -= inc;
}

void body::fastgravsub(node *p, double *ai, double pdrsq, double epssq) {
  double drsq;
  double drabs, inc, mor3;
  double tmpv[NDIM];

  drsq = pdrsq + epssq;
  drabs = sqrt(drsq);
  inc = p->Mass() / drabs;
//  phi -= inc;
  updatePhi(inc);
  mor3 = inc / drsq;
  tvecLoad(tmpv,ai);
  tvecScale(tmpv,mor3);
  acc.vecAdd(tmpv);
}

void body::gravsub(node *p, double epsSq){
  double phii;
  double ai[NDIM];

  phii = computeInter(p,epsSq,ai);
//  phi -= phii;
  updatePhi(phii);
  acc.vecAdd(ai);
}

inline void body::openCell(node *n, double tolsq, double dsq, double epssq){
  int i;
  node* nn;
  for(i=0; i < NSUB; i++) {
    nn = ((cell*)n)->Subp(i);
    if(nn != NULL){
      walksub(nn,tolsq,(dsq/4.0),epssq);
    }
  }
}

inline void body::openLeaf(node *n,double epssq, int iter){
  int i;
  body *bptr;
  for(i=0; i < iter; i++){
    bptr = ((leaf*)n)->Bodyp(i);
    if(bptr != this){
      gravsub((node*)bptr, epssq);
    }
  }
}

void body::walksub(node *n, double tolsq, double dsq, double epssq){
  double drsq;
  double ai[NDIM];
  drsq = subdiv(n,ai);
  if((tolsq * drsq) < dsq){
    if((n->Type()) == CELL){
      openCell(n,tolsq,dsq,epssq);
    } else {
      openLeaf(n,epssq,((leaf*)n)->NumBody());
    }
  } else {
    fastgravsub(n, ai, drsq, epssq);
  }
}

void body::advance(double hts, double ts){
  double dvel[NDIM], vel1[NDIM], dpos[NDIM];

  acc.vecStore(dvel);
  vel.vecStore(vel1);
  tvecScale(dvel,hts);
  tvecAdd(vel1,dvel);
  tvecLoad(dpos,vel1);
  tvecScale(dpos,ts);
  tvecScale(dvel,2.0);

  vel.vecAdd(dvel);
  pos.vecAdd(dpos);
}

void NBodySystem::Advance(double hts, double ts){
  int i;
  body *p;

  for(i=0; i < nbody; i++) {
    p = bodies->getbody(i);
    p->advance(hts,ts);
  }
}

void body::swapAcc() {
  double tmpv[NDIM];

  phi = 0.0;
  acc.vecStore(tmpv);
  tmp.vecLoad(tmpv);
  acc.vecClr();
}

void body::startVel(double f){
   double tmpv1[NDIM], tmpv2[NDIM];
   acc.vecStore(tmpv1);
   tmp.vecStore(tmpv2);
   tvecSub(tmpv1,tmpv2);
   tvecScale(tmpv1,f);
  vel.vecAdd(tmpv1);
}

void NBodySystem::SwapAccs(){
  int i;
  body *p;

  for(i=0; i < nbody; i++) {
    p = bodies->getbody(i);
    p->swapAcc();
  }
}

void NBodySystem::ComputeAccels(double tol, double eps){
  int i;
  body *p;

  for(i=0; i < nbody; i++) {
    p = bodies->getbody(i);
    p->walksub(BH_tree, tol, size*size, eps);
  }
}

void NBodySystem::StartVels(double hts){
  int i;
  body *p;

  for(i=0; i < nbody; i++) {
    p = bodies->getbody(i);
    p->startVel(hts);
  }
}

void NBodySystem::computeForces(double tol, double hts, double eps){
  SwapAccs();
  ComputeAccels(tol,eps);
  if(nstep > 0){
    StartVels(hts);
  }
  /*
  int i;
  body *p;
  for(i=0; i < nbody; i++) {
    p = bodies->getbody(i);
    if(i==0)
    printf("BODY [%d]: \n\
			  acc: [%f,%f,%f\n\
			  vel: [%f,%f,%f]\n\
			  tmp: [%f,%f,%f]\n\
			  phi: %f\n", 0,
			  p->acc.val[0],p->acc.val[1],p->acc.val[2],
			  p->vel.val[0],p->vel.val[1],p->vel.val[2],
			  p->acc.val[0],p->acc.val[1],p->acc.val[2],
			  p->phi);
  }
  */
}

// ----------------------------------------------------------------------------
int main(int argc, char **argv){
  Nbody.init();
  Nbody.startrun(&simparms);
  Nbody.initOutput(&simparms);
  Nbody.setbound();
  Nbody.clearTiming();
  Nbody.clearStats();

  Nbody.stepsystem(&simparms);

  //  bodyset * bset = Nbody.getbodies();
  //for (int i = 0; i < bset->size(); i++)
  //  bset->

  Nbody.outputTiming();
  //Nbody.outputStats();
  Nbody.output(&simparms);
  Nbody.outputEnergy();
  //Nbody.dump();
}

