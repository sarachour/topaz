#ifndef STREAM_H
#define STREAM_H
#include "stdio.h"
#include <math.h>
#include <limits.h>
#include <stdlib.h>
#include <fstream>

#define MAXNAMESIZE 1024 // max filename length
#define SEED 1
/* increase this to reduce probability of random error */
/* increasing it also ups running time of "speedy" part of the code */
/* SP = 1 seems to be fine */
#define SP 1 // number of repetitions of speedy must be >=1

/* higher ITER --> more likely to get correct # of centers */
/* higher ITER also scales the running time almost linearly */
#define ITER 3 // iterate ITER* k log k times; ITER >= 1

#define CACHE_LINE 32 // cache line in byte
#define MAX_PTS 1000
#define SIZ_FLT (MAX_PTS*4)
#define SIZ_INT (MAX_PTS+2)

extern int * prim_pts_int;
extern float * prim_pts_flt;

extern int * center_table;
extern bool * is_center;
extern float * switch_cost_delta;
extern int * switch_idx;
extern int n_switches;


/* this structure represents a point */
/* these will be passed around to avoid copying coordinates */
typedef struct {
    float weight;
    float *coord;
    long assign;  /* number of point where this one is assigned */
    float cost;  /* cost of that assignment, weight*distance */
} Point;

/* this is the array of points */
typedef struct {
    long num; /* number of points; may not be N if this is a sample */
    int dim;  /* dimensionality */
    Point *p; /* the array itself */
} Points;



class PStream {
    public:
    virtual size_t read( float* dest, int dim, int num ) = 0;
    virtual int ferror() = 0;
    virtual int feof() = 0;
    virtual ~PStream() {
    }
};

class FileStream : public PStream {
    public:
    FileStream(char* filename) {
        fp = fopen( filename, "rb");
        if( fp == NULL ) {
            fprintf(stderr,"error opening file %s\n.",filename);
            exit(1);
        }
    }
    size_t read( float* dest, int dim, int num ) {
        // don't support
        int k=0;
        int q=0;
        for(q=0; q < num; q++){
            if(feof() || ferror())
            return q;
            
            for(int i=0; i < dim; i++){
                fscanf(fp,"%f\t", &dest[k]);
                k++;
            }
            fscanf(fp, "\n");
        }
        return q;
    }
    int ferror() {
        return std::ferror(fp);
    }
    int feof() {
        return std::feof(fp);
    }
    ~FileStream() {
        fprintf(stderr,"closing file stream\n");
        fclose(fp);
    }
    private:
    FILE* fp;
};

class SimStream : public PStream {
    public:
    SimStream(long n_ ) {
        n = n_;
    }
    size_t read( float* dest, int dim, int num ) {
        size_t count = 0;
        for( int i = 0; i < num && n > 0; i++ ) {
            for( int k = 0; k < dim; k++ ) {
                dest[i*dim + k] = lrand48()/(float)INT_MAX;
            }
            n--;
            count++;
        }
        return count;
    }
    int ferror() {
        return 0;
    }
    int feof() {
        return n <= 0;
    }
    ~SimStream() {
    }
    private:
    long n;
};


int size_flt_primitive_pt(int dim);
int size_int_primitive_pt();
float* to_flt_primitive(Point& p, int dim, float * f);
int * to_int_primitive(Point& p, int * l);
float* from_flt_primitive(Point& p, int dim, float * f);
int * from_int_primitive(Point& p, int * l);

/*
* POINT LIST SERIALIZATION FUNCTION
*
*/
int size_flt_primitive_pts(Points& p);
int size_int_primitive_pts(Points& p);
float* to_flt_primitive(Points& p, float * f);
int* to_int_primitive(Points& p, int * f);
float* from_primitives(Points& p, float * f, int * l, bool& okay);
void free_points(Points& p);
float dist(Point p1, Point p2, int dim);


float pFL(Points *points, int *feasible, int numfeasible, float z, long *k, double cost, long iter, float e);
float pFL_helper(Points zpts, bool * iscent, int * centtbl, float * swcost, int *swidx, long int k, int nz, int x,
	int * nsw, float * gain, int * ok, bool * o_iscent, int * o_centtbl);
float pFL_main(Points *points, int *feasible, int numfeasible, float z, long *k, double cost, long iter, float e);

int isIdentical(float *i, float *j, int D);
/* comparator for floating point numbers */
static int floatcomp(const void *i, const void *j);
/* shuffle points into random order */
void shuffle(Points *points);
/* shuffle an array of integers */
void intshuffle(int *intarray, int length);
/* compute Euclidean distance squared between two points */
float dist(Point p1, Point p2, int dim);


float pspeedy(Points *points, float z, long *kcenter);

/* compute approximate kmedian on the points */
float pkmedian_main(Points *points, long kmin, long kmax, long* kfinal );
/* compute approximate kmedian on the points */
float pkmedian(Points *points, long kmin, long kmax, long* kfinal );

/* compute the means for the k clusters */
int contcenters(Points *points);

/* copy centers from points to centers */
void copycenters(Points *points, Points* centers, long* centerIDs, long offset);


void localSearch( Points* points, long kmin, long kmax, long* kfinal );
void localSearchMain( Points* points, long kmin, long kmax, long* kfinal );

void outcenterIDs( Points* centers, long* centerIDs, char* outfile );
int selectfeasible_fast(Points *points, int **feasible, int kmin);
void streamCluster( PStream* stream,
long kmin, long kmax, int dim,
long chunksize, long centersize, char* outfile );
#endif
