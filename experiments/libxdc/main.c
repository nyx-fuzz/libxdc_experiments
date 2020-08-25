#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <inttypes.h>

#include <libxdc.h>
#include "page_cache.h"

uint8_t* mapfile_read(char *fn, uint64_t *size){
	int fd = open(fn, O_RDONLY);
	if (fd < 0)
		return NULL;
	struct stat st;
	uint8_t *map = (void *)-1L;
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
	map[*size]=0x55; //End marker
	close(fd);
	return map;
}


int simple_test(uint64_t filter[4][2], uint8_t* trace, uint64_t trace_size, const char* page_cache_file, uint64_t final_hash){
	int ret_val;
	decoder_result_t ret;

	page_cache_t* page_cache =  page_cache_new(page_cache_file);
	void* bitmap = malloc(0x10000);
  	libxdc_t* decoder = libxdc_init(filter, &page_cache_fetch, page_cache, bitmap, 0x10000);
	ret = libxdc_decode(decoder, trace, trace_size);

	page_cache_destroy(page_cache);
	libxdc_free(decoder);
	free(bitmap);
	return ret_val;
}


int main(int argc, char** argv){

	uint64_t filter[4][2] = {0};
	uint64_t start, end;
	uint8_t* trace;
	uint64_t trace_size;
	uint64_t final_hash;
	
	int ret_val;

	start = strtoul(getenv("XDC_BASE"), NULL, 10);
	end = strtoul(getenv("XDC_END"), NULL, 10);
	trace = mapfile_read(getenv("XDC_TRACE"), &trace_size);
	char* image=getenv("XDC_IMAGE");
	if(!trace){
		printf("[ ] Trace file %s not found...\n",trace);
		exit(1);
	}

	printf("[*] Trace region:\t0x%lx-0x%lx\n", start, end);
	printf("[*] Code size:   \t0x%lx\n", end-start);
	printf("[*] Trace size:  \t0x%lx\n", trace_size);

	filter[0][0] = start;
	filter[0][1] = end;

	ret_val = simple_test(filter, trace, trace_size, image, final_hash);

	free(trace);	

  return ret_val;
}


