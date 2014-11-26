#include "detectors.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include <glpk.h>

#define CTRL_MEMORY (1/100000.0)
#define IDX(i,j,n) (i*(n)+j)
ControlIDetectorData::ControlIDetectorData(int n):
	IDetectorData(n){
	this->n = n;
	
	this->weights = new float[n];
	this->correlations = new float[(n)*(n)];
	this->reexec_rate = 0;
	memset(this->correlations, 0, sizeof(float)*(n)*(n));
	
	
	this->scratch = new int[n+1];
	this->dscratch = new double[n+1];
	
	this->weights_sum = 0;
	for(int i=0; i < this->n; i++){
		this->weights[i] = 1.0;
		this->correlations[IDX(i,i,n)] = 1.0;
		this->weights_sum += this->weights[i];
		
	}
}
ControlIDetectorData::~ControlIDetectorData(){
	delete this->correlations;
	delete this->weights;
	delete this->neighbors;
	
	delete this->scratch;
	delete this->dscratch;
}
void ControlIDetectorData::print(int q){
	if(q != 0) return;
	printf("\nTARGET: %f\n", this->target);
	printf("MEASURED: %f\n", this->reexec_rate);  
	printf("\n#weights\n");
	printf("index\tweight\n");
	int n = this->n;
	for(int i=0; i <n; i++){
		printf("%d\t%f\n",i, this->weights[i]/this->weights_sum);
	}
	printf("#correlations\n");
	for(int i=0; i <n; i++){
		printf("\t%d",i);
	}
	printf("\n");
	for(int i=0; i < n; i++){
		printf("%d",i);
		for(int j=0; j < this->n; j++){
			int idx =IDX(i,j,this->n);
			printf("\t%f", this->correlations[idx]);
		}
		printf("\n");
	}
	printf("#target reexecution rates\n");
	for(int i=0; i < n; i++){
		printf("%d\t%f\n",i,this->comp_targets[i]);
	}
	printf("#projected reexec rates\n");
	for(int i=0; i < n; i++){
		double amt=0;
		for(int j=0; j < n ; j++){
			amt += this->correlations[IDX(i,j,n)]*this->comp_targets[i];
		}
		printf("%d\t%f\n", i, amt);
	}
	printf("#measured reexec rate:%f\n", this->reexec_rate);
}
void ControlIDetectorData::set_target(double t){
	int n = this->n;
	IDetectorData::set_target(t);
	for(int i=0; i < n; i++){
		this->comp_targets[i] = t*this->weights[i]/this->weights_sum;
	}
}
void ControlIDetectorData::update(int idx, double reexec){
	//printf("%d %f\n", idx, reexec);
	this->scratch[idx] = reexec< 0.5? 1 : 0;
}

#define MAT1 3
#define a(i,j) a[(i)*MAT1+(j)]

// Crout uses unit diagonals for the upper triangle
void Crout(int d,float*S,float*D){
   for(int k=0;k<d;++k){
      for(int i=k;i<d;++i){
         float sum=0.;
         for(int p=0;p<k;++p)sum+=D[i*d+p]*D[p*d+k];
         D[i*d+k]=S[i*d+k]-sum; // not dividing by diagonals
      }
      for(int j=k+1;j<d;++j){
         float sum=0.;
         for(int p=0;p<k;++p)sum+=D[k*d+p]*D[p*d+j];
			D[k*d+j]=(S[k*d+j]-sum)/D[k*d+k];
      }
   }
}
double solveCrout(int d,float*LU,float*b,double*x){
   float y[d];
   for(int i=0;i<d;++i){
      float sum=0.;
      for(int k=0;k<i;++k)
		sum+=LU[i*d+k]*y[k];
      y[i]=(b[i]-sum)/LU[i*d+i];
   }
   double sums=0;
   for(int i=d-1;i>=0;--i){
      float sum=0.;
      for(int k=i+1;k<d;++k)
		sum+=LU[i*d+k]*x[k];
      x[i]=(y[i]-sum); // not dividing by diagonals
	  sums += x[i];
   }
   return sums;
}


int CNT=0;


void ControlIDetectorData::resolve(){
	int n = this->n;
	bool isrej=false;
	//update cross correlationn matrix C
	CNT++;
	for(int i=0; i < n; i++){
		//frequency we reject.
		if(this->scratch[i] == 1) isrej = true;
		for(int j=i; j < n; j++){
			float corr = this->correlations[IDX(i,j,n)];
			//neither point has an error.
			if(this->scratch[i] == 0 && this->scratch[j] == 0){
				
			}
			else{
				//update cross correlations
				float iscorr = this->scratch[i] == this->scratch[j] ? 1 : 0;
				this->correlations[IDX(j,i,n)] = 
				this->correlations[IDX(i,j,n)] = corr*(1.0 - CTRL_MEMORY) + iscorr*CTRL_MEMORY;
			}
			
		}
	}
	this->reexec_rate *= (1.0 - CTRL_MEMORY);
	if(isrej) this->reexec_rate += CTRL_MEMORY;
	
	if(isrej){
		this->reexec_rate += CTRL_MEMORY;
		this->weights_sum = 0;
		for(int i=0; i < n; i++){
			this->weights[i] = this->weights[i]*(1.0 - CTRL_MEMORY) + this->scratch[i]*CTRL_MEMORY;
			this->weights_sum += this->weights[i];
		}
		for(int i=0; i < n; i++){
			//this->comp_targets[i] = this->target*this->weights[i]/this->weights_sum;
		}
	}
	

	//perform back-substitution to figure out good target rates.
	//### CROUT LU DECOMPOSITION
	/*
	if(isrej){
		float * LU = (float*) malloc(sizeof(float)*n*n);
		float * targ = (float *) malloc(sizeof(float)*n);
		for(int i=0; i < n; i++){
			targ[i] = this->target/(n);
		}
		Crout(n,this->correlations,LU); //LU Decomposition
		//LU Decomposition, 
		for(int i=0; i < n; i++){
			double indivsum = solveCrout(n,LU,targ,this->comp_targets);
			
		}
		free(targ);
		free(LU);
	}
	*/
	//## BACK SUBSTITUTION
	/*
	for(int i=n-1; i >= 0; i--){
		float compute = 0;
		for(int j=i+1; j < n; j++){
			compute += this->correlations[IDX(i,j,n)]*this->comp_targets[j];
		}
		this->comp_targets[i] = this->target*this->weights[i];
		this->comp_targets[i] -= compute;
	} 
	*/

	//Linear Convec Problem
	if(isrej){
		char name[10];
		glp_term_out(false);
		glp_prob * lp = glp_create_prob();
		glp_set_prob_name(lp, "corr");
		glp_set_obj_dir(lp, GLP_MAX);
		glp_add_rows(lp,1);
		glp_add_cols(lp,n);
		for(int i=0; i < n; i++){
			int idx = i+1;
			this->dscratch[idx] = 0;
			for(int j=0; j < n ; j++){
				this->dscratch[idx] += this->correlations[IDX(i,j,n)];
			}
			sprintf(name, "e%d",i);
			glp_set_col_name(lp,idx,name);
			glp_set_col_bnds(lp,idx,GLP_DB, this->target/n*0.5, this->target/n*1.5);
			glp_set_obj_coef(lp,idx, this->weights[i]);
			this->scratch[idx] = idx;
		}
		glp_set_row_name(lp,1,"c");
		///this->comp_targets[i]
		glp_set_row_bnds(lp,1,GLP_DB, this->target*0.9, this->target*1.1); // we want to reexecute everything
		glp_set_mat_row(lp, 1, n, this->scratch, this->dscratch);
		//maximize this
			
		
		glp_simplex(lp,NULL);
		float res = glp_get_obj_val(lp);
		for(int i=0; i < n; i++){
			float par = glp_get_col_prim(lp,i+1);
			this->dscratch[i] = par;
		}
		for(int i=0; i < n; i++){
			this->comp_targets[i] = 0;
			for(int j=0; j < n; j++){
				this->comp_targets[i] += this->correlations[IDX(i,j,n)]*this->dscratch[j];
			}
		}
		glp_delete_prob(lp);
		glp_free_env();
	}
	
	
}

bool ControlIDetectorData::train(int id){
	return true;
}
