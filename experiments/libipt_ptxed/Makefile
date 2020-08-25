INCLUDE=-I../libipt/build/libipt/include/ -I../libipt/libipt/internal/include/ -I../xed/kits/xed-install-base/include/xed
LIB=-L../libipt/build/lib/ -lipt -L../xed/kits/xed-install-base/lib/ -lxed

CC=gcc
CFLAGS= -g -Ofast -fPIC -fvisibility=hidden -flto -finline-functions #-fprofile-use=program.gcda #-fprofile-generate #-g -fsanitize=address 
LDFLAGS=-flto

ODIR=build
SDIR=src

_OBJ = ptxed.o pt_cpu.o pt_cpuid.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))

default: tester

$(ODIR)/%.o: $(SDIR)/%.c
	$(CC) -c -o $@ $< $(INCLUDE) $(CFLAGS)

tester: $(OBJ) 
	$(CC) $(OBJ) $(CFLAGS) $(LDFLGAS) $(LIB) -o tester

.PHONY: clean

run: tester
	LD_LIBRARY_PATH=../../libipt/build/lib ./tester

clean:
	rm -f $(ODIR)/*.o bin/*
