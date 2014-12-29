
#ifndef DETECTORS_H
#define DETECTORS_H

#include "ctrl_system.h"

#define ABS_DETECTOR_TEST 0
#define ABS_DETECTOR_TRAIN 1
#define ABS_DETECTOR_KEY 2
#define ABS_DETECTOR_LOG 3

class DetectorLogInfo;
class AbsDetector{
	private:
		float thresh;
	protected:
		float target;
		float * data_key; // key
		float * data; // result
		int n;
	public:
		static int MODE;
		static bool LOG_ACCEPTED;
		AbsDetector(int nelems){
			this->data_key = new float[nelems];
			this->data = new float[nelems];
			this->n = nelems;
			this->thresh = 1e-10;
		}
		AbsDetector(){
			
		}
		~AbsDetector(){
			this->clean();
		}
		void clean(){
			delete data_key;
			delete data;
		}
		static void setLog_Accepted(bool is_accept){
			AbsDetector::LOG_ACCEPTED = is_accept;
		}
		static void setMode(int m);
		static int getMode();
		void setTarget(float t){
			this->target = t;
		}
		void set(int i, float val){
			if(AbsDetector::getMode() != ABS_DETECTOR_KEY)
				this->data[i] = val;
			else
				this->data_key[i] = val;
		}
		bool compare();
		bool run(){
			if(AbsDetector::getMode() == ABS_DETECTOR_TEST){
				return this->test();
			}
			else if(AbsDetector::getMode() == ABS_DETECTOR_TRAIN){
				return this->train();
			}
			else{
				this->log();
				return true;
			}
		}
		virtual void log() = 0;
		virtual bool test() = 0;
		virtual bool train() = 0;
		virtual void print(){}
		
	
};

class RegionStats {
	float p_train_err;
	float p_train_corr;
	float p_train_total;
	float p_train_n;
	float p_test_err;
	float p_test_corr;
	float p_test_total;
	float p_test_n;
	public:
		RegionStats();
		void update_accept_rate(bool iselem, bool is_acc);
		void update_accuracy_rate(bool iselem, bool is_true);
		void merge(RegionStats * o);
		void print();
		int log(DetectorLogInfo * d, int r, int i);
};

class GlobalStats {
	float n_acc;
	float n_rej;
	float n_true;
	float n_false;
	float n_total_train;
	float n_total_test;
	public:
		GlobalStats();
		void update_accept_rate(bool is_corr_label);
		void update_accuracy_rate(bool isacc);
		void print();
		int log(DetectorLogInfo * d, int i);
		float get_reexec();
};
#define MAX_REGIONS 5
#define WINDOW 1000.0
#define FRAC_WINDOW (1.0/WINDOW)
#define FRAC_REST_WINDOW (1.0 - FRAC_WINDOW)

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
		void log();
		void print();
};



class AbsDetectorManager {
	private:
		AbsDetector ** detectors;
		int n;
	public:
	    AbsDetectorManager(int siz){this->init(siz);}
	    AbsDetectorManager();
	    ~AbsDetectorManager();
	    void print();
	    void init(int siz);
		void add(int taskid, int nouts);
		void setTarget(int id, float val){this->detectors[id]->setTarget(val);}
		//train particular output for task
		void set(int taskid, int i, float v){this->detectors[taskid]->set(i,v);}
		bool test(int taskid){return this->detectors[taskid]->test();}
		void train(int taskid){this->detectors[taskid]->train();}
		bool run(int id){return this->detectors[id]->run();}
};

#endif
