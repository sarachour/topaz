#include "streamcluster.h"
#include <stdio.h>
#include <iostream>
#include <fstream>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <sys/resource.h>
#include <limits.h>
#include "pin_util.h"

#ifdef ENABLE_THREADS
#include <pthread.h>
#include "parsec_barrier.hpp"
#endif
#include "streamcluster.h"


#ifdef ENABLE_PARSEC_HOOKS
#include <hooks.h>
#endif

using namespace std;


int prim_pts_int[SIZ_INT];
float prim_pts_flt[SIZ_FLT];



int size_flt_primitive_pt(int dim){
    return (dim+2);
}
int size_int_primitive_pt(){
    return 1;
}
/* POINT SERIALIZATION FUNCTION
*
*/
float* to_flt_primitive(Point& p, int dim, float * f){
    *f = p.weight; f++;
    *f = p.cost; f++;
    for(int i=0; i < dim; i++){
        *f = p.coord[i]; f++;
    }
    return f;
}
int * to_int_primitive(Point& p, int * l){
    *l = p.assign; l++;
    return l;
}


/*
* POINT LIST SERIALIZATION FUNCTION
*
*/
int size_flt_primitive_pts(Points& p){
    return (size_flt_primitive_pt(p.dim)*p.num + 0);
}
int size_int_primitive_pts(Points& p){
    return (size_int_primitive_pt()*p.num+4);
}
float* to_flt_primitive(Points& p, float * f){
    for(int i=0; i < p.num; i++){
        f = to_flt_primitive(p.p[i], p.dim, f);
    }
    return f;
}

int checksum(int var){
	int res=0;
	for(int i=0; i < 32; i++){
		res+=(0b1<<i)>>i;
	}
	return res;
}
int* to_int_primitive(Points& p, int * f){
    *f = p.num; f++;
    *f = p.num; f++;
    *f = p.dim; f++;
    *f = p.dim; f++;
    for(int i=0; i < p.num; i++){
        f = to_int_primitive(p.p[i], f);
    }
    return f;
}
int from_flt_primitive(Point& p, int dim, float * f){
    p.weight = f[0]; 
    p.cost = f[1];
	chkbnd(dim,2);
    p.coord = new float[dim]; //allocate point
    for(int i=0; i < dim; i++){
        p.coord[i] = f[i+2];
    }
    return dim+2;
}

int from_int_primitive(Point& p, int * l){
    p.assign = l[0];
    return 1;
}
float* from_primitives(Points& p, float * f, int * l, bool& okay){
    int lver,dver;
    okay = true;
    p.num = l[0];
    lver = l[1];
    
    p.dim = l[2];
    dver = l[3];
    //printf("q\n");
    if((p.num) != lver){
		okay = false; return f;
	}
    if((p.dim) != dver){
		okay = false; return f;
	}
	//printf("r\n");
	chkbnd(p.num,MAX_PTS);
    p.p = new Point[p.num];
    //printf("mm\n");
	int fmax = size_flt_primitive_pts(p);
	int lmax = size_int_primitive_pts(p);
    int li=4;
    int fi=0;
    //printf("m\n");
    for(int i=0; i < p.num; i++){
		chkbnd(i,p.num);
		chkbnd(fi,fmax);
        fi += from_flt_primitive(p.p[i], p.dim, &f[fi]);
		chkbnd(li,lmax);
        li += from_int_primitive(p.p[i], &l[li]);
    }
    //printf("q\n");
    return &f[fi];
}

void free_points(Points& p){
    for(int i=0; i < p.num; i++){
        free(p.p[i].coord);
    }
    free(p.p);
}

/*
* DONE...
*
*/

float dist(Point p1, Point p2, int dim);




int isIdentical(float *i, float *j, int D)
// tells whether two points of D dimensions are identical
{
    int a = 0;
    int equal = 1;
    
    while (equal && a < D) {
        if (i[a] != j[a]) equal = 0;
        else a++;
    }
    if (equal) return 1;
    else return 0;
    
}

/* comparator for floating point numbers */
static int floatcomp(const void *i, const void *j)
{
    float a, b;
    a = *(float *)(i);
    b = *(float *)(j);
    if (a > b) return (1);
    if (a < b) return (-1);
    return(0);
}

/* shuffle points into random order */
void shuffle(Points *points)
{
    long i, j;
    Point temp;
    for (i=0;i<points->num-1;i++) {
        j=(lrand48()%(points->num - i)) + i;
        temp = points->p[i];
        points->p[i] = points->p[j];
        points->p[j] = temp;
    }
}

/* shuffle an array of integers */
void intshuffle(int *intarray, int length)
{
    long i, j;
    int temp;
    for (i=0;i<length;i++) {
        j=(lrand48()%(length - i))+i;
        temp = intarray[i];
        intarray[i]=intarray[j];
        intarray[j]=temp;
    }
}

/* compute Euclidean distance squared between two points */
float dist(Point p1, Point p2, int dim)
{
    int i;
    float result=0.0;
    for (i=0;i<dim;i++)
    result += (p1.coord[i] - p2.coord[i])*(p1.coord[i] - p2.coord[i]);
    return(result);
}



float pspeedy(Points *points, float z, long *kcenter)
{
    #ifdef ENABLE_THREADS
    pthread_barrier_wait(barrier);
    #endif
    //my blockpoints->num
    long size = points->num;
    static double totalcost;
    
    static bool open = false;
    static double cost; //cost for each thread.
    static int i;
    
    #ifdef ENABLE_THREADS
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    static pthread_cond_t cond = PTHREAD_COND_INITIALIZER;
    #endif
    
    /* create center at first point, send it to itself */
    for( int k = 0; k < points->num; k++ )    {
        float distance = dist(points->p[k],points->p[0],points->dim);
        points->p[k].cost = distance * points->p[k].weight;
        points->p[k].assign=0;
    }
    
    *kcenter = 1;
    
    
    for(i = 1; i < points->num; i++ )  {
        bool to_open = ((float)lrand48()/(float)INT_MAX)<(points->p[i].cost/z);
        if( to_open )  {
            (*kcenter)++;
            open = true;
            for( int k = 0; k < points->num; k++ )  {
                float distance = dist(points->p[i],points->p[k],points->dim);
                if( distance*points->p[k].weight < points->p[k].cost )  {
                    points->p[k].cost = distance * points->p[k].weight;
                    points->p[k].assign=i;
                }
            }
            open = false;
        }
    }
    open = true;
    
    open = false;
    double mytotal = 0;
    for( int k = 0; k < points->num; k++ )  {
        mytotal += points->p[k].cost;
    }
    cost = mytotal;
    
    
    totalcost=z*(*kcenter);
    totalcost+=cost;
    
    return(cost);
}


/* compute approximate kmedian on the points */
float pkmedian_main(Points *points, long kmin, long kmax, long* kfinal )
{
    int i;
    double cost;
    double lastcost;
    double hiz, loz, z;
    int npts = points->num;
    bool is_center[npts];
    
    long k;
    int *feasible;
    int numfeasible;
    double hizs;
    
    hiz = loz = 0.0;
    long numberOfPoints = points->num;
    long ptDimension = points->dim;
    
    //my block
    long size = points->num;
    
    for(int j = 0; j < npts; j++) is_center[j] = false;
    
    double myhiz = 0;
    for (long kk=0;kk < size; kk++ ) {
        myhiz += dist(points->p[kk], points->p[0],
        ptDimension)*points->p[kk].weight;
    }
    hizs = myhiz;
    hiz += hizs;
    
    loz=0.0; z = (hiz+loz)/2.0;
    /* NEW: Check whether more centers than points! */
    if (points->num <= kmax) {
        /* just return all points as facilities */
        for (long kk=0;kk<size;kk++) {
            points->p[kk].assign = kk;
            points->p[kk].cost = 0;
        }
        cost = 0;
        *kfinal = k;
        return cost;
    }
    
    shuffle(points);
    cost = pspeedy(points, z, &k);
    
    i=0;
    /* give speedy SP chances to get at least kmin/2 facilities */
    while ((k < kmin)&&(i<SP)) {
        cost = pspeedy(points, z, &k);
        i++;
    }
    
    /* if still not enough facilities, assume z is too high */
    while (k < kmin) {
        if (i >= SP) {hiz=z; z=(hiz+loz)/2.0; i=0;}
        shuffle(points);
        cost = pspeedy(points, z, &k);
        i++;
    }
    printf("> mins finished. selecting feasible\n");
    
    /* now we begin the binary search for real */
    /* must designate some points as feasible centers */
    /* this creates more consistancy between FL runs */
    /* helps to guarantee correct # of centers at the end */
    
    numfeasible = selectfeasible_fast(points,&feasible,kmin);
    for( int i = 0; i< points->num; i++ ) {
        is_center[points->p[i].assign]= true;
    }
    printf("> starting pfl (k=%d) > [%d,%d]\n", k, kmin, kmax);
    
    while(1) {
        /* first get a rough estimate on the FL solution */
        lastcost = cost;
        cost = pFL(points, is_center, feasible, numfeasible, z, &k, cost, (long)(ITER*kmax*log((double)kmax)), 0.1);
        /* if number of centers seems good, try a more accurate FL */
        if (((k <= (1.1)*kmax)&&(k >= (0.9)*kmin))||((k <= kmax+2)&&(k >= kmin-2))) {
            
            /* may need to run a little longer here before halting without
            improvement */
            cost = pFL(points, is_center, feasible, numfeasible, z, &k, cost, (long)(ITER*kmax*log((double)kmax)), 0.001);
        }
        
        if (k > kmax) {
            /* facilities too cheap */
            /* increase facility cost and up the cost accordingly */
            loz = z; z = (hiz+loz)/2.0;
            cost += (z-loz)*k;
        }
        if (k < kmin) {
            /* facilities too expensive */
            /* decrease facility cost and reduce the cost accordingly */
            hiz = z; z = (hiz+loz)/2.0;
            cost += (z-hiz)*k;
        }
        
        /* if k is good, return the result */
        /* if we're stuck, just give up and return what we have */
        if (((k <= kmax)&&(k >= kmin))||((loz >= (0.999)*hiz)) )
        {
            break;
        }
        
    }
    printf("	> finished running feasible\n");
    //clean up...
    free(feasible);
    *kfinal = k;
    
    return cost;
}


/* compute approximate kmedian on the points */
float pkmedian(Points *points, long kmin, long kmax, long* kfinal )
{
    int i;
    double cost;
    double lastcost;
    double hiz, loz, z;
    int npts = points->num;
    bool is_center[npts];
    
    long k;
    int *feasible;
    int numfeasible;
    double hizs;
    
    hiz = loz = 0.0;
    long numberOfPoints = points->num;
    long ptDimension = points->dim;
    
    for(int j = 0; j < npts; j++) is_center[j] = false;
    
    //my block
    long size = points->num;
    
    
    double myhiz = 0;
    for (long kk=0;kk < size; kk++ ) {
        myhiz += dist(points->p[kk], points->p[0],
        ptDimension)*points->p[kk].weight;
    }
    hizs = myhiz;
    hiz += hizs;
    
    loz=0.0; z = (hiz+loz)/2.0;
    /* NEW: Check whether more centers than points! */
    if (points->num <= kmax) {
        /* just return all points as facilities */
        for (long kk=0;kk<size;kk++) {
            points->p[kk].assign = kk;
            points->p[kk].cost = 0;
        }
        cost = 0;
        *kfinal = k;
        return cost;
    }
    
    shuffle(points);
    cost = pspeedy(points, z, &k);
    
    i=0;
    /* give speedy SP chances to get at least kmin/2 facilities */
    while ((k < kmin)&&(i<SP)) {
        cost = pspeedy(points, z, &k);
        i++;
    }
    
    /* if still not enough facilities, assume z is too high */
    while (k < kmin) {
        if (i >= SP) {hiz=z; z=(hiz+loz)/2.0; i=0;}
        shuffle(points);
        cost = pspeedy(points, z, &k);
        i++;
    }
    
    /* now we begin the binary search for real */
    /* must designate some points as feasible centers */
    /* this creates more consistancy between FL runs */
    /* helps to guarantee correct # of centers at the end */
    
    numfeasible = selectfeasible_fast(points,&feasible,kmin);
    for( int i = 0; i< points->num; i++ ) {
        is_center[points->p[i].assign]= true;
    }
    
    
    printf("start: pfl %d : [%d,%d]\n",k,kmin,kmax);
    while(1) {
        /* first get a rough estimate on the FL solution */
        printf("pfl %d ? [%d,%d] / cost: %f\n",k,kmin,kmax,cost);
        lastcost = cost;
        cost = pFL(points, is_center, feasible, numfeasible, z, &k, cost, (long)(ITER*kmax*log((double)kmax)), 0.1);
        /* if number of centers seems good, try a more accurate FL */
        if (((k <= (1.1)*kmax)&&(k >= (0.9)*kmin))|| ((k <= kmax+2)&&(k >= kmin-2))) {
            
            /* may need to run a little longer here before halting without
            improvement */
            cost = pFL(points, is_center, feasible, numfeasible, z, &k, cost, (long)(ITER*kmax*log((double)kmax)), 0.001);
        }
        printf("pfl %d -> [%d,%d]\n",k,kmin,kmax);
        
        if (k > kmax) {
            /* facilities too cheap */
            /* increase facility cost and up the cost accordingly */
            loz = z; z = (hiz+loz)/2.0;
            cost += (z-loz)*k;
        }
        if (k < kmin) {
            /* facilities too expensive */
            /* decrease facility cost and reduce the cost accordingly */
            hiz = z; z = (hiz+loz)/2.0;
            cost += (z-hiz)*k;
        }
        
        /* if k is good, return the result */
        /* if we're stuck, just give up and return what we have */
        //printf("%d max=%d min=%d, (%f,%f)\n", kmax, kmin, hiz,loz);
        if (((k <= kmax)&&(k >= kmin))||((loz >= (0.999)*hiz)) )
        {
			if(k <= kmax && k >= kmin) printf("+ within bounds\n");
			else printf("+ hit tolerance ceiling\n");
            break;
        }
        
    }
    printf("done\n");
    //clean up...
    free(feasible);
    *kfinal = k;
    
    return cost;
}



/* compute the means for the k clusters */
int contcenters(Points *points)
{
    long i, ii;
    float relweight;
    
    for (i=0;i<points->num;i++) {
        /* compute relative weight of this point to the cluster */
        if (points->p[i].assign != i) {
            relweight=points->p[points->p[i].assign].weight + points->p[i].weight;
            relweight = points->p[i].weight/relweight;
            for (ii=0;ii<points->dim;ii++) {
                points->p[points->p[i].assign].coord[ii]*=1.0-relweight;
                points->p[points->p[i].assign].coord[ii]+=
                points->p[i].coord[ii]*relweight;
            }
            points->p[points->p[i].assign].weight += points->p[i].weight;
        }
    }
    
    return 0;
}





int selectfeasible_fast(Points *points, int **feasible, int kmin)
{
    int numfeasible = points->num;
    if (numfeasible > (ITER*kmin*log((double)kmin)))
    numfeasible = (int)(ITER*kmin*log((double)kmin));
    *feasible = (int *)malloc(numfeasible*sizeof(int));
    
    float* accumweight;
    float totalweight;
    
    /*
    Calcuate my block.
    For now this routine does not seem to be the bottleneck, so it is not parallelized.
    When necessary, this can be parallelized by setting 0 and numfeasible to
    proper values and calling this routine from all threads ( it is called only
    by thread 0 for now ).
    Note that when parallelized, the randomization might not be the same and it might
    not be difficult to measure the parallel speed-up for the whole program.
    */
    //  long bsize = numfeasible;
    
    float w;
    int l,r,k;
    
    /* not many points, all will be feasible */
    if (numfeasible == points->num) {
        for (int i=0;i<numfeasible;i++)
        (*feasible)[i] = i;
        return numfeasible;
    }
    
    accumweight= (float*)malloc(sizeof(float)*points->num);
    
    accumweight[0] = points->p[0].weight;
    totalweight=0;
    for( int i = 1; i < points->num; i++ ) {
        accumweight[i] = accumweight[i-1] + points->p[i].weight;
    }
    totalweight=accumweight[points->num-1];
    
    for(int i=0; i<numfeasible; i++ ) {
        w = (lrand48()/(float)INT_MAX)*totalweight;
        //binary search
        l=0;
        r=points->num-1;
        if( accumweight[0] > w )  {
            (*feasible)[i]=0;
            continue;
        }
        while( l+1 < r ) {
            k = (l+r)/2;
            if( accumweight[k] > w ) {
                r = k;
            }
            else {
                l=k;
            }
        }
        (*feasible)[i]=r;
    }
    
    
    free(accumweight);
    
    return numfeasible;
}


/* copy centers from points to centers */
void copycenters(Points *points, Points* centers, long* centerIDs, long offset)
{
    long i;
    long k;
    
    bool *is_a_median = (bool *) calloc(points->num, sizeof(bool));
    
    /* mark the centers */
    for ( i = 0; i < points->num; i++ ) {
        is_a_median[points->p[i].assign] = 1;
    }
    
    k=centers->num;
    
    /* count how many  */
    for ( i = 0; i < points->num; i++ ) {
        if ( is_a_median[i] ) {
            memcpy( centers->p[k].coord, points->p[i].coord, points->dim * sizeof(float));
            centers->p[k].weight = points->p[i].weight;
            centerIDs[k] = i + offset;
            k++;
        }
    }
    
    centers->num = k;
    
    free(is_a_median);
}



void localSearchTopaz( Points* points, long kmin, long kmax, long* kfinal ) {
    pkmedian(points,kmin,kmax,kfinal);
    
    
}

double pgain_main(long x, Points *points,bool * is_center, double z, long int *numcenters, int * switch_idx, float * switch_cost_delta)
{
    //my block
    long size = points->num;
    int dim = points->dim;
    int center_table[size];
    int i;
    int number_of_centers_to_close = 0;
    double gl_cost_of_opening_x;
    int gl_number_of_centers_to_close;
    
    
    //each thread takes a block of working_mem.
    int stride = *numcenters+2;
    //make stride a multiple of CACHE_LINE
    int cl = CACHE_LINE/sizeof(double);
    if( stride % cl != 0 ) {
        stride = cl * ( stride / cl + 1);
    }
    int K = stride -2 ; // K==*numcenters
    
    //my own cost of opening x
    double cost_of_opening_x = 0;
    
    double work_mem[stride*2];
    int work_mem_size = stride*2;
    gl_cost_of_opening_x = 0;
    gl_number_of_centers_to_close = 0;
    
    
    /*For each center, we have a *lower* field that indicates
    how much we will save by closing the center.
    Each thread has its own copy of the *lower* fields as an array.
    We first build a table to index the positions of the *lower* fields.
    */
    //printf("allocating table\n");
    memset(center_table, 0, size*sizeof(int));
    
    int count = 0;
    for( int i = 0; i < size; i++ ) {
        if( is_center[i] ) {
			//update the center table
            center_table[i] = count++;
        }
    }
    //printf("number of centers: %d\n",count);
    
    
    //now we finish building the table. clear the working memory.
    bool switch_membership[MAX_PTS];
    memset(work_mem, 0, stride*sizeof(double));
    memset(work_mem+stride,0,stride*sizeof(double));
    
    //my *lower* fields
    double* lower = &work_mem[0];
    int lower_size=stride*2;
    double* gl_lower = &work_mem[stride];
    int gl_lower_size = stride;
    
    chkbnd(x,size);
    for ( i = 0; i < size; i++ ) {
        float x_cost = dist(points->p[i], points->p[x], points->dim) * points->p[i].weight;
        float current_cost = points->p[i].cost;
        
        if ( x_cost < current_cost ) {
            
            // point i would save cost just by switching to x
            // (note that i cannot be a median,
            // or else dist(p[i], p[x]) would be 0)
            
            switch_membership[i] = 1;
            cost_of_opening_x += x_cost - current_cost;
            
            } else {
            
            // cost of assigning i to x is at least current assignment cost of i
            
            // consider the savings that i's **current** median would realize
            // if we reassigned that median and all its members to x;
            // note we've already accounted for the fact that the median
            // would save z by closing; now we have to subtract from the savings
            // the extra cost of reassigning that median and its members
            int assign = points->p[i].assign;
            int idx = center_table[assign];
            chkbnd(idx,lower_size);
            lower[idx] += current_cost - x_cost;
        }
        
    }
    
    // at this time, we can calculate the cost of opening a center
    // at x; if it is negative, we'll go through with opening it
    
    for ( int i = 0; i < size; i++ ) {
        if( is_center[i] ) {
            double low = z;
            int idx;
            //aggregate from all threads
            low += work_mem[center_table[i]];
		
			idx = center_table[i];
			
            chkbnd(idx,gl_lower_size);
            gl_lower[idx] = low;
            if ( low > 0 ) {
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
    gl_number_of_centers_to_close += (int)work_mem[K];
    gl_cost_of_opening_x += work_mem[K+1];
    // Now, check whether opening x would save cost; if so, do it, and
    // otherwise do nothing
    int MAX_SWITCHES = MAX_PTS;
    if(switch_idx != NULL) 
		memset(switch_idx, 0, sizeof(int)*size);
    if(switch_cost_delta != NULL) 
		memset(switch_cost_delta, 0, sizeof(float)*size);
    int n_switches=0;
    if ( gl_cost_of_opening_x < 0 ) {
        //  we'd save money by opening x; we'll do it
        for ( int i = 0; i < size; i++ ) {
			int idx;
			int iidx;
			
			idx=points->p[i].assign;
			iidx=center_table[idx];
            chkbnd(iidx, gl_lower_size);
            
            bool close_center = gl_lower[iidx] > 0 ;
            if ( switch_membership[i] || close_center ) {
                // Either i's median (which may be i itself) is closing,
                // or i is closer to x than to its current median
                float ocost = points->p[i].cost;
                points->p[i].cost = points->p[i].weight * dist(points->p[i], points->p[x], points->dim);
                points->p[i].assign = x;
                if(switch_idx != NULL && n_switches < MAX_SWITCHES){
					switch_idx[n_switches] = i;
					switch_cost_delta[n_switches] = points->p[i].cost-ocost;
					n_switches++;
				}
            }
        }
        
        for( int i = 0; i < size; i++ ) {
			//remove center
            if( is_center[i] && gl_lower[center_table[i]] > 0 ) {
                is_center[i] = false;
            }
        }
        
        if( x >= 0 && x < size ) {
            is_center[x] = true;
        }
        //printf("closing %d centers\n", gl_number_of_centers_to_close);
        *numcenters = *numcenters + 1 - gl_number_of_centers_to_close;
    }
    else {
        gl_cost_of_opening_x = 0;  // the value we'll return
    }
    //printf("returning\n");
    
    //delete switch_membership;
    return -gl_cost_of_opening_x;
}

/* facility location on the points using local search */
/* z is the facility cost, returns the total cost and # of centers */
/* assumes we are seeded with a reasonable solution */
/* cost should represent this solution's cost */
/* halt if there is < e improvement after iter calls to gain */
/* feasible is an array of numfeasible points which may be centers */


float pFL_main(Points *points, bool * is_center, int *feasible, int numfeasible, float z, long *k, double cost, long iter, float e){
	double change;
    long numberOfPoints;
    //sizeof(k) = 1 long
    numberOfPoints = points->num;
    change = cost;
    //allocate
    //is_center = new bool[numberOfPoints];
	//center_table = new int[numberOfPoints];
	float switch_cost_delta[numberOfPoints];
	int switch_idx[numberOfPoints];
	//printf("allocated\n");
	 while (change/cost > 1.0*e) {
		 //printf("looping\n");
        change = 0.0;
        numberOfPoints = points->num;
        //MAKE SURE TO ENABLE RANDOMIZE
        intshuffle(feasible, numfeasible);
        int x;
        //printf("		- starting iters\n");
        for (int i=0;i<iter;i++) {
            x = i%numfeasible;
            //printf("gain %d %d\n", z, *k);
            float gain = pgain_main(feasible[x], points,is_center, z, k, switch_idx, switch_cost_delta);
            //printf("+ gain done\n");
            change += gain;
        }
        cost -= change;
        
    }
    //delete is_center;
    //delete center_table;
    //printf("done.");
    return(cost);
}

void localSearchMain( Points* points, long kmin, long kmax, long* kfinal ) {
    pkmedian_main(points,kmin,kmax,kfinal);
    //pkmedian(points,kmin,kmax,kfinal);
    
}

void outcenterIDs( Points* centers, long* centerIDs, char* outfile ) {
    FILE* fp = fopen(outfile, "w");
    if( fp==NULL ) {
        fprintf(stderr, "error opening %s\n",outfile);
        exit(1);
    }
    int* is_a_median = (int*)calloc( sizeof(int), centers->num );
    for( int i =0 ; i< centers->num; i++ ) {
        is_a_median[centers->p[i].assign] = 1;
    }
    
    for( int i = 0; i < centers->num; i++ ) {
        if( is_a_median[i] ) {
            fprintf(fp, "%u\n", centerIDs[i]);
            fprintf(fp, "%lf\n", centers->p[i].weight);
            for( int k = 0; k < centers->dim; k++ ) {
                fprintf(fp, "%lf ", centers->p[i].coord[k]);
            }
            fprintf(fp,"\n\n");
        }
    }
    fclose(fp);
}

void localSearch( Points* points, long kmin, long kmax, long* kfinal ) {
	localSearchTopaz(points,kmin, kmax,kfinal);
}

void streamCluster( PStream* stream,
long kmin, long kmax, int dim,
long chunksize, long centersize, char* outfile )
{
    float* block = (float*)malloc( chunksize*dim*sizeof(float) );
    float* centerBlock = (float*)malloc(centersize*dim*sizeof(float) );
    long* centerIDs = (long*)malloc(centersize*dim*sizeof(long));
    if( block == NULL ) {
        fprintf(stderr,"not enough memory for a chunk!\n");
        exit(1);
    }
    
    Points points;
    points.dim = dim;
    points.num = chunksize;
    points.p =
    (Point *)malloc(chunksize*sizeof(Point));
    
    
    for( int i = 0; i < chunksize; i++ ) {
        points.p[i].coord = &block[i*dim];
    }
    
    Points centers;
    centers.dim = dim;
    centers.p =
    (Point *)malloc(centersize*sizeof(Point));
    
    centers.num = 0;
    
    for( int i = 0; i< centersize; i++ ) {
        centers.p[i].coord = &centerBlock[i*dim];
        centers.p[i].weight = 1.0;
    }
    
    long IDoffset = 0;
    long kfinal;
    while(1) {
        size_t numRead  = stream->read(block, dim, chunksize );
        fprintf(stderr,"read %d points\n",numRead);
        
        if( stream->ferror() || numRead < (unsigned int)chunksize && !stream->feof() ) {
            fprintf(stderr, "error reading data!\n");
            exit(1);
        }
        points.num = numRead;
        for( int i = 0; i < points.num; i++ ) {
            points.p[i].weight = 1.0;
        }
        
        
        //fprintf(stderr,"center_table = 0x%08x\n",(int)center_table);
        //fprintf(stderr,"is_center = 0x%08x\n",(int)is_center);
        
        printf("main:localsearch\n");
        localSearch(&points,kmin, kmax,&kfinal); // parallel
        //fprintf(stderr,"finish local search\n");
        
        contcenters(&points); /* sequential */
        if( kfinal + centers.num > centersize ) {
            //here we don't handle the situation where # of centers gets too large.
            printf("%d > max centers %d\n", kfinal + centers.num, centersize);
            exit(1);
            //break;
        }
        
        printf("main:copy centers\n");
        copycenters(&points, &centers, centerIDs, IDoffset); /* sequential */
        IDoffset += numRead;
        printf(">	%d centers:\n", centers.num);
		//TODO: Removed free lines.
        //free(is_center);
        //free(center_table);
        
        if( stream->feof() ) {
            break;
        }
    }
    printf("# LOCAL SEARCHES DONE\n");
    //Global Streamcluster Alignment
    printf("allocated.. conducting local search\n");
    localSearch( &centers, kmin, kmax ,&kfinal ); // parallel
    printf("final center count: %d\n", kfinal);
    contcenters(&centers);
    printf("cont center complete.. outcenter\n");
    outcenterIDs( &centers, centerIDs, outfile);
    printf("done..\n");
}

