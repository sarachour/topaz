#ifndef BULLETIN_H
#define BULLETIN_H

#include "stdio.h"
#include "stdint.h"
#include "util.h"


/*
 * Shared Memory - allows for communication between tasks. 
 */
class SharedMemory {
	typedef struct {
		uint8_t id;
		uint32_t size;
	} shared_memory_entry_t;
	uint8_t * ptr;
	uint32_t offset;
	uint32_t size;
	uint32_t remainder;
	int fd;
	public:
		SharedMemory(const char * name, int size);
		~SharedMemory();
		uint8_t * allocate(int id, uint32_t size);
		uint8_t * get(int id, uint32_t& size);
};
//A circular buffer
template <class T>
class CircularBuffer {
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
		int size();
		void load(void * base);
		void create(void * base);
		void create();
		void write(T v);
		T read();
};

template <class T>
class Buffer {
	public:
		Buffer(int n);
		int size();
		void load(void * base);
		void create(void * base);
		void create();
		void write(T * v);
		T * get();
		void copy(T * dest);
	
};
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
	  void define(int rowid, int col, int size); //definitions up top.
	  int size();
	  void load(void * base);
	  void create(void * base); //creates a table
	  void create(); //mallocs a table
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
		typedef enum {
			PENDING,
			RUNNING,
			DONE,
			EMPTY=0
		} job_status_t;
		
		typedef struct {
			job_status_t status;
			int jid; //job id
			int tid; //task id
		} job_info_t;
		
		void * base;
		CircularBuffer<uint8_t> queue;
		Buffer<void> * const_data; // const data is stored here, and the reference is used.
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
		TaskSpecManager();
};

#endif
