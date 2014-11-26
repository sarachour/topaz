

void sum_block(task_info_t * in, pack_info * pi, task_info_t * out, pack_info * po){
	int * d;
	int m;
	int i=0;
	int lsum=0;
	unpack_int_array(in, pi, &d, &m);
	int(i=0; i < m; i++){
		lsum += d[i];
	}
	pack_int(out, po, lsum);
}
/*compute matrix sum*/

void shared_obj_ex(){
	
	
}
topaz_main(topaz_info_t topaz, int argc, char ** argv){
	
	int off;
	int i=0;
	
	int ** arr;
	int task_id = add_task(&sum_block);
	int n, m;
	int sum=0; 
	pack_info p;
	/*
	 * create a 2d array arr with rows n and cols m
	*/
	task_info_t in = create_task_info(m);
	task_info_t out = create_task_info(1);
	for(i=0; i < n; i++){
		int off;
		init_input_info(&in, task_id, i);
		init_pack_info(&p);
		pack_int_array(&in, &p, arr[i], m);
		dispatch_task(&topaz, &in, &p);
	}
	while(tasks_left()){
		collect_task(&topaz, &out, NULL);
		int lsum;
		unpack_int(out.data,0,&lsum);
		sum += lsum;
	}
	printf("%d\n", sum);
}

/*
 * tasks(i=1:n)
 * task IN(arr[i] as p)
 * 
 * 
 * 
 */
