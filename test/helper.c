#include <stdio.h>
#include <stdint.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <unistd.h>
#include <sys/mman.h>
#include <stdbool.h>
#include <stdlib.h>
#include <time.h>
#include <libxdc.h>
#include <string.h>

#include "helper.h"




/* https://github.com/andikleen/simple-pt/blob/master/map.c */
#define round_up(x, y) (((x) + (y) - 1) & ~((y) - 1))

void *mapfile(char *fn, uint64_t *size)
{
	int fd = open(fn, O_RDONLY);
	if (fd < 0)
		return NULL;
	struct stat st;
	void *map = (void *)-1L;
	if (fstat(fd, &st) >= 0) {
		*size = (uint64_t)st.st_size; /* 1 extra byte to insert PT_TRACE_END */
		map = mmap(NULL, round_up(*size, sysconf(_SC_PAGESIZE)),
			   PROT_READ|PROT_WRITE,
			   MAP_PRIVATE, fd, 0);
	}
	close(fd);

	if(map){
		/*	The mmap() function can be used to map a region of memory that is larger than the current size of the object. 
		 *	Memory access within the mapping but beyond the current end of the underlying objects may result in SIGBUS signals being sent to the process.
		 */
		void* copy = malloc(*size + 1);
		memcpy(copy, map, st.st_size);
		munmap(map, round_up(*size, sysconf(_SC_PAGESIZE)));
    ((uint8_t*)copy)[*size] = PT_TRACE_END; /* PT_TRACE_END */
		return copy;
	}
	return NULL;
}

void* mapfile_read(char *fn, uint64_t *size){
	int fd = open(fn, O_RDONLY);
	if (fd < 0)
		return NULL;
	struct stat st;
	void *map = (void *)-1L;
	if (fstat(fd, &st) >= 0) {
		*size = (uint64_t)st.st_size; /* 1 extra byte to insert PT_TRACE_END */
		map = malloc(*size+16);
		size_t readsize = 0;
		char* buf = map;
		ssize_t res = 0;
		while(res = read(fd, buf+readsize, st.st_size-readsize), res > 0){
			readsize+=res;
		}
	}
	close(fd);
	((uint8_t*)map)[*size] = PT_TRACE_END; /* PT_TRACE_END */
	return map;
}

void print_result_code(decoder_result_t result){
	switch(result){
		case decoder_success:
			printf("[*] decoder returned: " ANSI_COLOR_GREEN  "decoder_success" ANSI_COLOR_RESET "\n");
			break;
		case decoder_success_pt_overflow:
			printf("[*] decoder returned: " ANSI_COLOR_YELLOW  "decoder_success_pt_overflow" ANSI_COLOR_RESET "\n");
			break;
		case decoder_page_fault:
			printf("[*] decoder returned: " ANSI_COLOR_RED  "decoder_page_fault" ANSI_COLOR_RESET "\n");
			break;
		case decoder_error:
			printf("[*] decoder returned: " ANSI_COLOR_RED  "decoder_error" ANSI_COLOR_RESET "\n");
			break;
		case decoder_unkown_packet:
			printf("[*] decoder returned: " ANSI_COLOR_RED  "decoder_unkown_packet" ANSI_COLOR_RESET "\n");
			break;
	}

}

int handle_result(libxdc_t* decoder, decoder_result_t ret, uint64_t final_hash){
	int ret_val;
	print_result_code(ret);
	if(ret != decoder_success && ret != decoder_success_pt_overflow){
		//printf("[*] page not found:   \t0x%lx\n", libxdc_get_page_fault_addr(decoder));
		ret_val = 1;
	}
	else{
		if(final_hash != libxdc_bitmap_get_hash(decoder)){
			printf("[*] hash mismatch detected " ANSI_COLOR_RED "(%lx != %lx)!" ANSI_COLOR_RESET "\n", final_hash, libxdc_bitmap_get_hash(decoder));
			ret_val = 1;
		}
		else{
			printf("[*] hash is matching " ANSI_COLOR_GREEN  "(%lx == %lx)!" ANSI_COLOR_RESET "\n", final_hash, libxdc_bitmap_get_hash(decoder));
			ret_val = 0;
		}
	}
	return ret_val;
}

size_t get_file_size(char* filename){
	struct stat st;
	stat(filename, &st);
	return st.st_size;
}