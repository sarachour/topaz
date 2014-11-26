#include "mem.h"
#include "stdlib.h"
#include "stdio.h"
void mem_report_error(const char * c){
	fprintf(stderr, "ERROR (mem.c): %s\n", c);
	exit(1);
}
void init_mem_info(mem_info * m){
	int i;
	m->mem = (memloc_info_node * )malloc(HASHTABLE_SIZE*sizeof(memloc_info_node));
	for(i=0; i < HASHTABLE_SIZE; i++){
		m->mem[i].next = NULL;
		m->mem[i].v.id = -1;
	}
	m->size = 0;
	m->max_size = HASHTABLE_SIZE;
}
void destroy_mem_info(mem_info * m){
	int i;
	for(i=0; i < HASHTABLE_SIZE; i++){
		memloc_info_node * c = m->mem[i].next;
		memloc_info_node * n = NULL;
		while(c != NULL){
			n = c;
			c = c->next;
			free(n);
		}
	}
	m->size = 0;
	m->max_size = HASHTABLE_SIZE;
	free(m->mem);
}
int mem_alloc(mem_info * t, int size){
	topaz_ptr id = (topaz_ptr) t->size;
	void * ptr = (void *) malloc(size);
	mem_store(t,id,ptr,size);
	return id;
}
void mem_free(mem_info * t, topaz_ptr ptr){
	int idx = topaz_hash(t, ptr);
	memloc_info_node * n = &t->mem[idx];
	memloc_info_node * p = NULL;
	while(n != NULL && n->v.id != ptr){
		p = n;
		n = n->next;
	}
	if(n != NULL){
		n->v.id = -1;
		free(n->v.ptr);
		if(p != NULL){
			p->next = n->next;
			free(n);
		}
	}
	else{
		mem_report_error("could not find ptr to free");
	}
}
void * mem_deref(mem_info * t, topaz_ptr p){
	int idx = topaz_hash(t, p);
	
	memloc_info_node * n = &t->mem[idx];
	while(n != NULL && n->v.id != p){
		n = n->next;
	}
	if(n != NULL){
		return n->v.ptr;
	}
	else{
		//mem_report_error("could not find ptr to dereference.");
		return 0;
	}
}
int mem_size(mem_info * t, topaz_ptr p){
	int idx = topaz_hash(t, p);
	memloc_info_node * n = &t->mem[idx];
	while(n != NULL && n->v.id != p){
		n = n->next;
	}
	if(n != NULL){
		return n->v.size;
	}
	else{
		mem_report_error("could not find ptr to dereference for size.");
		return -1;
	}
}

void mem_store (mem_info * t, topaz_ptr id, void * m, int size){
	int idx = topaz_hash(t, id);
	memloc_info l;
	l.size = size;
	l.ptr = m;
	l.id = id;
	if(t->mem[idx].v.id == -1){
		t->mem[idx].v = l;
	}
	else{
		memloc_info_node * curr = &t->mem[idx];
		while(curr->next != NULL){
			if(curr->v.id == id){
				//exists
				free(curr->v.ptr);
				curr->v = l;
				return;
			}
			curr = curr->next;
		}
		//allocate new
		memloc_info_node * newn = (memloc_info_node *) malloc(sizeof(memloc_info_node));
		newn->v = l;
		curr->next = newn;
	}
	t->size++;
}

#define __MEM_DUMP_TYPE int
#define __MEM_DUMP_FORMAT "%d "

void mem_dump(mem_info * t){
	int i;
	for(i=1; i < t->max_size; i++){
		if(t->mem[i].v.id != -1){
			memloc_info_node * c = &t->mem[i];
			while(c != NULL){
				__MEM_DUMP_TYPE * ptr = (__MEM_DUMP_TYPE *) c->v.ptr;
				int size = c->v.size/sizeof(__MEM_DUMP_TYPE);
				printf("%d[%d]: ", c->v.id, size);
				int k;
				for(k=0; k < size; k++){
						printf(__MEM_DUMP_FORMAT, ptr[k]);
				}
				printf("\n");
				c = c->next;
			}
		}
	}
}
