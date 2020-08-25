#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <inttypes.h>

#include "pt.h"

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

    uint8_t* bitmap = malloc(_HF_PERF_BITMAP_SIZE_16M);
    arch_ptAnalyze(trace, trace+trace_size, end, bitmap);
	free(trace);	

  return ret_val;
}


