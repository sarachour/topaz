#include "bulletin.h"
#include <unistd.h>
#include "fcntl.h"
#include <sys/types.h>
#include <sys/mman.h>

SharedMemory::SharedMemory(const char * name, int size){
	size_t page_size;
	if((fd = open(name, O_RDWR | O_CREAT | O_TRUNC, (mode_t) 0770)) < 0){
		panic("Could not create file");
	}
	page_size = (size_t) sysconf(_SC_PAGESIZE);
	size = (size + page_size)/page_size*page_size;
	remainder = size - sizeof(shared_memory_entry_t);
	offset = 1;
	
	uint32_t res = lseek(fd, size, SEEK_SET);
	if(res == -1){
		close(fd);
		panic("Error calling lseek to stretch the file");
	}
	res = write(fd,"",1);
	if(res == -1){
		close(fd);
		panic("Error calling writing to end of file.");
	}
	ptr = (uint8_t *)  mmap(0, size, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
	ptr[0] = 0;
}
SharedMemory::~SharedMemory(){
	close(fd);
	ptr = NULL;
}
uint8_t * SharedMemory::allocate(int id, uint32_t size){
	shared_memory_entry_t e;
	shared_memory_entry_t * optr = (reinterpret_cast<shared_memory_entry_t *>(ptr+offset));
	uint8_t * dptr = reinterpret_cast<uint8_t *>(optr+1);
	e.size = size;
	e.id = id;
	
	
	if(remainder < size){
		panic("ERROR: shared memory ran out of space.");
	}
	optr[0] = e;
	uint32_t amt = e.size + sizeof(shared_memory_entry_t);
	offset += amt;
	remainder -= amt; 
	ptr[0] += 1;
	//return value
	return dptr;
}
uint8_t * SharedMemory::get(int id, uint32_t& size){
	uint8_t nallocs = ptr[0];
	uint8_t * dptr = ptr+1;
	
	size=-1;
	for(int i=0; i < nallocs; i++){
		shared_memory_entry_t m = (reinterpret_cast<shared_memory_entry_t*> (dptr))[0];
		dptr += sizeof(shared_memory_entry_t);
		if(m.id == id){
			size = m.size;
			return dptr;
		}
		dptr += m.size;
	}
	return NULL;
}
