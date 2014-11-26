#include <stdio.h>
#include <string.h>
#include <math.h>

#include <iostream>
#include <fstream>
#include "barnes.h"

void node_cons(node_t * n){
	n->type = 0; 
	n->mass = 0.0; 
	vecClr(&n->pos); 
	n->level = 0; 
	n->parent = REF_NULL; 
	n->child_num =0;
}

void node_clear(node_t * n){
	n->type = 0; 
	n->mass = 0.0; 
	vecClr(&n->pos); 
	n->level = 0; 
	n->parent = REF_NULL; 
	n->child_num =0;
}

void node_adjLevel(node_t * me, node_t * parent){
	  if (parent == NULL) me->level = (IMAX >> 1);
	  else me->level = (parent->level >> 1);
}

void cell_cons(cell_t * l){
	node_cons(&l->node);
	l->node.type = CELL;
}

void cell_clear(cell_t * l){
	node_clear(&l->node);
	int i;
	for (i = 0; i < NSUB; i++)
		l->subp[i] = REF_NULL;
}


void leaf_cons(leaf_t * l){
	node_cons(&l->node);
	l->node.type = LEAF;
}

void leaf_clear(leaf_t * l){
	node_clear(&l->node);
	l->num_bodies = 0;
	
}

int  leaf_full(leaf_t * l){ 
	return (l->num_bodies == LEAFMAXBODIES); 
}

void body_cons(body_t * l){
	node_cons(&l->node);
	l->node.type = BODY;
}


void body_clear(body_t * l){
	node_clear(&l->node);
	
}

void body_swapAcc(body_t * l) {
  double tmpv[NDIM];

  l->phi = 0.0;
  vecStore(&l->acc, tmpv);
  vecLoad(&l->tmp, tmpv);
  vecClr(&l->acc);
}
inline node_t * __NODE(node_ref ref, nbody_t * nb){
	if(isnull(ref)) return NULL;
	else{
		if(ref.type == LEAF) return &leafset_get(&nb->leaves, ref)->node;
		else if(ref.type == CELL) return &cellset_get(&nb->cells, ref)->node;
		else return &bodyset_get(&nb->bodies, ref)->node;
	}
}

inline double body_computeInter(body_t * cbody, node_t * curr, double epsSq, double *res){
  int i;
  double drabs, inc, mor3;
  double drsq, d;

  drsq = epsSq;
  for (i = 0; i < NDIM; i++) {
    d = curr->pos.val[i]-cbody->node.pos.val[i];
    drsq += d*d;
    res[i] = d;
  }
  drabs = sqrt(drsq);
  inc = curr->mass / drabs;
  mor3 = inc / drsq;
  tvecScale(res,mor3);
  return inc;
}

#define updatePhi(inc, curr) {curr->phi -= inc;}

void body_fastgravsub(body_t * cbody, node_t * proc, double *ai, double pdrsq, double epssq) {
  double drsq;
  double drabs, inc, mor3;
  double tmpv[NDIM];

  drsq = pdrsq + epssq;
  drabs = sqrt(drsq);
  inc = proc->mass / drabs;
  updatePhi(inc, cbody);
  mor3 = inc / drsq;
  tvecLoad(tmpv,ai);
  tvecScale(tmpv,mor3);
  vecAdd(&cbody->acc, tmpv);
}

void body_gravsub(body_t * cbody, node_t * curr, double epsSq){
  double phii;
  double ai[NDIM];

  phii = body_computeInter(cbody,curr,epsSq,ai);
  updatePhi(phii, cbody);
  vecAdd(&cbody->acc, ai);
}

void body_openCell(node_ref cbody, cell_t *curr, double tolsq, double dsq, double epssq, nbody_t * nb){
  int i;
  node_ref child_ptr;
  for(i=0; i < NSUB; i++) {
    child_ptr = curr->subp[i];
    if(!isnull(child_ptr)){
      body_walksub(cbody,child_ptr,tolsq,(dsq/4.0),epssq, nb);
    }
  }
}

void body_openLeaf(node_ref cbody, leaf_t * currleaf,double epssq, int iter, nbody_t * nb){
  int i;
  body_t * cbody_dat = bodyset_get(&nb->bodies, cbody); 
  for(i=0; i < iter; i++){
    node_ref bptr = currleaf->bodyp[i];
    body_t * bptr_data = bodyset_get(&nb->bodies, bptr);
    if(!isequals(cbody, bptr)){
      body_gravsub(cbody_dat, &bptr_data->node, epssq);
    }
  }
}

double body_subdiv(node_ref cbody, node_ref child, double *res, nbody_t * nb) {
  int i;
  double drsq, d;
  node_t * cbody_dat = __NODE(cbody, nb);
  node_t * child_dat = __NODE(child, nb);
  drsq = 0.0;
  for (i = 0; i < NDIM; i++) {
    d = child_dat->pos.val[i]-cbody_dat->pos.val[i];
    drsq += d*d;
    res[i] = d;
  }
  return(drsq);
}

void body_walksub(node_ref cbody, node_ref head, double tolsq, double dsq, double epssq, nbody_t * nb){
	double drsq;
	double ai[NDIM];
	body_t * cbody_data = bodyset_get(&nb->bodies, cbody); 
	drsq = body_subdiv(cbody, head,ai, nb);
	if((tolsq * drsq) < dsq){
		if(head.type == CELL){
		  cell_t * head_cell = cellset_get(&nb->cells, head);
		  body_openCell(cbody, head_cell,tolsq,dsq,epssq, nb);
		} 
		else {
		  leaf_t * head_leaf = leafset_get(&nb->leaves,head);
		  body_openLeaf(cbody, head_leaf,epssq,head_leaf->num_bodies, nb);
		}
	} 
	else {
		body_fastgravsub(cbody_data, __NODE(head, nb), ai, drsq, epssq);
	}
}

void body_startVel(body_t * l, double f){
   double tmpv1[NDIM], tmpv2[NDIM];
   vecStore(&l->acc, tmpv1);
   vecStore(&l->tmp, tmpv2);
   tvecSub(tmpv1,tmpv2);
   tvecScale(tmpv1,f);
   vecAdd(&l->vel, tmpv1);
}

void body_advance(body_t * l, double hts, double ts){
  double dvel[NDIM], vel1[NDIM], dpos[NDIM];

  vecStore(&l->acc, dvel);
  vecStore(&l->vel, vel1);
  tvecScale(dvel,hts);
  tvecAdd(vel1,dvel);
  tvecLoad(dpos,vel1);
  tvecScale(dpos,ts);
  tvecScale(dvel,2.0);

  vecAdd(&l->vel, dvel);
  vecAdd(&l->node.pos, dpos);
}

