
#include <stdio.h>
#include <string.h>
#include <math.h>
#include <stdlib.h>
#include <iostream>
#include <fstream>
#include <limits.h>


#include "barnes.h"
#include "stdlib.h"

extern "C" int          scanbind(char **bvec, char *name);
extern "C" char*        extrvalue(char *arg);


using namespace std;
char    buf[128];
// --- PARMS CODE ------------------------------------------------------------
void parms_cons(parms_t * p)  {
  int i;
  // SET DEFAULT PARAMETER VALUES
  strcpy(p->headline,"");
  strcpy(p->infile,"");
  strcpy(p->outfile,"");
  p->dtime = 0.0;
  p->dtout = 0.0;
  p->tstop = 0.0;
  p->fcells = 0.0;
  p->fleaves = 0.0;
  p->tol = 0.0;
  p->tolsq = 0.0;
  p->eps = 0.0;
  p->epssq = 0.0;
  p->dthf = 0.0;

  for(i = 0; i < 12; i++)
    p->defaults[i] = (char*)malloc(MAX_NAME);

   /* file names for input/output                                 */
  strcpy(p->defaults[0],"in=");    /* snapshot of initial conditions  */
  strcpy(p->defaults[1],"out=");   /* stream of output snapshots      */
  /* params, used if no input specified, to make a Plummer Model   */
  strcpy(p->defaults[2],"nbody=1024");  /* number of particles to generate */
  strcpy(p->defaults[3],"seed=123");    /* random number generator seed    */
  /* params to control N-body integration                          */
  strcpy(p->defaults[4],"dtime=0.025"); /* integration time-step      */
  strcpy(p->defaults[5],"eps=0.05");    /* usual potential softening  */
  strcpy(p->defaults[6],"tol=1.0");     /* cell subdivision tolerence */
  strcpy(p->defaults[7],"fcells=2.0");  /* cell allocation parameter  */
  strcpy(p->defaults[8],"fleaves=0.5"); /* leaf allocation parameter  */
  strcpy(p->defaults[9],"tstop=0.075"); /* time to stop integration   */
  strcpy(p->defaults[10],"dtout=0.25"); /* data-output interval       */
  strcpy(p->defaults[11],"");
}

void parms_setparms(parms_t * p, char **fnames, double *args){
  strcpy(p->headline,fnames[0]);
  strcpy(p->infile,fnames[1]);
  strcpy(p->outfile,fnames[2]);
  p->dtime = args[0];
  p->dtout = args[1];
  p->tstop = args[2];
  p->fcells = args[3];
  p->fleaves = args[4];
  p->tol = args[5];
  p->tolsq = args[6];
  p->eps = args[7];
  p->epssq = args[8];
  p->dthf = args[9];
}

void parms_getparam(parms_t * p, const char *name) {
  int i, leng;
  char *temp;

  buf[0] = '\0';
  if(p->defaults == NULL){
    cerr << "getparam: called before initparam\n";
  }
  i = scanbind(p->defaults, (char *) name);
  if(i < 0){
     cerr << "getparam: " << name << " unknown\n";
  }
  temp = extrvalue(p->defaults[i]);
  fgets(buf, 128, stdin);
  leng = strlen(buf) + 1;
  if(leng <= 1)
    strcpy(buf,temp);
}

int parms_getiparam(parms_t * p, const char *name){
  parms_getparam(p,name);
  return (atoi(buf));
}

// ---------------------------------------------------------------------------
double parms_getdparam(parms_t * p, const char *name){
   parms_getparam(p,name);
   return (atof(buf));
}


// ---------------------------------------------------------------------------
void parms_loadInfile(parms_t * p){
  parms_getparam(p,"in");
  strcpy(p->infile,buf);
}

// ---------------------------------------------------------------------------
void parms_loadParms(parms_t * p){
  parms_getparam(p,"out");
  strcpy(p->outfile,buf);
  p->dtime = parms_getdparam(p,"dtime");
  p->dthf = 0.5 * p->dtime;
  p->eps = parms_getdparam(p,"eps");
  p->epssq = p->eps*p->eps;
  p->tol = parms_getdparam(p,"tol");
  p->tolsq = p->tol*p->tol;
  p->fcells = parms_getdparam(p,"fcells");
  p->fleaves = parms_getdparam(p,"fleaves");
  p->tstop = parms_getdparam(p,"tstop");
  p->dtout = parms_getdparam(p,"dtout");
}

