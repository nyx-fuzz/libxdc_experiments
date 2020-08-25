/*

Copyright (C) 2020 Sergej Schumilo

This file is part of libxdc (eXtreamely fast DeCoder).

QEMU-PT is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

QEMU-PT is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with QEMU-PT.  If not, see <http://www.gnu.org/licenses/>.

*/

#include <stdio.h>
#include <libxdc.h>
#include <unistd.h>
#include <fcntl.h>
#include <time.h>
#include "page_cache.h"
#include "helper.h"

#ifdef LIBFUZZER
page_cache_t* page_cache_tmp = NULL;
void* bitmap = NULL;
libxdc_t* decoder = NULL;
int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size) {
  int ret_val;
	decoder_result_t ret;
	static int i = 0;
	uint64_t filter[4][2] = {0};
	if (!page_cache_tmp){
		filter[0][0] = 0x555555820000;
		filter[0][1] = 0x55555588A000;
		bitmap = malloc(0x10000);
		page_cache_tmp =  page_cache_new("./test_data/qemu2/page_cache");
  	decoder = libxdc_init(filter, &page_cache_fetch, page_cache_tmp, bitmap, 0x10000);
	}
	if(i++%20000 == 0){
		libxdc_reset_trace_cache(decoder);
	}
	uint8_t* trace = malloc(size +16);
	memcpy(trace, data, size);
	trace[size] = 0x55;
	trace[size+1] = 0x55;
	trace[size+2] = 0x55;
	trace[size+3] = 0x55;
	trace[size+4] = 0x55;
	trace[size+5] = 0x55;
	trace[size+6] = 0x55;
	trace[size+7] = 0x55;
	trace[size+8] = 0x55;
	trace[size+9] = 0x55;
	trace[size+10] = 0x55;
	trace[size+11] = 0x55;
	trace[size+12] = 0x55;
	trace[size+13] = 0x55;
	trace[size+14] = 0x55;
	trace[size+15] = 0x55;
	//trace[size+1] = 0x55;
	ret = libxdc_decode(decoder, trace, size);
	//ret_val = handle_result(decoder, ret, 0);
	//page_cache_destroy(page_cache);
	//libxdc_free(decoder);
	//free(bitmap);
	free(trace);
	return 0;
}
#endif

int simple_test(uint64_t filter[4][2], uint8_t* trace, uint64_t trace_size, const char* page_cache_file, uint64_t final_hash){
	int ret_val;
	decoder_result_t ret;

	page_cache_t* page_cache =  page_cache_new(page_cache_file);
	void* bitmap = malloc(0x10000);
  libxdc_t* decoder = libxdc_init(filter, &page_cache_fetch, page_cache, bitmap, 0x10000);
	ret = libxdc_decode(decoder, trace, trace_size);
	ret_val = handle_result(decoder, ret, final_hash);

	page_cache_destroy(page_cache);
	libxdc_free(decoder);
	free(bitmap);
	return ret_val;
}


int dynamic_test(uint64_t filter[4][2], uint8_t* trace, uint64_t trace_size, const char* page_cache_file, uint64_t final_hash){
	int ret_val;
	decoder_result_t ret;

	int ignore = system("rm /tmp/empty_page_cache.addr");
	ignore = system("rm /tmp/empty_page_cache.dump");
	ignore = system("touch /tmp/empty_page_cache.addr");
	ignore = system("touch /tmp/empty_page_cache.dump");

	page_cache_t* page_cache =  page_cache_new("/tmp/empty_page_cache");
	page_cache_t* page_cache_tmp =  page_cache_new(page_cache_file);
	void* bitmap = malloc(0x10000);
  libxdc_t* decoder = libxdc_init(filter, &page_cache_fetch, page_cache, bitmap, 0x10000);

	bool success = true;
	uint64_t page_ptr = 0;

	ret = libxdc_decode(decoder, trace, trace_size);
	while(ret == decoder_page_fault){
		//printf("[*] page not found:   \t0x%lx\n", libxdc_get_page_fault_addr(decoder));
		page_ptr = (uint64_t) page_cache_fetch(page_cache_tmp, libxdc_get_page_fault_addr(decoder) & 0xFFFFFFFFFFFFF000ULL, &success);
		
		libxdc_bitmap_reset(decoder);
		
		if(!success){
			printf("\n[!] " ANSI_COLOR_RED  "Page not found: 0x%lx! Aborting! " ANSI_COLOR_RESET "\n", libxdc_get_page_fault_addr(decoder));
			goto fail;
		}
		assert(success);

		append_page(page_cache, libxdc_get_page_fault_addr(decoder), (uint8_t*) page_ptr);

		libxdc_bitmap_reset(decoder);

		//printf("\r[ ] Updating page cache! Current size: %d", page_cache->num_pages);
		fflush(stdout);
		ret = libxdc_decode(decoder, trace, trace_size);
	}

	printf("\n[*] decode_buffer finished!\n");
	print_result_code(ret);
	ret_val = 0;

	fail:

	page_cache_destroy(page_cache_tmp);
	page_cache_destroy(page_cache);
	libxdc_free(decoder);
	free(bitmap);

	return ret_val;
}

/* 100 MB */
#define CHUNK_SIZE 0x6400000 

int performance_test(uint64_t filter[4][2], uint8_t* trace, uint64_t trace_size, const char* page_cache_file, uint64_t final_hash){
	int ret_val;
	decoder_result_t ret;

	page_cache_t* page_cache =  page_cache_new(page_cache_file);
	void* bitmap = malloc(0x10000);
  libxdc_t* decoder = libxdc_init(filter, &page_cache_fetch, page_cache, bitmap, 0x10000);

	double time_spent = 0;
	clock_t end_time;
	clock_t begin_time = clock();
	for(int j = 0; j < 5; j++){
		for(uint64_t i = 0; i < CHUNK_SIZE; i+= trace_size){
			ret = libxdc_decode(decoder, trace, trace_size);

			if(ret != decoder_success && ret != decoder_success_pt_overflow){
				printf("[*] decode_buffer failed\n");
				//printf("[*] page not found:   \t0x%lx\n", libxdc_get_page_fault_addr(decoder));
				goto fail;
			}
		}
		end_time = clock();
		time_spent = (double)(end_time - begin_time) / CLOCKS_PER_SEC;

		printf("Time: %f\t %f MB/sec\t %f Exec/sec\n", time_spent,  (CHUNK_SIZE/time_spent)/(1024*1024), (CHUNK_SIZE/trace_size)/time_spent);
		begin_time = clock();
	}
	print_result_code(ret);

	ret_val = 0;

	fail:

	page_cache_destroy(page_cache);
	libxdc_free(decoder);
	free(bitmap);

	return ret_val;
}

static void rq_callback(void* opaque, uint64_t start_addr, uint64_t cofi_addr){
	(*((uint64_t*)opaque))++;
}

int redqueen_test(uint64_t filter[4][2], uint8_t* trace, uint64_t trace_size, const char* page_cache_file, uint64_t final_hash){
	int ret_val;
	decoder_result_t ret;
	uint64_t rq_calls = 0;

	page_cache_t* page_cache =  page_cache_new(page_cache_file);
	void* bitmap = malloc(0x10000);
  libxdc_t* decoder = libxdc_init(filter, &page_cache_fetch, page_cache, bitmap, 0x10000);

	libxdc_register_bb_callback(decoder, rq_callback, &rq_calls);
	ret = libxdc_decode(decoder, trace, trace_size);

	ret_val = handle_result(decoder, ret, final_hash);

	printf("[!] redqueen calls: %ld\n", rq_calls);

	page_cache_destroy(page_cache);
	libxdc_free(decoder);
	free(bitmap);
	return ret_val;
}

void trace_log(void* fd, uint64_t src, uint64_t dst){
	dprintf(*(int*)fd, "%lx->%lx\n", src,dst);
	printf("%lx->%lx\n", src,dst);
}

int trace_test(uint64_t filter[4][2], uint8_t* trace, uint64_t trace_size, const char* page_cache_file, uint64_t final_hash){
	int ret_val;
	decoder_result_t ret;
	int fd;
	
	int ignore = system("rm -f /tmp/decoder_temp_trace_file");
	fd = open("/tmp/decoder_temp_trace_file", O_CREAT | O_TRUNC | O_WRONLY, S_IRWXU);

	page_cache_t* page_cache =  page_cache_new(page_cache_file);
	void* bitmap = malloc(0x10000);
  	libxdc_t* decoder = libxdc_init(filter, &page_cache_fetch, page_cache, bitmap, 0x10000);
	libxdc_enable_tracing(decoder);
	libxdc_register_edge_callback(decoder, &trace_log, &fd);
	ret = libxdc_decode(decoder, trace, trace_size);
	libxdc_disable_tracing(decoder);
	close(fd);
	
	ret_val = handle_result(decoder, ret, final_hash);
	printf("[!] trace file size: %ld\n", get_file_size("/tmp/decoder_temp_trace_file"));


	page_cache_destroy(page_cache);
	libxdc_free(decoder);
	free(bitmap);
	return ret_val;
}


#ifndef LIBFUZZER
int main(int argc, char** argv){

	uint64_t filter[4][2] = {0};
	uint64_t start, end;
	uint8_t* trace;
	uint64_t trace_size;
	uint64_t final_hash;
	
	int ret_val;

	if (argc != 7){
		printf("Usage: %s <ip_start> <ip_end> <page_cache> <trace_data> <hash> <MODE> (MODE: simple/dynamic/performance)\n", argv[0]);
		printf("[ ] Aborting...\n");
		return 1;
	}

	printf("[*] Loading files...\n");

	start = strtoul(argv[1], NULL, 16);
	end = strtoul(argv[2], NULL, 16);
	trace = mapfile_read(argv[4], &trace_size);
	final_hash = (uint64_t)strtoull(argv[5], NULL, 16);

	if(!trace){
		printf("[ ] Trace file not found...\n");
		exit(1);
	}

	printf("[*] Trace region:\t0x%lx-0x%lx\n", start, end);
	printf("[*] Code size:   \t0x%lx\n", end-start);
	printf("[*] Trace size:  \t0x%lx\n", trace_size);

	filter[0][0] = start;
	filter[0][1] = end;

	if(!strcmp(argv[6], "simple")){
		ret_val = simple_test(filter, trace, trace_size, argv[3], final_hash);
	}
	else if(!strcmp(argv[6], "dynamic")){
		ret_val = dynamic_test(filter, trace, trace_size, argv[3], final_hash);
	}
	else if(!strcmp(argv[6], "performance")){
		ret_val = performance_test(filter, trace, trace_size, argv[3], final_hash);
	}
	else if(!strcmp(argv[6], "redqueen")){
		ret_val = redqueen_test(filter, trace, trace_size, argv[3], final_hash);
	}
	else if(!strcmp(argv[6], "trace")){
		ret_val = trace_test(filter, trace, trace_size, argv[3], final_hash);
	}
	else{
		printf("[ ] Invalid mode (%s)...\n", argv[6]);
		exit(1);
	}

	free(trace);	

  return ret_val;
}
#endif
