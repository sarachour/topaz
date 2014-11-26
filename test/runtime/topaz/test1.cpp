#include "topaz.h"
#include "test.h"

#include "stdio.h"

int topaz_init(Topaz * tpz, int argc, char ** argv){
	printf("main-init\n");
	return 0;
}

int topaz_finish(Topaz * tpz, int argc, char ** argv){
	printf("main-finish\n");
	return 0;
}


int topaz_main(Topaz * tpz, int argc, char ** argv){
	printf("main-body\n");
	return 0;
}

int topaz_worker(Topaz * tpz, int argc, char ** argv){
	printf("worker\n");
	return 0;
}
