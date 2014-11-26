#include "dist_detector.h"
#include "test_util.h"
#include "math.h"
#include "stdio.h"
#include "stdlib.h"

bool cmp(float a, float b){
	float eps = 0.0000001;
	if(fabs(a-b) < eps) return true;
	else return false;
}
test_results_t test_dist(){
	test_results_t r = new_results();
	Distribution d;
	float val;
	
	d.output(1);
	print_test("distribution of just 1");
	val = 1;
	print_assert("D(1) = 1.0", tassert(&r, cmp( d.get_output_prob(d.find_chunk(val),val), 1.00) ));
	print_assert("E(1) = 0.0", tassert(&r, cmp(d.get_error_prob(d.find_chunk(val),val), 0.00) ));
	val = 4;
	print_assert("D(4) = 0.0", tassert(&r, cmp( d.get_output_prob(d.find_chunk(val),val), 0.00) ));
	print_assert("E(4) = 0.0", tassert(&r, cmp(d.get_error_prob(d.find_chunk(val),val), 0.00) ));
	
	
	print_test("distribution of 1,2,3");
	d.clear();
	d.output(1);
	d.output(2);
	d.output(3);
	val = 1;
	print_assert("D(1) = 0.3333", tassert(&r, cmp(d.get_output_prob(d.find_chunk(val),val), 0.333333333) ));
	print_assert("E(1) = 0.0", tassert(&r, cmp(d.get_error_prob(d.find_chunk(val),val), 0.00)));
	print_assert("CUM D() = 1.0", tassert(&r, cmp(d.get_output_cum_prob(), 1) ));
	print_assert("CUM E() = 0.0", tassert(&r, cmp(d.get_error_cum_prob(), 0.00)));
	
	print_test("distribution of out=1,3,4, err=3,4");
	d.clear();
	d.output(1);
	d.error(4);
	d.output(3);
	d.error(3);
	
	val = 1;
	print_assert("D(1) = 1.0", tassert(&r, cmp(d.get_output_prob(d.find_chunk(val),val), 0.25) ));
	print_assert("E(1) = 0.0", tassert(&r, cmp( d.get_error_prob(d.find_chunk(val),val), 0.00)));
	val = 4;
	print_assert("D(4) = 0.0", tassert(&r, cmp(d.get_output_prob(d.find_chunk(val),val), 0.0) ));
	print_assert("E(4) = 1.0", tassert(&r, cmp(d.get_error_prob(d.find_chunk(val),val), 0.25)));
	val = 3;
	print_assert("D(4) = 0.0", tassert(&r, cmp(d.get_output_prob(d.find_chunk(val),val), 0.25) ));
	print_assert("E(4) = 1.0", tassert(&r, cmp(d.get_error_prob(d.find_chunk(val),val), 0.25)));
	
	d.clear();
	for(int i=0; i < 1000; i++){
		int q = rand()%5;
		int delta = (rand()%50-25);
		if(q==4) d.error(0);
		if(q == 0) d.output(100+delta);
		if(q == 1) d.output(200+delta);
		if(q == 2) d.error(200+delta);
		if(q == 3) d.error(300000+delta);
		q = rand()%50;
		if(q == 0) d.error(500+delta);
		if(q == 10) d.error(60000+delta);
		if(q == 20) d.error(99000+delta);
		if(q == 15) d.error(44000+delta);
		if(q == 25) d.error(100000+delta);
		if(q == 18) d.error(-60000+delta);
		if(q == 26) d.error(-99000+delta);
		if(q == 33) d.error(200000+delta);
		if(q == 24) d.error(150000+delta);
		
	}
	d.allocate(0.05);
	d.print();
	val = 10000;
	printf("rej(%f): %f\n",val, d.get_prob_reject(d.find_chunk(val),val));
	
	val = 189.5;
	printf("rej(%f): %f\n", val, d.get_prob_reject(d.find_chunk(val),val));
	return r;
}
