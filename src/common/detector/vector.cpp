#include "dist_detector.h"
#include "math.h"

vector_t Vector::create(int n){
	vector_t v;
	v.d = new float[n];
	v.n = n;
	return v;
}

vector_t Vector::create(float * d, int n){
	vector_t v = Vector::create(n);
	for(int i=0; i < n; i++) v.d[i] = d[i];
	return v;
}

vector_t Vector::create(vector_t d, int n){
	vector_t v = Vector::create(n);
	for(int i=0; i < n; i++) v.d[i] = d.d[i];
	return v;
}

vector_t Vector::create(float d, int n){
	vector_t v = Vector::create(n);
	for(int i=0; i < n; i++) v.d[i] = d;
	return v;
}

vector_t Vector::wrap(float * d, int n){
	vector_t v;
	v.d = d;
	v.n = n;
	return v;
}

void Vector::dealloc(vector_t a){
	delete a.d;
}

void Vector::set(vector_t a, vector_t r){
	for(int i=0; i < a.n; i++) a.d[i] = r.d[i];
}


void Vector::foreach(float (*fun)(float,float), vector_t a, vector_t b, vector_t r){
	for(int i=0; i < a.n; i++){
		r.d[i] = fun(a.d[i],b.d[i]);
	}
}

void Vector::foreach(float (*fun)(float,float,float*), vector_t a, vector_t b, vector_t r, float * args){
	for(int i=0; i < a.n; i++){
		r.d[i] = fun(a.d[i],b.d[i],args);
	}
}

float Vector::reduce(float (*fun)(float,float,float), vector_t a, vector_t b, float r){
		for(int i=0; i < a.n; i++){
			r = fun(a.d[i],b.d[i],r);
		}
		return r;
}

ds_vector_t DSVector::create(int n){
	ds_vector_t v;
	v.n = n;
	v.pos = new float[n];
	v.neg = new float[n];
	return v;
}

ds_vector_t DSVector::create(vector_t a){
	ds_vector_t v = DSVector::create(a.n);
	for(int i=0; i < v.n; i++){
		v.pos[i] = v.neg[i] = 0;
		if(a.d[i] >= 0) v.pos[i] = a.d[i];
		else v.neg[i] = -a.d[i];
	}
	return v;
}

ds_vector_t DSVector::create(vector_t a, vector_t b){
	ds_vector_t v = DSVector::create(a.n);
	for(int i=0; i < v.n; i++){
		v.pos[i] = fabs(a.d[i]);
		v.neg[i] = fabs(b.d[i]);
	}
	return v;
}

ds_vector_t DSVector::create(ds_vector_t x){
	ds_vector_t v = DSVector::create(v.n);
	for(int i=0; i < v.n; i++){
		v.pos[i] = fabs(x.pos[i]);
		v.neg[i] = fabs(x.neg[i]);
	}
	return v;
}

ds_vector_t DSVector::create(float x, int n){
	ds_vector_t v = DSVector::create(n);
	for(int i=0; i < v.n; i++){
		v.pos[i] = v.neg[i] = x;
	}
	return v;
}

void DSVector::dealloc(ds_vector_t v){
	delete v.pos;
	delete v.neg;
}

void DSVector::foreach(float (*fun)(float,float), ds_vector_t a, vector_t b, ds_vector_t e){
	for(int i=0; i < a.n; i++){
		if(b.d[i] >= 0) e.pos[i] = fun(a.pos[i], b.d[i]);
		else e.neg[i] = fun(a.neg[i], b.d[i]);
	}
}

void DSVector::foreach(float (*fun)(float,float,float*), ds_vector_t a, vector_t b, ds_vector_t e, float * args){
	for(int i=0; i < a.n; i++){
		if(b.d[i] >= 0) e.pos[i] = fun(a.pos[i], b.d[i],args);
		else e.neg[i] = fun(a.neg[i], b.d[i],args);
	}
}

void DSVector::foreach(float (*fun)(float,float), ds_vector_t a, ds_vector_t b, ds_vector_t e){
	for(int i=0; i < a.n; i++){
		e.pos[i] = fun(a.pos[i], b.pos[i]);
	}
	for(int i=0; i < a.n; i++){
		e.neg[i] = fun(a.neg[i], b.neg[i]);
	}
}

void DSVector::foreach(float (*fun)(float,float, float *), ds_vector_t a, ds_vector_t b, ds_vector_t e, float * args){
	for(int i=0; i < a.n; i++){
		e.pos[i] = fun(a.pos[i], b.pos[i],args);
	}
	for(int i=0; i < a.n; i++){
		e.neg[i] = fun(a.neg[i], b.neg[i],args);
	}
}

void DSVector::foreach(float (*fun)(float), ds_vector_t a, ds_vector_t e){
	for(int i=0; i < a.n; i++){
		e.pos[i] = fun(a.pos[i]);
	}
	for(int i=0; i < a.n; i++){
		e.neg[i] = fun(a.neg[i]);
	}
}

float DSVector::reduce(float (*fun)(float,float,float), ds_vector_t a, vector_t b,float v){
	for(int i=0; i < a.n; i++){
		if(b.d[i] >= 0)
			v = fun(a.pos[i],b.d[i],v);
		else
			v= fun(a.neg[i],b.d[i],v);
	}
	return v;
}

float DSVector::reduce(float (*fun)(float,float,float), ds_vector_t a, ds_vector_t b, float v){
	for(int i=0; i < a.n; i++){
		v = fun(a.pos[i],b.pos[i],v);
	}
	for(int i=0; i < a.n; i++){
		v= fun(a.neg[i],b.neg[i],v);
	}
	return v;
}

float DSVector::reduce(float (*fun)(float,float), ds_vector_t a, float v){
	for(int i=0; i < a.n; i++){
		v = fun(a.pos[i],v);
	}
	for(int i=0; i < a.n; i++){
		v= fun(a.neg[i],v);
	}
	return v;
}

