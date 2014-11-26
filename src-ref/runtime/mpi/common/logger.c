#include "logger.h"
#include "string.h"
#include "stdlib.h"
#include "const.h"
#include <stdio.h>


#define ENTRY_META_LEN ((NAME_LEN*sizeof(char) + sizeof(int)*2))

int __get_esize(entry_type type){
	int esize;
	switch(type){
		case ENTRY_TYPE_CHAR:
			esize = sizeof(char);
			break;
		case ENTRY_TYPE_INT:
			esize = sizeof(int);
			break;
		case ENTRY_TYPE_FLOAT:
			esize = sizeof(float);
			break;
		case ENTRY_TYPE_DOUBLE:
			esize = sizeof(double);
			break;
	}
	return esize;
}
void __write_new_logfile(log_info * l, FILE * file){
	fwrite(&l->nentries, sizeof(int), 1, file);
	l->entries = (log_entry *) malloc(sizeof(log_entry)*l->nentries);
	fseek(file, ENTRY_META_LEN*l->nentries, SEEK_CUR);
}
void __write_entry_to_logfile(log_entry * l, int idx, FILE * file){
	int oldpos = ftell(file);
	fseek(file, sizeof(int), SEEK_SET);
	fseek(file, ENTRY_META_LEN*idx, SEEK_CUR);
	//write to file
	fwrite(l->name, sizeof(char), NAME_LEN, file);
	fwrite(&l->type, sizeof(int), 1, file);
	fwrite(&l->size, sizeof(int), 1, file);
	#ifdef DEBUG
	printf("LOG[WRITE]: entry %d [%s, type=%d, size=%d] written\n", idx, l->name, l->type, l->size);
	#endif
	//go to previous position
	fseek(file, oldpos, SEEK_SET);
}
void __read_existing_logfile(log_info * l, FILE * file){
	int nentries = -1, i;
	fread(&nentries, sizeof(int), 1, file);
	#ifdef DEBUG
	fprintf(stderr, "LOG[READ]: number of entries %d\n", nentries);
	#endif
	l->nentries = nentries;
	l->entries = (log_entry *) malloc(sizeof(log_entry)*nentries);
	for(i=0; i < nentries; i++){
		fread(l->entries[i].name, sizeof(char), NAME_LEN, file);
		fread(&l->entries[i].type, sizeof(int), 1, file);
		fread(&l->entries[i].size, sizeof(int), 1, file);
		l->entries[i].history = malloc(__get_esize(l->entries[i].type)*l->entries[i].size*LOG_BUFFER_SIZE);
		#ifdef DEBUG
		fprintf(stderr, "LOG[READ]: entry [%s, type=%d, size=%d] discovered\n", 
			l->entries[i].name, l->entries[i].type, l->entries[i].size);
		#endif
		
	}
	l->idx = nentries;
	#ifdef DEBUG
	fprintf(stderr, "LOG[READ]: done\n");
	#endif
}

void log_init_info(log_info * l,  char * name, int nentries){
	int file_exists; 
	int namelen = strlen(name);
	FILE * file;
	l->nentries = nentries;
	l->idx = 0;
	l->name = (char*)malloc(sizeof(char)*(namelen));
	memcpy(l->name, name, namelen);
	
	file=fopen(name,"rb");
    if (file==NULL){
		file = fopen(name, "w+b"); //create, open for writing
		__write_new_logfile(l, file);
		file_exists=false;
	}
    else {
		file_exists=true; 
		__read_existing_logfile( l, file);
		fclose(file);
		file = fopen(name,"a+b");
	}
	l->fptr = file;
	l->opn_log_already_exists = file_exists;
	
}
void log_destroy(log_info * l){
	int i=0;
	for(i=0; i < l->idx; i++){
		//free(l->entries[i].name);
		free(l->entries[i].history);
	}
	free(l->name);
	free(l->entries);
	fclose(l->fptr);
}

int log_add_entry(log_info * l, const char * name, entry_type type, int size){
	int idx = l->idx;
	if(l->opn_log_already_exists){
		int i;
		for(i=0; i < l->nentries; i++){
			if(strcmp(name, l->entries[i].name) == 0 &&
				type == l->entries[i].type &&
				size == l->entries[i].size){
						l->entries[i].index = 0;
						l->entries[i].history = malloc(__get_esize(type)*size*LOG_BUFFER_SIZE);
						return i;
				}
		}
		printf("ERROR: no entry with the name %s exists in the log %s\n please remove the log and run again.\n",name, l->name);
		exit(1);
	}
	else{
		//printf("Adding [%d]: size: %d, entry type: %d\n", idx, size, type);
		l->entries[idx].type = type;
		l->entries[idx].size = size;
		l->entries[idx].index = 0;
		strcpy(l->entries[idx].name, name);
		l->entries[idx].history = malloc(__get_esize(type)*size*LOG_BUFFER_SIZE);
		//write the entry to the file.
		__write_entry_to_logfile(&l->entries[idx], idx, l->fptr);
		l->idx++;
	}
	return idx;
}
void __log_write_out_entry(log_info * l, int entry){
	log_entry * e = &l->entries[entry];
	int bufsize = e->index;
	int linesize = __get_esize(e->type)*e->size;
	fwrite(&entry, sizeof(int), 1, l->fptr);
	fwrite(&linesize, sizeof(int), 1, l->fptr);
	fwrite(&bufsize, sizeof(int), 1, l->fptr);
	fwrite(&e->type, sizeof(int), 1, l->fptr);
	fwrite(&e->size, sizeof(int), 1, l->fptr);
	fwrite(e->history, linesize, bufsize, l->fptr);
	e->index = 0;
}
void log_add_entry_line(log_info * l, int entry, const void * elem){
	log_entry * e = &l->entries[entry];
	int linesize = __get_esize(e->type)*e->size;
	if(e->index == LOG_BUFFER_SIZE){
		__log_write_out_entry(l, entry);
	}
	int idx = linesize*(e->index);
	char * dest = &((char *) e->history)[idx];
	memcpy(dest, elem, linesize);
	e->index++;
}

void log_write_out_entries(log_info * l){
	int i;
	for(i=0; i < l->nentries; i++){
		if(l->entries[i].index > 0){
			__log_write_out_entry(l, i);
		}
	}
}

void log_fprint_entry_list(FILE * out, const char * name){
	log_info l; int i;
	char type_name[20];
	FILE * log = fopen(name, "r");
	__read_existing_logfile(&l, log);
	for(i=0; i < l.nentries; i++){
		if(strcmp(l.entries[i].name, "") != 0){
			switch(l.entries[i].type){
				case ENTRY_TYPE_CHAR: strcpy(type_name, "char"); break;
				case ENTRY_TYPE_INT: strcpy(type_name, "int"); break;
				case ENTRY_TYPE_DOUBLE: strcpy(type_name, "double"); break;
				case ENTRY_TYPE_FLOAT: strcpy(type_name, "float"); break;
			}
			printf("ENTRY: %s		[type=%s,#elems=%d]\n", 
					l.entries[i].name,
					type_name,
					l.entries[i].size
				);
		}
		
	}
	fclose(log);
}
void log_fprint_entry(FILE * out, const char * name, const char * ename){
	int entry, linesize, bufsize, type, size, i, entryno=-1;
	int iseof = false;
	log_info l;
	FILE * log = fopen(name, "r");
	void * block=0, * pos;
	int bsize = 0;
	__read_existing_logfile(&l, log);
	fprintf(stderr, "nentries: %d\n", l.nentries);
	for(i=0; i < l.nentries; i++){
		if(strcmp(l.entries[i].name, ename) == 0){
			entryno = i;
			break;
		}
	}
	if(entryno < 0){ fclose(log); fprintf(stderr, "ERROR: Field doesn't exist\n"); exit(1); }
	//fprintf(out, "## LOG: %s : Entry %s [%d]\n", name, ename, entryno);
	
	fread(&entry, sizeof(int), 1, log);
	fread(&linesize, sizeof(int), 1, log);
	fread(&bufsize, sizeof(int), 1, log);
	fread(&type, sizeof(int), 1, log);
	fread(&size, sizeof(int), 1, log);
		
	while(!feof(log) && iseof == false){
		if(bsize < linesize*bufsize){
			free(block);
			block = malloc(linesize*bufsize);
			bsize = linesize*bufsize;
		}
		//fprintf(stderr, "entry=%d, linesize=%d, bufsize=%d, type=%d, size=%d\n", entry,linesize,bufsize,type,size);
			
		if(entry == entryno){
			int i,j;
			fread(block, linesize, bufsize, log);
			pos = block;
			for(i=0; i < bufsize; i++){
				for(j=0; j < size; j++){
					switch(type){
						case ENTRY_TYPE_INT: fprintf(out, "%d", ((int*) pos)[j]); break;
						case ENTRY_TYPE_FLOAT: fprintf(out, "%f", ((float*) pos)[j]); break;
						case ENTRY_TYPE_DOUBLE: fprintf(out, "%lf", ((double*) pos)[j]); break;
						case ENTRY_TYPE_CHAR: fprintf(out, "%c" , ((char*) pos)[j]); break;
					}
					if(j < size-1) fprintf(out,",");
				}
				fprintf(out,"\n");
				pos += linesize;
			}
		}
		else{
			char m;
			fseek(log, linesize*bufsize, SEEK_CUR);
			long nloc=ftell(log); 
			//fprintf(stderr, "seek forward %d, %dl\n", linesize*bufsize, nloc);
			m = getc(log);
			iseof = feof(log) ? true : false;
			ungetc(m, log);
			
		}
		fread(&entry, sizeof(int), 1, log);
		fread(&linesize, sizeof(int), 1, log);
		fread(&bufsize, sizeof(int), 1, log);
		fread(&type, sizeof(int), 1, log);
		fread(&size, sizeof(int), 1, log);
	}
	//fprintf(out, "###########\n");
	fclose(log);
}
