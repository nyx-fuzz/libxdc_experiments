IDIR =-I../../../libxdc/
CC=gcc
CFLAGS= -Ofast
LDFLAGS=-flto

tester: ../../../libxdc/build/libxdc.a ../../../libxdc/libxdc.h main.c page_cache.c page_cache.h khash.h
	$(CC) $(CFLAGS) $(LDFLAGS) $(IDIR) main.c page_cache.c ../../../libxdc/build/libxdc.a -l:libcapstone.so.4  -o tester
