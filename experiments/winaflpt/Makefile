
INCLUDE=-I../libipt/build/libipt/include/ -I../libipt/libipt/internal/include/
LIB=-L../libipt/build/lib/ -lipt
CFLAGS="-ggdb -Ofast -march=native"


IDIR =../include
CC=gcc
CFLAGS= -g -Ofast -fPIC -fvisibility=hidden -flto -finline-functions #-fprofile-use=program.gcda #-fprofile-generate #-g -fsanitize=address 
LDFLAGS=-flto

ODIR=build
SDIR=src
LDIR =bin

_OBJ = ptdecode.o pt_cpu.o pt_cpuid.o main.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

default: tester

$(ODIR)/%.o: %.c *.h 
	$(CC) -c -o $@ $< $(INCLUDE) $(CFLAGS)

tester: $(OBJ) *.h
	$(CC) $(OBJ) $(CFLAGS) $(LDFLGAS) $(LIB) -o tester

.PHONY: clean

run: tester
	LD_LIBRARY_PATH=../libipt/build/lib ./tester

clean:
	rm -f $(ODIR)/*.o bin/*
