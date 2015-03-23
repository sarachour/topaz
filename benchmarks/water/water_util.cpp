#include "water.h"
#include "math.h"
#include "relax_lib.h"
#include <iostream>
#include "stdio.h"
#include "stdlib.h"

#define max(f1,f2) (f1 > f2? f1: f2)
FILE * en_outfile;
FILE * pos_outfile;
simparm_t * parms;

// -----------------------------------------------------------------------
// GLOBALS 
// -----------------------------------------------------------------------
int lcm(int x, int y){
	int a = x;
	int b = y;
	int t = 0;
	int gcd, lcm;
	while (b != 0) {
		t = b;
		b = a % b;
		a = t;
	}

	gcd = a;
	lcm = (x*y)/gcd;
	return lcm;
}

void acc_double::addval(double d){
  val += d; 
}




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

void skratch_pad::update_forces_1D(double Res[9]) {
  H1force.vecAdd(&Res[0]); 
  Oforce.vecAdd(&Res[3]); 
  H2force.vecAdd(&Res[6]); 
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
  DBLUREL(LocPot); DBLUREL(DTS); DBLUREL(R1); DBLUREL(R2); DBLUREL(RX); DBLUREL(COS);
  DBLUREL(DT); DBLUREL(DR1); DBLUREL(DR2); DBLUREL(DR1S); DBLUREL(DR2S); DBLUREL(DRP);
  DBLNUREL(t1,3); DBLNUREL(t2,3); DBLNUREL(t3,3); DBLNUREL(t4,3);
  DBLNUREL(h1,3); DBLNUREL(Ox,3); DBLNUREL(h2,3); DBLNUREL(VM,3);

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
  DBLUREL(DT); DBLUREL(DTS); DBLUREL(DR1); DBLUREL(DR1S); DBLUREL(DR2); DBLUREL(DR2S);
  DBLUREL(R1S); DBLUREL(R2S); DBLUREL(F1); DBLUREL(F2); DBLUREL(F3);
  DBLNUREL(vr1,3); DBLNUREL(vr2,3); DBLNUREL(dt1,3); DBLNUREL(dt3,3);
  DBLNUREL(dr11,3); DBLNUREL(dr23,3); DBLNUREL(s,3); DBLNUREL(v1,3);
  DBLNUREL(v2,3); DBLNUREL(v3,3); DBLNUREL(h1,3); DBLNUREL(Ox,3);
  DBLNUREL(h2,3); 
  
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



void spar_setPS1(simparm_t * sp, double *v) { 
	sp->R1 =v[0]; 
	sp->ELPST=v[1]; 
}

void spar_setPS2(simparm_t * sp, int *v){ 
	sp->IRST=v[0]; 
	sp->NVAR=v[1]; 
	sp->NXYZ=v[2]; 
	sp->NXV=v[3]; 
	sp->IXF=v[4]; 
	sp->IYF=v[5]; 
	sp->IZF=v[6]; 
	sp->IMY=v[7]; 
	sp->IMZ=v[8]; 
}
void spar_setPS3(simparm_t * sp, double *v) { 
	sp->TEMP =v[0]; 
	sp->RHO=v[1]; 
	sp->TSTEP=v[2]; 
	sp->BOXL=v[3]; 
	sp->BOXH=v[4]; 
	sp->CUTOFF=v[5]; 
	sp->CUT2=v[6];
}


void spar_setPS4(simparm_t * sp, int *v){ 
	sp->NMOL=v[0]; 
	sp->NORDER=v[1]; 
	sp->NATMO=v[2]; 
	sp->NATMO3=v[3]; 
	sp->NMOL1=v[4];
	sp->NSTEP=v[5]; 
	sp->NSAVE=v[6]; 
	sp->NRST=v[7]; 
	sp->NPRINT=v[8]; 
	sp->NFMC=v[9]; 
	sp->I2=v[10]; 
}

inline void spar_setPS5(simparm_t * sp, double *v) {
	sp->FHM=v[0];
	sp->FOM=v[1];
	sp->REF1=v[2]; 
	sp->REF2=v[3]; 
	sp->REF4=v[4]; 
}

double spar_computeFAC(simparm_t * sp){
	return BOLTZ*sp->TEMP*sp->NATMO/UNITM * pow((UNITT*sp->TSTEP/UNITL),2.0);
}

// --- SIMPARM.C --------------------------------------------------------------

void spar_CONS(simparm_t * sp){
  int i;

  for(i=0; i < 100; i++)
    sp->TLC[i] = 0.0;
  for(i=0; i < 11; i++)
    sp->PCC[i] = 0.0;
  sp->R1 = sp->ELPST = 0.0;
  sp->IRST = sp->NVAR = sp->NXYZ = sp->NXV = sp->IXF = sp->IYF = sp->IZF = sp->IMY = sp->IMZ = 0;
  sp->TEMP = sp->RHO = sp->TSTEP = sp->BOXL = sp->BOXH = sp->CUTOFF = sp->CUT2 = 0.0;
  sp->NMOL = sp->NORDER = sp->NATMO = sp->NATMO3 = sp->NMOL1 = sp->NSTEP = sp->NSAVE = 0;
  sp->NRST = sp->NPRINT = sp->NFMC = sp->I2 = 0;
  sp->FHM = sp->FOM = sp->REF1 = sp->REF2 = sp->REF4 = 0.0;
}
void spar_DEST(simparm_t * sp){
	
	
}

void spar_CNSTNT(simparm_t * sp){
  int NN,N1,K1,N;
  double TN, TK;

  N = sp->NORDER+1;
  sp->TLC[1] = 1.00;
  for (N1 = 2; N1<=N; N1++){  
    NN = N1-1;
    TN = NN;
    sp->TLC[N1] = 1.00;
    TK = 1.00;
    for (K1=2;K1<=N1;K1++) { 
      sp->TLC[(K1-1)*N+NN] = sp->TLC[(K1-2)*N+NN+1]*TN/TK;
      NN = NN-1;
      TN = TN-ONE;
      TK = TK+ONE;
    }
  }

  sp->PCC[2] = ONE;
  N1 = N-1;
  if((N1 == 1) || (N1 == 2)){
    std::cout << "***** ERROR: THE ORDER HAS TO BE GREATER THAN 2 ****\n";
  }
  if(N1 == 3){
    sp->PCC[0] = ONE/6.00;
    sp->PCC[1] = FIVE/6.00;
    sp->PCC[3] = ONE/3.00;
  }
  if(N1 == 4){
    sp->PCC[0] = (double) 19.00/120.00;
    sp->PCC[1] = THREE/4.00;
    sp->PCC[3] = ONE/2.00;
    sp->PCC[4] = ONE/12.00;
  }
  if(N1 == 5){
    sp->PCC[0] = THREE/20.00;
    sp->PCC[1] = (double) 251.00/360.00;
    sp->PCC[3] = (double) 11.00/18.00;
    sp->PCC[4] = ONE/6.00;
    sp->PCC[5] = ONE/60.00;
  }
  if(N1 ==  6){
	sp->PCC[0] = (double) 863.00/6048.00;
	sp->PCC[1] = (double) 665.00/1008.00;
	sp->PCC[3] = (double) 25.00/36.00;
	sp->PCC[4] = (double) 35.00/144.00;
	sp->PCC[5] = ONE/24.00;
	sp->PCC[6] = ONE/360.00;
  }
    if(N1 ==  7){
	sp->PCC[0] = (double) 275.00/2016.00;
	sp->PCC[1] = (double) 19087.00/30240.00;
	sp->PCC[3] = (double) 137.00/180.00;
	sp->PCC[4] = FIVE/16.00;
	sp->PCC[5] = (double) 17.00/240.00;
	sp->PCC[6] = ONE/120.00;
	sp->PCC[7] = ONE/2520.00;
    }
}

void spar_SYSCNS(simparm_t * sp) {
  sp->TSTEP=sp->TSTEP/UNITT;
  sp->NATMO=NATOM*sp->NMOL;
  sp->NATMO3=sp->NATMO*3;
  sp->BOXL=pow((sp->NMOL*WTMOL*UNITM/sp->RHO),(1.00/3.00));
  sp->BOXL=sp->BOXL/UNITL;
  sp->BOXH=sp->BOXL*0.50;
  sp->CUTOFF=max(sp->BOXH,sp->CUTOFF);
  sp->REF1= -QQ/(sp->CUTOFF*sp->CUTOFF*sp->CUTOFF);
  sp->REF2=TWO*sp->REF1;
  sp->REF4=TWO*sp->REF2;
  sp->CUT2=sp->CUTOFF*sp->CUTOFF;
  sp->FPOT= UNITM * pow((UNITL/UNITT),2.0) / (BOLTZ*sp->TEMP*sp->NATMO);
  sp->FKIN=sp->FPOT*0.50/(sp->TSTEP*sp->TSTEP);
  sp->FHM=(sp->TSTEP*sp->TSTEP*0.50)/HMAS;
  sp->FOM=(sp->TSTEP*sp->TSTEP*0.50)/OMAS;
  sp->NMOL1=sp->NMOL-1;
}

void spar_loadParms(simparm_t * sp, const char * file){
  FILE * infile;

  std::cout << "OUTPUT FOR PERFECT CLUB BENCHMARK: MDG Revision: 1.*  Author: kipp\n";
  sp->NORDER = 5;
  sp->TEMP = 298.0;
  sp->RHO = 0.9980;
  sp->TSTEP = 1.5e-16;
  sp->CUTOFF = 0.0;

  infile = fopen(file, "r");
  if(infile == NULL){
    std::cout << " *** ERROR: Cannot open LWI5\n";
    exit(-1);
  }
  fscanf(infile,"%lf",&sp->TSTEP);
  fscanf(infile,"%d", &sp->NMOL);
  fscanf(infile,"%d", &sp->NSTEP);
  fscanf(infile,"%d", &sp->NORDER);
  fscanf(infile,"%d", &sp->NSAVE);
  fscanf(infile,"%d", &sp->NRST);
  fscanf(infile,"%d", &sp->NPRINT);
  fscanf(infile,"%d", &sp->NFMC);


  spar_CNSTNT(sp);
  std::cout << "\nTEMPERATURE                = "<< sp->TEMP << "\n";
  std::cout << "DENSITY                    = "<< sp->RHO << " G/C.C.\n";
  std::cout << "NUMBER OF MOLECULES        = "<< sp->NMOL << "\n";
  std::cout << "TIME STEP                  = "<< sp->TSTEP << " SEC\n";
  std::cout << "ORDER USED TO SOLVE F=MA   = "<< sp->NORDER << "\n";
  std::cout << "NO. OF TIME STEPS          = "<< sp->NSTEP <<"\n";
  std::cout << "FREQUENCY OF DATA SAVING   = "<< sp->NSAVE <<"\n";
  std::cout << "FREQUENCY TO WRITE RST FILE= "<< sp->NRST <<"\n";
  std::cout.flush();
  spar_SYSCNS(sp);
  std::cout << "SPHERICAL CUTOFF RADIUS    = "<< sp->CUTOFF <<" ANGSTROM\n";
  sp->IRST=0;
  fclose(infile);
}



void ensemble_init_ensemble(ensemble * e, int num){

  e->numMol = num;
  e->molecule = new h2o[e->numMol];
  //ensure skratchpad is a multiple of 
  int l = lcm(AUTOICHUNKSIZE, AUTOJCHUNKSIZE);
  int rem = l - (e->numMol % l);
  e->pad = new skratch_pad[e->numMol+rem > MAX_MOLEC ? e->numMol+rem  : MAX_MOLEC];

  e->TTMV = 0.0;
  e->TVIR = 0.0;
  e->TKIN = 0.0;
  e->VIR.writeval(0.0);
  e->POT.vecClr();
  e->KIN.vecClr();
}

void ensemble_destruct_ensemble(ensemble * e){
  delete[] e->molecule;
  delete[] e->pad;
}

void ensemble_clearTVIR(ensemble * e){ 
  e->TVIR = 0.0;
}

void ensemble_updateTVIR(ensemble * e){ 
  e->TVIR -= e->VIR.readval();
}

void ensemble_BNDRY(ensemble * e, double size) {
  int i;

  for(i = 0; i < e->numMol; i++){
    e->molecule[i].bndry(size);
  }
} 

void ensemble_INITIA(ensemble * e, const char * file){
  vector minimum;
  double t1[3];
  FILE *random_numbers; 
  double XMAS[3],XS,ZERO,WCOS,WSIN,XT[3],YT[3],ZT[3],SU[3],FAC,SUM[3],t[3],NS;
  int i, j, k, mol;

  random_numbers = fopen(file,"r");
  if (random_numbers == NULL) { 
    std::cout << " *** Error: Can't open file random.in (ensemble_INITIA)\n";
    std::cout.flush();
    exit(0);
  }

     NS = pow((double) e->numMol, 1.0/3.0) - 0.00001;
     XS = parms->BOXL/NS;
     ZERO = XS * 0.50;
     WCOS = ROH * cos(ANGLE * 0.5);
     WSIN = ROH * sin(ANGLE * 0.5);

     std::cout << "***** NEW RUN STARTING FROM REGULAR LATTICE *****\n";
     std::cout.flush();
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
	   e->molecule[mol].loadDirPos(XDIR,XT);
	   e->molecule[mol].loadDirPos(YDIR,YT);
	   e->molecule[mol].loadDirPos(ZDIR,ZT);
           mol++;
	   if (mol == e->numMol) goto exitLoop;
           ZT[0] += XS; ZT[1] += XS; ZT[2] += XS;
         }
         YT[1] += XS;
       }
       XT[1] += XS;
     }

 exitLoop: if(e->numMol != mol) {
       std::cout << " *** Error: Lattice number of mol mismatch \n";
       exit(-1);
     }

  /* ASSIGN RANDOM MOMENTA */
  fscanf(random_numbers,"%lf",&SU[0]);

  tvecClr(SUM);
  for(i = 0; i < e->numMol; i++) {
    fscanf(random_numbers,"%lf",&t[0]);
    fscanf(random_numbers,"%lf",&t[1]);
    fscanf(random_numbers,"%lf",&t[2]);
//    fscanf(random_numbers,"%lf%lf%lf",&t[0],&t[1],&t[2]);

    e->molecule[i].loadH1Vel(t);
    tvecAdd(SUM,t);
    fscanf(random_numbers,"%lf",&t[0]);
    fscanf(random_numbers,"%lf",&t[1]);
    fscanf(random_numbers,"%lf",&t[2]);

//    fscanf(random_numbers,"%lf%lf%lf",&t[0],&t[1],&t[2]);
    e->molecule[i].loadOVel(t);
    tvecAdd(SUM,t);
    fscanf(random_numbers,"%lf",&t[0]);
    fscanf(random_numbers,"%lf",&t[1]);
    fscanf(random_numbers,"%lf",&t[2]);

//    fscanf(random_numbers,"%lf%lf%lf",&t[0],&t[1],&t[2]);
    e->molecule[i].loadH2Vel(t);
    tvecAdd(SUM,t);
  } 
  /* .....FIND AVERAGE MOMENTA PER ATOM */
  SUM[0] /= (NATOM*parms->NMOL);
  SUM[1] /= (NATOM*parms->NMOL);
  SUM[2] /= (NATOM*parms->NMOL);

  /* FIND NORMALIZATION FACTOR SO THAT <K.E.>=KT/2 */

  tvecClr(SU);
  for (i = 0; i < e->numMol; i++) {
    e->molecule[i].storeDirVel(XDIR,t);
    SU[0] += (pow((t[0]-SUM[0]),2.0) + pow((t[2]-SUM[0]),2.0))/HMAS 
	+ pow((t[1]-SUM[0]),2.0)/OMAS;
    e->molecule[i].storeDirVel(YDIR,t);
    SU[1] += (pow((t[0]-SUM[1]),2.0) + pow((t[2]-SUM[1]),2.0))/HMAS 
	+ pow((t[1]-SUM[1]),2.0)/OMAS;
    e->molecule[i].storeDirVel(ZDIR,t);
    SU[2] += (pow((t[0]-SUM[2]),2.0) + pow((t[2]-SUM[2]),2.0))/HMAS 
	+ pow((t[1]-SUM[2]),2.0)/OMAS;
  }
  FAC=spar_computeFAC(parms);
  SU[0]=sqrt(FAC/SU[0]);
  SU[1]=sqrt(FAC/SU[1]);
  SU[2]=sqrt(FAC/SU[2]);
  /* NORMALIZE INDIVIDUAL VELOCITIES SO THAT THERE IS NO BULK MOMENTA */

  XMAS[0] = 1.0/HMAS;
  XMAS[1] = 1.0/OMAS;
  XMAS[2] = 1.0/HMAS;

  for(i = 0; i < e->numMol; i++) {
    e->molecule[i].storeH1Vel(t1);
    tvecSub(t1,SUM); 
    tvecProd(t1,SU); 
    tvecScale(t1,XMAS[0]);
    e->molecule[i].loadH1Vel(t1);

    e->molecule[i].storeOVel(t1);
    tvecSub(t1,SUM); 
    tvecProd(t1,SU); 
    tvecScale(t1,XMAS[1]);
    e->molecule[i].loadOVel(t1);

    e->molecule[i].storeH2Vel(t1);
    tvecSub(t1,SUM); 
    tvecProd(t1,SU); 
    tvecScale(t1,XMAS[2]);
    e->molecule[i].loadH2Vel(t1);
  } 
} 




// ---------------------------------------------------------------------------
void ensemble_interPoteng2Aux(skratch_pad *p1, skratch_pad *p2, double *res){
  int KC,K;
  double CL[3][15], RS[14], RL[14], PotR, PotF, S[2];
  double Cut2, Cutoff, Ref1, Ref2;

  PotF = 0.0;
  PotR = 0.0;
  S[0] = parms->BOXH;	// BoxH
  S[1] = parms->BOXL;	// BoxL
  Cut2 = parms->CUT2;
  Cutoff = parms->CUTOFF;
  Ref1 = parms->REF1;
  Ref2 = parms->REF2;

  ensemble_CSHIFT2(p1,p2,CL,S);
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
      PotR += A1 * t_exp(-B1*RL[9]) +A2*(t_exp(-B2*RL[5])+ 
          t_exp(-B2*RL[6])+t_exp(-B2*RL[7])+t_exp(-B2*RL[ 8]))
     	+A3*(t_exp(-B3*RL[10])+t_exp(-B3*RL[11])+t_exp(-B3*RL[12])
	+t_exp(-B3*RL[13]))-A4*(t_exp(-B4*RL[10])+t_exp(-B4*RL[11])
	+t_exp(-B4*RL[12])+t_exp(-B4*RL[13]));
    } 
  } 
  res[0] = 0.0;
  res[1] = PotR;
  res[2] = PotF;
}

/*
void ensemble_inter_poteng2(skratch_pad *p1, skratch_pad *p2){
  double tmp[3];

  interPoteng2Aux(p1,p2,tmp);
  POT.vecAdd(tmp);
}
*/

// ----------------------------------------------------------------------
void ensemble_INTRA_POTENG(ensemble * e){
  int i;
  for(i=0; i < e->numMol; i++){
    e->molecule[i].intra_poteng(&e->POT);
  }
}

// ----------------------------------------------------------------------
void ensemble_POTENG(ensemble * e){
  e->POT.vecClr();
  ensemble_INTRA_POTENG(e);
  ensemble_INTER_POTENG(e);
  e->POT.vecScale(parms->FPOT);
}

// ----------------------------------------------------------------------
// KINETI
// ----------------------------------------------------------------------
void ensemble_clearTKIN(ensemble * e){ 
  e->TKIN = 0.0; 
}

void ensemble_updateTKIN(ensemble * e){ 
  e->TKIN += e->KIN.vecNorm1();
}

void ensemble_KINETI(ensemble * e){
  int i;
  e->KIN.vecClr();
  for(i=0; i < e->numMol; i++){
    e->molecule[i].kineti(&e->KIN);
  }
  ensemble_updateTKIN(e);
}

// ----------------------------------------------------------------------
// VIR
// ----------------------------------------------------------------------
void ensemble_computeVIR(ensemble * e){
  int i;

  for(i=0; i < e->numMol; i++){
    e->molecule[i].vir(&e->VIR);
  }
}

// ----------------------------------------------------------------------
void ensemble_printENERGY(ensemble * e, int iter){
  double XVIR, TENN, XTT, AVGT;
  double loc_pot[3];

  e->POT.vecStore(loc_pot);
  XVIR = e->TVIR*parms->FPOT*0.50/((double)e->TTMV);
  AVGT = e->TKIN*parms->FKIN*(parms->TEMP)*2.00/(3.00*((double)e->TTMV));
  TENN = e->KIN.vecNorm1() * parms->FKIN;
  XTT = tvecNorm1(loc_pot)+TENN;

  if ((iter % parms->NPRINT) == 0){
    std::cout << "     "<<iter;
    std::cout << " "<<TENN;
    std::cout << " "<<loc_pot[0];
    std::cout << " "<<loc_pot[1];
    std::cout << " "<<loc_pot[2];
    std::cout << " "<<XTT;
    std::cout << " "<<AVGT;
    std::cout << " "<<XVIR<< "\n";;
  }
}

// ----------------------------------------------------------------------------
void ensemble_PREDIC(ensemble * e){
  int i, ord;
  double *coeffs;

  for(i=0; i < e->numMol; i++){
    ord = parms->NORDER;
    coeffs = parms->TLC;
    e->molecule[i].predic(ord,coeffs);
  }
}

// ----------------------------------------------------------------------------
void ensemble_CORREC(ensemble * e){
  int i, ord;
  double *coeffs;

  for(i=0; i < e->numMol; i++){
    ord = parms->NORDER;
    coeffs = parms->PCC;
    e->molecule[i].correc(ord,coeffs);
  }
}

// ----------------------------------------------------------------------------
// MDMAIN --- MOLECULAR DYNAMICS LOOP 
// ----------------------------------------------------------------------------
void ensemble_MDMAIN(ensemble * e){

  std::cout << "RESTART "<< parms->IRST;
  std::cout << " AFTER "<< parms->ELPST << " SECONDS\n";
  ensemble_clearTVIR(e);
  ensemble_clearTKIN(e);

  if(parms->NSAVE > 0){
    std::cout << "COLLECTING X AND V DATA AT EVERY %4d TIME STEPS\n";
    std::cout << parms->NSAVE;
  }
  std::cout << "INTERMEDIATE RESULTS (ENERGY EXPRESSED IN UNITS OF KT ATOM) \n";
  std::cout << "  TIME       KINETIC   INTRA POT   INTER POT   REACT POT       ";
  std::cout << "TOTAL  \n<TEMPERATURE>   <VIR>\n";

  ensemble_stepsystem(e);

}
void ensemble_DUMP_NEW(ensemble *e, int iter);
// --- MAIN LOOP -------------------------------------------------------
void ensemble_stepsystem(ensemble * e){
  int i, n, start, stop;
  int start_serial, stop_serial, total_serial;
  int ticks;
  double dticks;

  total_serial = 0;
  reset_stat();
  get_time(&start);

  n = parms->NSTEP;
  ensemble_DUMP_NEW(e,0);
  for(i=1;i <= n; i++) {

    e->TTMV += 1.00;
#if defined(DEBUG_OPT)
  std::cout << " >>> Step: " << i << "\n";
#endif

    get_time(&start_serial); 
    ensemble_PREDIC(e);
#if defined(DEBUG_OPT)
  std::cout << "    >>> Predic: done.\n";
#endif
    ensemble_INTRAF(e);
    e->VIR.writeval(0.0);
    ensemble_computeVIR(e);
#if defined(DEBUG_OPT)
  std::cout << "    >>> Intraf: done.\n";
#endif
    get_time(&stop_serial); 
    total_serial += (stop_serial - start_serial);
    ensemble_INTERF(e,FORCES);
    ensemble_SCALEFORCES(e,FORCES);

#if defined(DEBUG_OPT)
  std::cout << "    >>> Interf: done.\n";
#endif
    get_time(&start_serial); 
    ensemble_CORREC(e);
#if defined(DEBUG_OPT)
  std::cout << "    >>> Correc: done.\n";
#endif
    ensemble_BNDRY(e,parms->BOXL);
#if defined(DEBUG_OPT)
  std::cout << "    >>> Bondary: done.\n";
#endif
    ensemble_KINETI(e);
#if defined(DEBUG_OPT)
  std::cout << "    >>> Kinetic: done.\n";
#endif
    ensemble_updateTVIR(e);

    get_time(&stop_serial); 
    total_serial += (stop_serial - start_serial);

    if(( (i % parms->NPRINT) == 0) || 
		((parms->NSAVE > 0) && ((i % parms->NSAVE) == 0))) {
      ensemble_POTENG(e);
      ensemble_printENERGY(e, i);
      ensemble_DUMP_NEW(e,i);
    }

  } 
  
  get_time(&stop);
  print_stat();
  get_ticks_time(&ticks);
  dticks = ticks;

  std::cout << "execution time = " << (stop-start) / dticks << "\n";
} 

void ensemble_CSHIFT2(skratch_pad *p1, skratch_pad *p2, double L[3][15], double *S){
  int i;

  // ---  XDIR ---
  double vm1,vm2,h1pos1,h1pos2,h2pos1,h2pos2,opos1,opos2;
  DBLUREL(vm1); DBLUREL(vm2); 
  DBLUREL(h1pos1); DBLUREL(h1pos2);
  DBLUREL(h2pos1); DBLUREL(h2pos2);
  DBLUREL(opos1); DBLUREL(opos2);
  
  vm1 = p1->VM.val[0];
  vm2 = p2->VM.val[0];
  L[0][0] = vm1-vm2;
  h1pos2  = p2->H1pos.val[0];
  L[0][1] = vm1-h1pos2;
  h2pos2  = p2->H2pos.val[0];
  L[0][2] = vm1-h2pos2;
  h1pos1 = p1->H1pos.val[0];
  L[0][3] = h1pos1-vm2;
  h2pos1 = p1->H2pos.val[0];
  L[0][4] = h2pos1-vm2;
  L[0][5] = h1pos1-h1pos2;
  L[0][6] = h1pos1-h2pos2;
  L[0][7] = h2pos1-h1pos2;
  L[0][8] = h2pos1-h2pos2;
  opos1 = p1->Opos.val[0];
  opos2 = p2->Opos.val[0];
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
  vm1 = p1->VM.val[1];
  vm2 = p2->VM.val[1];
  h1pos1 = p1->H1pos.val[1];
  L[1][0] = vm1-vm2;
  h1pos2 = p2->H1pos.val[1];
  L[1][1] = vm1-h1pos2;
  h2pos2 = p2->H2pos.val[1];
  L[1][2] = vm1-h2pos2;
  L[1][3] = h1pos1-vm2;
  h2pos1 = p1->H2pos.val[1];
  L[1][4] = h2pos1 - vm2;
  L[1][5] = h1pos1-h1pos2;
  L[1][6] = h1pos1-h2pos2;
  L[1][7] = h2pos1-h1pos2;
  L[1][8] = h2pos1-h2pos2;
  opos1 = p1->Opos.val[1];
  opos2 = p2->Opos.val[1];
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
  vm1 = p1->VM.val[2];
  vm2 = p2->VM.val[2];
  L[2][0] = vm1-vm2;
  h1pos2 = p2->H1pos.val[2];
  L[2][1] = vm1-h1pos2;
  h2pos2 = p2->H2pos.val[2];
  L[2][2] = vm1-h2pos2;
  h1pos1 = p1->H1pos.val[2];
  L[2][3] = h1pos1-vm2;
  h2pos1 = p1->H2pos.val[2];
  L[2][4] = h2pos1-vm2;
  L[2][5] = h1pos1-h1pos2;
  L[2][6] = h1pos1-h2pos2;
  L[2][7] = h2pos1-h1pos2;
  L[2][8] = h2pos1-h2pos2;
  opos1 = p1->Opos.val[2];
  opos2 = p2->Opos.val[2];
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
void ensemble_INTRAF(ensemble * e){
  int i;
  for (i = 0; i < e->numMol; i++){
    e->molecule[i].intraf();
  }
}

// ------------------------------------------------------------------------
// interf2 - operates on the skratch pad areas
// ------------------------------------------------------------------------
double ensemble_interf2_aux(skratch_pad *p1, skratch_pad *p2, double Res1[3][3], double Res2[3][3]){

  int KC, K;
  double CL[3][15], RS[15], FF[15], RL[15], GG[15];
  double G110, G23, G45, TT1, TT, FTEMP;
  double gCUT2, gREF1, gREF2, gREF4;
  double loc_vir;
  double S[2];
  
  DBLUREL(G110); DBLUREL(G23); DBLUREL(G45); DBLUREL(TT1); DBLUREL(TT); DBLUREL(FTEMP);
  DBLUREL(gCUT2); DBLUREL(gREF1); DBLUREL(gREF2); DBLUREL(gREF4);
  DBLNUREL(S,2); DBLNUREL(CL[0],15); DBLNUREL(CL[1],15); DBLNUREL(CL[2],15);
  DBLNUREL(RS,15); DBLNUREL(FF,15); DBLNUREL(RL,15); DBLNUREL(GG,15);
  DBLNUREL(Res1[0],3); DBLNUREL(Res1[1],3); DBLNUREL(Res1[2],3);
  DBLNUREL(Res2[0],3); DBLNUREL(Res2[1],3); DBLNUREL(Res2[2],3);

  
  S[0] = parms->BOXH;
  S[1] = parms->BOXL;
  gCUT2 = parms->CUT2;
  gREF1 = parms->REF1;
  gREF2 = parms->REF2;
  gREF4 = parms->REF4;
  loc_vir = 0.0;

  ensemble_CSHIFT2(p1,p2,CL,S);
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
      FF[9]=AB1*t_exp(-B1*RL[9])/RL[9];
      loc_vir = loc_vir + FF[9]*RS[9];
      for (K = 10; K < 14; K++) { 
	FTEMP=AB2*t_exp(-B2*RL[K-5])/RL[K-5];
	FF[K-5]=FF[K-5]+FTEMP;
	loc_vir= loc_vir+FTEMP*RS[K-5];
	RS[K]=CL[0][K]*CL[0][K]+CL[1][K]*CL[1][K]+CL[2][K]*CL[2][K];
	RL[K]=sqrt(RS[K]);
	FF[K]=(AB3*t_exp(-B3*RL[K])-AB4*t_exp(-B4*RL[K]))/RL[K];
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


void ensemble_interf2(ensemble * e, skratch_pad *p1, skratch_pad *p2){
  double incr, Res1[3][3], Res2[3][3];
  //This is the payload
  incr = ensemble_interf2_aux(p1,p2,Res1,Res2);
  p1->update_forces(Res1);
  p2->update_forces(Res2);
  e->VIR.addval(incr);
}
// --------------------------------------------------------------------
void ensemble_SCALEFORCES(ensemble * e, int Dest){
  int i;
  double HM, OM;

  for(i=0; i < e->numMol; i++){
    HM = parms->FHM;
    OM = parms->FOM;
    e->molecule[i].scaleMomenta(Dest,HM,OM);
  }
}
void ensemble_loadData(ensemble * e){
  int i;
  h2o *mol;
  skratch_pad *p;

  for(i=0; i < e->numMol; i++){
    mol = ensemble_getMolecule(e,i);
    p = ensemble_getPad(e,i);
    p->read_data(mol);
  }
}

void ensemble_storeData(ensemble * e, int dest){
  int i;
  h2o *p1;
  skratch_pad *p2;

  for(i=0; i < e->numMol; i++){
    p1 = ensemble_getMolecule(e,i);
    p2 = ensemble_getPad(e,i);
    p1->updateFields(dest,p2);
  }
}
h2o* ensemble_getMolecule(ensemble * e, int idx)  	
	{ return &(e->molecule[idx]); }
skratch_pad* ensemble_getPad(ensemble * e, int idx)	
	{ return &(e->pad[idx]); }
// --------------------------------------------------------------------
void ensemble_DUMP(ensemble * e, int iter){
  int i;

  printf(" >> STEP: %d\n",iter);
  printf(" NMOL: %d\n",e->numMol);
  printf(" TVIR: %6.5f\n",e->TVIR);
  printf("  VIR: %6.5f\n",e->VIR.readval());
  printf(" FKIN: %6.5f\n",parms->FKIN);
  printf(" TKIN: %6.5f\n",e->TKIN);
  printf("  KIN: "); e->KIN.vecPrint(); printf("\n");
  printf(" FPOT: %6.5f\n",parms->FPOT);
  printf("  POT: "); e->POT.vecPrint(); printf("\n");
  printf("\n");
  for (i = 0; i < e->numMol; i++){
    e->molecule[i].dump();
    printf("\n");
  }
}

void ensemble_DUMP_ENERGY(ensemble * e, int iter){
	fprintf(en_outfile, "%d	%d	%6.5f	%6.5f	%6.5f	%6.5f	%6.5f	%6.5f	%6.5f	%6.5f	%6.5f	%6.5f	%6.5f\n",
		iter,e->numMol,
		e->TVIR,e->VIR.readval(),parms->FKIN,e->TKIN,
		e->KIN.val[0],e->KIN.val[1],e->KIN.val[2],
		parms->FPOT,
		e->POT.val[0],e->POT.val[1],e->POT.val[2]
		);
}

void ensemble_DUMP_POS(ensemble * e, int iter){
  int i;
	for (i = 0; i < e->numMol; i++){
    h2o m = e->molecule[i];
    fprintf(pos_outfile, "%d	%6.5f	%6.5f	%6.5f	%6.5f	%6.5f	%6.5f	%6.5f	%6.5f	%6.5f\n",
			iter, 
			m.H1.getM(DISP)->val[0],m.H1.getM(DISP)->val[1],m.H1.getM(DISP)->val[2],
			m.O.getM(DISP)->val[0],m.O.getM(DISP)->val[1],m.O.getM(DISP)->val[2],
			m.H2.getM(DISP)->val[0],m.H2.getM(DISP)->val[1],m.H2.getM(DISP)->val[2]
			);
  }
}
void ensemble_DUMP_HEADER(){
  fprintf(en_outfile, "ITER	NMOL	TVIR	VIR	FKIN	TKIN	KIN[0]	KIN[1]	KIN[2]	FPOT	POT[0]	POT[1]	POT[2]\n");
  fprintf(pos_outfile, "ITER	H1.POS[0]	H1.POS[1]	H1.POS[2]	O.POS[0]	O.POS[1]	O.POS[2]	H2.POS[0] H2.POS[1]	H2.POS[2]\n");
  
}
void ensemble_DUMP_NEW(ensemble *e, int iter){
	ensemble_DUMP_POS(e,iter);
	ensemble_DUMP_ENERGY(e,iter);
}


