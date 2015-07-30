#include "barnes.h"
#include "stdlib.h"
const double    PI = 3.14159265358979323846;

using namespace std;
extern "C" double       xrand(double xl, double xh);
extern "C" void         pranset(int seed);
extern "C" void         get_ticks_time(int *r);
extern "C" void 		print_stat();
extern "C" void         get_time(int *r);
extern "C" void			reset_stat();

FILE * posfile;
// ----------------------------------------------------------------------------
void nbody_loadData(nbody_t * n, char *name){
  int i, ndim;
  body_t *p;
  double m, tmp[3];
  ifstream instr(name);

  cerr << "Reading input file : " << name << '\n';
  cerr.flush();
  if (!instr)
     cerr << "loadData: cannot find file " << name << '\n';
  instr >> n->nbody;
  if (n->nbody < 1)
     cerr << "loadData: nbody = " << n->nbody << " is absurd\n";
  instr >> ndim;
  if(ndim != NDIM)
    cerr << "inputdata: NDIM = " << NDIM << "ndim = " << ndim << " is absurd\n";
  instr >> n->tnow;

  for(i = 0; i < n->nbody; i++){
    p = bodyset_get(&n->bodies, bodyptr(i));
    instr >> m;
    p->node.mass = (m);
  }
  for(i = 0; i < n->nbody; i++){
    p = bodyset_get(&n->bodies, bodyptr(i));
    in_vector(instr,tmp);
    vecLoad(&p->node.pos, tmp);
  }
  for(i = 0; i < n->nbody; i++){
    p = bodyset_get(&n->bodies, bodyptr(i));
    in_vector(instr,tmp);
    vecLoad(&p->vel, tmp);
  }
  instr.close();
}
void nbody_pickshell(nbody_t * nb, double *vec, double rad){
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
void nbody_loadTestData(nbody_t * n){
  double rsc, vsc, r, v, x, y, offset, tmp;
  double cmr[NDIM], cmv[NDIM], tmpv[NDIM], tmpv2[NDIM];
  body_t *p, *cp;
  int nbody = n->nbody;
  int halfnbody, i, k;

  n->tnow = 0.0;
  rsc = 9 * PI / 16;
  vsc = sqrt(1.0 / rsc);

  tvecClr(cmr);
  tvecClr(cmv);

  srand(123);

  halfnbody = nbody / 2;
  if (nbody % 2 != 0) halfnbody++;

  for(k = 0; k < halfnbody; k++) {
    p = bodyset_newbody(&n->bodies);
    do {
      tmp = xrand(0.0, 0.999);
      r = 1.0 / sqrt(pow(tmp, -2.0/3.0) - 1);
    } while (r > 9.0);

    nbody_pickshell(n,tmpv, rsc * r);
    vecAdd(&p->node.pos, tmpv);
    tvecAdd(cmr,tmpv);
    do{
      x = xrand(0.0, 1.0);
      y = xrand(0.0, 0.1);
    } while (y > x*x * pow(1 - x*x, 3.5));
    v = sqrt(2.0) * x / pow(1 + r*r, 0.25);
    nbody_pickshell(n, tmpv, vsc * v);
    vecAdd(&p->vel, tmpv);
    tvecAdd(cmv,tmpv);
  }

  offset = 4.0;
  for(k = halfnbody; k < nbody; k++) {
    p = bodyset_newbody(&n->bodies);
    //p->setMass((double)(1.0/nbody));
    //p->setMass(0.1 + 0.9*((double)rand()/INT_MAX));
    //p->setMass(0.5);


    cp = bodyset_get(&n->bodies, bodyptr(k-halfnbody));
    vecStore(&cp->node.pos, tmpv);
    tvecClr(tmpv2);
    for(i=0; i < NDIM; i++){
      tmpv2[i] = tmpv[i] + offset;
      tvecAdd(cmr,tmpv2);
    }
    vecAdd(&p->node.pos, tmpv2);
    vecStore(&cp->vel, tmpv);
    tvecClr(tmpv2);
    for(i=0; i < NDIM; i++){
      tmpv2[i] = tmpv[i];
      tvecAdd(cmv,tmpv2);
    }
    vecAdd(&p->vel, tmpv2);
  }

  tvecScale(cmr, 1.0/nbody);
  tvecScale(cmv, 1.0/nbody);

  for(k=0; k < nbody; k++) {
    p = bodyset_get(&n->bodies,bodyptr(k));
    //p->setMass(0.1 + 0.9*((double)rand()/INT_MAX));
	p->node.mass = 0.5;
	vecStore(&p->node.pos, tmpv);
    tvecSub(tmpv,cmr);
    vecLoad(&p->node.pos, tmpv);
    vecStore(&p->vel, tmpv);
    tvecSub(tmpv,cmv);
    vecLoad(&p->vel, tmpv);
  }
}

// ----------------------------------------------------------------------------
void nbody_outputEnergy(nbody_t * n){
  cout << "\n\nENERGY TOTALS\n";
  cout << " MASS:        "<< n->mtot << "\n";
  cout << " ENERGY:     "; vecPrint(&n->etot); cout << "\n";
  cout << " KIN ENERGY: "; matPrint(&n->keten); cout << "\n";
  cout << " POT ENERGY: "; matPrint(&n->peten); cout << "\n";
  cout << " AGGREGATE CM POS: "; vecPrint(&n->cmphasePos); cout << "\n";
  cout << " AGGREGATE CM VEL: "; vecPrint(&n->cmphaseVel); cout << "\n";
  cout << " ANGULAR MOMENTUM: "; vecPrint(&n->amvec); cout << "\n\n";
}
void nbody_outputPos_Header(){
	fprintf(posfile, "STEP,BODY,POS[0],POS[1],POS[2]\n");
	
}
void nbody_outputPos_initFile(const char * filename){
	posfile = fopen(filename, "w");
	nbody_outputPos_Header();
}


void nbody_outputPos(nbody_t *n, int tstep){
	body_t *p, *cp;
	int i;
	for(i = 0; i < n->nbody; i++){
		p = bodyset_get(&n->bodies, bodyptr(i));
		fprintf(posfile,"%d,%d,%le,%le,%le\n",tstep,i,p->node.pos.val[0], p->node.pos.val[1], p->node.pos.val[2]);
	}
	
}
void nbody_outputEnergy_Header(const char * c){
	printf("MASS,EN[0],EN[1],EN[2],\
KIN[0][0],KIN[0][1],KIN[0][2],KIN[1][0],KIN[1][1],KIN[1][2],KIN[2][0],KIN[2][1],KIN[2][2],\
POT[0][0],POT[0][1],POT[0][2],POT[1][0],POT[1][1],POT[1][2],POT[2][0],POT[2][1],POT[2][2],\
POS[0],POS[1],POS[2],VEL[0],VEL[1],VEL[2],MOM[0],MOM[1],MOM[2]\n");			
	nbody_outputPos_initFile(c);
}
void nbody_outputEnergy_Delim(nbody_t * n, int tstep){
	printf("%f,%f,%f,%f,\
%f,%f,%f,\
%f,%f,%f,\
%f,%f,%f,\
%f,%f,%f,\
%f,%f,%f,\
%f,%f,%f,\
%f,%f,%f,%f,%f,%f,%f,%f,%f,\n", n->mtot,
	n->etot.val[0],n->etot.val[1],n->etot.val[2],
	n->keten.val[0][0],n->keten.val[0][1],n->keten.val[0][2],
	n->keten.val[1][0],n->keten.val[1][1],n->keten.val[1][2],
	n->keten.val[2][0],n->keten.val[2][1],n->keten.val[2][2],
	n->peten.val[0][0],n->peten.val[0][1],n->peten.val[0][2],
	n->peten.val[1][0],n->peten.val[1][1],n->peten.val[1][2],
	n->peten.val[2][0],n->peten.val[2][1],n->peten.val[2][2],
	n->cmphasePos.val[0],n->cmphasePos.val[1],n->cmphasePos.val[2],
	n->cmphaseVel.val[0],n->cmphaseVel.val[1],n->cmphaseVel.val[2],
	n->amvec.val[0],n->amvec.val[1],n->amvec.val[2]
	);
	nbody_outputPos(n, tstep);

}


void nbody_clearStats(nbody_t * n){
  n->nstep = 0;
  n->n2bcalc = 0;
  n->nbccalc = 0;
}

void nbody_setbound(nbody_t * nb){
  int i; int nbody = nb->nbody;
  double tmp1[NDIM], tmp2[NDIM], side;
  body_t *p;

  vecSetUnit(&nb->mincorner);
  vecScale(&nb->mincorner, 1E99);
  vecSetUnit(&nb->maxcorner);
  vecScale(&nb->maxcorner, -1E99);
  for(i=0; i < nbody; i++) {
    p = bodyset_get(&nb->bodies, bodyptr(i));
    vecStore(&p->node.pos, tmp1);
    vecMin(&nb->mincorner, tmp1); 
    vecMax(&nb->maxcorner, tmp1); 
  }

  side = 0.0;
  vecStore(&nb->maxcorner, tmp1);
  vecStore(&nb->mincorner, tmp2);
  tvecSub(tmp1,tmp2);
  for(i=0; i < NDIM; i++)
    if(side < tmp1[i])
      side = tmp1[i];

  for(i=0; i < NDIM; i++)
    tmp1[i] = -side/100000;
  vecAdd(&nb->mincorner, tmp1);
  nb->size = 1.00002*side;
  /*
  printf("BOUNDS: \n   size: %f\n   mincorner:(%f,%f,%f)\n   maxcorner:(%f,%f,%f)\n",
			nb->size,
			nb->mincorner.val[0],nb->mincorner.val[1],nb->mincorner.val[2],
			nb->maxcorner.val[0],nb->maxcorner.val[1],nb->maxcorner.val[2]);
 */
}

void nbody_init(nbody_t * nb) {
  vecCons(&nb->etot);
  matCons(&nb->keten);
  matCons(&nb->peten);
  vecCons(&nb->cmphasePos);
  vecCons(&nb->cmphaseVel);
  vecCons(&nb->amvec);
  vecCons(&nb->mincorner);
  vecCons(&nb->maxcorner);
}
// ----------------------------------------------------------------------------
void nbody_output(nbody_t * n, parms_t *sim) {
  if( (n->tout - 0.01 * sim->dtime) <= n->tnow)
    n->tout += sim->dtout;
}

// ----------------------------------------------------------------------------
void nbody_clearTiming(nbody_t * nb){
   nb->tracktime = 0;
   nb->updatetime = 0;
   nb->partitiontime = 0;
   nb->treebuildtime = 0;
   nb->forcecalctime = 0;
   
}

// ----------------------------------------------------------------------------
void nbody_initOutput(nbody_t * nb, parms_t *p){

  cout << "\n\t\tOutput: " << p->headline << "\n\n";
  cout << "nbody   dtime   eps   tol   dtout   tstop   fcells\n";
  cout << nb->nbody << "     " << p->dtime << "   " << p->eps
      << "   " << p->tol << "   " << p->dtout << "    "
      << p->tstop << "   " << p->fcells  << "\n\n";

}

void nbody_outputTiming(nbody_t * nb){
  int ticks;
  double dticks;

  get_ticks_time(&ticks);
  dticks = ticks;
  cout << "COMPUTESTART  = " <<
    (nb->computestart / dticks) << "\n";
  cout << "COMPUTEEND    = " <<
    (nb->computeend / dticks) << "\n";
  cout << "COMPUTETIME   = " <<
    ((nb->computeend - nb->computestart) / dticks) << "\n";
  cout << "TRACKTIME     = " <<
    (nb->tracktime / dticks) << "\n";
  cout << "PARTITIONTIME = " <<
    (nb->partitiontime / dticks) << "\t"
        << ((float)nb->partitiontime)/nb->tracktime << "\n";
  cout << "TREEBUILDTIME = " <<
    (nb->treebuildtime / dticks) << "\t"
        << ((float)nb->treebuildtime)/nb->tracktime << "\n";
  cout << "FORCECALCTIME = " <<
    (nb->forcecalctime / dticks) << "\t"
        << ((float)nb->forcecalctime)/nb->tracktime << "\n";
  cout << "UPDATEPOSTIME = " <<
    (nb->updatetime / dticks) << "\t"
        << ((float)nb->updatetime)/nb->tracktime << "\n";
  cout << "RESTTIME      = " <<
    ((nb->tracktime - nb->partitiontime -
     nb->updatetime - nb->treebuildtime - nb->forcecalctime)
    / dticks) << "\t"
        << ((float)(nb->tracktime-nb->partitiontime- nb->updatetime - nb->treebuildtime-
        nb->forcecalctime))/nb->tracktime<<"\n";
  cout.flush();
}


void nbody_startrun(nbody_t * n, parms_t *p){
  int seed;
  char *name, headbuf[128];

  parms_loadInfile(p);
  name = p->infile;
  
  if(strlen(name) > 1) {
    sprintf(headbuf, "Hack code: input file %s\n", name);
    strcpy(p->headline,headbuf);
  } 
  else {
    n->nbody = parms_getiparam(p,"nbody");
    if(n->nbody < 1)
     cerr << "startrun: absurd nbody\n";
    seed = parms_getiparam(p, "seed");
    strcpy(p->headline,"Hack code: Plummer model");
  }

  parms_loadParms(p);

  // allocation of data structures filing is done later
  cellset_cons(&n->cells, (int)(p->fcells*p->fleaves*n->nbody));
  leafset_cons(&n->leaves, (int)(p->fleaves*n->nbody));
  bodyset_cons(&n->bodies, n->nbody);

  n->tout = n->tnow + p->dtout;
  pranset(seed);

  name = p->infile;
  
  if(strlen(name) > 1) {
    nbody_loadData(n,name);
  } else {
    nbody_loadTestData(n);
  }
  
}

void nbody_updateEnergy(nbody_t * n){
  int k;
  matrix_t m1;
  double v0[3], v1[3], v2[3], v3[3], v4[3], m, velsq;
  body_t *p;

  n->mtot = 0.0;
  vecClr(&n->etot);
  matClr(&n->keten);
  matClr(&n->peten);
  vecClr(&n->cmphasePos);
  vecClr(&n->cmphaseVel);
  vecClr(&n->amvec);

  tvecClr(v4);
  for(k = 0; k < n->nbody; k++) {
	p = bodyset_get(&n->bodies, bodyptr(k));
    m = p->node.mass;
    n->mtot += m;
    vecStore(&p->vel, v1);
    velsq = tvecDotProd(v1,v1);
    v4[1] += 0.5 * m * velsq;
    v4[2] += 0.5 * m * p->phi;
    vecStore(&p->node.pos, v1);
    vecStore(&p->vel, v2);
    vecStore(&p->acc, v3);
    tvecLoad(v0,v2);
    tvecScale(v0,0.5*m);
    matOutProd(&m1,v0,v2);
    matAdd(&n->keten, &m1);
    tvecLoad(v0,v1);
    tvecScale(v0,m);
    matOutProd(&m1,v0,v3);
    matAdd(&n->peten, &m1);
    tvecLoad(v0,v1);
    tvecScale(v0,m);
    vecAdd(&n->cmphasePos, v0);
    tvecLoad(v0,v2);
    tvecScale(v0,m);
    vecAdd(&n->cmphaseVel, v0);
    tvecCrossProd(v0,v1,v2);
    tvecScale(v0,m);
    vecAdd(&n->amvec,v0);
  }
   v4[0] = v4[1] + v4[2];
   vecLoad(&n->etot,v4);

   if (n->mtot != 0.0){
      vecScale(&n->cmphasePos, 1.0/n->mtot);
      vecScale(&n->cmphaseVel,1.0/n->mtot);
   }
}
void nbody_computecofm(nbody_t * n, node_ref root_ref){
	int i;
	//node_t *nptr;
	//body_t *p;
	//leaf_t *leaf_data;
	//cell_t *ce;
	double mass;
	double tmpv[NDIM];

	
	if(root_ref.type == LEAF){
		leaf_t * root_leaf = leafset_get(&n->leaves, root_ref);
		for(i=0; i < root_leaf->num_bodies; i++) {
		  node_ref body_ref = root_leaf->bodyp[i];
		  body_t * curr_body = bodyset_get(&n->bodies, body_ref);
		  mass = curr_body->node.mass;
		  root_leaf->node.mass += mass;
		  vecStore(&curr_body->node.pos, tmpv);
		  tvecScale(tmpv,mass);
		  vecAdd(&root_leaf->node.pos, tmpv);
		}
		vecStore(&root_leaf->node.pos, tmpv);
		tvecScale(tmpv,1.0/root_leaf->node.mass);
		vecLoad(&root_leaf->node.pos, tmpv);
	} 
	else {
		cell_t * root_cell = cellset_get(&n->cells, root_ref);
		for(i=0; i < NSUB; i++){
		  node_ref child_ref = root_cell->subp[i];
		  if(!isnull(child_ref)){
			node_t * child_node;
			nbody_computecofm(n, child_ref);
			if(child_ref.type == LEAF) child_node = &leafset_get(&n->leaves, child_ref)->node;
			else child_node = &cellset_get(&n->cells, child_ref)->node;
			mass = child_node->mass;
			root_cell->node.mass += mass;
			vecStore(&child_node->pos, tmpv);
			tvecScale(tmpv,mass);
			vecAdd(&root_cell->node.pos, tmpv);
		  }
		}
		vecStore(&root_cell->node.pos, tmpv);
		tvecScale(tmpv,1.0/root_cell->node.mass);
		vecLoad(&root_cell->node.pos, tmpv);
	}
}


void nbody_Advance(nbody_t * n, double hts, double ts){
  int i;
  body_t *body_ptr;

  for(i=0; i < n->nbody; i++) {
	body_ptr = bodyset_get(&n->bodies, bodyptr(i));
    body_advance(body_ptr, hts, ts);
  }
}


void nbody_stepsystem(nbody_t * n, parms_t *p){
  
  int i;
  body_t *bptr;
  double incr, hts, ts, stop, tol, eps;

  int trackstart, trackend;
  int treebuildstart, treebuildend;
  int forcecalcstart, forcecalcend;
  int updatestart, updateend;
  int totalstep = 0;

  incr = p->dtime;
  hts = p->dthf;
  ts = p->dtime;
  stop = p->tstop;
  tol = p->tolsq;
  eps = p->epssq;
  nbody_outputEnergy_Header("pos.txt");
  while (n->tnow < stop + 0.1 * incr){
    get_time(&trackstart);
    leafset_reset(&n->leaves);
    cellset_reset(&n->cells);
    n->BH_tree = cellset_make(&n->cells, REF_NULL);
    get_time(&treebuildstart);
    nbody_maketree(n);
    nbody_computecofm(n, n->BH_tree);
    //print_node(n, n->BH_tree, 0);
    get_time(&treebuildend);
    n->treebuildtime += treebuildend - treebuildstart;

    
    get_time(&forcecalcstart);
    //nbody_Simulate(n,tol,hts,eps,ts);
    nbody_Simulate(n,tol,hts,eps,ts);
    get_time(&forcecalcend);
    n->forcecalctime += forcecalcend - forcecalcstart;

    get_time(&trackend);
    n->tracktime += trackend - trackstart;
	
    nbody_setbound(n);

	n->nstep++;
    n->tnow = n->tnow + incr;
    totalstep++;
	
    nbody_updateEnergy(n);
    //nbody_outputEnergy(n);
	nbody_outputEnergy_Delim(n, n->nstep);
    if (totalstep == 2) {
      reset_stat();
      nbody_clearTiming(n);
      nbody_clearStats(n);
      get_time(&n->computestart);
    }
  }
  
  get_time(&n->computeend);
  print_stat();
  n->computeend = n->computeend - n->computestart;
  n->computestart = 0;
  

}
