#include <topaz.h> 
#include <task.h> 
#include <checker.h> 
/*
* Copyright (C) 2008 Princeton University
* All rights reserved.
* Authors: Jia Deng, Gilberto Contreras
*
* streamcluster - Online clustering algorithm
*
*/ 
#include <stdio.h>
 #include <iostream>
 #include <fstream>
 #include <stdlib.h>
 #include <string.h>
 #include <assert.h>
 #include <math.h>
 #include <sys/resource.h>
 #include <limits.h>
 
#ifdef ENABLE_THREADS
 #include <pthread.h>
 #include "parsec_barrier.hpp"
 #endif
 #include "streamcluster.h"
 

#ifdef ENABLE_PARSEC_HOOKS
 #include <hooks.h>
 #endif
 
using namespace std; 


/* For a given point x, find the cost of the following operation:
* -- open a facility at x if there isn't already one there,
* -- for points y such that the assignment distance of y exceeds dist(y, x),
*    make y a member of x,
* -- for facilities y such that reassigning y and all its members to x
*    would save cost, realize this closing and reassignment.
*
* If the cost of this operation is negative (i.e., if this entire operation
* saves cost), perform this operation and return the amount of cost saved;
* otherwise, do nothing.
*/ 

/* numcenters will be updated to reflect the new number of centers */ 
/* z is the facility cost, x is the number of this point in the array
points */ 



/*
*  we should also compute the error between the points and the centers.
*/ 

double pgain (long x, Points *points, double z, long int *numcenters )
{
	
	
	
	//my block
 long size = points->num; 
	
	int i; 
	int number_of_centers_to_close = 0; 
	
	double *work_mem; 
	double gl_cost_of_opening_x; 
	int gl_number_of_centers_to_close; 
	
	
	//each thread takes a block of working_mem.
 int stride = *numcenters+2; 
	//make stride a multiple of CACHE_LINE
 int cl = CACHE_LINE/sizeof (double ); 
	if (stride % cl != 0 ){
		stride = cl * (stride / cl + 1 ); 
	}
	int K = stride -2 ; // K==*numcenters
 
	//my own cost of opening x
 double cost_of_opening_x = 0; 
	
	work_mem = new double[stride*2]; 
	gl_cost_of_opening_x = 0; 
	gl_number_of_centers_to_close = 0; 
	
	
	/*For each center, we have a *lower* field that indicates
    how much we will save by closing the center.
    Each thread has its own copy of the *lower* fields as an array.
    We first build a table to index the positions of the *lower* fields.
    */ 
	int count = 0; 
	for (int i = 0; i < size; i++ ){
		if (is_center[i] ){
			center_table[i] = count++; 
		}
	}
	work_mem[0] = count; 
	
	
	int accum = 0; 
	int tmp = (int )work_mem[0]; 
	work_mem[0] = accum; 
	accum += tmp; 
	
	
	for (int i = 0; i < size; i++ ){
		if (is_center[i] ){
			center_table[i] += (int )work_mem[0]; 
		}
	}
	
	//now we finish building the table. clear the working memory.
 bool* switch_membership = new bool[size]; 
	memset (switch_membership + 0, 0, (size-0 )*sizeof (bool )); 
	memset (work_mem, 0, stride*sizeof (double )); 
	memset (work_mem+stride,0,stride*sizeof (double )); 
	
	//my *lower* fields
 double* lower = &work_mem[0]; 
	//global *lower* fields
 double* gl_lower = &work_mem[stride]; 
	
	for (i = 0; i < size; i++ ){
		float x_cost = dist (points->p[i], points->p[x], points->dim )
		* points->p[i].weight; 
		float current_cost = points->p[i].cost; 
		
		if (x_cost < current_cost ){
			
			// point i would save cost just by switching to x
 // (note that i cannot be a median,
 // or else dist(p[i], p[x]) would be 0)
 
			switch_membership[i] = 1; 
			cost_of_opening_x += x_cost - current_cost; 
			
		}else {
			
			// cost of assigning i to x is at least current assignment cost of i
 
			// consider the savings that i's **current** median would realize
 // if we reassigned that median and all its members to x;
 // note we've already accounted for the fact that the median
 // would save z by closing; now we have to subtract from the savings
 // the extra cost of reassigning that median and its members
 int assign = points->p[i].assign; 
			lower[center_table[assign]] += current_cost - x_cost; 
		}
	}
	
	// at this time, we can calculate the cost of opening a center
 // at x; if it is negative, we'll go through with opening it
 
	for (int i = 0; i < size; i++ ){
		if (is_center[i] ){
			double low = z; 
			//aggregate from all threads
 low += work_mem[center_table[i]]; 
			gl_lower[center_table[i]] = low; 
			if (low > 0 ){
				// i is a median, and
 // if we were to open x (which we still may not) we'd close i
 
				// note, we'll ignore the following quantity unless we do open x
 ++number_of_centers_to_close; 
				cost_of_opening_x -= low; 
			}
		}
	}
	
	//use the rest of working memory to store the following
 work_mem[K] = number_of_centers_to_close; 
	work_mem[K+1] = cost_of_opening_x; 
	
	
	//  printf("thread %d cost complete\n",pid);
 
	gl_cost_of_opening_x = z; 
	gl_number_of_centers_to_close += (int )work_mem[K]; 
	gl_cost_of_opening_x += work_mem[K+1]; 
	// Now, check whether opening x would save cost; if so, do it, and
 // otherwise do nothing
 
	n_switches=0; 
	if (gl_cost_of_opening_x < 0 ){
		//  we'd save money by opening x; we'll do it
 for (int i = 0; i < size; i++ ){
			bool close_center = gl_lower[center_table[points->p[i].assign]] > 0 ; 
			if (switch_membership[i] || close_center ){
				// Either i's median (which may be i itself) is closing,
 // or i is closer to x than to its current median
 float ocost = points->p[i].cost; 
				points->p[i].cost = points->p[i].weight * dist (points->p[i], points->p[x], points->dim ); 
				points->p[i].assign = x; 
				if (switch_idx != NULL ){
					switch_idx[n_switches] = i; 
					switch_cost_delta[n_switches] = points->p[i].cost-ocost; 
					n_switches++; 
				}
			}
		}
		
		for (int i = 0; i < size; i++ ){
			if (is_center[i] && gl_lower[center_table[i]] > 0 ){
				is_center[i] = false; 
			}
		}
		
		if (x >= 0 && x < size ){
			is_center[x] = true; 
		}
		*numcenters = *numcenters + 1 - gl_number_of_centers_to_close; 
	}
	else {
		gl_cost_of_opening_x = 0; // the value we'll return
 }
	delete (work_mem ); 
	delete switch_membership; 
	
	//delete switch_membership;
 return -gl_cost_of_opening_x; 
}



/* facility location on the points using local search */ 
/* z is the facility cost, returns the total cost and # of centers */ 
/* assumes we are seeded with a reasonable solution */ 
/* cost should represent this solution's cost */ 
/* halt if there is < e improvement after iter calls to gain */ 
/* feasible is an array of numfeasible points which may be centers */ 



float pFL_main (Points *points, int *feasible, int numfeasible, float z, long *k, double cost, long iter, float e ){
	double change; 
	long numberOfPoints; 
	//sizeof(k) = 1 long
 numberOfPoints = points->num; 
	change = cost; 
	//allocate
 //is_center = new bool[numberOfPoints];
 //center_table = new int[numberOfPoints];
 switch_cost_delta = new float[numberOfPoints]; 
	switch_idx = new int[numberOfPoints]; 
	
	while (change/cost > 1.0*e ){
		change = 0.0; 
		numberOfPoints = points->num; 
		//MAKE SURE TO ENABLE RANDOMIZE
 //intshuffle(feasible, numfeasible);
 int x; 
		//printf("		- starting iters\n");
 for (int i=0;i<iter;i++ ){
			//printf("		- iter %d\n", i);
 //printf("center table: %x\n", center_table);
 //printf("is center: %x\n", is_center);
 //printf("switch cost delta: %x\n", switch_cost_delta);
 //printf("switch idx: %x\n", switch_idx);
 //printf("# switches: %d\n", n_switches);
 x = i%numfeasible; 
			float gain = pgain (feasible[x], points, z, k ); 
			change += gain; 
		}
		cost -= change; 
		
	}
	//delete is_center;
 //delete center_table;
 delete switch_cost_delta; 
	delete switch_idx; 
	return (cost ); 
}

float pFL_helper (Points zpts, bool * iscent, int * centtbl, float * swcost, int *swidx, long int k, int nz, int x, 
int * nsw, float * gain, int * ok, bool * o_iscent, int * o_centtbl )// output
 {
	
	is_center = iscent; 
	center_table = centtbl; 
	switch_cost_delta = swcost; 
	switch_idx = swidx; 
	
	*gain = pgain (x, &zpts, nz, &k ); // calculate gain
 *nsw = n_switches; 
	
	for (int q =0; q < MAX_PTS; q++ ){
		o_iscent[q] = is_center[q]; 
		o_centtbl[q] = center_table[q]; 
	}
	*ok = k; 
	
	free_points (zpts ); 
}
Points * gpoints; 
float pFL (Points *points, int *feasible, int numfeasible, float z, long *k, double cost, long iter, float e )
{
	double change; 
	long numberOfPoints; 
	gpoints = points; 
	//return pFL_main(points, feasible,numfeasible,z,k,cost,iter,e);
 
	//sizeof(k) = 1 long
 change = cost; 
	to_flt_primitive (*points, prim_pts_flt ); 
	to_int_primitive (*points, prim_pts_int ); 
	
	while (change/cost > 1.0*e ){
		change = 0.0; 
		numberOfPoints = points->num; 
		// randomize order in which centers are considered
 //MAKE SURE TO ENABLE RANDOMIZE
 //intshuffle(feasible, numfeasible);
 //taskset
 int tx ; 
		int tk ; 
		char* t_iscent ; 
		int* t_centtbl ; 
		float tz ; 
		float* tflt_pts ; 
		int* tint_pts ; 
		int ok ; 
		float ogain ; 
		char* o_iscent ; 
		int* o_centtbl ; 
		int* swidx ; 
		float* swcostdelta ; 
		int nsw ; 
		Topaz::topaz->start (0 ); 
		for (int i = 0 ; i <= (iter)-1 ; i += 1 )
		{
			Topaz::topaz->send (0 , i , feasible[i%numfeasible] , k[0] , (char*)is_center , center_table , z , prim_pts_flt , prim_pts_int ); 
			Topaz::topaz->unpack_inputs (0 , &i , &tx , &tk , &t_iscent , &t_centtbl , &tz , &tflt_pts , &tint_pts ); 
			if (Topaz::topaz->receive (0 , &i , &ok , &ogain , &o_iscent , &o_centtbl , &swidx , &swcostdelta , &nsw )){
				change += ogain; 
				int x = feasible[i%numfeasible]; 
				bool * biscent = (bool * )o_iscent; 
				for (int q=0; q < points->num; q++ ){
					is_center[q] = biscent[q]; //copy over center table
 center_table[q] = o_centtbl[q]; 
				}
				
				//modify points.
 for (int q=0; q < nsw; q++ ){//for each switch
 int idx = swidx[q]; 
					points->p[idx].cost += swcostdelta[q]; 
					points->p[idx].assign = x; 
				}
				to_flt_primitive (*points, prim_pts_flt ); 
				to_int_primitive (*points, prim_pts_int ); 
				*k = ok; 
				//printf("iteration %d\n",i);
 }
		}
		cost -= change; 
		
	}
	return (cost ); 
}





//synthetic stream
 
//int main(int argc, char **argv)
 int topaz_main (int argc, char **argv )
{
	char *outfilename = new char[MAXNAMESIZE]; 
	char *infilename = new char[MAXNAMESIZE]; 
	long kmin, kmax, n, chunksize, clustersize; 
	int dim; 
	
	#ifdef PARSEC_VERSION
 #define __PARSEC_STRING(x) #x
 #define __PARSEC_XSTRING(x) __PARSEC_STRING(x)
 fprintf (stderr,"PARSEC Benchmark Suite Version "__PARSEC_XSTRING(PARSEC_VERSION)" \n" ); 
	fflush (NULL ); 
	#else
 fprintf (stderr,"PARSEC Benchmark Suite\n" ); 
	fflush (NULL ); 
	#endif //PARSEC_VERSION
 #ifdef ENABLE_PARSEC_HOOKS
 __parsec_bench_begin (__parsec_streamcluster ); 
	#endif
 
	if (argc<7 ){
		fprintf (stderr,"usage: %s k1 k2 chunksize clustersize infile outfile\n", 
		argv[0] ); 
		fprintf (stderr," k1: Min. number of centers allowed\n" ); 
		fprintf (stderr," k2: Max. number of centers allowed\n" ); 
		fprintf (stderr," chunksize: Number of data points to handle per step\n" ); 
		fprintf (stderr," clustersize: Maximum number of intermediate centers\n" ); 
		fprintf (stderr," infile: Input file (if n<=0 )\n" ); 
		fprintf (stderr," outfile: Output file\n" ); 
		fprintf (stderr,"\n" ); 
		fprintf (stderr, "if n > 0, points will be randomly generated instead of reading from infile.\n" ); 
		exit (1 ); 
	}
	
	
	
	kmin = atoi (argv[1] ); 
	kmax = atoi (argv[2] ); 
	chunksize = atoi (argv[3] ); 
	clustersize = atoi (argv[4] ); 
	strcpy (infilename, argv[5] ); 
	strcpy (outfilename, argv[6] ); 
	dim = 2; // force into 2d
 
	srand48 (SEED ); 
	PStream* stream; 
	stream = new FileStream (infilename ); 
	
	
	#ifdef ENABLE_PARSEC_HOOKS
 __parsec_roi_begin (); 
	#endif
 
	streamCluster (stream, kmin, kmax, dim, chunksize, clustersize, outfilename ); 
	
	#ifdef ENABLE_PARSEC_HOOKS
 __parsec_roi_end (); 
	#endif
 
	delete stream; 
	
	#ifdef ENABLE_PARSEC_HOOKS
 __parsec_bench_end (); 
	#endif
 
	return 0; 
}
void pfl_COMPUTE_TPZ (Task* __INPUT , Task* __OUTPUT )
{
	int i ; 
	int tx ; 
	int tk ; 
	char* t_iscent ; 
	int* t_centtbl ; 
	float tz ; 
	float* tflt_pts ; 
	int* tint_pts ; 
	int ok ; 
	float ogain ; 
	char o_iscent [ MAX_PTS ] ; 
	int o_centtbl [ MAX_PTS ] ; 
	int swidx [ MAX_PTS ] ; 
	float swcostdelta [ MAX_PTS ] ; 
	int nsw ; 
	Topaz::topaz->unpack_inputs (0 , &i , &tx , &tk , &t_iscent , &t_centtbl , &tz , &tflt_pts , &tint_pts ); 
	pin_start_inject_errors (); 
	
	Points zpts; 
	long int lk; 
	from_primitives (zpts, tflt_pts, tint_pts ); 
	
	pFL_helper (zpts, (bool * )t_iscent, t_centtbl, swcostdelta, swidx, tk, tz, tx, 
	&nsw, &ogain, &ok, (bool* )o_iscent, o_centtbl ); 
	pin_stop_inject_errors (); 
	Topaz::topaz->pack_outputs (0 , i , ok , ogain , o_iscent , o_centtbl , swidx , swcostdelta , nsw ); 
}
bool pfl_TRANS_TPZ (Task* __INPUT , Task* __OUTPUT ){
	
	int i ; 
	int tx ; 
	int tk ; 
	char* t_iscent ; 
	int* t_centtbl ; 
	float tz ; 
	float* tflt_pts ; 
	int* tint_pts ; 
	int ok ; 
	float ogain ; 
	char* o_iscent ; 
	int* o_centtbl ; 
	int* swidx ; 
	float* swcostdelta ; 
	int nsw ; 
	float t_gain ; 
	Topaz::topaz->unpack_inputs (__INPUT , 0 , &i , &tx , &tk , &t_iscent , &t_centtbl , &tz , &tflt_pts , &tint_pts ); 
	Topaz::topaz->unpack_outputs (__OUTPUT , 0 , &i , &ok , &ogain , &o_iscent , &o_centtbl , &swidx , &swcostdelta , &nsw ); 
	
	t_gain = ogain; 
	return Topaz::topaz->check (0 , i , t_gain ); 
}
int topaz_init (int argv , char** argc ){
	Topaz::topaz->add (0 , pfl_COMPUTE_TPZ , pfl_TRANS_TPZ , 7 , 7 , 1 , 
		 TASK_ARG_SCALAR , INT , 1 , 
		 TASK_ARG_SCALAR , INT , 1 , 
		 TASK_ARG_ARRAY , CHAR , MAX_PTS , 
		 TASK_ARG_ARRAY , INT , MAX_PTS , 
		 TASK_ARG_CONST_SCALAR , FLOAT , 1 , 
		 TASK_ARG_CONST_ARRAY , FLOAT , SIZ_FLT , 
		 TASK_ARG_CONST_ARRAY , INT , SIZ_INT , 
		 TASK_ARG_SCALAR , INT , 1 , 
		 TASK_ARG_SCALAR , FLOAT , 1 , 
		 TASK_ARG_ARRAY , CHAR , MAX_PTS , 
		 TASK_ARG_ARRAY , INT , MAX_PTS , 
		 TASK_ARG_ARRAY , INT , MAX_PTS , 
		 TASK_ARG_ARRAY , FLOAT , MAX_PTS , 
		 TASK_ARG_SCALAR , INT , 1 ); 
	
}
int topaz_finish (int argv , char** argc ){
}
int topaz_worker (int argv , char** argc ){
}