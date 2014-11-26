#ifndef DETECTOR_H
#define DETECTOR_H

class Checker{
	private:
		bool __internal;
	public:
		Checker();
		bool accept();
		bool reject();
		void assert(bool expr);
};

class DetectorData {
		static int ID;
	protected:
		int id;
	public:
		DetectorData();
		virtual void set_target(float f);
		virtual ~DetectorData();
		virtual void accept(double d, double d0);
		virtual double accepts(double d);
		virtual void print();
};

class MeanDetectorData : public DetectorData {
		typedef enum DETECTOR_CODE_FP{
			BAN_ALL= 0,
			NAN_OK = 0b01,
			INF_OK = 0b10,
			NEG_INF_OK = 0b100
		} detector_code_t;
		double mean;
		double stddev;
		double sqsum;
		int nstd;
		int code;
		double npts;
	public:
		MeanDetectorData();
		~MeanDetectorData();
		void accept(detector_code_t code);
		double accepts(detector_code_t code);
		void accept(double d, double d0);
		double accepts(double d);
		double getStdDev(){
			return stddev;
		}
		double getMean(){
			return mean;
		}
		int getN(){
			return npts;
		}
};

class ControlDetectorData: public DetectorData {
	MeanDetectorData * seed;
	double left;
	double right;
	double dat_lbound;
	double dat_rbound;
	double mean;
	int n;
	
	double rx_targ;
	
	int lt_n;
	double lt_freq;
	double lt_obs;
	double lt_e; //left error
	double lt_de; //derivative of error
	double lt_ie; //integral of error
	
	int rt_n;
	double rt_freq;
	double rt_obs;
	double rt_e; //left error
	double rt_de; //derivative of error
	double rt_ie; //integral of error
	bool reexec;
	void update_left(double reexec);
	void update_right(double reexec);
	void record(double d, int cat);
	int prob_accept(double d);
	public:
	void print();
	ControlDetectorData();
	~ControlDetectorData();
	void set_target(float f);
	void accept(double d, double dold);
	double accepts(double d);
	/*control system*/
	
};

class Detector{
	private:
	    int n;
	    DetectorData ** data;
	public:
		Detector(int siz);
		Detector();
		~Detector();
		void init(int siz);
		int get_n(){
			return n;
		}
		void print();
		void set_target(int idx, float target);
		void train(int idx, double val, double val_old);
		double test(int idx, double val);
};

#endif
