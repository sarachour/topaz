/*
 * File: fit.c
 * This is a general non-linear fitting program. It's used to a an
 * optimal set of coefficients for the r_over _i ratio.
 */

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/time.h>
#include "myrandom.h"


//#define _DOPRINT_ // #define NTRAJ 2500 //#define POINT 51

#define NTRAJ 500
#define E_MIN 0.05
#define POINT 51
#define NPAR 4
#define ERRCRI 0.01
#define PI 3.14159265358979

//#define _DOPRINT_
unsigned int INITIAL_SEED = 123;

/*---------------*/
/* function 'rmse'.
 * This function calculates the RMS error for the inputting
 * bat and ba 
 * 'bat' is the back scattering coefficient of our own MC simulation
 * 'ba'  is the full quantum MC simulation and is used as experiment
 *       to which we are fitting to
 */
double rmse(double bat[], double ba[]);

/* function: rmse */
double rmse(double bat[], double ba[])
{
  double rms;
  int i;

  rms=0.0;
  for(i=0;i<POINT;i++)
    {
    rms += (bat[i]-ba[i])*(bat[i]-ba[i])/bat[i]/(1.0-bat[i]);
    }

  return (rms);
}


/*---------------*/
/* private function 'back'.
 * This function simulates the backscattering coefficient for the
 * given element and energy
 * array point stores the energy and element information in the
 * sequence  E,Z,A,rho  (energy, atomic#, atomic weight, density)
 * array coef is the array of the parameters (the parameters we are
 * adjusting to make a good fit)
 * backsc is the address of the output back scattering coefficient
 */


/* Function: backsc */
double back(double point[], double coef[])
{
  int i,j,k,jj;
  int icic,iback;
  double eng;
  double engo,sigma,lamda,alpha,r_over_i;
  double ran1,ran2,ran3,ran4,step,zcor,cz,sz,de,c_phi,s_phi,psi,cc;
  int filtr,filtf;
  double base=(double)2147483647;

/*-----------------------*/
/* Here is the section for setting up the some initial values for
 * calculating sigma, alpha..., so that these values won't need to be
 * recalculated for each trajectory. This section need modification if
 * wemodify the formulas of alpha, r_over_i..
 */
  double p1=coef[0],p2=coef[1],p3=coef[2],p4=coef[3];
  double engt=point[0],zz=point[1],aa=point[2],rho=point[3];
  double z133=pow(zz,1.33),z165=pow(zz,1.65),zex=(1.0-zz/1000.0);
  double lamda_a = aa/6.022e23/rho;
  double j_j=0.001*(9.76*zz+58.5/pow(zz,0.19));
  double j_a=1.166/j_j;
  double p_sa = -78500.0*rho*zz/aa;
/*-----------------------*/

/*printf("p1,p2,p3,p4,p5,p6 %e %e %e %e %e %e\n",p1,p2,p3,p4,p5,p6);
printf("z133,z15 %e %e\n",z133,z15);
printf("lamda_a,j_j,j_a,p_sa, %e %e %e %e\n",lamda_a,j_j,j_a,p_sa);
*/

  long int result;

  srand_mr(INITIAL_SEED);

/*----------------------------------------------------------------
 *
 * Now, we begin to calculate the back-scattering coefficient for
 * this set of coefficient
 */

  /* now, the loop for all NTRAJ trajectories */
  iback=0; // accumulator...
  for(jj=0;jj<NTRAJ;jj++)
    {
    eng=engt;

    engo = (eng+511.0)/(eng+1024.0);
    sigma = p4*4.0e-18*z165*engo*engo/
       (eng+p3*1.0e-2*z133*pow(eng,0.5));
    lamda = lamda_a/sigma;
    alpha = 7.0e-3/eng;
    /*printf("engo,sigma,lamda,alpha %e %e %e %e \n",
       engo,sigma,lamda,alpha);
       */

    result = rand_mr();
    ran1=result/base;
    step = -lamda*log(ran1);
    /*printf("ran1,step %e %e\n",ran1,step);
    */

    zcor=step;
    cz=1;

    de=step*p_sa*log(j_a*eng+1.0)/eng;
    eng=eng+de;
    /*printf("de,eng %e %e\n",de,eng);
    */

    /*--------------------------------
     * now the loop for tracing the trajectory until it backscattered
     * or it has energy too small to backscatter
     */
    filtf=zcor>0 && eng>E_MIN && eng>(-de*zcor/step);
    icic=filtf;
/*  printf("\n");
    printf("icic,zcor,eng,de,step %d %e %e %e %e\n",
       icic,zcor,eng,de,step);
       */
    while(icic)
      {

      r_over_i=p1*100.0*pow(eng,zex)/zz+zz*zz*zz/eng/p2/1.0e5;

      engo=(eng+511)/(eng+1024);
      sigma = p4*4.0e-18*z165*engo*engo/
              (eng+p3*1.0e-2*z133*pow(eng,0.5));
      lamda= lamda_a/sigma;
      alpha=7.0e-3/eng;

      result = rand_mr();
      ran1=result/base;
      result = rand_mr();
      ran2=result/base;
      result = rand_mr();
      ran3=result/base;
      result = rand_mr();
      ran4=result/base;
      filtr=ran4<r_over_i/(r_over_i+1.0);

      step = -lamda*log(ran1);
      c_phi=filtr*(-2*alpha*ran2/(alpha-ran2+1)+1)+(1-filtr)*(1-2*ran2);
      s_phi=sqrt(1.0-c_phi*c_phi);
      psi=2*PI*ran3;
      sz=sqrt(1.0-cz*cz);
      cc = -s_phi*cos(psi)*sz+c_phi*cz;

      zcor=zcor+step*cc;
      de=step*p_sa*log(j_a*eng+1.0)/eng;
      eng=eng+de;
      cz=cc;

      filtf=zcor>0 && eng>E_MIN && eng>(-de*zcor/step);
      icic=filtf;

      }  /* end of the tracing process for this trajectory */

    if(zcor<0) iback++;

    }  /* end of the NTRAJ trajectories */

#ifdef _DOPRINT_
  printf("point: engt %f, zz %f, A %f, rho %f\n",engt,zz,aa,rho);
  printf("backscattering coeff for this point is %f \n",(double)iback/(double)NTRAJ);
#endif
  return (double)iback/(double)NTRAJ;

}  /* end of back subroutine */
/**************************************************************/


/**********************************************************************/

/**********************************************************************/
/**********************************************************************/
/**********************     main function       ***********************/
/**********************                         ***********************/

main(int argc, char ** argv)
{
int i,j,k,m;
int ii,jj,icic,icic2,icc;
double coe[NPAR];
const char *fitfile, *montefile, *paramfile;
FILE *fpot, *fcoe, *fbac;
double point[POINT][4];
double backsct[POINT],baexp[POINT],ajunk,bjunk;
double pointt[4],coet[NPAR],backcoe;
double rms,oldrms;

/* ----------------------------------------------------------
 * Read in the elements that need to be considered and the energy that
 * need to be calculated for the back-scattering coefficient
 */

if (argc < 4) {
   fprintf(stderr, "Not all files specified.\n Correct command line: %s fit.inp monte.dat param.dat\n", argv[0]);
   exit(1);
}

fitfile = argv[1];
montefile = argv[2];
paramfile = argv[3];


if ((fpot=fopen(fitfile,"r")) == NULL)
  {
  printf ("Error in opening %s\n", fitfile);
  exit(1);
  }

if ((fcoe=fopen(paramfile,"r")) == NULL)
  {
  printf ("Error in opening %s\n", paramfile);
  exit(1);
  }

if ((fbac=fopen(montefile,"r")) == NULL)
  {
  printf ("Error in opening %s\n", montefile);
  exit(1);
  }
/*-----------------------------------------------*/

/*-----------------------------------------------*/
/* Now, read in the data */

for(i=0;i<POINT;i++)
  {
  for(j=0;j<4;j++) fscanf(fpot,"%lf",&point[i][j]);
  for(j=0;j<4;j++) printf("%e ",point[i][j]);
  printf("\n");
  }
printf("\n");

for(i=0;i<NPAR;i++)
  {
  fscanf(fcoe,"%lf",&coe[i]);
  printf("%e \n",coe[i]);
  }
printf("\n");

for(i=0;i<POINT;i++)
  {
  fscanf(fbac,"%lf%lf%lf",&ajunk,&bjunk,&baexp[i]);
  printf("%e %e %e\n",ajunk,bjunk,baexp[i]);
  }
printf("\n\n");
fflush(stdout);

fclose(fpot);
fclose(fcoe);
fclose(fbac);

/****************************************************/
/*-----------------------
 * now calculate the POINT points of backscattering coefficients
 */

printf("first plot\n");
printf("parameter set: \n");
for(i=0;i<NPAR;i++) printf(" %e ",coe[i]);
printf("\n\n");
fflush(stdout);
 
for(i=0;i<NPAR;i++) coet[i]=coe[i];

for(ii=0;ii<POINT;ii++)
  {
  for(i=0;i<4;i++) pointt[i]=point[ii][i];
  backcoe = back(pointt,coet);
  backsct[ii]=backcoe;
  fflush(stdout);
  }

rms=rmse(backsct,baexp);
oldrms=rms;
icic=oldrms>ERRCRI;
printf("icic %d, rms %f \n\n",icic,oldrms);
fflush(stdout);

/*---------------------------------*/
/* Now we begin the loop for automated fitting      */

while(icic)
  {

/*----------------------------------------------------*/
  icc=0;

  for(jj=0;jj<NPAR;jj++)
    {
    printf("Changing # %d \n\n",jj);
    fflush(stdout);

    for(i=0;i<NPAR;i++) coet[i]=coe[i];
    coet[jj]=coe[jj]*1.2;

    for(ii=0;ii<POINT;ii++)
      {
      for(i=0;i<4;i++) pointt[i]=point[ii][i];
      backcoe = back(pointt,coet);
      backsct[ii]=backcoe;
      fflush(stdout);
      }

    rms=rmse(backsct,baexp);
    icic2=rms<oldrms;

    if(icic2) 
    while(icic2)
      {
      icc=1;
      oldrms=rms;
      for(i=0;i<NPAR;i++) coe[i]=coet[i];
      printf("new parameter set: \n");
      for(i=0;i<NPAR;i++) printf(" %e ",coe[i]);
      printf("\n");
      printf("corresponding rms: %f \n\n",oldrms);
      fflush(stdout);

      printf("increasing # %d \n",jj);
      fflush(stdout);

      for(i=0;i<NPAR;i++) coet[i]=coe[i];
      coet[jj]=coe[jj]*1.2;

      for(ii=0;ii<POINT;ii++)
	{
        for(i=0;i<4;i++) pointt[i]=point[ii][i];
        backcoe = back(pointt,coet);
        backsct[ii]=backcoe;
	fflush(stdout);
        }

      rms=rmse(backsct,baexp);
      icic2=rms<oldrms;
      } /* end of while and if */

    else  
      {
      for(i=0;i<NPAR;i++) coet[i]=coe[i];
      coet[jj]=coe[jj]*0.8;

      for(ii=0;ii<POINT;ii++)
        {
        for(i=0;i<4;i++) pointt[i]=point[ii][i];
        backcoe = back(pointt,coet);
        backsct[ii]=backcoe;
        fflush(stdout);
        }

      rms=rmse(backsct,baexp);
      icic2=rms<oldrms;

      while(icic2)
        {
	icc=1;
        oldrms=rms;
        for(i=0;i<NPAR;i++) coe[i]=coet[i];
        printf("new parameter set: \n");
        for(i=0;i<NPAR;i++) printf(" %e ",coe[i]);
        printf("\n");
        printf("corresponding rms: %f \n\n",oldrms);
        fflush(stdout);

        printf("decreasing # %d \n",jj);
        fflush(stdout);

        for(i=0;i<NPAR;i++) coet[i]=coe[i];
        coet[jj]=coe[jj]*0.8;

        for(ii=0;ii<POINT;ii++)
          {
          for(i=0;i<4;i++) pointt[i]=point[ii][i];
          backcoe = back(pointt,coet);
          backsct[ii]=backcoe;
	  fflush(stdout);
          }

        rms=rmse(backsct,baexp);
        icic2=rms<oldrms;
        }  /* end of while */
      }  /* end of else */
    } /* end of for(jj=0... */

  icic=icc*(oldrms>ERRCRI);

  }  /* end of while(icic) */

  printf("\n\n***********************************************\n\n");
  printf("End of fitting process\n\n");
  printf("final parameter set: \n");
  for(i=0;i<NPAR;i++) printf(" %e ",coe[i]);
  printf("\n\n Corresponding rmse: %f \n\n",oldrms);
  printf("----------------------------------------\n\n");
  fflush(stdout);

}
