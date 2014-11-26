#ifndef SCALE_H
#define SCALE_H

int main_routine(int argc, char**argv);

extern "C" void scale(float factor, int * src, int sw, int sh, int * dest, int dw, int dh);

#endif


