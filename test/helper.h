#pragma once

#define ANSI_COLOR_RED     "\x1b[31m"
#define ANSI_COLOR_GREEN   "\x1b[32m"
#define ANSI_COLOR_YELLOW  "\x1b[33m"
#define ANSI_COLOR_BLUE    "\x1b[34m"
#define ANSI_COLOR_MAGENTA "\x1b[35m"
#define ANSI_COLOR_CYAN    "\x1b[36m"
#define ANSI_COLOR_RESET   "\x1b[0m"

void *mapfile(char *fn, uint64_t *size);
void *mapfile_read(char *fn, uint64_t *size);
void print_result_code(decoder_result_t result);
int handle_result(libxdc_t* decoder, decoder_result_t ret, uint64_t final_hash);

size_t get_file_size(char* filename);