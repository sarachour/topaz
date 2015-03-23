#ifndef WATER_H
#define WATER_H

#include "stdio.h"
#include "pin_util.h"
#define AUTOICHUNKSIZE 1
#define AUTOJCHUNKSIZE 1

#define max(f1,f2) (f1 > f2? f1: f2)


#define MAX_MOLEC 512

extern FILE * en_outfile;
extern FILE * pos_outfile;
// -------- CONSTANT_H ---------------------------------------------------

const int NDIM = 3;


const double ONE 	= (double) 1.00;
const double TWO	= (double) 2.00;
const double THREE	= (double) 3.00;
const double FIVE	= (double) 5.00;

const double ROH = 0.9572;
const double ROHI = (1.00/ROH);
const double ROHI2 = (ROHI*ROHI);
const double ANGLE = 1.824218;
const double OMAS = 15.99945;
const double HMAS = 1.007825;
const double WTMOL = (OMAS+2.00*HMAS);

/*.....UNITS USeD TO SCALe VARIABLeS (IN C.G.S.) */

const double UNITT = (1.0e-15);
const double UNITL = (1.0e-8);
const double UNITM = (1.6605655e-24);
const double BOLTZ = (1.380662e-16);
const double AVGNO = (6.0220045e23);

/* .....FORCe CONSTANTS SCALeD(DIVIDeD) BY (UNITM/UNITT**2) */

const double FC11  = (0.512596);
const double FC33  = (0.048098);
const double FC12 = (-0.0058230);
const double FC13  = (0.016452);
const double FC111 = (-0.57191);
const double FC333 = (-0.007636);
const double FC112 = (-0.001867);
const double FC113 = (-0.002047);
const double FC123 = (-0.03083);
const double FC133 = (-0.0094245);
const double FC1111  = (0.8431);
const double FC3333 = (-0.00193);
const double FC1112 = (-0.0030);
const double FC1122  = (0.0036);
const double FC1113 = (-0.012);
const double FC1123  = (0.0060);
const double FC1133 = (-0.0048);
const double FC1233  = (0.0211);
const double FC1333  = (0.006263);

/*.....WATeR-WATeR INTeRACTION PARAMeTeRS */

const double QQ = 0.07152158;
const double A1 = 455.313100;
const double B1 = 5.15271070;
const double A2 = 0.27879839;
const double B2 = 2.76084370;
const double A3 = 0.60895706;
const double B3 = 2.96189550;
const double A4 = 0.11447336;
const double B4 = 2.23326410;
const double CM = 0.45682590;
const double AB1 = (A1*B1);
const double AB2 = (A2*B2);
const double AB3 = (A3*B3);
const double AB4 = (A4*B4);
const double C1 = (1.00-CM);
const double C2 = (0.50*CM);
const double QQ2 = (2.00*QQ);
const double QQ4 = (2.00*QQ2);

// ------ EXTERN_H ---------

extern "C" void reset_stat();
extern "C" void print_stat();
extern "C" void get_time(int *v);
extern "C" void get_ticks_time(int *v);

inline double sign(double a, double b){
  if(b < 0) {
    if(a < 0)
      return a;
    return -a;
  } else {
    if(a < 0)
      return -a;
    return a;
  }
}

// ------ VECTOR_H ---------




class vector {
public:
  double val[NDIM];
  vector(){
    for (int i = 0; i < NDIM; i++) { 
      val[i] = 0.0;
    }
    DBLNUREL(val,NDIM);
  }
  ~vector(){}
void vecPrint() { printf("(%6.4f %6.4f %6.4f) ",val[0],val[1],val[2]); }
inline double vecField(int idx)		{ return val[idx]; }
inline void vecFieldAdd(int idx,double v)	{ val[idx] += v; }
inline void vecFieldClr(int idx)		{ val[idx] = 0.0; }
inline void vecFieldSet(int idx, double v)	{ val[idx] = v; }
inline void vecLoad(double *v) { int i; for(i=0; i < NDIM; i++) val[i] = v[i]; }
inline void vecStore(double *v) { int i; for(i=0; i < NDIM; i++) v[i] = val[i]; }
void vecAdd(double *v);
void vecScale(double s);

inline void vecMin(double *v){
  int i;
  for(i=0; i < NDIM; i++)
    if(val[i] > (v[i]))
      val[i] = (v[i]);
}

inline double vecDotProd(double *v){ 
  int i; double d; d = 0.0; 
  for(i=0; i < NDIM; i++) 
    d += (val[i] * v[i]); 
  return d; 
}

inline double vecNorm1(){ 
  int i; double d=0.0;
  for(i=0; i < NDIM; i++) 
    d += val[i]; 
  return d;
}

inline void vecClr()   { 
  int i; 
    for(i=0; i < NDIM; i++) 
      val[i] = 0.0; 
}

inline void vecSub(double *v) { 
  int i; 
  for(i=0; i < NDIM; i++) 
    val[i] -= v[i];
}

inline void vecProd(double *v){ 
  int i; 
  for(i=0; i < NDIM; i++) 
    val[i] *= v[i];
}

};


inline void vector::vecAdd(double *v){
  int i;
  for(i=0; i < NDIM; i++)
    val[i] += v[i];
}

inline void vector::vecScale(double s){
  int i; 
  for(i=0; i < NDIM; i++) 
    val[i] *= s;
}

// --- To be used as a scalar accumulator
class acc_double {
  double val;
public:
  acc_double()			{ 
	  val = 0.0; 
      DBLUREL(val);
  }
  ~acc_double()			{ }
  double readval()		{ return val; }
  void writeval(double d)	{ val = d; }
  void addval(double d);
};


// ----------------------------------------------------------------------------

inline void tvecClr(double *v) { int i; for(i=0; i < NDIM; i++) v[i] = 0.0; }

inline void tvecAdd(double *v1, double *v2) { int i; 
	for(i=0; i < NDIM; i++) v1[i] += v2[i];
}

inline void tvecSub(double *v1, double *v2) { int i; 
	for(i=0; i < NDIM; i++) v1[i] -= v2[i];
}

inline void tvecLoad(double *v1, double *v2) { int i; 
	for(i=0; i < NDIM; i++) v1[i] = v2[i]; 
}

inline void tvecStore(double *v1, double *v2) { int i; 
	for(i=0; i < NDIM; i++) v2[i] = v1[i]; 
}

inline void tvecDiv(double *v1, double *v2) { int i;
	for(i=0; i < NDIM; i++) v1[i] /= v2[i];
}

inline void tvecProd(double *v1, double *v2) { int i;
	for(i=0; i < NDIM; i++) v1[i] *= v2[i];
}

inline void tvecScale(double *v, double s) { int i;
	for(i=0; i < NDIM; i++) v[i] *= s;
}

inline double tvecDotProd(double *v1,double *v2) {
  int i; double d = 0.0;
  for(i=0; i < NDIM; i++)
    d += (v1[i] * v2[i]);
  return d;
}

inline void tvecMin(double *v1,double  *v2){ int i;
	for(i=0; i < NDIM; i++)
		if(v1[i] > v2[i]) v1[i] = v2[i];
}

inline double tvecNorm1(double *v){ int i; double d=0.0;
	for(i=0; i < NDIM; i++) d += v[i];
	return d;
}
// ------ H2O_H ---------

const int NDIR = 3;             /* XDIR, YDIR, ZDIR             */
const int XDIR = 0;
const int YDIR = 1;
const int ZDIR = 2;

const int NATOM = 3;		/* H1, Oxy, H2 			*/

const int MAXODR = 7;		/* Predictor/corrector order	*/
const int MAXODR2 = 9;		// (MAXODR+2);

const int DISP   = 0;		/* Displacement (position)      */
const int VEL    = 1;           /* Velocity                     */
const int ACC    = 2;           /* Acceleration                 */
const int DER_3  = 3;		/* Higher order derivatives ... */
const int DER_4  = 4;
const int DER_5  = 5;
const int DER_6  = 6;
const int FORCES = 7;

class atom {
  vector M[MAXODR2];
public:
	atom() { }
	~atom() { }
	vector *getM(int idx)		{ return &M[idx]; }
	//void setM(int idx, double *v)	{ M[idx].vecLoad(v); }
	//void addM(int idx, double *v)	{ M[idx].vecAdd(v); }
	//void clearM(int idx) 		{ M[idx].vecClr(); }
	void clearAtom() 		{ 
	  int i; 
	  for(i=0; i < MAXODR2;i++) 
	   M[i].vecClr();
	}
	void predic(int n, double *c);
	void correc(int n, double *c);
};

class skratch_pad;
class h2o {
public:
  atom H1;
  atom O;
  atom H2;
  vector V;
  h2o() { }
  ~h2o() { }
void  clear();

//vector *getH1M(int idx)   { return &(H1.M[idx]); }
//vector *getOM(int idx)    { return &(O.M[idx]); }
//vector *getH2M(int idx)   { return &(H2.M[idx]); }

void loadH1Pos(double *v) { H1.getM(DISP)->vecLoad(v); }
void loadOPos(double *v)  {  O.getM(DISP)->vecLoad(v); }
void loadH2Pos(double *v) { H2.getM(DISP)->vecLoad(v); }

void storeH1Pos(double *v) { H1.getM(DISP)->vecStore(v); }
void storeOPos(double *v)  {  O.getM(DISP)->vecStore(v); }
void storeH2Pos(double *v) { H2.getM(DISP)->vecStore(v); }

void loadH1Vel(double *v) { H1.getM(VEL)->vecLoad(v); }
void loadOVel(double *v)  {  O.getM(VEL)->vecLoad(v); }
void loadH2Vel(double *v) { H2.getM(VEL)->vecLoad(v); }

void storeH1Vel(double *v) { H1.getM(VEL)->vecStore(v); }
void storeOVel(double *v)  {  O.getM(VEL)->vecStore(v); }
void storeH2Vel(double *v) { H2.getM(VEL)->vecStore(v); }

//vector *getV()            { return &V; }
void loadV(double *v)     { V.vecLoad(v); }
void storeV(double *v)    { V.vecStore(v); }

// --------------------------------------------------------------------
void 	loadDirPos(int dir,double *v);
void	storeDirVel(int dir,double *v);
void	shiftAxis(int dir, double v);
void	shift(double *v);
// void	PosMin(vector *min);

void	kineti(vector *v);
void	bndry(double b);

void	intra_poteng(vector *v);
void	intraf();
void    vir(acc_double *v);

void	updateFields(int dest, skratch_pad *p);

void	predic(int n, double *c);
void	correc(int n, double *c);
void	scaleMomenta(int Dest, double HM, double OM);
void	dump();
};
// --------------------------------------------------------------------
// PAD: to perform the accumulation of forces during INTERF
// --------------------------------------------------------------------
class skratch_pad {
public:
  vector H1pos;	// input data
  vector  Opos;   
  vector H2pos;
  vector    VM;
  vector H1force; // output data
  vector  Oforce;
  vector H2force;
  skratch_pad() { H1pos.vecClr(); Opos.vecClr(); H2pos.vecClr(); VM.vecClr(); 
		H1force.vecClr(); Oforce.vecClr(); H2force.vecClr(); }
  ~skratch_pad() { }
  void storeVM(double *v)	{ VM.vecStore(v); }
  void storeH1pos(double *v) { H1pos.vecStore(v); }
  void storeOpos(double *v)  { Opos.vecStore(v); }
  void storeH2pos(double *v) { H2pos.vecStore(v); }
  void storeH1force(double *v) { H1force.vecStore(v); }
  void storeOforce(double *v)  { Oforce.vecStore(v); }
  void storeH2force(double *v) { H2force.vecStore(v); }
  void read_data(h2o *m);
  void update_forces(double Res[3][3]);
  void update_forces_1D(double Res[9]);
};
// --- ENSEMBLE_H -------------------------

typedef struct ENSEMBLE_T {
  int numMol;
  h2o *molecule;
  skratch_pad *pad;
  double TTMV;
  double TVIR;
  acc_double VIR;
  vector POT;
  double TKIN;
  vector KIN;
} ensemble;


typedef struct SIMPARM_T{
  double 	TLC[100];		/* Taylor Series Coeffs		*/
  double	PCC[11];		/* Predictor/Corrector Coeffs	*/

  double        R1;
  double        ELPST;

  int	IRST;
  int	NVAR;
  int	NXYZ;
  int 	NXV;
  int 	IXF;
  int 	IYF;
  int 	IZF;
  int 	IMY;
  int 	IMZ;

  double TEMP;		/* Temperature			*/
  double RHO;		/* Density			*/
  double TSTEP;		/* Time step			*/
  double BOXL;		/* Box length			*/
  double BOXH;		/* Box height			*/
  double CUTOFF;	/* Cut-Off radius		*/
  double CUT2;		/* Square of Cut-Off radius	*/

  double FKIN;
  double FPOT;

  int NMOL;			/* Number of Molecules		*/
  int NORDER;			/* Integration order		*/
  int NATMO;			/* Number of Atoms		*/
  int NATMO3;
  int NMOL1;
  int NSTEP;			/* Number of time steps		*/
  int NSAVE; 
  int NRST; 
  int NPRINT;
  int NFMC;
  int I2;

  double FHM;
  double FOM;
  double REF1;
  double REF2;
  double REF4;	
	
} simparm_t;


extern simparm_t * parms;

void spar_CONS(simparm_t * sp);
void spar_DEST(simparm_t * sp);
void spar_SYSCNS(simparm_t * sp);
void spar_CNSTNT(simparm_t * sp);
void spar_loadParms(simparm_t * sp, const char * name);

inline void spar_resetStat(simparm_t * sp) { 
	sp->ELPST=0.00; 
}
 
 
void ensemble_interfLoop1(ensemble * e);
void ensemble_INTERF(ensemble * e, int dest);
void ensemble_stepsystem(ensemble * e);
void ensemble_MDMAIN(ensemble * e);

h2o* ensemble_getMolecule(ensemble * e, int idx);
skratch_pad* ensemble_getPad(ensemble * e, int idx);
void ensemble_INTER_POTENG(ensemble * e);
void ensemble_INTRA_POTENG(ensemble * e);
void ensemble_interfLoop2(ensemble * e, int idx);
void ensemble_potengLoop1(ensemble * e);
void ensemble_potengLoop2(ensemble * e, int idx);
double ensemble_interf2_aux(skratch_pad *p1, skratch_pad *p2,
double Res1[3][3], double Res2[3][3]);
void ensemble_interPoteng2Aux(skratch_pad *p1, skratch_pad *p2, double *res);
void ensemble_interf2(ensemble * e, skratch_pad *p1, skratch_pad *p2);
void ensemble_init_ensemble(ensemble * e, int n);
void ensemble_destruct_ensemble(ensemble * e);

void ensemble_BNDRY(ensemble * e, double size);
void ensemble_INITIA(ensemble * e, const char * );
void ensemble_clearTVIR(ensemble * e);	
void ensemble_updateTVIR(ensemble * e);	
void ensemble_computeVIR(ensemble * e);
void ensemble_POTENG(ensemble * e);
void ensemble_KINETI(ensemble * e);
void ensemble_clearTKIN(ensemble * e);
void ensemble_updateTKIN(ensemble * e);
void ensemble_INTRAF(ensemble * e);
void ensemble_loadData(ensemble * e);
void ensemble_storeData(ensemble * e, int dest);
void ensemble_SCALEFORCES(ensemble * e, int dest);
void ensemble_printENERGY(ensemble * e, int iter);
void ensemble_PREDIC(ensemble * e);
void ensemble_CORREC(ensemble * e);
void ensemble_CSHIFT2(skratch_pad *p1, skratch_pad *p2,double CL[3][15],double *S);
void ensemble_DUMP(ensemble * e, int iter);
void ensemble_DUMP_ENERGY(ensemble * e, int iter);
void ensemble_DUMP_POS(ensemble * e, int iter);
void ensemble_DUMP_HEADER();
void ensemble_DUMP_NEW(ensemble *e, int iter);


#endif
