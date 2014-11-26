
#ifndef DETECTORS_H
#define DETECTORS_H


#define ABS_DETECTOR_TEST 0
#define ABS_DETECTOR_TRAIN 1
#define ABS_DETECTOR_KEY 2
#define ABS_DETECTOR_LOG 3
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

#define MAX_REGIONS 5
#define WINDOW 1000.0
typedef struct REGION_T{
	float * min;
	float * max;
	float * center;
	//the error probability of training data
	float p_train_err;
	float p_train_corr;
	float p_train_total;
	float p_train_n;
	//the correct probability of training data.
	float p_test_err;
	float p_test_corr;
	float p_test_total;
	float p_test_n;
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
typedef struct GLBL_STATS_T{
	float n_acc;
	float n_rej;
	float n_true;
	float n_false;
	float n_total_train;
	float n_total_test;
} stats_t;

class AbsSolidRegionDetector : public AbsDetector {
		region_t * regions[MAX_REGIONS]; //regions from the distribution
		env_t * environment; //rest of distribution
		stats_t stats;
		const int max_regions = MAX_REGIONS;
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


class AbsRegionDetector : public AbsDetector {
		region_t * regions[MAX_REGIONS]; //regions from the distribution
		env_t * environment; //rest of distribution
		stats_t stats;
		const int max_regions = MAX_REGIONS;
		int n_regions;
		int dim;
		bool is_valid(float * d);
		void allocate_region(region_t * region, float * d);
		void deallocate_region(region_t * region);
		int find_region(float * d, float * score);
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
		AbsRegionDetector(int n);
		~AbsRegionDetector();
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
		void add(int taskid, int nouts){this->detectors[taskid] = new AbsSolidRegionDetector(nouts);}
		void setTarget(int id, float val){this->detectors[id]->setTarget(val);}
		//train particular output for task
		void set(int taskid, int i, float v){this->detectors[taskid]->set(i,v);}
		bool test(int taskid){return this->detectors[taskid]->test();}
		void train(int taskid){this->detectors[taskid]->train();}
		bool run(int id){return this->detectors[id]->run();}
};

#endif
