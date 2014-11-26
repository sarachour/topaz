
#ifndef DETECTORS_H
#define DETECTORS_H

class Detector;


class IDetectorData {
	protected:
		int n;
		double target;
		double * comp_targets;
	public:
		IDetectorData(int n);
		IDetectorData();
		~IDetectorData();
		virtual void init(int n) = 0;
		virtual void print(int i) = 0;
		virtual void set_target(double target) = 0;
		virtual void update(int idx, double reexec) = 0; // get a reexecution rate
		virtual void resolve() = 0;
		virtual bool train(int i) = 0;
		double get_target(int idx);  // get the current reexecution rate for an element.
		
};

/* Every target iteration - we compute the error between the target, observed
 * reexecution rate, we come up with a desired pertubation
 * W
 * 
 */
class ControlIDetectorData: public IDetectorData {
		float n; // number timepoints
		float * correlations; //how well correlated these are.
		int * scratch;
		double * dscratch;
		float * weights;
		float * neighbors;
		float weights_sum;
		float reexec_rate;
		//float comp_targets;
	public:
		ControlIDetectorData(int n);
		~ControlIDetectorData();
		void print(int i);
		void set_target(double target);
		void update(int idx, double reexec); // get a reexecution rate
		void resolve();
		bool train(int i);
		void init(int n){};
};

class UniformIDetectorData: public IDetectorData {
	int * scratch;
	float rejr;
	public:
		UniformIDetectorData(int n);
		~UniformIDetectorData();
		void init(int n){}
		void print(int i);
		void set_target(double target);
		bool train(int i){return true;}
		void update(int idx, double reexec); // get a reexecution rate
		void resolve();
};

//tracks correlation of outptus 
class IDetector{
	private:
		Detector * detectors; //each of these is an n-element output
		IDetectorData * idetector; //correlation, control information for every output
		int ** mappings;
		int n;
		int get_id(int outid, int siz);
	public: 
		IDetector(int siz, int nelem); // ouypuyd
		IDetector();
		~IDetector();
		void init(int siz, int nelem);
		void print();
		void set_target(double targ); //set the target for the task.
		void add(int outid, int siz); // set the ith input to size siz
		int size(int outid);
		double test(int outi, int idx, double val); //test value, update reexecution rate, update target.
		void train(int outi, int idx, double val, double oval); //train data
		void complete_test(); //move to next timestep, update correlation, change reexecution rates accordinly.
		void complete_train();
};


class DetectorManager {
	private:
		IDetector * classes;
		int nclasses;
	public:
	    DetectorManager(int siz);
	    DetectorManager();
	    ~DetectorManager();
	    void print();
		void init(int nclasses);
		void add(int taskid, int nouts, int total_nelems);
		void add_output(int taskid, int outid, int nelems);
		void set_target(int id, float val);
		//train particular output for task
		void train(int taskid, int outid, double * oval, double * nval);
		void train(int taskid, int outid, float * oval, float * nval);
		void train(int taskid, int outid, int * oval, int * nval);
		void train(int taskid, int outid, char * oval, char * nval);
		double test(int taskid, int outid, double * val);
		double test(int taskid, int outid, int * val);
		double test(int taskid, int outid, float * val);
		double test(int taskid, int outid, char * val);
		
		void complete_test(int i);
		void complete_train(int i);
};



#endif
