#ifndef DIST_DETECTOR
#define DIST_DETECTOR
#include "ctrl_system.h"
#include "abs_detectors.h"
//n dimensional sphere

class Vector {
	private:
		int n;
		float * e; // only dealloc if not passed in.
		float * ALLOC;
	public:
		Vector(float * e, int n);
		Vector(int n);
		Vector(const Vector& v);
		~Vector();
		void set(int i, float f);
		void set(float f);
		float* get();
		Vector* sub(const Vector v);
		Vector* add(const Vector v);
		const float dot(const Vector v);
		const float mag();
};

template <class T>
class Hypersphere {
	private:
		int n;
		T * elements;
	public:
		T get(Vector pt); // get something in the hypersphere
		
};

class Distribution {
	private:
		Hypersphere<Vector> sigma; //standard deviation over
		Vector mean;
		ControlSystem * ctrl_f; //factors to consider
		ControlSystem * ctrl_targ; //target rate to consider
		float p_pt;
		float p_fp;
		int n; //n pts
		static const float WT = 0.0001;
		static const float WT_I = 0.9999;
		float f; // number of standard deviations to consider.
		float targ_fp;
	public:
		Distribution();
		~Distribution();
		void update(const Vector pt); //update the mean, stddev appropriately
		float dist(const Vector pt); //how many stddevs a point is from the mean.
		void merge(const Distribution& d);
		//statistics
		void fp(){p_fp += WT;};
		void decay_fp(){p_fp *= WT_I;}
		void use(){p_pt += WT;}
		void decay_use(){p_pt *= WT_I;}
		
		void update_ctrl();
		
		float thresh(){return f;}
};
class AbsDistDetector : public AbsDetector{
	private:
		Distribution * dists;
		int n_dists;
		int max_dists;
		static const float EPS = 1E-23;
	public:
		AbsDistDetector(int n);
		~AbsDistDetector();
		bool test();
		bool train();
		void log();
		void print();
};

#endif
