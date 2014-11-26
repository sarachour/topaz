
#include "stdio.h"
#include "stdlib.h"
#include "logger.h"

int main(int argc, char ** argv){
	if(argc < 2){
		printf("USAGE: log-reader LOG_NAME [FIELD_ID]\n");
		exit(1);
	}
	char * fname = argv[1];
	if(argc == 2){
		log_fprint_entry_list(stdout, fname);
	}
	else{
		char * id = (argv[2]);
		log_fprint_entry(stdout, fname, id);
	}
}
