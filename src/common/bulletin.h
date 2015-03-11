#ifndef BULLETIN_H
#define BULLETIN_H
/*
 * Shared Memory - allows for communication between tasks. 
 */
class SharedMemory {
	void * ptr;
	uint32_t offset;
	uint32_t size;
	uint32_t remainder;
	public:
		SharedMemory(int size);
		~SharedMemory();
		allocate(int id, int size);
};
//A circular buffer
class CircularBuffer<T> {
	typedef struct {
		int h;
		int t;
		int n;
	} circ_buf_t;
	void * base;
	circ_buf_t * meta;
	T * data;
	public:
		CircularBuffer(int n);
		size();
		load(uint32_t * base);
		create(SharedMemory& s);
		create();
		write(T v);
		T read();
};
class Buffer<T> {
	public:
		Buffer(int n);
		size();
		load(uint32_t * base);
		void create(SharedMemory & s);
		void create();
		void write(T * v);
		T * get();
		void copy(T * dest);
	
}
//A table with a particular structure. special memory mapped structure.
class Table {
	  typedef struct {
		  uint8_t rows;
		  uint8_t cols;
	  } table_t;
	  typedef struct {
		 uint8_t rid; //ID of row
		 uint8_t col;
		 uint8_t size;
		 uint8_t offset;
	  } table_mapping_t;
	  
	  void * base;
	  table_t * meta;
	  table_mapping_t * map;
	  uint32_t * tbl;
	public:
	  Table(int rows, int cols);
	  ~Table();
	  define(int rowid, int col, int size); //definitions up top.
	  size();
	  load(uint32_t * base);
	  create(SharedMemory& s); //creates a table
	  create(); //mallocs a table
	  void set(int i, int j, void * val);
	  void* get(int i, int j); // get a table entry
	  void copy(int i, int j, void * dest);
	  
};
/* 
 * A memory mapped file where the jobs are posted. These jobs exist in reliable memory.
 * This could alternatively be communication. we can use a circular buffer of a particular
 * size to keep track of the pending jobs. Each job is of static width. The memory addresses of the inputs
 * are posted. 
 */
class JobManager {
	private:
		typedef struct {
			int jid; //job id
			int tid; //task id
		} job_info_t;
		
		void * base;
		CircularBuffer<uint8_t> queue;
		Table const_data; // const data is stored here, and the reference is used.
		Table jobs; //jobs with hashes
		
	public:
	
	
};

/* Task Tracker
 * Communicates tasks with child nodes. 
 */
class TaskSpecManager {
	private:
		typedef struct {
			fxnptr function;
			uint8_t inputs;
			uint8_t outputs;
		} task_info_t;
		
		typedef enum {
			READONLY,
			CONST,
			DYNAMIC
		} io_type;
		typedef enum {
			INT,
			DOUBLE,
			FLOAT,
			CHAR
		} var_type;
		
		struct {
			uint8_t size; //n elems
			io_type type; //const or not const
			var_type vtype; //variable type.
		} io_info_t;
		
		void * base;
		Table tasks;
	
	public:
	
}
#endif
