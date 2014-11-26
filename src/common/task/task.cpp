#include "task.h"
#include "string.h"
#include "stdio.h"
#include "topaz.h"
#include "logger.h"

Task::Task(){
	this->parcel.id = 0;
	this->parcel.rank = 0;
	this->parcel.inst = 0;
	this->parcel.has_failed = false;
	this->parcel.is_refresh = false;
	this->parcel.data = NULL;
	this->size = 0;
	this->__offset = NULL;
	this->__checkpoint = NULL;
}
void Task::update(int id, int inst, int rank, int size, bool has_failed, bool is_refresh){
	this->parcel.id = id;
	this->parcel.rank = rank;
	this->parcel.inst = inst;
	this->parcel.is_refresh = is_refresh;
	this->parcel.has_failed = has_failed;
	if(this->size < size){
		if(this->parcel.data != NULL){
			delete this->parcel.data;
		}
		this->header_size = sizeof(int)*5;
		if(Topaz::topaz->config.GODMODE_ENABLED){
			this->header_size += Topaz::topaz->getLog()->get_pack_size();
			
		}
		this->size = size;
		this->parcel.data = new char[this->header_size + this->size];
		
	}
	
}
void Task::update(int id, int inst, int rank, int size){
	this->update(id,inst,rank,size,false,false);
}
Task::~Task(){
	delete this->parcel.data;
}
int Task::getId(){
	return this->parcel.id;
}
bool Task::hasFailed(){
	return (this->parcel.has_failed == true);
}
void Task::setFailed(){
	this->parcel.has_failed = true;
}
void Task::setRefresh(){
	this->parcel.is_refresh = true;
}
bool Task::isRefresh(){
	return (this->parcel.is_refresh == true);
}
int Task::getRank(){
	return this->parcel.rank;
}
int Task::getEffectiveSize(){
	return (this->__size);
}
int Task::getMaxSize(){
	return (this->size+this->header_size);
}
char * Task::getData(){
	return this->parcel.data;
}
int Task::getInstId(){
	return this->parcel.inst;
}
void Task::startUnpack(){
	this->__checkpoint = NULL;
	this->__size = this->header_size;
	//save ptr;
	int * ptr = (int*) this->parcel.data;
	this->parcel.id = *ptr; ptr++;
	this->parcel.inst = *ptr; ptr++;
	this->parcel.rank = *ptr; ptr++;
	this->parcel.has_failed = *ptr; ptr++;
	this->parcel.is_refresh = *ptr; ptr++;
	if(Topaz::topaz->config.GODMODE_ENABLED){
		if(Topaz::topaz->isMain()){
			pin_task_info_t pint;
			pin_load_task(&pint, (double*) ptr);
			Topaz::setPinInfo(pint);
			//Topaz::topaz->getLog()->unpack(Topaz::topaz->getMachineId(),this->parcel.id, this->parcel.inst, this->parcel.rank, (char*) idat);
		}
	}
	this->__offset = &this->parcel.data[this->header_size];
}

void __PIN_LOAD_INFO(unsigned int * data, unsigned int * data2){}

Task * Task::clone(){
	Task * t = new Task();
	t->update(this->parcel.id, this->parcel.inst, this->parcel.rank, this->size, this->parcel.has_failed, this->parcel.is_refresh);
	return t;
	
}
void Task::startPack(){
	this->__checkpoint = NULL;
	this->__size = this->header_size;
	int * pdat = (int *) this->parcel.data;
	*pdat = this->parcel.id; pdat++;
	*pdat = this->parcel.inst; pdat++;
	*pdat = this->parcel.rank; pdat++;
	*pdat = this->parcel.has_failed; pdat++;
	*pdat = this->parcel.is_refresh; pdat++;
	
	if(Topaz::topaz->config.GODMODE_ENABLED){
		if(!Topaz::topaz->isMain())
			Topaz::topaz->getLog()->pack(Topaz::topaz->getMachineId(), this->parcel.id, this->parcel.inst, this->parcel.rank, (char*) pdat);
	}
	this->__offset = &this->parcel.data[this->header_size];
}

void Task::checkpoint(){
	this->__checkpoint = this->parcel.data;
}
void Task::rollback(){
	this->__size = this->__checkpoint - this->__offset;
	this->__offset = this->__checkpoint;
}
#define PACK_SINGLE_BODY(val, typ) { \
		typ * ptr = (typ *) (this->__offset); \
		ptr[0] = val; \
		this->__offset += sizeof(typ); \
		this->__size += sizeof(typ); \
	}
	
#define PACK_ARRAY_BODY(val, n ,typ) { \
	int * ptr = (int *) this->__offset; \
	ptr[0] = n; \
	this->__offset += sizeof(int); \
	this->__size += sizeof(int); \
	memcpy((void *) &ptr[1], val, n*sizeof(typ)); \
	this->__offset += sizeof(typ)*n; \
	this->__size += sizeof(typ)*n; \
}
	
#define UNPACK_SINGLE_BODY(v, typ) { \
	typ * ptr = (typ *) (this->__offset); \
	*v = ptr[0]; \
	this->__offset += sizeof(typ); \
	this->__size += sizeof(typ); \
} 

#define UNPACK_ARRAY_BODY(v,typ) { \
	int * ptr = (int *) (this->__offset); \
	int siz = ptr[0]; \
	this->__offset += sizeof(int); \
	this->__size += sizeof(int); \
	*v = (typ *) &ptr[1]; \
	this->__offset += sizeof(typ)*siz;	\
	this->__size += sizeof(typ)*siz;	\
}
void Task::pack(double val){
	PACK_SINGLE_BODY(val, double);
}
void Task::pack(int val){
	PACK_SINGLE_BODY(val, int);
}
void Task::pack(char val){
	PACK_SINGLE_BODY(val, char);
}
void Task::pack(float val){
	PACK_SINGLE_BODY(val, float);
}
void Task::pack(const int * val, int n){
	PACK_ARRAY_BODY(val, n, int);
}
void Task::pack(const float * val, int n){
	PACK_ARRAY_BODY(val, n, float);
}
void Task::pack(const double * val, int n){
	PACK_ARRAY_BODY(val, n, double);
}
void Task::pack(const char * val, int n){
	PACK_ARRAY_BODY(val, n, char);
}

//unpack scalars
void Task::unpack(int * val){
	UNPACK_SINGLE_BODY(val, int);
}
void Task::unpack(float * val){
	UNPACK_SINGLE_BODY(val, float);
}
void Task::unpack(double * val){
	UNPACK_SINGLE_BODY(val, double);
}
void Task::unpack(char * val){
	UNPACK_SINGLE_BODY(val, char);
}
//unpack arrats
void Task::unpack(int ** val){
	UNPACK_ARRAY_BODY(val, int);
}
void Task::unpack(float ** val){
	UNPACK_ARRAY_BODY(val, float);
}
void Task::unpack(double ** val){
	UNPACK_ARRAY_BODY(val, double);
}
void Task::unpack(char ** val){
	UNPACK_ARRAY_BODY(val, char);
}
