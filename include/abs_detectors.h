
#ifndef DETECTORS_H
#define DETECTORS_H

#include "ctrl_system.h"
#include "stats.h"

#define ABS_DETECTOR_TEST 0
#define ABS_DETECTOR_TRAIN 1
#define ABS_DETECTOR_KEY 2
#define ABS_DETECTOR_LOG 3

#define MAX_REGIONS 5
#define WINDOW 1000.0
#define FRAC_WINDOW (1.0/WINDOW)
#define FRAC_REST_WINDOW (1.0 - FRAC_WINDOW)


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
		static void setMode(int m);
		static int getMode();
		int getDim(){return n;}
		float * getKeyVector(){return data_key;}
		float * getOutputVector(){return data;}
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
				return true;
			}
		}
		virtual bool test() = 0;
		virtual bool train() = 0;
		virtual void print(){}
		
	
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
		AbsDetector * getDetector(int id){return this->detectors[id];}
		void setTarget(int id, float val){this->detectors[id]->setTarget(val);}
		//train particular output for task
		void set(int taskid, int i, float v){this->detectors[taskid]->set(i,v);}
		bool test(int taskid){return this->detectors[taskid]->test();}
		void train(int taskid){this->detectors[taskid]->train();}
		bool run(int id){return this->detectors[id]->run();}
};

#endif
