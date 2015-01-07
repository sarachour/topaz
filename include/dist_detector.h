#ifndef DIST_DETECTOR
#define DIST_DETECTOR

#include "abs_detectors.h"
//n dimensional sphere

class Vector {
	private:
		int n;
		float * e;
	public:
		void set(int i, float f);
		float* get();
		void sub(Vector v);
		void add(Vector v);
		void dot(Vector v);
		void mag(Vector v);
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
		float p_pt;
		float p_fp;
		int n; //n pts
		static const float WT = 0.0001;
		static const float WT_I = 0.9999;
		float f; // number of standard deviations to consider.
	public:
		Distribution();
		~Distribution();
		void update(Vector pt); //update the mean, stddev appropriately
		float dist(Vector pt); //how many stddevs a point is from the mean.
};
class AbsDistDetector : public AbsDetector{
	private:
		Distribution * dists;
		int n;
	public:
		AbsDistDetector(int n);
		~AbsDistDetector();
		bool test();
		bool train();
		void log();
		void print();
};

#endif
