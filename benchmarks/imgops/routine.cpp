#include "ops.h"
#include "image.h"
#include "stdio.h"
#include "math.h"
//for software analysis.
#include "time.h"
#include "float.h"
#include "stdlib.h"

int * allocate_dest_image(double scale_factor, size_t sw, size_t sh, size_t * dw, size_t * dh){
	int * result;
	(*dw) = ceil(sw*scale_factor);
	(*dh) = ceil(sh*scale_factor);
	
	printf("dest image size: %zu x %zu\n", *dw, *dh);
	result = (int*) malloc((*dw)*(*dh)*3*sizeof(int));
	if(result == NULL){
		fprintf(stderr, ">> Could not allocate destination image..\n");
		exit(1);
	}
	return result;
}

int main_routine(int argc, char**argv){
	
	if(argc <= 3){
		printf("USAGE: scale FACTOR INPUT OUTPUT\n");
		return 1;
	}
	double scale_factor = atof(argv[1]);
	char* in_filename = argv[2];
	char * out_filename = argv[3];
	printf("scale by %f: %s -> %s\n", scale_factor, in_filename, out_filename);

	int* src, * dest;

	size_t sw, sh, dw, dh;
	printf("read from \"%s\" ...\n", in_filename);
	src = read_image(in_filename, &sw, &sh);

	if(src == NULL){
		fprintf(stderr, ">> Failed to read image %s\n", in_filename);
		exit(1);
	}
	dest = allocate_dest_image(scale_factor, sw, sh, &dw, &dh);
	
	scale(scale_factor, src, sw, sh, dest, dw, dh);
	printf("write to \"%s\" ...\n", out_filename);
	write_image(out_filename, dest, dw, dh);
	printf("written.\n");
	free((void *) src);
	free((void *) dest);


	return 0;

}
