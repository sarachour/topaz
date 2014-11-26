#include "barnes.h"
#include "stdlib.h"
#include <iostream>

using namespace std;

void cellset_reset(cellset_t * c){
	int i;
	c->numcell =  0;
	for(i=0; i < c->maxcell; i++){
		cell_clear(&c->celltab[i]);
	}
	
}


void cellset_cons(cellset_t * c, int max){
	int i;
	c->celltab = (cell_t*) malloc(sizeof(cell_t)*max); 
	c->maxcell=max; 
	c->numcell=0;
	for(i=0; i < c->maxcell; i++){
		cell_cons(&c->celltab[i]);
	}
	cellset_reset(c);
	
	
}

cell_t * cellset_get(cellset_t * c, node_ref i){
	return &c->celltab[i.offset];
}

node_ref cellset_new(cellset_t * s){
	  cell_t *c;
	  int sid = s->numcell;
	  if(s->numcell == s->maxcell){
		cerr << "newcell: More than " << s->maxcell << " cells; increase fcells\n";
		return REF_NULL;
	  }
	  c = &s->celltab[s->numcell];
	  s->numcell++;
	  cell_clear(c);
	  return cellptr(sid);
}
node_ref cellset_make(cellset_t * s, node_ref parent){
	  cell_t *c; 
	  node_t * n = (isnull(parent) || cellset_get(s, parent) == NULL ? NULL : &cellset_get(s, parent)->node);
	  node_ref sid = cellset_new(s);
	  if(isnull(sid)) return REF_NULL;
	  c = cellset_get(s, sid);
	  c->node.parent = parent;
	  node_adjLevel(&c->node, n);
	  return sid;
}

void leafset_reset(leafset_t * c){
	 int i; 
	 c->numleaf = 0;
	 for(i=0; i < c->maxleaf; i++){
		 leaf_clear(&c->leaftab[i]);
	 }
}

void leafset_cons(leafset_t * c, int max){
	 int i; 
	 c->leaftab =(leaf_t*) malloc(sizeof(leaf_t)*max); 
	 c->maxleaf=max; 
	 c->numleaf=0; 
	 for(i=0; i < c->maxleaf; i++){
		 leaf_cons(&c->leaftab[i]);
	 }
	 leafset_reset(c);
}

leaf_t * leafset_get(leafset_t * c, node_ref idx){
	return &c->leaftab[idx.offset];
}


node_ref leafset_new(leafset_t * c){
  leaf_t * p;
  int sid = c->numleaf;
  if(c->numleaf == c->maxleaf){
    cerr << "newleaf: More than " << c->maxleaf << " leaves; increase fleaves\n";
    return REF_NULL;
  }
  p = &c->leaftab[c->numleaf];
  leaf_cons(p);
  c->numleaf++;
  leaf_clear(p);
  return leafptr(sid);
}

node_ref leafset_make(leafset_t * s, cellset_t * sc, node_ref parent){
  node_t * n = (isnull(parent) || cellset_get(sc, parent) == NULL ? NULL : &cellset_get(sc, parent)->node);
  node_ref l; leaf_t * l_data;
  l = leafset_new(s);
  if(isnull(l)) return REF_NULL;
  l_data = leafset_get(s, l);
  l_data->node.parent = parent;
  node_adjLevel(&l_data->node, n);
  return (l);	
}


void bodyset_cons(bodyset_t * c, int m){
	 int i; 
	 c->bodytab = (body_t*) malloc(sizeof(body_t)*m); 
	 c->maxbody=m; 
	 c->numbody=0; 
	 for(i=0; i < c->maxbody; i++){
		 body_cons(&c->bodytab[i]);
		 body_clear(&c->bodytab[i]);
	 }
}

body_t * bodyset_get(bodyset_t * c, node_ref i){
	return &c->bodytab[i.offset];
}

body_t * bodyset_newbody(bodyset_t * c){
	  body_t *p;
	  if(c->numbody == c->maxbody){
		cerr << "newbody: More than " << c->maxbody << "body; increase nbody\n";
		return NULL;
	  }
	  p = &c->bodytab[c->numbody];
	  c->numbody++;
	  return p;
}
