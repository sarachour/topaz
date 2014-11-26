#include "image.h"
#include "stdio.h"
#include "stdint.h"
#include "stdlib.h"

int* read_image(char * filename, size_t* w, size_t* h){
	FILE *fp;
	char header[16];
	size_t width, height,depth,rheight;
	int* data;
	
	fp = fopen(filename, "rb");
	if(!fp){
		fprintf(stderr, "ERROR: Cannot open file %s\n", filename);
		return NULL;
	}
	
	if(!fgets(header, sizeof(header), fp)){
		perror(filename);
		return NULL;
	}
	
	if(header[0] != 'P' || header[1] != '6'){
		fprintf(stderr, "Invalid image format. Must be P6");
		return NULL;
	}
	//check for comments
    char c = getc(fp);
    while (c == '#') {
    while (getc(fp) != '\n') ;
         c = getc(fp);
    }
    ungetc(c, fp);
    
	if(fscanf(fp, "%zu %zu", &width, &height) != 2){
		fprintf(stderr, "Cannot read image size: %s\n", filename);
		return NULL;
	}
	if(fscanf(fp, "%zu", &depth) != 1){
		fprintf(stderr, "Invalid rgb component: %s\n", filename);
		return NULL;
	}
	/*
	if( (int) depth == 255){
		fprintf(stderr, "NOT 8 bit component: %d\n", depth);
		return NULL;
	}
	*/
	fprintf(stdout, "%s: %zu x %zu : %zu\n", filename, width, height, depth);
	//ignore newlines
	while(fgetc(fp) != '\n');

	int num_elems = width*height*3;
	data = (int*) malloc(num_elems * sizeof(int));
	if(!data){
		perror("malloc");
		fprintf(stderr, "Cannot allocate space for image data: %zu\n", width*height*3*sizeof(int));
		return NULL;
	}
	/*
	if((rheight=fread(data, 3*width, height, fp)) != height){
		fprintf(stderr, "Error loading image '%s', read %d/%d lines\n", filename, rheight, height);
		return NULL;
	}
	*/
	int i,j,k=0;
	uint8_t buf[3*width];
	for(i=0; i < height; i++){
		if(fread(&buf, 3*width, 1, fp) != 1){
			fprintf(stderr, "Error loading image '%s', read %zu/%zu lines\n", filename, rheight, height);
			return NULL;
		}
		for(j=0; j < width; j++,k++){
			data[k*3] = buf[j*3+0];
			data[k*3+1] = buf[j*3+1];
			data[k*3+2] = buf[j*3+2];
		}
	}
	fclose(fp);
	*w = width;
	*h = height;
	return data;
}

void write_image(char * filename, int * data, size_t w, size_t h){
	FILE *fp;
	fp = fopen(filename, "wb");
	if(!fp){
		fprintf(stderr, "ERROR: Cannot open file %s\n", filename);
		return;
	}
	
	fprintf(fp, "%s\n#\n%zu %zu %u\n", "P6", w, h, 255);
	
	int i,j,k=0;
	uint8_t buf[3*w];
	for(i=0; i < h; i++){
		for(j=0; j < w; j++,k++){
			buf[j*3+0] = data[k*3];
			buf[j*3+1] = data[k*3+1];
			buf[j*3+2] = data[k*3+2];
		}
		fwrite(buf, sizeof(uint8_t)*3*w,1, fp);
	}
	
	
}

