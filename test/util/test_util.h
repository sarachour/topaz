#ifndef TEST_UTIL
#define TEST_UTIL

typedef struct TEST_RESULTS {
	int pass;
	int fail;
	int total;
} test_results_t;


void print_header(const char * title);
void print_category(const char * test);
void print_test(const char * test);
void print_assert(const char * description, bool result);

void print_results(const char * title, test_results_t results);
test_results_t new_results();
test_results_t combine_results(test_results_t a, test_results_t b);
bool tassert(test_results_t * results, bool assertion);



#endif

