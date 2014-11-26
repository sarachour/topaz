#ifndef SCALE_H
#define SCALE_H

#ifdef __cplusplus 

extern "C" void scale(float factor, int * src, int sw, int sh, int * dest, int dw, int dh);

#else
void scale(float factor, int * src, int sw, int sh, int * dest, int dw, int dh);

#endif                   

#endif
