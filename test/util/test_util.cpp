#include "stdio.h"
#include "test_util.h"


void print_header(const char * title){
	printf("######	%s  #######\n", title);
	
}
void print_category(const char * title){
	printf("# %s\n", title);
}
void print_test(const char * title){
	printf("	+ %s\n", title);
}
void print_assert(const char * title, bool result){
	printf("		> %s	%s\n", result ? "PASSED" : "FAILED", title);
}

void print_results(const char * title, test_results_t results){
	printf("===	TEST RESULTS: %s	===\n", title);
	printf("	PASSED:		%d\n", results.pass);
	printf("	FAILED:		%d\n", results.fail);
	printf("	TOTAL:		%d\n", results.total);
	printf("==========================\n");
}

bool tassert(test_results_t * results, bool assertion){
	results->total++;
	if(assertion)
		results->pass++;
	else
		results->fail++;
	return assertion;
}

test_results_t new_results(){
	test_results_t n;
	n.total = 0;
	n.fail = 0;
	n.pass = 0;
	return n;
}
test_results_t combine_results(test_results_t a, test_results_t b){
		test_results_t n;
		n.total = a.total + b.total;
		n.fail = a.fail + b.fail;
		n.pass = a.pass + b.pass;
		return n;
}
