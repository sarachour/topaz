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

// -------- WATER_H -----------------------------------------------------


using namespace std;

#include <stdio.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>

// -------- CONSTANT_H ---------------------------------------------------

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

#define _OPENMP_
#include "water.h"

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

// -----------------------------------------------------------------------------
// ATOM
// -----------------------------------------------------------------------------
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

// -----------------------------------------------------------------------------
// WATER 
// -----------------------------------------------------------------------------
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
};

// ---- H2O.C -----------------------------------------------------
void skratch_pad::read_data(h2o *m) { 
  double tmp[3];

  m->storeH1Pos(tmp); 
  H1pos.vecLoad(tmp);
  m->storeOPos(tmp);
  Opos.vecLoad(tmp);
  m->storeH2Pos(tmp); 
  H2pos.vecLoad(tmp);
  m->storeV(tmp);
  VM.vecLoad(tmp);
  H1force.vecClr(); 
  Oforce.vecClr();
  H2force.vecClr();
}

void skratch_pad::update_forces(double Res[3][3]) {
  H1force.vecAdd(Res[0]); 
  Oforce.vecAdd(Res[1]); 
  H2force.vecAdd(Res[2]); 
}

void h2o::clear(){
  H1.clearAtom();
   O.clearAtom();
  H2.clearAtom();
}

void h2o::predic(int n, double *c){
  H1.predic(n,c);
   O.predic(n,c);
  H2.predic(n,c);
}

void h2o::correc(int n, double *c){
  H1.correc(n,c);
   O.correc(n,c);
  H2.correc(n,c);
}

void h2o::scaleMomenta(int Dest, double HM, double OM){
  H1.getM(Dest)->vecScale(HM); 
   O.getM(Dest)->vecScale(OM); 
  H2.getM(Dest)->vecScale(HM);
}

void h2o::loadDirPos(int dir,double *v){
  H1.getM(DISP)->vecFieldSet(dir,v[0]);
   O.getM(DISP)->vecFieldSet(dir,v[1]);
  H2.getM(DISP)->vecFieldSet(dir,v[2]);
}

// ---------------------------------------------------------------------------
void h2o::storeDirVel(int dir,double *v){
  v[0] = H1.getM(VEL)->vecField(dir);
  v[1] =  O.getM(VEL)->vecField(dir);
  v[2] = H2.getM(VEL)->vecField(dir);
}
// ---------------------------------------------------------------------------
void h2o::shiftAxis(int dir, double v){
  H1.getM(DISP)->vecFieldAdd(dir,v);
   O.getM(DISP)->vecFieldAdd(dir,v);
  H2.getM(DISP)->vecFieldAdd(dir,v);
}
// ---------------------------------------------------------------------------
void h2o::shift(double *v){
  H1.getM(DISP)->vecSub(v);
   O.getM(DISP)->vecSub(v);
  H2.getM(DISP)->vecSub(v);
}
// ---------------------------------------------------------------------------
/*
void h2o::PosMin(vector *min){ 
  min->vecMin(&(H1.M[DISP]));
  min->vecMin(&(O.M[DISP]));
  min->vecMin(&(H2.M[DISP]));
}
*/
// ---------------------------------------------------------------------------
void h2o::kineti(vector *s){
  double T1[3], T2[3], v1[3], v2[3], v3[3];
  H1.getM(VEL)->vecStore(v1);
   O.getM(VEL)->vecStore(v2);
  H2.getM(VEL)->vecStore(v3);
  tvecLoad(T1,v1);
  tvecProd(T1,T1);
  tvecLoad(T2,v3);
  tvecProd(T2,T2);
  tvecAdd(T1,T2);
  tvecScale(T1,HMAS);
  tvecProd(v2,v2);
  tvecScale(v2,OMAS);
  tvecAdd(T1,v2);
  s->vecAdd(T1);
}

// ---------------------------------------------------------------------------
void h2o::bndry(double b){
  int i;
  double t[3];

  O.getM(DISP)->vecStore(t);
  for(i = XDIR; i <= ZDIR; i++ ) {
    if(t[i] > b) {
      shiftAxis(i,-b);
    } else if (t[i] < 0.00) {
      shiftAxis(i,b);
    }
  }
} 

// ---------------------------------------------------------------------------
//  INTRA-MOLECULAR POTENTIAL ENERGY
// ---------------------------------------------------------------------------
void h2o::intra_poteng(vector *v){
  double LocPot, DTS, R1, R2, RX, COS, DT, DR1, DR2, DR1S, DR2S, DRP;
  double t1[3], t2[3], t3[3], t4[3], h1[3], Ox[3], h2[3], VM[3];
  double tmp[3];

  tvecClr(tmp);
  LocPot = 0.0;

  H1.getM(DISP)->vecStore(h1);
   O.getM(DISP)->vecStore(Ox);
  H2.getM(DISP)->vecStore(h2);

  // Compute VM = Ox.Pos * C1 + (H1.Pos + H2.Pos) *C2
  tvecLoad(t1,h1);
  tvecLoad(t2,Ox);
  tvecLoad(t3,h2);
  tvecScale(t2,C1);
  tvecAdd(t3,t1);
  tvecScale(t3,C2);
  tvecAdd(t2,t3);
  tvecLoad(VM,t2);

  // R1 = OxH1Potential();
  tvecLoad(t1,Ox);
  tvecSub(t1,h1);
  R1 = tvecDotProd(t1,t1);

  // R2 = OxH2Potential();
  tvecLoad(t1,Ox);
  tvecSub(t1,h2);
  R2 = tvecDotProd(t1,t1);

  // RX = H1xOxH2Potential();
  tvecLoad(t1,h1);
  tvecLoad(t2,Ox);
  tvecLoad(t3,h2);
  tvecLoad(t4,t2);
  tvecSub(t2,t1);
  tvecSub(t4,t3);
  RX = tvecDotProd(t2,t4);

  R1 = sqrt(R1);
  R2 = sqrt(R2);
  COS = RX/(R1*R2);
  DT = (acos(COS)-ANGLE)*ROH;
  DR1 = R1-ROH;
  DR2 = R2-ROH;
  DR1S = DR1*DR1;
  DR2S = DR2*DR2;
  DRP = DR1+DR2;
  DTS = DT*DT;

  LocPot += (FC11*(DR1S+DR2S)+FC33*DTS)*0.5+FC12*DR1*DR2+FC13*DRP*DT
     +(FC111*(DR1S*DR1+DR2S*DR2)+FC333*DTS*DT+FC112*DRP*DR1*DR2
     +FC113*(DR1S+DR2S)*DT+FC123*DR1*DR2*DT+FC133*DRP*DTS)*ROHI;
  LocPot += (FC1111*(DR1S*DR1S+DR2S*DR2S)+FC3333*DTS*DTS+
     FC1112*(DR1S+DR2S)*DR1*DR2+FC1122*DR1S*DR2S+
     FC1113*(DR1S*DR1+DR2S*DR2)*DT+FC1123*DRP*DR1*DR2*DT+
     FC1133*(DR1S+DR2S)*DTS+FC1233*DR1*DR2*DTS+
     FC1333*DRP*DTS*DT)*ROHI2;

  tmp[0] = LocPot;
  V.vecLoad(VM);
  v->vecAdd(tmp);
}

// ---------------------------------------------------------------------------
void atom::predic(int norder, double *coeffs){
  int JIZ, JI, L, f;
  double S[3], T[3];

  JIZ=2;
  for (f = 0; f < norder; f++) {
    JI = JIZ;
    tvecClr(S);
    for (L = f; L < norder; L++) {
      M[L+1].vecStore(T);
      tvecScale(T,coeffs[JI]);
      tvecAdd(S,T);
      JI++;
    } 
    M[f].vecAdd(S);
    JIZ += (norder+1);
  } 
} 

// ---------------------------------------------------------------------------
void atom::correc(int norder, double *coeffs){
  int f;
  double S[3], T[3];

  M[FORCES].vecStore(S);
  M[ACC].vecStore(T);
  tvecSub(S,T);
  for(f = 0; f < (norder+1); f++){
    tvecLoad(T,S);
    tvecScale(T,coeffs[f]);
    M[f].vecAdd(T);
  }
}

// ------------------------------------------------------------------------
// computeVIR:
// ------------------------------------------------------------------------
void h2o::vir(acc_double *v){
  double loc_vir;
  double tmp1[3], tmp2[3];

  loc_vir = 0.0;
  H1.getM(DISP)->vecStore(tmp1);
  H1.getM(FORCES)->vecStore(tmp2);
  loc_vir += tvecDotProd(tmp1,tmp2);
  O.getM(DISP)->vecStore(tmp1);
  O.getM(FORCES)->vecStore(tmp2);
  loc_vir += tvecDotProd(tmp1,tmp2);
  H2.getM(DISP)->vecStore(tmp1);
  H2.getM(FORCES)->vecStore(tmp2);
  loc_vir += tvecDotProd(tmp1,tmp2);
  v->addval(loc_vir);
}


// -----------------------------------------------------------------------
// intraf: Sets V and FORCES fields in each ATOM 
// -----------------------------------------------------------------------
void h2o::intraf(){
  double SUM, R1, R2, COS, SIN;
  double DT, DTS, DR1, DR1S, DR2, DR2S, R1S, R2S, F1, F2, F3;
  double vr1[3], vr2[3], dt1[3], dt3[3], dr11[3], dr23[3];
  double s[3], v1[3], v2[3], v3[3], h1[3], Ox[3], h2[3];

  SUM=0.0;
  R1=0.0;
  R2=0.0;

  tvecClr(s);

  H1.getM(DISP)->vecStore(h1);
   O.getM(DISP)->vecStore(Ox);
  H2.getM(DISP)->vecStore(h2);

  tvecLoad(v1,Ox);
  tvecScale(v1,C1);
  tvecLoad(v2,h1);
  tvecLoad(v3,h2);
  tvecAdd(v2,v3);
  tvecScale(v2,C2);
  tvecAdd(v1,v2);
  V.vecLoad(v1);

  tvecLoad(vr1,Ox);
  tvecLoad(v1,h1);
  tvecSub(vr1,v1);
  R1 = tvecDotProd(vr1,vr1);
  tvecLoad(vr2,Ox);
  tvecLoad(v2,h2);
  tvecSub(vr2,v2);
  R2 = tvecDotProd(vr2,vr2);
  SUM = tvecDotProd(vr1,vr2);

  R1=sqrt(R1);
  R2=sqrt(R2);

/* CALCULATE COS(THETA), SIN(THETA), DELTA(R1), DELTA(R2), AND DELTA(THETA) */
  COS=SUM/(R1*R2);
  SIN=sqrt(ONE-COS*COS);
  DT=(acos(COS)-ANGLE)*ROH;
  DTS=DT*DT;
  DR1=R1-ROH;
  DR1S=DR1*DR1;
  DR2=R2-ROH;
  DR2S=DR2*DR2;

/* CALCULATE DERIVATIVES OF R1/X1, R2/X3, THETA/X1, AND THETA/X3 */
  R1S=ROH/(R1*SIN);
  R2S=ROH/(R2*SIN);
  tvecLoad(dr11,vr1); tvecScale(dr11,(1.0/R1));
  tvecLoad(dr23,vr2); tvecScale(dr23,(1.0/R2));
  tvecLoad(dt1,dr11); tvecScale(dt1,COS);
  tvecSub(dt1,dr23); tvecScale(dt1,R1S);
  tvecLoad(dt3,dr23); tvecScale(dt3,COS);
  tvecSub(dt3,dr11); tvecScale(dt3,R2S);

/* CALCULATE FORCES */
  F1=FC11*DR1+FC12*DR2+FC13*DT;
  F2=FC33*DT +FC13*(DR1+DR2);
  F3=FC11*DR2+FC12*DR1+FC13*DT;
  F1=F1+(3.0*FC111*DR1S+FC112*(2.0*DR1+DR2)*DR2
     +2.0*FC113*DR1*DT+FC123*DR2*DT+FC133*DTS)*ROHI;
  F2=F2+(3.0*FC333*DTS+FC113*(DR1S+DR2S)
     +FC123*DR1*DR2+2.0*FC133*(DR1+DR2)*DT)*ROHI;
  F3=F3+(3.0*FC111*DR2S+FC112*(2.0*DR2+DR1)*DR1
     +2.0*FC113*DR2*DT+FC123*DR1*DT+FC133*DTS)*ROHI;
  F1=F1+(4.0*FC1111*DR1S*DR1+FC1112*(3.0*DR1S+DR2S)
     *DR2+2.0*FC1122*DR1*DR2S+3.0*FC1113*DR1S*DT
     +FC1123*(2.0*DR1+DR2)*DR2*DT+(2.0*FC1133*DR1
     +FC1233*DR2+FC1333*DT)*DTS)*ROHI2;
  F2=F2+(4.0*FC3333*DTS*DT+FC1113*(DR1S*DR1+DR2S*DR2)
     +FC1123*(DR1+DR2)*DR1*DR2+2.0*FC1133*(DR1S+DR2S)
     *DT+2.0*FC1233*DR1*DR2*DT+3.0*FC1333*(DR1+DR2)*DTS)*ROHI2;
  F3=F3+(4.0*FC1111*DR2S*DR2+FC1112*(3.0*DR2S+DR1S)
     *DR1+2.0*FC1122*DR1S*DR2+3.0*FC1113*DR2S*DT
     +FC1123*(2.0*DR2+DR1)*DR1*DT+(2.0*FC1133*DR2
     +FC1233*DR1+FC1333*DT)*DTS)*ROHI2;

  tvecLoad(v1,dr11); 
  tvecScale(v1,F1);
  tvecLoad(v2,dt1); 
  tvecScale(v2,F2);
  tvecAdd(v1,v2);
  tvecLoad(v3,v1);
  H1.getM(FORCES)->vecLoad(v1);

  tvecLoad(v1,dr23); 
  tvecScale(v1,F3);
  tvecLoad(v2,dt3); 
  tvecScale(v2,F2);
  tvecAdd(v1,v2);
  H2.getM(FORCES)->vecLoad(v1);

  tvecAdd(v3,v1);
  tvecScale(v3,-1.00);
  O.getM(FORCES)->vecLoad(v3);
}

// --------------------------------------------------------------------
void h2o::updateFields(int d, skratch_pad *p){
  double tmp[3];
   p->storeH1force(tmp);
   H1.getM(d)->vecAdd(tmp);
   p->storeOforce(tmp);
   O.getM(d)->vecAdd(tmp);
  p->storeH2force(tmp);
  H2.getM(d)->vecAdd(tmp);
}

// --------------------------------------------------------------------
void h2o::dump(){
  int i;
  for(i=0; i <= FORCES; i++){
    H1.getM(i)->vecPrint();
    O.getM(i)->vecPrint();
    H2.getM(i)->vecPrint();
    printf("\n");
  }
  printf("\n V:");
  V.vecPrint();
  printf("\n");
}

// --- SIMPARM.H --------------------------------------------------------------
class simparm {
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

public:
  simparm();
  ~simparm();
  void SYSCNS();
  void CNSTNT();
  void loadParms(const char *);

  double *getTLC()		{ return TLC; }
  double *getPCC()		{ return PCC; }

  double getR1()         { return R1; }
  double getELPST()      { return ELPST; }

  double getFKIN()      { return FKIN; }
  double getFPOT()      { return FPOT; }

  void setPS1(double *v) { R1 =v[0]; ELPST=v[1]; }
  void resetStat()       { ELPST=0.00; }

 int getIRST() 	{ return IRST; }
 int getNVAR() 	{ return NVAR; }
 int getNXYZ() 	{ return NXYZ; }
 int getNXV() 	{ return NXV; }
 int getIXF() 	{ return IXF; }
 int getIYF() 	{ return IYF; }
 int getIZF() 	{ return IZF; }
 int getIMY() 	{ return IMY; }
 int getIMZ() 	{ return IMZ; }

 void setPS2(int *v){ IRST=v[0]; NVAR=v[1]; NXYZ=v[2]; 
NXV=v[3]; IXF=v[4]; IYF=v[5]; IZF=v[6]; IMY=v[7]; IMZ=v[8]; }

 double getTEMP()	{ return TEMP; }
 double getRHO() 	{ return RHO; }
 double getTSTEP()	{ return TSTEP; }
 double getBOXL()	{ return BOXL; }
 double getBOXH()	{ return BOXH; }
 double getCUTOFF()	{ return CUTOFF; }
 double getCUT2()  	{ return CUT2; }

 void setPS3(double *v) { TEMP =v[0]; RHO=v[1]; 
TSTEP=v[2]; BOXL=v[3]; BOXH=v[4]; CUTOFF=v[5]; CUT2=v[6];}

 int getNMOL()		{ return  NMOL; }
 int getNORDER()	{ return  NORDER; }
 int getNATMO()		{ return  NATMO; }
 int getNATMO3()	{ return  NATMO3; }
 int getNMOL1()		{ return  NMOL1; }
 int getNSTEP()		{ return  NSTEP; }
 int getNSAVE()		{ return  NSAVE; }
 int getNRST()		{ return  NRST; }
 int getNPRINT()	{ return  NPRINT; }
 int getNFMC()		{ return  NFMC; }
 int getI2()		{ return  I2; }

 void setPS4(int *v){ NMOL=v[0]; NORDER=v[1]; NATMO=v[2]; 
NATMO3=v[3]; NMOL1=v[4]; NSTEP=v[5]; NSAVE=v[6]; NRST=v[7]; NPRINT=v[8]; 
NFMC=v[9]; I2=v[10]; }
 void setNFMC(int v) { NFMC = v; }

 double getFHM() { return FHM; }
 double getFOM() { return FOM; }
 double getREF1() { return REF1; }
 double getREF2() { return REF2; }
 double getREF4() { return REF4; }

 void   setPS5(double *v) {FHM=v[0];FOM=v[1];REF1=v[2]; REF2=v[3]; REF4=v[4]; }

 double computeFAC(){
   return BOLTZ*TEMP*NATMO/UNITM * pow((UNITT*TSTEP/UNITL),2.0);
 }

// void	dump();

};

// --- SIMPARM.C --------------------------------------------------------------

simparm::simparm(){
  int i;

  for(i=0; i < 100; i++)
    TLC[i] = 0.0;
  for(i=0; i < 11; i++)
    PCC[i] = 0.0;
  R1 = ELPST = 0.0;
  IRST = NVAR = NXYZ = NXV = IXF = IYF = IZF = IMY = IMZ = 0;
  TEMP = RHO = TSTEP = BOXL = BOXH = CUTOFF = CUT2 = 0.0;
  NMOL = NORDER = NATMO = NATMO3 = NMOL1 = NSTEP = NSAVE = 0;
  NRST = NPRINT = NFMC = I2 = 0;
  FHM = FOM = REF1 = REF2 = REF4 = 0.0;
}

void simparm::CNSTNT(){
  int NN,N1,K1,N;
  double TN, TK;

  N = NORDER+1;
  TLC[1] = 1.00;
  for (N1 = 2; N1<=N; N1++){  
    NN = N1-1;
    TN = NN;
    TLC[N1] = 1.00;
    TK = 1.00;
    for (K1=2;K1<=N1;K1++) { 
      TLC[(K1-1)*N+NN] = TLC[(K1-2)*N+NN+1]*TN/TK;
      NN = NN-1;
      TN = TN-ONE;
      TK = TK+ONE;
    }
  }

  PCC[2] = ONE;
  N1 = N-1;
  if((N1 == 1) || (N1 == 2)){
    cout << "***** ERROR: THE ORDER HAS TO BE GREATER THAN 2 ****\n";
  }
  if(N1 == 3){
    PCC[0] = ONE/6.00;
    PCC[1] = FIVE/6.00;
    PCC[3] = ONE/3.00;
  }
  if(N1 == 4){
    PCC[0] = (double) 19.00/120.00;
    PCC[1] = THREE/4.00;
    PCC[3] = ONE/2.00;
    PCC[4] = ONE/12.00;
  }
  if(N1 == 5){
    PCC[0] = THREE/20.00;
    PCC[1] = (double) 251.00/360.00;
    PCC[3] = (double) 11.00/18.00;
    PCC[4] = ONE/6.00;
    PCC[5] = ONE/60.00;
  }
  if(N1 ==  6){
	PCC[0] = (double) 863.00/6048.00;
	PCC[1] = (double) 665.00/1008.00;
	PCC[3] = (double) 25.00/36.00;
	PCC[4] = (double) 35.00/144.00;
	PCC[5] = ONE/24.00;
	PCC[6] = ONE/360.00;
  }
    if(N1 ==  7){
	PCC[0] = (double) 275.00/2016.00;
	PCC[1] = (double) 19087.00/30240.00;
	PCC[3] = (double) 137.00/180.00;
	PCC[4] = FIVE/16.00;
	PCC[5] = (double) 17.00/240.00;
	PCC[6] = ONE/120.00;
	PCC[7] = ONE/2520.00;
    }
}

void simparm::SYSCNS() {
  TSTEP=TSTEP/UNITT;
  NATMO=NATOM*NMOL;
  NATMO3=NATMO*3;
  BOXL=pow((NMOL*WTMOL*UNITM/RHO),(1.00/3.00));
  BOXL=BOXL/UNITL;
  BOXH=BOXL*0.50;
  CUTOFF=max(BOXH,CUTOFF);
  REF1= -QQ/(CUTOFF*CUTOFF*CUTOFF);
  REF2=TWO*REF1;
  REF4=TWO*REF2;
  CUT2=CUTOFF*CUTOFF;
  FPOT= UNITM * pow((UNITL/UNITT),2.0) / (BOLTZ*TEMP*NATMO);
  FKIN=FPOT*0.50/(TSTEP*TSTEP);
  FHM=(TSTEP*TSTEP*0.50)/HMAS;
  FOM=(TSTEP*TSTEP*0.50)/OMAS;
  NMOL1=NMOL-1;
}

void simparm::loadParms(const char * file){
  FILE * infile;

  cout << "OUTPUT FOR PERFECT CLUB BENCHMARK: MDG Revision: 1.*  Author: kipp\n";
  NORDER = 5;
  TEMP = 298.0;
  RHO = 0.9980;
  TSTEP = 1.5e-16;
  CUTOFF = 0.0;

  infile = fopen(file, "r");
  if(infile == NULL){
    cout << " *** ERROR: Cannot open LWI5\n";
    exit(-1);
  }
  fscanf(infile,"%lf",&TSTEP);
  fscanf(infile,"%d", &NMOL);
  fscanf(infile,"%d", &NSTEP);
  fscanf(infile,"%d", &NORDER);
  fscanf(infile,"%d", &NSAVE);
  fscanf(infile,"%d", &NRST);
  fscanf(infile,"%d", &NPRINT);
  fscanf(infile,"%d", &NFMC);


  CNSTNT();
  cout << "\nTEMPERATURE                = "<< TEMP << "\n";
  cout << "DENSITY                    = "<< RHO << " G/C.C.\n";
  cout << "NUMBER OF MOLECULES        = "<< NMOL << "\n";
  cout << "TIME STEP                  = "<< TSTEP << " SEC\n";
  cout << "ORDER USED TO SOLVE F=MA   = "<< NORDER << "\n";
  cout << "NO. OF TIME STEPS          = "<< NSTEP <<"\n";
  cout << "FREQUENCY OF DATA SAVING   = "<< NSAVE <<"\n";
  cout << "FREQUENCY TO WRITE RST FILE= "<< NRST <<"\n";
  cout.flush();
  SYSCNS();
  cout << "SPHERICAL CUTOFF RADIUS    = "<< CUTOFF <<" ANGSTROM\n";
  IRST=0;
  fclose(infile);
}

// --- ENSEMBLE_H -------------------------

class ensemble {
public:
  int numMol;
  h2o *molecule;
  skratch_pad *pad;
  double TTMV;
  double TVIR;
  acc_double VIR;
  vector POT;
  double TKIN;
  vector KIN;
h2o* getMolecule(int idx)  	{ return &(molecule[idx]); }
skratch_pad* getPad(int idx)	{ return &(pad[idx]); }
void INTER_POTENG();
void INTRA_POTENG();
void interfLoop1();
void interfLoop2(int idx);
void potengLoop1();
void potengLoop2(int idx);
double interf2_aux(skratch_pad *p1, skratch_pad *p2,
double Res1[3][3], double Res2[3][3]);
void interPoteng2Aux(skratch_pad *p1, skratch_pad *p2, double *res);
void interf2(skratch_pad *p1, skratch_pad *p2);
  ensemble(int n);
  ~ensemble();

void BNDRY(double size);
void INITIA(const char * );
void clearTVIR();	
void updateTVIR();	
void computeVIR();
void POTENG();
void KINETI();
void clearTKIN();
void updateTKIN();
void INTRAF();
void loadData();
void storeData(int dest);
void INTERF(int dest);
void SCALEFORCES(int dest);
void printENERGY(int iter);
void PREDIC();
void CORREC();
void CSHIFT2(skratch_pad *p1, skratch_pad *p2,double CL[3][15],double *S);
void MDMAIN();
void stepsystem();
void DUMP(int iter);
};

// -----------------------------------------------------------------------
// GLOBALS 
// -----------------------------------------------------------------------
simparm *parms;
ensemble *liquid;

ensemble::ensemble(int num){

  numMol = num;
  molecule = new h2o[numMol];
  pad = new skratch_pad[numMol];

  TTMV = 0.0;
  TVIR = 0.0;
  TKIN = 0.0;
  VIR.writeval(0.0);
  POT.vecClr();
  KIN.vecClr();
}

ensemble::~ensemble(){
  delete[] molecule;
  delete[] pad;
}

void ensemble::clearTVIR(){ 
  TVIR = 0.0;
}

void ensemble::updateTVIR(){ 
  TVIR -= VIR.readval();
}

void ensemble::BNDRY(double size) {
  int i;

  for(i = 0; i < numMol; i++){
    molecule[i].bndry(size);
  }
} 

void ensemble::INITIA(const char * file){
  vector minimum;
  double t1[3];
  FILE *random_numbers; 
  double XMAS[3],XS,ZERO,WCOS,WSIN,XT[3],YT[3],ZT[3],SU[3],FAC,SUM[3],t[3],NS;
  int i, j, k, mol;

  random_numbers = fopen(file,"r");
  if (random_numbers == NULL) { 
    cout << " *** Error: Can't open file random.in (ensemble::INITIA)\n";
    cout.flush();
    exit(0);
  }

     NS = pow((double) numMol, 1.0/3.0) - 0.00001;
     XS = parms->getBOXL()/NS;
     ZERO = XS * 0.50;
     WCOS = ROH * cos(ANGLE * 0.5);
     WSIN = ROH * sin(ANGLE * 0.5);

     cout << "***** NEW RUN STARTING FROM REGULAR LATTICE *****\n";
     cout.flush();
     XT[1] = ZERO;
     mol = 0;
     for(i=0; i < NS; i++) {
       XT[0]=XT[1]+WCOS;
       XT[2]=XT[0];
       YT[1]=ZERO;
       for(j=0; j < NS; j++) {
         YT[0]=YT[1]+WSIN;
         YT[2]=YT[1]-WSIN;
         ZT[0]=ZT[1]=ZT[2]=ZERO;
         for(k = 0; k < NS; k++) {
	   molecule[mol].loadDirPos(XDIR,XT);
	   molecule[mol].loadDirPos(YDIR,YT);
	   molecule[mol].loadDirPos(ZDIR,ZT);
           mol++;
	   if (mol == numMol) goto exitLoop;
           ZT[0] += XS; ZT[1] += XS; ZT[2] += XS;
         }
         YT[1] += XS;
       }
       XT[1] += XS;
     }

 exitLoop: if(numMol != mol) {
       cout << " *** Error: Lattice number of mol mismatch \n";
       exit(-1);
     }

  /* ASSIGN RANDOM MOMENTA */
  fscanf(random_numbers,"%lf",&SU[0]);

  tvecClr(SUM);
  for(i = 0; i < numMol; i++) {
    fscanf(random_numbers,"%lf",&t[0]);
    fscanf(random_numbers,"%lf",&t[1]);
    fscanf(random_numbers,"%lf",&t[2]);
//    fscanf(random_numbers,"%lf%lf%lf",&t[0],&t[1],&t[2]);

    molecule[i].loadH1Vel(t);
    tvecAdd(SUM,t);
    fscanf(random_numbers,"%lf",&t[0]);
    fscanf(random_numbers,"%lf",&t[1]);
    fscanf(random_numbers,"%lf",&t[2]);

//    fscanf(random_numbers,"%lf%lf%lf",&t[0],&t[1],&t[2]);
    molecule[i].loadOVel(t);
    tvecAdd(SUM,t);
    fscanf(random_numbers,"%lf",&t[0]);
    fscanf(random_numbers,"%lf",&t[1]);
    fscanf(random_numbers,"%lf",&t[2]);

//    fscanf(random_numbers,"%lf%lf%lf",&t[0],&t[1],&t[2]);
    molecule[i].loadH2Vel(t);
    tvecAdd(SUM,t);
  } 
  /* .....FIND AVERAGE MOMENTA PER ATOM */
  SUM[0] /= (NATOM*parms->getNMOL());
  SUM[1] /= (NATOM*parms->getNMOL());
  SUM[2] /= (NATOM*parms->getNMOL());

  /* FIND NORMALIZATION FACTOR SO THAT <K.E.>=KT/2 */

  tvecClr(SU);
  for (i = 0; i < numMol; i++) {
    molecule[i].storeDirVel(XDIR,t);
    SU[0] += (pow((t[0]-SUM[0]),2.0) + pow((t[2]-SUM[0]),2.0))/HMAS 
	+ pow((t[1]-SUM[0]),2.0)/OMAS;
    molecule[i].storeDirVel(YDIR,t);
    SU[1] += (pow((t[0]-SUM[1]),2.0) + pow((t[2]-SUM[1]),2.0))/HMAS 
	+ pow((t[1]-SUM[1]),2.0)/OMAS;
    molecule[i].storeDirVel(ZDIR,t);
    SU[2] += (pow((t[0]-SUM[2]),2.0) + pow((t[2]-SUM[2]),2.0))/HMAS 
	+ pow((t[1]-SUM[2]),2.0)/OMAS;
  }
  FAC=parms->computeFAC();
  SU[0]=sqrt(FAC/SU[0]);
  SU[1]=sqrt(FAC/SU[1]);
  SU[2]=sqrt(FAC/SU[2]);
  /* NORMALIZE INDIVIDUAL VELOCITIES SO THAT THERE IS NO BULK MOMENTA */

  XMAS[0] = 1.0/HMAS;
  XMAS[1] = 1.0/OMAS;
  XMAS[2] = 1.0/HMAS;

  for(i = 0; i < numMol; i++) {
    molecule[i].storeH1Vel(t1);
    tvecSub(t1,SUM); 
    tvecProd(t1,SU); 
    tvecScale(t1,XMAS[0]);
    molecule[i].loadH1Vel(t1);

    molecule[i].storeOVel(t1);
    tvecSub(t1,SUM); 
    tvecProd(t1,SU); 
    tvecScale(t1,XMAS[1]);
    molecule[i].loadOVel(t1);

    molecule[i].storeH2Vel(t1);
    tvecSub(t1,SUM); 
    tvecProd(t1,SU); 
    tvecScale(t1,XMAS[2]);
    molecule[i].loadH2Vel(t1);
  } 
} 

// ----------------------------------------------------------------------
// POTENG
// ----------------------------------------------------------------------
void ensemble::potengLoop1(){
  int i;
  int max =  numMol-1;
  for(i=0; i < max; i++) {
    potengLoop2(i);
  }
}

void ensemble::potengLoop2(int idx){
  int i;


  int max =  numMol;
  for(i=idx+1; i < max; i++) {
     double tmp[3];
    interPoteng2Aux(&pad[idx],&pad[i],tmp);
    POT.vecAdd(tmp);
  }
}

void ensemble::INTER_POTENG() {
  loadData();
  potengLoop1();
}


// ---------------------------------------------------------------------------
void ensemble::interPoteng2Aux(skratch_pad *p1, skratch_pad *p2, double *res){
  int KC,K;
  double CL[3][15], RS[14], RL[14], PotR, PotF, S[2];
  double Cut2, Cutoff, Ref1, Ref2;

  PotF = 0.0;
  PotR = 0.0;
  S[0] = parms->getBOXH();	// BoxH
  S[1] = parms->getBOXL();	// BoxL
  Cut2 = parms->getCUT2();
  Cutoff = parms->getCUTOFF();
  Ref1 = parms->getREF1();
  Ref2 = parms->getREF2();

  CSHIFT2(p1,p2,CL,S);
  KC=0;
  for(K = 0; K < 9; K++) {
    RS[K]=CL[0][K]*CL[0][K]+CL[1][K]*CL[1][K]+CL[2][K]*CL[2][K];
    if(RS[K] > Cut2) 
      KC=KC+1;
  } 

  if (KC != 9) {
     for(K = 0; K < 9; K++) {
      if(RS[K] <= Cut2){
        RL[K] = sqrt(RS[K]);
      } else {
    	RL[K] = Cutoff;
	RS[K] = Cut2;
      } 
    } 

    PotR += (-QQ2/RL[1]-QQ2/RL[2]-QQ2/RL[3]-QQ2/RL[4]
      +QQ /RL[5]+QQ /RL[6]+QQ /RL[7]+QQ /RL[8]+QQ4/RL[0]);
    PotF += (-Ref2*RS[0]-Ref1*((RS[5]+RS[6]+RS[7]+RS[8])*0.5
      -RS[1]-RS[2]-RS[3]-RS[4]));

    if (KC <= 0) {
      for (K = 9; K <  14; K++) 
	RL[K]=sqrt(CL[0][K]*CL[0][K]+CL[1][K]*CL[1][K]+CL[2][K]*CL[2][K]);
      PotR += A1 * exp(-B1*RL[9]) +A2*(exp(-B2*RL[5])+ 
          exp(-B2*RL[6])+exp(-B2*RL[7])+exp(-B2*RL[ 8]))
     	+A3*(exp(-B3*RL[10])+exp(-B3*RL[11])+exp(-B3*RL[12])
	+exp(-B3*RL[13]))-A4*(exp(-B4*RL[10])+exp(-B4*RL[11])
	+exp(-B4*RL[12])+exp(-B4*RL[13]));
    } 
  } 
  res[0] = 0.0;
  res[1] = PotR;
  res[2] = PotF;
}


// ----------------------------------------------------------------------
void ensemble::INTRA_POTENG(){
  int i;
  for(i=0; i < numMol; i++){
    molecule[i].intra_poteng(&POT);
  }
}

// ----------------------------------------------------------------------
void ensemble::POTENG(){
  POT.vecClr();
  INTRA_POTENG();
  INTER_POTENG();
  POT.vecScale(parms->getFPOT());
}

// ----------------------------------------------------------------------
// KINETI
// ----------------------------------------------------------------------
void ensemble::clearTKIN(){ 
  TKIN = 0.0; 
}

void ensemble::updateTKIN(){ 
  TKIN += KIN.vecNorm1();
}

void ensemble::KINETI(){
  int i;
  KIN.vecClr();
  for(i=0; i < numMol; i++){
    molecule[i].kineti(&KIN);
  }
  updateTKIN();
}

// ----------------------------------------------------------------------
// VIR
// ----------------------------------------------------------------------
void ensemble::computeVIR(){
  int i;

  for(i=0; i < numMol; i++){
    molecule[i].vir(&VIR);
  }
}

// ----------------------------------------------------------------------
void ensemble::printENERGY(int iter){
  double XVIR, TENN, XTT, AVGT;
  double loc_pot[3];

  POT.vecStore(loc_pot);
  XVIR = TVIR*parms->getFPOT()*0.50/((double)TTMV);
  AVGT = TKIN*parms->getFKIN()*(parms->getTEMP())*2.00/(3.00*((double)TTMV));
  TENN = KIN.vecNorm1() * parms->getFKIN();
  XTT = tvecNorm1(loc_pot)+TENN;

  if ((iter % parms->getNPRINT()) == 0){
    cout << "     "<<iter;
    cout << " "<<TENN;
    cout << " "<<loc_pot[0];
    cout << " "<<loc_pot[1];
    cout << " "<<loc_pot[2];
    cout << " "<<XTT;
    cout << " "<<AVGT;
    cout << " "<<XVIR<< "\n";;
  }
}

// ----------------------------------------------------------------------------
void ensemble::PREDIC(){
  int i, ord;
  double *coeffs;

  for(i=0; i < numMol; i++){
    ord = parms->getNORDER();
    coeffs = parms->getTLC();
    molecule[i].predic(ord,coeffs);
  }
}

// ----------------------------------------------------------------------------
void ensemble::CORREC(){
  int i, ord;
  double *coeffs;

  for(i=0; i < numMol; i++){
    ord = parms->getNORDER();
    coeffs = parms->getPCC();
    molecule[i].correc(ord,coeffs);
  }
}

// ----------------------------------------------------------------------------
// MDMAIN --- MOLECULAR DYNAMICS LOOP 
// ----------------------------------------------------------------------------
void ensemble::MDMAIN(){

  cout << "RESTART "<< parms->getIRST();
  cout << " AFTER "<< parms->getELPST() << " SECONDS\n";
  clearTVIR();
  clearTKIN();

  if(parms->getNSAVE() > 0){
    cout << "COLLECTING X AND V DATA AT EVERY %4d TIME STEPS\n";
    cout << parms->getNSAVE();
  }
  cout << "INTERMEDIATE RESULTS (ENERGY EXPRESSED IN UNITS OF KT ATOM) \n";
  cout << "  TIME       KINETIC   INTRA POT   INTER POT   REACT POT       ";
  cout << "TOTAL  \n<TEMPERATURE>   <VIR>\n";

  stepsystem();

}

// --- MAIN LOOP -------------------------------------------------------
void ensemble::stepsystem(){
  int i, n, start, stop;
  int start_serial, stop_serial, total_serial;
  int ticks;
  double dticks;

  total_serial = 0;
  get_time(&start);

  n = parms->getNSTEP();
  for(i=1;i <= n; i++) {

    TTMV += 1.00;
#if defined(DEBUG_OPT)
  cout << " >>> Step: " << i << "\n";
#endif

    get_time(&start_serial); 
    PREDIC();
#if defined(DEBUG_OPT)
  cout << "    >>> Predic: done.\n";
#endif
    INTRAF();
    VIR.writeval(0.0);
    computeVIR();
#if defined(DEBUG_OPT)
  cout << "    >>> Intraf: done.\n";
#endif
    get_time(&stop_serial); 
    total_serial += (stop_serial - start_serial);
    INTERF(FORCES);
    SCALEFORCES(FORCES);

#if defined(DEBUG_OPT)
  cout << "    >>> Interf: done.\n";
#endif
    get_time(&start_serial); 
    CORREC();
#if defined(DEBUG_OPT)
  cout << "    >>> Correc: done.\n";
#endif
    BNDRY(parms->getBOXL());
#if defined(DEBUG_OPT)
  cout << "    >>> Bondary: done.\n";
#endif
    KINETI();
#if defined(DEBUG_OPT)
  cout << "    >>> Kinetic: done.\n";
#endif
    updateTVIR();

    get_time(&stop_serial); 
    total_serial += (stop_serial - start_serial);

    if(( (i % parms->getNPRINT()) == 0) || 
	((parms->getNSAVE() > 0) && ((i % parms->getNSAVE()) == 0))) {
      POTENG();
      printENERGY(i);
    }

  } 
  get_time(&stop);
  get_ticks_time(&ticks);
  dticks = ticks;

  cout << "execution time = " << (stop-start) / dticks << "\n";
} 

void ensemble::CSHIFT2(skratch_pad *p1, skratch_pad *p2, double L[3][15], double *S){
  int i;

  // ---  XDIR ---
  double vm1,vm2,h1pos1,h1pos2,h2pos1,h2pos2,opos1,opos2;
  vm1 = p1->VM.vecField(0);
  vm2 = p2->VM.vecField(0);
  L[0][0] = vm1-vm2;
  h1pos2  = p2->H1pos.vecField(0);
  L[0][1] = vm1-h1pos2;
  h2pos2  = p2->H2pos.vecField(0);
  L[0][2] = vm1-h2pos2;
  h1pos1 = p1->H1pos.vecField(0);
  L[0][3] = h1pos1-vm2;
  h2pos1 = p1->H2pos.vecField(0);
  L[0][4] = h2pos1-vm2;
  L[0][5] = h1pos1-h1pos2;
  L[0][6] = h1pos1-h2pos2;
  L[0][7] = h2pos1-h1pos2;
  L[0][8] = h2pos1-h2pos2;
  opos1 = p1->Opos.vecField(0);
  opos2 = p2->Opos.vecField(0);
  L[0][9] = opos1-opos2;
  L[0][10] = opos1-h1pos2;
  L[0][11] = opos1-h2pos2;
  L[0][12] = h1pos1-opos2;
  L[0][13] = h2pos1-opos2;
  for (i = 0; i < 14; i++) {
    if (fabs(L[0][i]) > S[0])
      L[0][i] -= (sign(S[1],L[0][i]));
  }

  // ---  YDIR ---
  vm1 = p1->VM.vecField(1);
  vm2 = p2->VM.vecField(1);
  h1pos1 = p1->H1pos.vecField(1);
  L[1][0] = vm1-vm2;
  h1pos2 = p2->H1pos.vecField(1);
  L[1][1] = vm1-h1pos2;
  h2pos2 = p2->H2pos.vecField(1);
  L[1][2] = vm1-h2pos2;
  L[1][3] = h1pos1-vm2;
  h2pos1 = p1->H2pos.vecField(1);
  L[1][4] = h2pos1 - vm2;
  L[1][5] = h1pos1-h1pos2;
  L[1][6] = h1pos1-h2pos2;
  L[1][7] = h2pos1-h1pos2;
  L[1][8] = h2pos1-h2pos2;
  opos1 = p1->Opos.vecField(1);
  opos2 = p2->Opos.vecField(1);
  L[1][9] = opos1-opos2;
  L[1][10] = opos1-h1pos2;
  L[1][11] = opos1-h2pos2;
  L[1][12] = h1pos1-opos2;
  L[1][13] = h2pos1-opos2;
  for (i = 0; i < 14; i++) {
    if (fabs(L[1][i]) > S[0])
      L[1][i] -= (sign(S[1],L[1][i]));
  }

  // ---  ZDIR ---
  vm1 = p1->VM.vecField(2);
  vm2 = p2->VM.vecField(2);
  L[2][0] = vm1-vm2;
  h1pos2 = p2->H1pos.vecField(2);
  L[2][1] = vm1-h1pos2;
  h2pos2 = p2->H2pos.vecField(2);
  L[2][2] = vm1-h2pos2;
  h1pos1 = p1->H1pos.vecField(2);
  L[2][3] = h1pos1-vm2;
  h2pos1 = p1->H2pos.vecField(2);
  L[2][4] = h2pos1-vm2;
  L[2][5] = h1pos1-h1pos2;
  L[2][6] = h1pos1-h2pos2;
  L[2][7] = h2pos1-h1pos2;
  L[2][8] = h2pos1-h2pos2;
  opos1 = p1->Opos.vecField(2);
  opos2 = p2->Opos.vecField(2);
  L[2][9] = opos1-opos2;
  L[2][10] = opos1-h1pos2;
  L[2][11] = opos1-h2pos2;
  L[2][12] = h1pos1-opos2;
  L[2][13] = h2pos1-opos2;
  for (i = 0; i < 14; i++) {
    if (fabs(L[2][i]) > S[0])
      L[2][i] -= (sign(S[1],L[2][i]));
  }
}

// -----------------------------------------------------------------------
// INTRAF:
// -----------------------------------------------------------------------
void ensemble::INTRAF(){
  int i;
  for (i = 0; i < numMol; i++){
    molecule[i].intraf();
  }
}

// ------------------------------------------------------------------------
// interf2 - operates on the skratch pad areas
// ------------------------------------------------------------------------
double ensemble::interf2_aux(skratch_pad *p1, skratch_pad *p2, double Res1[3][3], double Res2[3][3]){

  int KC, K;
  double CL[3][15], RS[15], FF[15], RL[15], GG[15];
  double G110, G23, G45, TT1, TT, FTEMP;
  double gCUT2, gREF1, gREF2, gREF4;
  double loc_vir;
  double S[2];

  S[0] = parms->getBOXH();
  S[1] = parms->getBOXL();
  gCUT2 = parms->getCUT2();
  gREF1 = parms->getREF1();
  gREF2 = parms->getREF2();
  gREF4 = parms->getREF4();
  loc_vir = 0.0;

  CSHIFT2(p1,p2,CL,S);
  KC=0;
  for (K = 0; K < 9; K++) {
    RS[K]=CL[0][K]*CL[0][K]+CL[1][K]*CL[1][K]+CL[2][K]*CL[2][K];
    if (RS[K] > gCUT2) 
      KC++;
  } 

  if(KC != 9) {
    for (K = 0; K < 14; K++) 
        FF[K]=0.0;
    if(RS[0] < gCUT2) {
      FF[0]=QQ4/(RS[0]*sqrt(RS[0]))+gREF4;
      loc_vir = loc_vir + FF[0]*RS[0];
    } 
    for (K = 1; K < 5; K++) {
      if(RS[K] < gCUT2) { 
	FF[K]= -QQ2/(RS[K]*sqrt(RS[K]))-gREF2;
	loc_vir = loc_vir + FF[K]*RS[K];
      } 
      if(RS[K+4] <= gCUT2) { 
	RL[K+4]=sqrt(RS[K+4]);
 	FF[K+4]=QQ/(RS[K+4]*RL[K+4])+gREF1;
	loc_vir = loc_vir + FF[K+4]*RS[K+4];
      } 
    } 

    if(KC == 0) {
      RS[9]=CL[0][9]*CL[0][9]+CL[1][9]*CL[1][9]+CL[2][9]*CL[2][9];
      RL[9]=sqrt(RS[9]);
      FF[9]=AB1*exp(-B1*RL[9])/RL[9];
      loc_vir = loc_vir + FF[9]*RS[9];
      for (K = 10; K < 14; K++) { 
	FTEMP=AB2*exp(-B2*RL[K-5])/RL[K-5];
	FF[K-5]=FF[K-5]+FTEMP;
	loc_vir= loc_vir+FTEMP*RS[K-5];
	RS[K]=CL[0][K]*CL[0][K]+CL[1][K]*CL[1][K]+CL[2][K]*CL[2][K];
	RL[K]=sqrt(RS[K]);
	FF[K]=(AB3*exp(-B3*RL[K])-AB4*exp(-B4*RL[K]))/RL[K];
	loc_vir = loc_vir + FF[K]*RS[K];
      }
    } 

  /* CALCULATE X-COMPONENT FORCES */
    for (K = 0; K < 14; K++) 
      GG[K+1]=FF[K]*CL[0][K];

    G110=GG[10]+GG[1]*C1;
    G23=GG[2]+GG[3];
    G45=GG[4]+GG[5];
    Res1[1][0] = G110+GG[11]+GG[12]+C1*G23;
    Res2[1][0] = -G110-GG[13]-GG[14]-C1*G45;
    TT1=GG[1]*C2;
    TT=G23*C2+TT1;
    Res1[0][0] = GG[6]+GG[7]+GG[13]+TT+GG[4];
//    Res2[0][0] = GG[8]+GG[9]+GG[14]+TT+GG[5];
    Res1[2][0] = GG[8]+GG[9]+GG[14]+TT+GG[5];
    TT=G45*C2+TT1;
//    Res1[2][0] = -GG[6]-GG[8]-GG[11]-TT-GG[2];
    Res2[0][0] = -GG[6]-GG[8]-GG[11]-TT-GG[2];
    Res2[2][0] = -GG[7]-GG[9]-GG[12]-TT-GG[3];

  /* CALCULATE Y-COMPONENT FORCES */
    for(K = 0; K < 14; K++)
      GG[K+1]=FF[K]*CL[1][K];

    G110=GG[10]+GG[1]*C1;
    G23=GG[2]+GG[3];
    G45=GG[4]+GG[5];
    Res1[1][1] = G110+GG[11]+GG[12]+C1*G23;
    Res2[1][1] = -G110-GG[13]-GG[14]-C1*G45;
    TT1=GG[1]*C2;
    TT=G23*C2+TT1;
    Res1[0][1] = GG[6]+GG[7]+GG[13]+TT+GG[4];
//    Res2[0][1] = GG[8]+GG[9]+GG[14]+TT+GG[5];
    Res1[2][1] = GG[8]+GG[9]+GG[14]+TT+GG[5];
    TT=G45*C2+TT1;
//    Res1[2][1] = -GG[6]-GG[8]-GG[11]-TT-GG[2];
    Res2[0][1] = -GG[6]-GG[8]-GG[11]-TT-GG[2];
    Res2[2][1] = -GG[7]-GG[9]-GG[12]-TT-GG[3];

  /* CALCULATE Z-COMPONENT FORCES */
    for(K = 0; K < 14; K++)
      GG[K+1]=FF[K]*CL[2][K];

    G110=GG[10]+GG[1]*C1;
    G23=GG[2]+GG[3];
    G45=GG[4]+GG[5];
    Res1[1][2] = G110+GG[11]+GG[12]+C1*G23;
    Res2[1][2] = -G110-GG[13]-GG[14]-C1*G45;
    TT1=GG[1]*C2;
    TT=G23*C2+TT1;
    Res1[0][2] = GG[6]+GG[7]+GG[13]+TT+GG[4];
//    Res2[0][2] = GG[8]+GG[9]+GG[14]+TT+GG[5];
    Res1[2][2] = GG[8]+GG[9]+GG[14]+TT+GG[5];
    TT=G45*C2+TT1;
//    Res1[2][2] = -GG[6]-GG[8]-GG[11]-TT-GG[2];
    Res2[0][2] = -GG[6]-GG[8]-GG[11]-TT-GG[2];
    Res2[2][2] = -GG[7]-GG[9]-GG[12]-TT-GG[3];

    /* Update DESTination of calculations */
//    p1->update_forces(Res1);
//    p2->update_forces(Res2);
  } else {
    tvecClr(Res1[0]); tvecClr(Res1[1]); tvecClr(Res1[2]);
    tvecClr(Res2[0]); tvecClr(Res2[1]); tvecClr(Res2[2]);
  }
  return loc_vir;
}


void ensemble::interf2(skratch_pad *p1, skratch_pad *p2){
  double incr, Res1[3][3], Res2[3][3];

  incr = interf2_aux(p1,p2,Res1,Res2);
  p1->update_forces(Res1);
  p2->update_forces(Res2);
  VIR.addval(incr);
}

void ensemble::loadData(){
  int i;
  h2o *mol;
  skratch_pad *p;

  for(i=0; i < numMol; i++){
    mol = getMolecule(i);
    p = getPad(i);
    p->read_data(mol);
  }
}

void ensemble::storeData(int dest){
  int i;
  h2o *p1;
  skratch_pad *p2;

  for(i=0; i < numMol; i++){
    p1 = getMolecule(i);
    p2 = getPad(i);
    p1->updateFields(dest,p2);
  }
}

void ensemble::interfLoop2(int idx){
  int i;
  skratch_pad *p1, *p2;
  int max =  numMol;

  for(i = idx+1; i < max; i++){
    p1 = getPad(idx);
    p2 = getPad(i);
    interf2(p1,p2);
  }
}

void ensemble::interfLoop1(){
  int i;
  int max =  numMol-1;
  for(i = 0; i < max; i++) {
    interfLoop2(i);
  }
}


void ensemble::INTERF(int DEST){
  loadData();
  interfLoop1();
  storeData(DEST);
}

// --------------------------------------------------------------------
void ensemble::SCALEFORCES(int Dest){
  int i;
  double HM, OM;

  for(i=0; i < numMol; i++){
    HM = parms->getFHM();
    OM = parms->getFOM();
    molecule[i].scaleMomenta(Dest,HM,OM);
  }
}

// --------------------------------------------------------------------
void ensemble::DUMP(int iter){
  int i;

  printf(" >> STEP: %d\n",iter);
  printf(" NMOL: %d\n",numMol);
  printf(" TVIR: %6.5f\n",TVIR);
  printf("  VIR: %6.5f\n",VIR.readval());
  printf(" FKIN: %6.5f\n",parms->getFKIN());
  printf(" TKIN: %6.5f\n",TKIN);
  printf("  KIN: "); KIN.vecPrint(); printf("\n");
  printf(" FPOT: %6.5f\n",parms->getFPOT());
  printf("  POT: "); POT.vecPrint(); printf("\n");
  printf("\n");
  for (i = 0; i < numMol; i++){
    molecule[i].dump();
    printf("\n");
  }
}


// ----------------------------------------------------------------------------
// MAIN
// ----------------------------------------------------------------------------
int main(int argc, char **argv) {

  int n, start_time, stop_time;
  int ticks;
  double dticks;

  const char * filename, * randomfilename;

  if (argc != 3) {
    printf("Usage: %s input_param_file random_file\n", argv[0]);
    return(1);
  }
  filename = argv[1];
  randomfilename = argv[2];

  cout << " >>> Program Started\n";
  cout.flush();
  get_ticks_time(&ticks);
  dticks = ticks;

  get_time(&start_time);

  parms = new simparm;
  parms->loadParms(filename);
  n = parms->getNMOL();
  liquid = new ensemble(n);

  liquid->INITIA(randomfilename);
  liquid->INTRAF();
  liquid->computeVIR();
  liquid->INTERF(ACC);
  liquid->SCALEFORCES(ACC);

  parms->setNFMC(-1);
  parms->resetStat();
  liquid->MDMAIN();
  get_time(&stop_time);

  get_ticks_time(&ticks);
  dticks = ticks;
  cout << "TOTAL CPU USED = " << ((stop_time-start_time)/dticks) << " SECONDS\n";
  cout << "\nELAPSED CPU TIME IN SECONDS: " << ((stop_time-start_time)/dticks) << "\n";
  cout << "\nMFLOP RATE: "<< 3432.550/((stop_time-start_time)/dticks) << "\n";
  cout << "\nTotal Time = " << ((stop_time-start_time)/dticks) << "\n";
  cout.flush();
} 
