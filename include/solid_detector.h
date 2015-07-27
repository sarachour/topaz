#ifndef SOLID_DETECTOR
#define SOLID_DETECTOR

#include "abs_detectors.h"

class AbsSolidRegionDetector : public AbsDetector {
			
		typedef struct REGION_T{
			float * min;
			float * max;
			float * center;
			RegionStats stats;
		} region_t;
		typedef struct NEG_REGION_T {
			float p_test_err;
			float p_test_corr;
			float p_test_total;
			float p_test_n;
			float p_train_err;
			float p_train_corr;
			float p_train_total;
			float p_train_n;
		} env_t;
		region_t * regions[MAX_REGIONS]; //regions from the distribution
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
		void update_test_regions(int id, bool iserr);
		void update_train_regions(int id, float * val, bool iserr);
		void insert_point(float * d, bool iserr);
		bool test_point(float * d);
		
		void record(int cat);
	public:
		AbsSolidRegionDetector(int n);
		~AbsSolidRegionDetector();
		bool test();
		bool train();
		void print();
};



#endif
