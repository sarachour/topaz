#include "logger.h"
#include "topaz.h"
#include "float.h"

void __init_task_log_info(task_log_info * t){
	t->rel_in_idx = -1;
	t->rel_out_idx = -1;
	t->urel_out_idx = -1;
	t->log_enable = false;
}
entry_type __arg_type_to_entry_type(task_arg_type argtype){
	switch(argtype){
		case TASK_ARG_TYPE_CHAR: return ENTRY_TYPE_CHAR;
		case TASK_ARG_TYPE_DOUBLE: return ENTRY_TYPE_DOUBLE;
		case TASK_ARG_TYPE_FLOAT: return ENTRY_TYPE_FLOAT;
		case TASK_ARG_TYPE_INT: return ENTRY_TYPE_INT;
		case TASK_ARG_TYPE_PTR: return ENTRY_TYPE_PTR;
	}
}

void log_init_task(task_info * t){
	int i;
	__init_task_log_info(&t->rank_log_meta);
	__init_task_log_info(&t->id_log_meta);
	for(i=0; i < t->in_nargs; i++){
		__init_task_log_info(&t->inputs[i].arg_log_meta);
	}
	for(i=0; i < t->out_nargs; i++){
		__init_task_log_info(&t->outputs[i].arg_log_meta);
	}
}

void log_task_input(task_info * info, int arg_id, const char * handle){
	task_info_arg * arginfo = &info->inputs[arg_id];
	char name[255];
	entry_type entrytype = __arg_type_to_entry_type(arginfo->type);
	arginfo->arg_log_meta.log_enable = true;
	sprintf(name, "in.%s", handle);
	arginfo->arg_log_meta.rel_in_idx = log_add_entry(&info->log, name, entrytype, arginfo->size);
	arginfo->arg_log_meta.rel_out_idx = -1;
	arginfo->arg_log_meta.urel_out_idx = -1;
}
void log_task_output(task_info * info, int arg_id, const char * handle){
	task_info_arg * arginfo = &info->outputs[arg_id];
	char name[255];
	entry_type entrytype = __arg_type_to_entry_type(arginfo->type);
	arginfo->arg_log_meta.log_enable = true;
	sprintf(name, "out.%s.rel", handle);
	arginfo->arg_log_meta.rel_out_idx = log_add_entry(&info->log, name, entrytype, arginfo->size);
	sprintf(name, "out.%s.urel", handle);
	arginfo->arg_log_meta.urel_out_idx = log_add_entry(&info->log, name, entrytype, arginfo->size);
	arginfo->arg_log_meta.rel_in_idx = -1;
}

void log_task(task_info * info, const char * handle){
	char name[255];
	int maxnlogs = (info->in_nargs + info->out_nargs*2)+6;
	sprintf(name, "%s.log", handle);
	log_init_info(&info->log, name, maxnlogs);

	info->rank_log_meta.log_enable = true;
	info->id_log_meta.log_enable = true;
	info->rank_log_meta.rel_in_idx = log_add_entry(&info->log, "task.rank.in.rel", ENTRY_TYPE_INT, 1);
	info->rank_log_meta.rel_out_idx  = log_add_entry(&info->log, "task.rank.out.rel", ENTRY_TYPE_INT, 1);
	info->rank_log_meta.urel_out_idx  = log_add_entry(&info->log, "task.rank.out.urel", ENTRY_TYPE_INT, 1);
	
	info->id_log_meta.rel_in_idx = log_add_entry(&info->log, "task.id.in.rel", ENTRY_TYPE_INT, 1);
	info->id_log_meta.rel_out_idx  = log_add_entry(&info->log, "task.id.out.rel", ENTRY_TYPE_INT, 1);
	info->id_log_meta.urel_out_idx  = log_add_entry(&info->log, "task.id.out.urel", ENTRY_TYPE_INT, 1);

}

inline int get_log_idx(task_log_info info, int is_input, int is_reliable){
	return (is_input == true ? info.rel_in_idx :
			(is_reliable == true ? info.rel_out_idx : info.urel_out_idx)
		   );
}
void log_task_message(task_info * info, task_message * msg, int is_input, int is_reliable){
	int i; int task_idx_log;
	task_data_accessor pos;
	task_info_arg * arglist; int nargs;
	arglist= is_input == true ? info->inputs : info->outputs;
	nargs = is_input == true ? info->in_nargs : info->out_nargs;
	if(info->rank_log_meta.log_enable) 
		log_add_entry_line(&info->log, get_log_idx(info->rank_log_meta, is_input, is_reliable), &msg->task_rank);
	if(info->id_log_meta.log_enable) 
		log_add_entry_line(&info->log, get_log_idx(info->id_log_meta, is_input, is_reliable), &msg->task_id);
	task_unpack_task(msg, &pos);
	for(i=0; i < nargs; i++){
		char arg_en_log = arglist[i].arg_log_meta.log_enable;
		int arg_idx_log = get_log_idx(arglist[i].arg_log_meta, is_input, is_reliable);
		int arg_type = arglist[i].type;
		int arg_size = arglist[i].size;
		switch(arg_type){
			case TASK_ARG_TYPE_INT:
				if(arg_size == 1){
					int tmp = task_unpack_int(&pos);
					if(arg_en_log == true){
						log_add_entry_line(&info->log, arg_idx_log, &tmp);
					}
				}
				else{
					int * tmp; task_quick_unpack_int_array(&pos, &tmp);
					if(arg_en_log == true){
						log_add_entry_line(&info->log, arg_idx_log, tmp);
					}
				}
				break;
			case TASK_ARG_TYPE_CHAR:
				if(arg_size == 1){
					char tmp = task_unpack_char(&pos);
					if(arg_en_log == true){
						log_add_entry_line(&info->log, arg_idx_log, &tmp);
					}
				}
				else{
					char * tmp; task_quick_unpack_char_array(&pos, &tmp);
					if(arg_en_log == true){
						log_add_entry_line(&info->log, arg_idx_log, tmp);
					}
				}
				break;
			case TASK_ARG_TYPE_DOUBLE:
				if(arg_size == 1){
					double tmp = task_unpack_double(&pos);
					if(arg_en_log == true){
						log_add_entry_line(&info->log, arg_idx_log, &tmp);
					}
				}
				else{
					double * tmp; task_quick_unpack_double_array(&pos, &tmp);
					if(arg_en_log == true){
						log_add_entry_line(&info->log, arg_idx_log, tmp);
					}
				}
				break;
			case TASK_ARG_TYPE_FLOAT:
				if(arg_size == 1){
					float tmp = task_unpack_float(&pos);
					if(arg_en_log == true){
						log_add_entry_line(&info->log, arg_idx_log, &tmp);
					}
				}
				else{
					float * tmp; task_quick_unpack_float_array(&pos, &tmp);
					if(arg_en_log == true){
						log_add_entry_line(&info->log, arg_idx_log, tmp);
					}
				}
				break;
			case TASK_ARG_TYPE_PTR:
				if(arg_size == 1){
					topaz_ptr tmp = task_unpack_ptr(&pos);
					if(arg_en_log == true){
						log_add_entry_line(&info->log, arg_idx_log, &tmp);
					}
				}
				else{
					topaz_ptr * tmp; task_quick_unpack_ptr_array(&pos, &tmp);
					if(arg_en_log == true){
						log_add_entry_line(&info->log, arg_idx_log, tmp);
					}
				}
				break;
			default:
				printf("LOGGING UNIMPLEMENTED %d %d\n", arg_type, arg_size);
				break;
		}
		
	}
}


void log_task_learning(task_info * info, task_message * msg, task_message * rmsg, int place){
	int i; int task_idx_log;
	task_data_accessor pos;
	task_data_accessor rpos;
	task_info_arg * arglist; int nargs;
	arglist= info->outputs;
	nargs = info->out_nargs;
	if(info->rank_log_meta.log_enable) 
		log_add_entry_line(&info->log, get_log_idx(info->rank_log_meta, false, true), &msg->task_rank);
	if(info->id_log_meta.log_enable) 
		log_add_entry_line(&info->log, get_log_idx(info->id_log_meta, false, true), &place);
	task_unpack_task(msg, &pos);
	task_unpack_task(rmsg, &rpos);
	for(i=0; i < nargs; i++){
		char arg_en_log = arglist[i].arg_log_meta.log_enable;
		int arg_idx_log = get_log_idx(arglist[i].arg_log_meta, false, false);
		int arg_idx_log_r = get_log_idx(arglist[i].arg_log_meta, false, true);
		int arg_type = arglist[i].type;
		int arg_size = arglist[i].size;
		switch(arg_type){
			case TASK_ARG_TYPE_INT:
				if(arg_size == 1){
					int tmp = task_unpack_int(&pos);
					int rtmp = task_unpack_int(&rpos);
					if(arg_en_log == true && tmp != rtmp){
						log_add_entry_line(&info->log, arg_idx_log, &tmp);
						log_add_entry_line(&info->log, arg_idx_log_r, &rtmp);
					}
				}
				else{
					int * tmp; task_quick_unpack_int_array(&pos, &tmp);
					int * rtmp; task_quick_unpack_int_array(&rpos, &rtmp);
					int j; int same = true;
					for(j=0; j < arg_size; j++){
						if(tmp[j] != rtmp[j]) same = false;
					}
					if(arg_en_log == true && !same){
						log_add_entry_line(&info->log, arg_idx_log, tmp);
						log_add_entry_line(&info->log, arg_idx_log_r, rtmp);
					}
				}
				break;
			case TASK_ARG_TYPE_CHAR:
				if(arg_size == 1){
					char tmp = task_unpack_char(&pos);
					char rtmp = task_unpack_char(&rpos);
					
					if(arg_en_log == true && tmp != rtmp){
						log_add_entry_line(&info->log, arg_idx_log, &tmp);
						log_add_entry_line(&info->log, arg_idx_log_r, &rtmp);
					}
				}
				else{
					char * tmp; task_quick_unpack_char_array(&pos, &tmp);
					char * rtmp; task_quick_unpack_char_array(&rpos, &rtmp);
					int j; int same = true;
					for(j=0; j < arg_size; j++){
						if(tmp[j] != rtmp[j]) same = false;
					}
					if(arg_en_log == true && !same){
						log_add_entry_line(&info->log, arg_idx_log, tmp);
						log_add_entry_line(&info->log, arg_idx_log_r, rtmp);
					}
				}
				break;
			#define same_dbl(a,b) (abs(a-b) < 1E-299)
			
			case TASK_ARG_TYPE_DOUBLE:
				if(arg_size == 1){
					double tmp = (task_unpack_double(&pos));
					double rtmp = (task_unpack_double(&rpos));
					//printf("%e =? %e\n", tmp, rtmp);
					if(arg_en_log == true && rtmp != tmp){
						//printf("logging\n");
						log_add_entry_line(&info->log, arg_idx_log, &tmp);
						log_add_entry_line(&info->log, arg_idx_log_r, &rtmp);
					}
				}
				else{
					double * tmp; task_quick_unpack_double_array(&pos, &tmp);
					double * rtmp; task_quick_unpack_double_array(&rpos, &rtmp);
					int j; int same = true;
					for(j=0; j < arg_size; j++){
						if(tmp[j] != rtmp[j]) same = false;
					}
					if(arg_en_log == true && !same){
						//printf("logging\n");
						log_add_entry_line(&info->log, arg_idx_log, tmp);
						log_add_entry_line(&info->log, arg_idx_log_r, rtmp);
					}
				}
				break;
			case TASK_ARG_TYPE_FLOAT:
				if(arg_size == 1){
					float tmp = (task_unpack_float(&pos));
					float rtmp = (task_unpack_float(&rpos));
					//printf("%e =? %e\n", tmp, rtmp);
					if(arg_en_log == true && rtmp != tmp){
						//printf("logging\n");
						log_add_entry_line(&info->log, arg_idx_log, &tmp);
						log_add_entry_line(&info->log, arg_idx_log_r, &rtmp);
					}
				}
				else{
					float * tmp; task_quick_unpack_float_array(&pos, &tmp);
					float * rtmp; task_quick_unpack_float_array(&rpos, &rtmp);
					int j; int same = true;
					for(j=0; j < arg_size; j++){
						if(tmp[j] != rtmp[j]) same = false;
					}
					if(arg_en_log == true && !same){
						//printf("logging\n");
						log_add_entry_line(&info->log, arg_idx_log, tmp);
						log_add_entry_line(&info->log, arg_idx_log_r, rtmp);
					}
				}
				break;
			case TASK_ARG_TYPE_PTR:
				if(arg_size == 1){
					topaz_ptr tmp = task_unpack_ptr(&pos);
					topaz_ptr rtmp = task_unpack_ptr(&rpos);
					if(arg_en_log == true && (tmp == rtmp)){
						log_add_entry_line(&info->log, arg_idx_log, &tmp);
						log_add_entry_line(&info->log, arg_idx_log_r, &rtmp);
					}
				}
				else{
					topaz_ptr * tmp; task_quick_unpack_ptr_array(&pos, &tmp);
					topaz_ptr * rtmp; task_quick_unpack_ptr_array(&pos, &rtmp);
					int j; int same = true;
					for(j=0; j < arg_size; j++){
						if((tmp != rtmp)) same = false;
					}
					if(arg_en_log == true){
						log_add_entry_line(&info->log, arg_idx_log, tmp);
						log_add_entry_line(&info->log, arg_idx_log_r, rtmp);
					}
				}
				break;
			default:
				//printf("LOGGING UNIMPLEMENTED %d %d\n", arg_type, arg_size);
				break;
		}
		
	}
}
