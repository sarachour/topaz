#include "barnes.h"
#include "stdlib.h"

using namespace std;

void nbody_intcoord(nbody_t * n, int *xp, double *rp){
  int k;
  double xsc, v[NDIM];

  vecStore(&n->mincorner,v);
  for(k = 0; k < NDIM; k++) {
	  xsc = (rp[k] - v[k]) / n->size;
      xp[k] = (int)(floor(IMAX * xsc));
  }
}

int subindex(int *x, int l){
  int i, k, yes;

  i = 0;
  yes = 0;
  for(k = 0; k < NDIM; k++) {
    if(((x[k] & l) && !yes) || (!(x[k] & l) && yes)) {
      i += NSUB >> (k + 1);
      yes = 1;
    }
    else yes = 0;
  }
  return (i);
}

void nbody_addbodytoleaf(nbody_t * n, node_ref leaf_ptr, node_ref bp){                  // Assumes enough capacity
  body_t * bdata = bodyset_get(&n->bodies, bp);
  leaf_t * tdata = leafset_get(&n->leaves, leaf_ptr);
  bdata->node.parent = leaf_ptr;
  bdata->node.level = tdata->node.level;
  bdata->node.child_num = tdata->num_bodies;
  tdata->bodyp[tdata->num_bodies] = bp;
  tdata->num_bodies++;
}


void print_node(nbody_t * n, node_ref parent_ref, int depth){
	int i=0; int j;
	cell_t * parent_data = cellset_get(&n->cells, parent_ref);
	if(depth == 0)
	printf("-----------------------------\n");
	for(i=0; i < NSUB; i++){
		node_ref chld = parent_data->subp[i];
		if(!isnull(chld)){
			if(chld.type == CELL){
				cell_t * child_data = cellset_get(&n->cells, chld);
				for(j=0; j < depth; j++){
					printf(" ");
				}
				printf("[%d] CELL [level=%d, mass=%f, pos=(%f,%f,%f) ] ]\n", i,  child_data->node.level,
													   child_data->node.mass,
													   child_data->node.pos.val[0],
													   child_data->node.pos.val[1],
													   child_data->node.pos.val[2]);
				print_node(n, chld, depth+1);
			}
			else if(chld.type == LEAF){
				leaf_t * child_data = leafset_get(&n->leaves, chld);
				for(j=0; j < depth; j++){
					printf(" ");
				}
				printf("[%d] LEAF: %d NODES [level=%d, mass=%f, pos=(%f,%f,%f) ]\n", i, child_data->num_bodies,child_data->node.level,
																			   child_data->node.mass,
																			   child_data->node.pos.val[0],
																			   child_data->node.pos.val[1],
																			   child_data->node.pos.val[2]);
			}
			else{
				for(j=0; j < depth; j++){
					printf(" ");
				}
				printf("UNKNOWN %d\n", chld.type);
			}
		}
	}
	if(depth == 0)
	printf("-----------------------------\n");
}

void nbody_subdivideleaf(nbody_t * n, node_ref leaf_ref, node_ref parent_ref, int idx){
  cell_t *cell_data; node_ref cell_ref;
  cell_t *parent_data;
  leaf_t *leaf_data;
  body_t *body_data; node_ref body_ref;
  double tmpv[NDIM];
  int i, index, xp[NDIM], num, Lev;
  node_ref loctab[LEAFMAXBODIES];
  
  leaf_data = leafset_get(&n->leaves,leaf_ref);
  num = leaf_data->num_bodies;
  for(i=0; i < num; i++) {
    loctab[i] = leaf_data->bodyp[i];
    leaf_data->bodyp[i] = REF_NULL;
  }
  leaf_data->num_bodies = 0;
  cell_ref = cellset_make(&n->cells, parent_ref);
  cell_data = cellset_get(&n->cells, cell_ref);
  cell_data->node.child_num = leaf_data->node.child_num;
  parent_data = cellset_get(&n->cells, parent_ref);
  parent_data->subp[idx] = cell_ref;
  Lev = leaf_data->node.level;
  body_ref = loctab[0];
  body_data = bodyset_get(&n->bodies, body_ref);
  vecStore(&body_data->node.pos, tmpv);
  nbody_intcoord(n, xp,tmpv);
  index = subindex(xp, Lev);
  cell_data->subp[index] = leaf_ref;
  //c->setSubp(index,this);
  leaf_data->node.child_num = index;
  leaf_data->node.parent = cell_ref;
  leaf_data->node.level = (Lev >> 1);
  nbody_addbodytoleaf(n,leaf_ref,body_ref);
  for(i=1; i < num; i++) {
    body_ref = loctab[i];
    body_data = bodyset_get(&n->bodies, body_ref);
    vecStore(&body_data->node.pos, tmpv);
    
    nbody_intcoord(n,xp,tmpv);
    index = subindex(xp, Lev);
    //printf("idx: %d,%d [%f,%f,%f] [%d,%d,%d]\n", index, n->size, tmpv[0], tmpv[1], tmpv[2], xp[0], xp[1], xp[2]);
    leaf_ref = cell_data->subp[index];
    leaf_data = leafset_get(&n->leaves,leaf_ref);
    if(isnull(leaf_ref)) {
      leaf_ref = leafset_make(&n->leaves, &n->cells, cell_ref);
      leaf_data = leafset_get(&n->leaves, leaf_ref);
      leaf_data->node.child_num = index;
      cell_data->subp[index] = leaf_ref;
    }
    nbody_addbodytoleaf(n,leaf_ref, body_ref);
  }
}



void nbody_addbodytocell(nbody_t * n, node_ref cell_ref, node_ref body_ref, int *coords) {
  int Lev, kidIndex;
  cell_t * cell_data = cellset_get(&n->cells, cell_ref);
  node_ref chld_ptr;
  kidIndex = subindex(coords,cell_data->node.level);
  chld_ptr = cell_data->subp[kidIndex];
  Lev = (cell_data->node.level >> 1);
  //printf("intcoords: %d,%d,%d\n", coords[0], coords[1], coords[2]);
  //printf("subindex: %d, %d\n", cell_data->node.level, kidIndex);
  if(Lev != 0){
    if(isnull(chld_ptr)) {
	  //printf("[%d] adding new leaf node\n", kidIndex);
	  leaf_t *leaf_data;
	  node_ref lf_ptr;
      lf_ptr = leafset_make(&n->leaves,&n->cells, cell_ref);
      leaf_data = leafset_get(&n->leaves,lf_ptr);
      leaf_data->node.child_num = (kidIndex);
      cell_data->subp[kidIndex] = lf_ptr;
      nbody_addbodytoleaf(n, lf_ptr, body_ref);
    } 
    else {
      if(chld_ptr.type == LEAF){
		leaf_t *leaf_data;
		//printf("[%d] using existing leaf node\n", kidIndex);
        leaf_data = leafset_get(&n->leaves,chld_ptr);
        if(leaf_full(leaf_data)){
	      //printf("[%d] leaf node is full, subdivide\n", kidIndex);
          nbody_subdivideleaf(n, chld_ptr, cell_ref,kidIndex);
          nbody_addbodytocell(n, cell_ref, body_ref,coords);
        } 
        else {
		  //printf("[%d] leaf node not full, adding\n", kidIndex);
          nbody_addbodytoleaf(n, chld_ptr, body_ref);
        }
      } 
      else {
		//printf("[%d] descending into child node\n", kidIndex);
        nbody_addbodytocell(n, chld_ptr, body_ref, coords);
      }
    }
  } 
  else {
    cerr << " *** Error: Not enough levels in tree...(CellAddBody)\n";
  }
}

void nbody_maketree(nbody_t * n){
  int i, xp[NDIM];
  double v[NDIM];
  body_t *p;

  for(i=0; i < n->nbody; i++){
    p = bodyset_get(&n->bodies,bodyptr(i));
    vecStore(&p->node.pos, v);
    nbody_intcoord(n, xp,v);
    //printf("Body: Adding %d\n", i);
    //printf("pos: %f, %f, %f\n", p->node.pos.val[0], p->node.pos.val[1],p->node.pos.val[2]);
    //printf("nbody= size: %f, mincorner: (%f,%f,%f)\n", n->size, n->mincorner.val[0],n->mincorner.val[1],n->mincorner.val[2]);
    nbody_addbodytocell(n, n->BH_tree, bodyptr(i), xp);
    //print_node(n,n->BH_tree,0);
    
  }
  
}
