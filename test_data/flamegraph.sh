LD_LIBRARY_PATH=../bin/ perf record -F 1000000 -g -- ../bin/tester 0xffffffff83a00000 0xffffffff83a26160 ./kafl_eval/data.pagecache ./kafl_eval/sample1_1x 0x0 performance
perf script | ~/proggen/source/FlameGraph/stackcollapse-perf.pl > out.perf-folded
~/proggen/source/FlameGraph/flamegraph.pl out.perf-folded > perf-kernel.svg
firefox perf-kernel.svg
