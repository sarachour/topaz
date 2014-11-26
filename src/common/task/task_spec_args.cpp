#include "task.h"

TaskArgSpec TaskArgSpec::create(bool is_const, task_arg_type_enum ta_type, type_id type, int nelems){
	TaskArgSpec t;
	t.setInfo(is_const, ta_type, type, nelems);
	return t;
}
TaskArgSpec::TaskArgSpec(){
	this->nelems = 0;
	this->type = UNKNOWN;
}
void TaskArgSpec::setInfo(bool is_const, task_arg_type_enum ta_type, type_id type, int nelems){
	this->is_const = is_const;
	this->type = type;
	this->nelems = nelems;
	this->ta_type = ta_type;
}
int TaskArgSpec::getNumberElements(){
	return this->nelems;
}
bool TaskArgSpec::isConst(){
	return is_const;
}
int TaskArgSpec::getSize(){
	return  sizeof(SIZEOF(type))*this->nelems;
}
task_arg_type_enum TaskArgSpec::getTaskArgType(){
	return this->ta_type;
}
type_id TaskArgSpec::getType(){
	return this->type;
}
