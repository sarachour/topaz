#ifndef DET_STATS
#define DET_STATS

class DetectorLogInfo;

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
		float getNTestPoints(){return p_test_n;}
		void print();
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
		float get_reexec();
		float get_frac_rej_tp();
};

#endif
