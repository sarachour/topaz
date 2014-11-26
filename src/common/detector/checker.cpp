#include "checker.h"

Checker::Checker(){
	this->__internal = true;
}
bool Checker::accept(){
	return this->__internal;
}
bool Checker::reject(){
	return !this->__internal;
}
void Checker::assert(bool expr){
	this->__internal &= expr;
}
