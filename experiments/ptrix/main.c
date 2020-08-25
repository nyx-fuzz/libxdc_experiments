#include <stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <inttypes.h>

#include "pt_parser_fast.h"

#define MAP_SIZE_POW2       16
#define PT_MAP_SIZE_POW2    (MAP_SIZE_POW2 - 0)
#define MAP_SIZE            (1 << MAP_SIZE_POW2)
#define RAND_MAP_SIZE 1<<16               /* size of the rand_map            */

u8  __afl_pt_fav_initial[MAP_SIZE];                 /* pt_fav_bits map(1/8 of original) */
u8 *__afl_pt_fav_ptr;
u8* __afl_pt_fav_ptr = __afl_pt_fav_initial;

u8  __afl_area_initial[MAP_SIZE];                      /* trace_bits map                  */
u8* __afl_area_ptr = __afl_area_initial;
u64 rand_map[RAND_MAP_SIZE];                           /* maps u8 val to random value UR()*/


/* decode context, needs to be preserved between any two runs of the parsing function     */
u64 ctx_curr_ip;
u64 ctx_last_ip;
u64 ctx_last_tip_ip;
u64 ctx_tnt_long;
u32 ctx_bit_selector;
u32 ctx_tnt_counter;
u64 ctx_curr_tnt_prod;
u64 ctx_tnt_container;
u8  ctx_tnt_short;
u8  ctx_tnt_go;
u8  ctx_tnt_lock;
u8  ctx_tip_counter;
u8  ctx_curr_tnt_cnt;


static inline u32 UR(u32 limit) {
    return random() % limit;
}

void gen_rand_map(u32 entries, u32 max){
      if(sizeof(rand_map)/sizeof(u64) < entries)
          assert(0);
      for (int i=0; i<entries; ++i){
          rand_map[i] = UR(max); 
      }
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
	return map;
}

int main(int argc, char** argv){

  gen_rand_map(RAND_MAP_SIZE, MAP_SIZE);
  printf("foo\n");

  	uint64_t filter[4][2] = {0};
	uint64_t start, end;
	uint8_t* trace_data;
	uint64_t trace_size;
	uint64_t final_hash;
	
	int ret_val;

	start = strtoul(getenv("XDC_BASE"), NULL, 10);
	end = strtoul(getenv("XDC_END"), NULL, 10);
	trace_data = mapfile_read(getenv("XDC_TRACE"), &trace_size);
	char* image=getenv("XDC_IMAGE");
	if(!trace_data){
		printf("[ ] Trace file %s not found...\n",trace_data);
		exit(1);
	}

	printf("[*] Trace region:\t0x%lx-0x%lx\n", start, end);
	printf("[*] Code size:   \t0x%lx\n", end-start);
	printf("[*] Trace size:  \t0x%lx\n", trace_size);

	filter[0][0] = start;
	filter[0][1] = end;


  pt_parse_packet(trace_data, trace_size, 1, 0);
  return 0;
}
