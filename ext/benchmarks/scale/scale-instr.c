#include "scale.h"
#include "math.h"
#include "instrument.h"
#define IDX(i,j,w) (i*w+j)
#define R(k) (3*k)
#define G(k) (3*k+1)
#define B(k) (3*k+2)
#define BILINEAR(ci, cj, nexti, nextj, previ, prevj, c_ul,c_ur,c_ll,c_lr) (1.0/((nexti-previ)*(nextj-prevj))*(c_lr*(nextj-cj)*(nexti-ci)+c_ur*(nextj-cj)*(ci-previ)+c_ll*(cj-prevj)*(nexti-ci)+c_ul*(cj-prevj)*(ci-previ)))


void scale(float factor, int* src, int sw, int sh, int * dest, int dw, int dh){
	int i,j,k=0;
	float si=0, sj=0;
	float delta_i=1/factor, delta_j=1/factor;
	inst_timer KERNEL_TIMER = create_timer();
	
	if(factor > 1){
		for(i=0; i < dh; i++, si+=delta_i){
			for(j=0, sj=0; j < dw; j++, sj+=delta_j){
				start_timer(&KERNEL_TIMER);
				int previ, prevj, nexti, nextj;
				int * ll, * lr, * ul, * ur;
				//corners
				previ = floor(si); prevj = floor(sj);
				nexti = ceil(si); nextj = ceil(sj);
				//corners
		
				previ = (int) floor(si); 
				prevj = (int) floor(sj);

				nexti = (int) ceil(si); 
				nextj = (int) ceil(sj);
			     
				//clip coordinates
				if (sh <= previ) {
				  previ = sh - 1;
				}
			      
				if (sw <= prevj) {
				  prevj = sw - 1;
				}

				if (sh <= nexti) {
				  nexti = sh - 1;
				}
			      
				if (sw <= nextj) {
				  nextj = sw - 1;
				}

				if (previ == nexti) {
				   if (0 == previ) {
				      if (sw - 1 == nexti) {
					//1-pixel wide, use same pixel
				      } else {
					nexti = nexti + 1;
				      }
				   } else {
				      previ = previ - 1;
				   }
				} 

				if (prevj == nextj) {
				   if (0 == prevj) {
				      if (sh - 1 == nextj) {
					//1-pixel high, use same pixel
				      } else {
					nextj = nextj + 1;
				      }
				   } else {
				      prevj = prevj - 1;
				   }
        			} 
				lr = &src[R(IDX(previ, prevj,sw))];
				ll = &src[R(IDX(previ, nextj,sw))];
				ur = &src[R(IDX(nexti, prevj,sw))];
				ul = &src[R(IDX(nexti, nextj,sw))];
				dest[R(IDX(i,j,dw))]= BILINEAR(si, sj, nexti, nextj, previ, prevj, ul[0], ur[0], ll[0], lr[0]);
				dest[G(IDX(i,j,dw))]= BILINEAR(si, sj, nexti, nextj, previ, prevj, ul[1], ur[1], ll[1], lr[1]);
				dest[B(IDX(i,j,dw))]= BILINEAR(si, sj, nexti, nextj, previ, prevj, ul[2], ur[2], ll[2], lr[2]);
				end_timer(&KERNEL_TIMER);
			}
		}
	}
	printf("KERNEL TIME\n");
	print_timer(&KERNEL_TIMER);

}
