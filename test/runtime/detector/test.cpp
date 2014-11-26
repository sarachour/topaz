
#include "test.h"

int main(){
	test_results_t child, parent;
	parent = new_results();
		
	child = test_dist();
	parent = combine_results(parent, child);
		
		
	print_results("", parent);
	return (parent.fail == 0 ? 0 : 1);
}
