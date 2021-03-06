#ifndef DIST_DETECTOR
#define DIST_DETECTOR
#include "ctrl_system.h"
#include "abs_detectors.h"
//n dimensional sphere
typedef struct VECTOR_T {
	float * d;
	int n;
} vector_t;

class Vector{
	public:
		static vector_t create(int n);
		static vector_t create(float * d, int n);
		static vector_t create(vector_t d, int n);
		static vector_t create(float d, int n);
		static vector_t wrap(float * d, int n);
		static void dealloc(vector_t a);
		static void set(vector_t a, vector_t r);
		static void foreach(float (*fun)(float,float), vector_t a, vector_t b, vector_t r);
		static void foreach(float (*fun)(float,float,float*), vector_t a, vector_t b, vector_t r, float * args);
		static float reduce(float (*fun)(float,float,float), vector_t a, vector_t b, float r);
		static void print(vector_t d);
		
};



typedef struct DS_VECTOR_T {
	float * pos;
	float * neg;
	int n;
} ds_vector_t;

class DSVector {
	public:
		static ds_vector_t create(int n);
		static ds_vector_t create(vector_t a);
		static ds_vector_t create(vector_t a, vector_t b);
		static ds_vector_t create(ds_vector_t v);
		static ds_vector_t create(float v, int n);
		static void dealloc(ds_vector_t v);
		static void foreach(float (*fun)(float,float), ds_vector_t a, vector_t b, ds_vector_t e);
		static void foreach(float (*fun)(float,float,float*), ds_vector_t a, vector_t b, ds_vector_t e, float * args);
		static void foreach(float (*fun)(float,float), ds_vector_t a, ds_vector_t b, ds_vector_t e);
		static void foreach(float (*fun)(float,float, float *), ds_vector_t a, ds_vector_t b, ds_vector_t e, float * args);
		static void foreach(float (*fun)(float), ds_vector_t a, ds_vector_t e);
		static float reduce(float (*fun)(float,float,float), ds_vector_t a, vector_t b,float v);
		static float reduce(float (*fun)(float,float,float), ds_vector_t a, ds_vector_t b, float v);
		static float reduce(float (*fun)(float,float), ds_vector_t a, float v);
		static void print(ds_vector_t a);
};

class Distribution {
	protected:
		ds_vector_t sigma; //standard deviation over
		ds_vector_t sqsum;
		vector_t mean;
		ControlSystem * ctrl_f; //factors to consider]
		float p_pt;
		float p_fp;
		int n; //n pts
		int d;
		
		static const float WT = 0.0001;
		static const float WINDOW_PTS = 1000;
		float f; // number of standard deviations to consider.
		float targ_fp;
	public:
		
		Distribution();
		~Distribution();
		vector_t get_mean(){return mean;}
		ds_vector_t get_sigma(){return sigma;}
		int get_npts(){return n;}
		float get_prob_dist(){return p_pt;}
		float get_prob_false_positive(){return p_fp;}
		float get_nstd_to_acc(){return f;}
		float get_target_fp_rate(){return targ_fp;}
		
		//standard distribution functions
		void init(int n);
		void update(vector_t pt); //update the mean, stddev appropriately
		float dist(vector_t pt); //how many stddevs a point is from the mean.
		void merge(const Distribution& d);
		//statistics
		void fp(){p_fp += WT;};
		void decay_fp(){p_fp *= 1.0-WT;}
		void use(){p_pt += WT;}
		void decay_use(){p_pt *= 1.0-WT;}
		
		void set_targ(float f){targ_fp = f;}
		void update_ctrl();
		
		float thresh(){return f;}
		void print();
};

class AbsDistDetector : public AbsDetector{
	private:
		Distribution * dists;
		int n_dists;
		int max_dists;
		static const float EPS = 1E-23;
		float targ_fp;
	public:
		AbsDistDetector(int n);
		~AbsDistDetector();
		bool test();
		bool train();
		void log();
		void print();
};

#endif
