#define _GNU_SOURCE
#include<stdio.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <assert.h>
#include <inttypes.h>

#include "types.h"
#include "config.h"
#include "ptdecode.h"

#include "pt_image.h"
#include "intel-pt.h"

address_range* coverage_ip_ranges;
size_t num_ip_ranges;
address_range* current_range;

u8 *trace_bits;


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

static int parse_range(const char *arg, uint64_t *begin, uint64_t *end)
{
	char *rest;

	if (!arg || !*arg)
		return 0;

	errno = 0;
	*begin = strtoull(arg, &rest, 0);
	if (errno)
		return -1;

	if (!*rest)
		return 1;

	if (*rest != '-')
		return -1;

	*end = strtoull(rest+1, &rest, 0);
	if (errno || *rest)
		return -1;

	return 2;
}

static int extract_base(char *arg, uint64_t *base)
{
	char *sep, *rest;

	sep = strrchr(arg, ':');
	if (sep) {
		uint64_t num;

		if (!sep[1])
			return 0;

		errno = 0;
		num = strtoull(sep+1, &rest, 0);
		if (errno || *rest)
			return 0;

		*base = num;
		*sep = 0;
		return 1;
	}

	return 0;
}


static int preprocess_filename(char *filename, uint64_t *offset, uint64_t *size)
{
	uint64_t begin, end;
	char *range;
	int parts;

	if (!filename || !offset || !size)
    assert(0);

	/* Search from the end as the filename may also contain ':'. */
	range = strrchr(filename, ':');
	if (!range) {
		*offset = 0ull;
		*size = 0ull;

		return 0;
	}

	/* Let's try to parse an optional range suffix.
	 *
	 * If we can, remove it from the filename argument.
	 * If we can not, assume that the ':' is part of the filename, e.g. a
	 * drive letter on Windows.
	 */
	parts = parse_range(range + 1, &begin, &end);
	if (parts <= 0) {
		*offset = 0ull;
		*size = 0ull;

		return 0;
	}

	if (parts == 1) {
		*offset = begin;
		*size = 0ull;

		*range = 0;

		return 0;
	}

	if (parts == 2) {
		if (end <= begin)
      assert(0);

		*offset = begin;
		*size = end - begin;

		*range = 0;

		return 0;
	}

  assert(0);
}

static int load_raw(struct pt_image_section_cache *iscache, struct pt_image *image, char *arg, const char *prog)
{
	uint64_t base, foffset, fsize;
	int isid, errcode, has_base;

	has_base = extract_base(arg, &base);
	if (has_base <= 0) {
    printf("SOME ERROR 0\n");
		//fprintf(stderr, "%s: failed to parse base address"
		//	"from '%s'.\n", prog, arg);
		return -1;
	}

	errcode = preprocess_filename(arg, &foffset, &fsize);
	if (errcode < 0) {
    printf("SOME ERROR 1\n");
		fprintf(stderr, "%s: bad file %s: %s.\n", prog, arg,	pt_errstr(pt_errcode(errcode)));
		return -1;
	}

	if (!fsize)
		fsize = UINT64_MAX;

	isid = pt_iscache_add_file(iscache, arg, foffset, fsize, base);
	if (isid < 0) {
    printf("SOME ERROR 2\n");
		fprintf(stderr, "%s: failed to add %s at 0x%" PRIx64 ": %s.\n", 	prog, arg, base, pt_errstr(pt_errcode(isid)));
		return -1;
	}

	errcode = pt_image_add_cached(image, iscache, isid, NULL);
	if (errcode < 0) {
    printf("SOME ERROR 3\n");
		fprintf(stderr, "%s: failed to add %s at 0x%" PRIx64 ": %s.\n", prog, arg, base, pt_errstr(pt_errcode(errcode)));
		return -1;
	}

	return 0;
}

int main(int argc, char** argv){


	uint64_t filter[4][2] = {0};
	uint64_t start, end;
	uint8_t* trace_data;
	uint64_t trace_size;
	uint64_t final_hash;

	int ret_val;

	start = strtoul(getenv("XDC_BASE"), NULL, 10);
	end = strtoul(getenv("XDC_END"), NULL, 10);
	trace_data = mapfile_read(getenv("XDC_TRACE"), &trace_size);
	char* image_name=getenv("XDC_IMAGE");

	bool skip_first_bb = false;
	int coverage_kind= COVERAGE_EDGE;

	num_ip_ranges = 1;
	coverage_ip_ranges = (address_range*)malloc(sizeof(address_range));
	coverage_ip_ranges->start = start;
	coverage_ip_ranges->end = end;
	coverage_ip_ranges->collect = true;
	current_range = coverage_ip_ranges;
	trace_bits=calloc(1, MAP_SIZE);

  	struct pt_image* image =  pt_image_alloc("foobar");
	struct  pt_image_section_cache* iscache = pt_iscache_alloc(NULL);

  	tracelet_cache_init(TRACE_CACHE_SIZE_MAX/100, TRACE_CACHE_SIZE_MAX);
	char* filename;
	assert(asprintf(&filename, "%s:0x%lx", image_name, start)>0);
	printf("loading image: %s\n", filename);
	load_raw(iscache, image, filename, argv[0]);

	analyze_trace_full_fast(trace_data, trace_size, coverage_kind, image, skip_first_bb);
	//analyze_trace_full_reference(trace_data, trace_size, coverage_kind, image, skip_first_bb);
	return 0;
}
