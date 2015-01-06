#ifndef SCAR_DETECTOR
#define SCAR_DETECTOR


#include "abs_detectors.h"


class AbsScarRegionDetector : public AbsDetector {
	protected:
		const float CENTER_WINDOW = 1000;
		typedef struct REGION_T{
			float * min;
			float * max;
			//weighted center of region
			float * center; 
			//the error probability of training data
			RegionStats stats;
		} region_t;
		
		region_t * regions[MAX_REGIONS]; //regions from the distribution
		ControlSystem * ctrl;
		RegionStats * env; //rest of distribution
		GlobalStats * stats;
		int max_regions;
		int n_regions;
		int dim;
		bool is_valid(float * d);
		void allocate_region(region_t * region, float * d);
		void deallocate_region(region_t * region);
		int find_region(float * d);
		void merge_regions(int r1, int r2);
		float score_region(region_t * r1, region_t * r2);
		int find_closest_region(int idx, float * score);
		//insert an output or an error
		void update_test_regions(int id, float * val, bool iserr);
		void update_train_regions(int id, float * val, bool iserr);
		void insert_point(float * d, bool iserr);
		bool test_point(float * d);
		
		void record(int cat);
		void adjust_region(region_t * region);
		
		virtual void adjust_control();
		virtual void set_control_target();
		
	public:
		AbsScarRegionDetector(int n);
		~AbsScarRegionDetector();
		bool test();
		bool train();
		void log();
		void print();
};

class AbsAutoScarRegionDetector : public AbsScarRegionDetector {
	private:
		virtual void adjust_control();
		virtual void set_control_target();
	public:
		AbsAutoScarRegionDetector(int n);
		~AbsAutoScarRegionDetector();
};



#endif
