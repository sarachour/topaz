#include <stdio.h>
#include <string.h>
#include <math.h>

#include <iostream>
#include <fstream>
#include "barnes.h"

#include "barnes.h"


void matClr(matrix_t * mmat) {
  int i, j;
  for(i = 0; i < NDIM; i++)
	for(j = 0; j < NDIM; j++)
	  mmat->val[i][j] = 0.0;
}

void matCons(matrix_t * mmat){
	matClr(mmat);
}

void matDes(matrix_t * mmat){
	matClr(mmat);
}

double  matGetVal(matrix_t * mmat, int row, int col) { 
	return mmat->val[row][col]; 

}


void    matSetId(matrix_t * mmat) {
  int i;
  matClr(mmat);
  for(i=0; i < NDIM; i++)
	mmat->val[i][i] = 1.0;
}

void    matSet(matrix_t * mmat, matrix_t *m) {
  int i, j;
  for(i = 0; i < NDIM; i++)
	for(j = 0; j < NDIM; j++)
	  mmat->val[i][j] = m->val[i][j];
}

void    matTrans(matrix_t * mmat) {
  int i, j;
  matrix_t tmp = *mmat;
  for(i=0; i < NDIM; i++)
	for(j=0; j < NDIM; j++)
	  //mmat->val[i][j] = tmp.val[j][i];
	  mmat->val[i][j] = mmat->val[j][i];
}

void    matAdd(matrix_t * mmat, matrix_t *m) {
  int i, j;
  for(i = 0; i < NDIM; i++)
	for(j = 0; j < NDIM; j++)
	  mmat->val[i][j] += m->val[i][j];
}

void    matSub(matrix_t * mmat, matrix_t *q, matrix_t *r) {
  int i, j;
  for(i = 0; i < NDIM; i++)
	for(j = 0; j < NDIM; j++)
	  mmat->val[i][j] = q->val[i][j] - r->val[i][j];
}

void    matMult(matrix_t * mmat, matrix_t *q, matrix_t *r) {
  int i, j, k;
  for(i=0; i < NDIM; i++)
	for(j=0; j < NDIM; j++) {
	  mmat->val[i][j] = 0.0;
	  for(k = 0; k < NDIM; k++)
		mmat->val[i][j] += q->val[i][k] * r->val[k][j];
	}
}

void    matScale(matrix_t * mmat, matrix_t *q, double f){
  int i, j;
  for(i = 0; i < NDIM; i++)
	for(j = 0; j < NDIM; j++)
	  mmat->val[i][j] = q->val[i][j] * f;
}

double  matTrace(matrix_t * mmat) {
  int i; double s;
  s = 0.0;
  for(i=0; i < NDIM; i++)
	s += mmat->val[i][i];
  return s;
}

void    matVectMult(matrix_t * mmat, double *v, double *u) {
  int i, j;
  for(i=0; i < NDIM; i++) {
	v[i] = 0.0;
	for(j=0; j < NDIM; j++)
	  v[i] += mmat->val[i][j] * u[j];
  }
}

void    matOutProd(matrix_t * mmat, double *v, double *u){
  int i, j;
  for(i=0; i < NDIM; i++)
	for(j=0; j < NDIM; j++)
	  mmat->val[i][j] = v[i] * u[j];
}

void    matPrint(matrix_t * mmat){ 
	  int i, j;
	  std::cout << "[";
	  for(i=0; i < NDIM; i++){
		std::cout << "(";
		for(j=0; j < NDIM; j++)
		  std::cout << mmat->val[i][j] << " ";
		std::cout << ")";
	  }
	  std::cout << "]";
}
