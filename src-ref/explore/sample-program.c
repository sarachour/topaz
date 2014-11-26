

void sum_block(task_input_info_t * in, task_output_info_t * out){
	int * d;
	int m;
	int i=0;
	int lsum=0;
	unpack_int_array(in->data, 0, &d, &m);
	int(i=0; i < m; i++){
		lsum += d[i];
	}
	pack_int(out->data, 0, lsum);
}
/*compute matrix sum*/
topaz_main(topaz_info_t topaz){
	task_input_info_t in;
	task_output_info_t out;
	int off;
	int i=0;
	
	int ** arr;
	int task_id = add_task(&sum_block);
	int n, m;
	int sum=0; 
	/*
	 * create a 2d array arr with rows n and cols m
	*/
	for(i=0; i < n; i++){
		int off;
		init_input_info(&in, task_id, i);
		pack_int_array(&in.data, 0, arr[i], m);
		dispatch_task(&topaz, &in);
	}
	while(tasks_left()){
		collect_task(&topaz, &out, NULL);
		int lsum;
		unpack_int(out.data,0,&lsum);
		sum += lsum;
	}
	printf("%d\n", sum);
}

