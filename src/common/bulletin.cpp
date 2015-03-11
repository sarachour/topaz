#include "bulletin.h"

SharedMemory::SharedMemory(int size){
	size_t page_size;
	page_size = (size_t) sysconf(_SC_PAGESIZE);
	size = (size + page_size)/page_size;
	ptr = mmap(NULL, size, 
			PROT_READ | PROT_WRITE,
			MAP_SHARED | MAP_ANONYMOUS,
			0,
			0);
}
