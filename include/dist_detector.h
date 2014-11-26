#ifndef PDETECT_H
#define PDETECT_H
#include "detector.h"

typedef struct CHUNK_T{
	float left;
	float right;
	float n_err;
	float n_out;
	float p_rej;
	struct CHUNK_T * next;
	struct CHUNK_T * prev;
} chunk_t;
	
#define MAX_NODES 10
class Distribution {
	float n_vals;
	float n_outs;
	float n_errs;
	int n;
	float oob_err;
	float oob_prob;
	chunk_t * allocate_chunk();
	void insert_value(float i, bool iserr);
	chunk_t * chunks;
	void merge_one();
	void merge();
	public:
		Distribution();
		~Distribution();
		void output(float i); //train an output
		void error(float i); //train an error
		chunk_t * find_chunk(float f); //get the chunk;
		float get_output_prob(chunk_t * chunk_id, float f); //get the output probability of a particular value
		float get_error_prob(chunk_t *  chunk_id, float f); //get error probability of a particular value
		float get_prob_reject(chunk_t *  chunk_id, float f);
		float get_error_cum_prob(); //get error probability of a particular value
		float get_output_cum_prob();
		float get_output_cond_prob(chunk_t * chunk_id, float f); //get the output probability of a particular value
		float get_error_cond_prob(chunk_t *  chunk_id, float f); //get error probability of a particular value
		void clear();
		void print();
		chunk_t get(int i);
		float getOutputCount(){return n_outs;}
		float getErrorCount(){return n_errs;}
		float getOutOfBoundErrorCount(){return oob_err;}
		float getNumberChunks(){return n;}
		void allocate(float pctrej);
};

class DistributionDetectorData : public DetectorData{
	private:
		Distribution * dist;
		float targ;
		float err;
		float derr; //deriative or target percent outputs reexec
		float ierr; //integral of target percent outputs reexec
		float rejr; //observed percent inputs reexecuted
		float param;
	public:
		DistributionDetectorData();
		void set_target(float f);
		~DistributionDetectorData();
		void accept(double d, double d0); //training point.
		double accepts(double d); //testing point
		void print();
		void record(double d, int cat);
};
#endif
