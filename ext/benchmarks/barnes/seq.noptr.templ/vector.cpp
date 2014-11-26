#include <stdio.h>
#include <string.h>
#include <math.h>

#include <iostream>
#include <fstream>
#include "barnes.h"

void in_vector(std::ifstream& f, double *v){
  double x1, x2, x3;
  f >> x1 >> x2 >> x3;
  v[0] = x1; v[1] = x2; v[2] = x3;
}

void tvecClr(double *v) { int i; for(i=0; i < NDIM; i++) v[i] = 0.0; }

void tvecAdd(double *v1, double *v2) { int i;
for(i=0; i < NDIM; i++) v1[i] += v2[i]; }

void tvecSub(double *v1, double *v2) { int i;
for(i=0; i < NDIM; i++) v1[i] -= v2[i]; }

void tvecLoad(double *v1, double *v2) { int i;
for(i=0; i < NDIM; i++) v1[i] = v2[i]; }

void tvecStore(double *v1, double *v2) { int i;
for(i=0; i < NDIM; i++) v2[i] = v1[i]; }

void tvecDiv(double *v1, double *v2) { int i;
for(i=0; i < NDIM; i++) v1[i] /= v2[i]; }

void tvecProd(double *v1, double *v2) { int i;
for(i=0; i < NDIM; i++) v1[i] *= v2[i]; }

void tvecScale(double *v, double s) { int i;
for(i=0; i < NDIM; i++) v[i] *= s; }

double tvecDotProd(double *v1,double *v2) { int i; double d = 0.0;
  for(i=0; i < NDIM; i++) d += (v1[i] * v2[i]); return d; }

void tvecCrossProd(double *v, double *u, double *w){
  v[0] = u[1]*w[2] - u[2]*w[1];
  v[1] = u[2]*w[0] - u[0]*w[2];
  v[2] = u[0]*w[1] - u[1]*w[0];
}

void tvecMin(double *v1, double  *v2){ int i;
  for(i=0; i < NDIM; i++) if(v1[i] > v2[i]) v1[i] = v2[i]; }

double tvecNorm1(double *v){ int i; double d;
d = 0.0; for(i=0; i < NDIM; i++) d += v[i]; return d; }


void vecPrint(vector_t * v) { 
	printf("(%8.4f %8.4f %8.4f) ",v->val[0],v->val[1],v->val[2]); 
}
void vecClr(vector_t * vc) { 
	int i; 
	for(i=0; i < NDIM; i++) 
		vc->val[i] = 0.0;
}
void vecCons(vector_t * vc){
	vecClr(vc);
}
void vecSetUnit(vector_t * vc) {
	 int i; 
	 for(i=0; i < NDIM; i++) 
		vc->val[i] = 1.0; 
}
void vecSub(vector_t * vc, double *v) {
	int i; 
	for(i=0; i < NDIM; i++) vc->val[i] -= v[i]; 
}
void vecProd(vector_t * vc, double *v){ 
	int i; 
	for(i=0; i < NDIM; i++) vc->val[i] *= v[i]; 
}
void vecScale(vector_t * vc,double s){ 
	int i; 
	for(i=0; i < NDIM; i++) vc->val[i] *= s; 
}
double vecGetVal(vector_t * vc,int idx)		{ 
	return vc->val[idx]; 
}
void vecFieldAdd(vector_t * vc,int idx,double v)	{ 
	vc->val[idx] += v; 
}
void vecFieldClr(vector_t * vc,int idx)		{ 
	vc->val[idx] = 0.0; 
}
void vecFieldSet(vector_t * vc,int idx, double v)	{ 
	vc->val[idx] = v; 
}
void vecLoad(vector_t * vc,double *v) { 
	int i; 
	for(i=0; i < NDIM; i++) 
		vc->val[i] = v[i]; 
	}
void PRTV(vector_t * vc,char *name)   { 
	std::cout << name <<"= ["<<vc->val[0]<<" "<<vc->val[1]<<" "<<vc->val[2]<<"] "; 
}

void vecMin(vector_t * vc,double *v){ int i;
  for(i=0; i < NDIM; i++)
    if(vc->val[i] > v[i])
      vc->val[i] = v[i];
}
void vecMax(vector_t * vc,double *v){ int i;
  for(i=0; i < NDIM; i++)
    if(vc->val[i] < v[i])
      vc->val[i] = v[i];
}

double vecNorm1(vector_t * vc){
  int i; double d=0.0;
  for(i=0; i < NDIM; i++)
    d += vc->val[i];
  return d;
}

double vecSqNorm2(vector_t * vc){
  int i; double d=0.0;
  for(i=0; i < NDIM; i++)
    d += (vc->val[i]*vc->val[i]);
  return d;
}


void vecAdd(vector_t * vc, double *v){
  int i;
  for(i=0; i < NDIM; i++){
    vc->val[i] += v[i];
  }
}

void vecStore(vector_t * vc, double *v){
  int i;
  for(i=0; i < NDIM; i++){
     v[i] = vc->val[i];
  }
}
