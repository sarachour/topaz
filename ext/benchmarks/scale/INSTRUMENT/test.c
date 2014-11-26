#include "stdio.h"
#include "instrument.h"


int main(){
	inst_timer t = create_timer();
	print_timer(&t);
	start_timer(&t);
	sleep(1);
	end_timer(&t);
	print_timer(&t);
	start_timer(&t);
	sleep(1);
	end_timer(&t);
	print_timer(&t);
	start_timer(&t);
	sleep(1);
	end_timer(&t);
	print_timer(&t);
	
}
