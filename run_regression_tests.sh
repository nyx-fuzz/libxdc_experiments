

echo "===== SIMPLE TESTS ======"
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0x400000 0x427000  test_data/Unzip/data test_data/Unzip/trace 0x15b89190dfa291b2 simple 
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 7ffff7819000 7ffff79ca000 test_data/foo/data test_data/foo/trace 0xa76b8b44c4d3fe97 simple 
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0xffffffff81000000 ffffffff81e53ba0 test_data/Kernel/data test_data/Kernel/trace 0xd367d17ff52a23be simple 
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 00400000 4b6000 test_data/mruby/data test_data/mruby/trace 0x988aae1ab68920a4 simple 
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0xffffffff83a00000 0xffffffff83a26160 test_data/kafl_eval/data.pagecache test_data/kafl_eval/sample1_1x 0x235a1f04d22a003b simple 
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0xffffffff83a00000 0xffffffff83a26160 test_data/kafl_eval/data.pagecache test_data/kafl_eval/sample1_1x_sanx 0x235a1f04d22a003b simple 
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0x000555555554000 0x00555555d60000 test_data/qemu/page_cache test_data/qemu/trace1 0x22b4b12103609fd2 simple 
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0xffffffff81000000 0xffffffff81e53ba0 test_data/infinite_loop_linux/page_cache test_data/infinite_loop_linux/loop_trace_26023_0 0x8aac0cd463e0e675 simple 
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0x1000 0x7ffffffff000 test_data/dyn_test/page_cache test_data/dyn_test/sample_raw_0 0x0 simple 
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0x1000 0x7ffffffff000 test_data/avscript_32/page_cache test_data/avscript_32/failed_trace_12709_1 0x22792c81fccf4f70 simple 
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0x1000 0xfffffffffffff000 test_data/icelake_tip/pagecache test_data/icelake_tip/ptlog eb2a5b786860a79f simple

echo "===== DYNAMIC TESTS ======"
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0x400000 0x427000  test_data/Unzip/data test_data/Unzip/trace 0x15b89190dfa291b2 dynamic
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 7ffff7819000 7ffff79ca000 test_data/foo/data test_data/foo/trace 0xa76b8b44c4d3fe97 dynamic
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0xffffffff81000000 ffffffff81e53ba0 test_data/Kernel/data test_data/Kernel/trace 0xd367d17ff52a23be dynamic
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 00400000 4b6000 test_data/mruby/data test_data/mruby/trace 0x988aae1ab68920a4 dynamic
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0xffffffff83a00000 0xffffffff83a26160 test_data/kafl_eval/data.pagecache test_data/kafl_eval/sample1_1x 0x235a1f04d22a003b dynamic
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0xffffffff83a00000 0xffffffff83a26160 test_data/kafl_eval/data.pagecache test_data/kafl_eval/sample1_1x_sanx 0x235a1f04d22a003b dynamic
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0x000555555554000 0x00555555d60000 test_data/qemu/page_cache test_data/qemu/trace1 0x22b4b12103609fd2 dynamic
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0xffffffff81000000 0xffffffff81e53ba0 test_data/infinite_loop_linux/page_cache test_data/infinite_loop_linux/loop_trace_26023_0 0x8aac0cd463e0e675 dynamic
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0x1000 0x7ffffffff000 test_data/dyn_test/page_cache test_data/dyn_test/sample_raw_0 0x0 dynamic
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0x1000 0x7ffffffff000 test_data/avscript_32/page_cache test_data/avscript_32/failed_trace_12709_1 0x22792c81fccf4f70 dynamic 
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0x1000 0xfffffffffffff000 test_data/icelake_tip/pagecache test_data/icelake_tip/ptlog eb2a5b786860a79f dynamic

echo "===== REDQUEEN TESTS ======"
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0x400000 0x427000  test_data/Unzip/data test_data/Unzip/trace 0x15b89190dfa291b2 redqueen | grep redqueen 
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 7ffff7819000 7ffff79ca000 test_data/foo/data test_data/foo/trace 0xa76b8b44c4d3fe97 redqueen | grep redqueen  
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0xffffffff81000000 ffffffff81e53ba0 test_data/Kernel/data test_data/Kernel/trace 0xd367d17ff52a23be redqueen | grep redqueen  
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 00400000 4b6000 test_data/mruby/data test_data/mruby/trace 0x988aae1ab68920a4 redqueen | grep redqueen  
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0xffffffff83a00000 0xffffffff83a26160 test_data/kafl_eval/data.pagecache test_data/kafl_eval/sample1_1x 0x235a1f04d22a003b redqueen | grep redqueen  
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0xffffffff83a00000 0xffffffff83a26160 test_data/kafl_eval/data.pagecache test_data/kafl_eval/sample1_1x_sanx 0x235a1f04d22a003b redqueen | grep redqueen  
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0x000555555554000 0x00555555d60000 test_data/qemu/page_cache test_data/qemu/trace1 0x22b4b12103609fd2 redqueen | grep redqueen  
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0xffffffff81000000 0xffffffff81e53ba0 test_data/infinite_loop_linux/page_cache test_data/infinite_loop_linux/loop_trace_26023_0 0x8aac0cd463e0e675 redqueen | grep redqueen  
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0x1000 0x7ffffffff000 test_data/dyn_test/page_cache test_data/dyn_test/sample_raw_0 0x0 redqueen | grep redqueen  
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0x1000 0x7ffffffff000 test_data/avscript_32/page_cache test_data/avscript_32/failed_trace_12709_1 0x22792c81fccf4f70 redqueen | grep redqueen   
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0x1000 0xfffffffffffff000 test_data/icelake_tip/pagecache test_data/icelake_tip/ptlog eb2a5b786860a79f redqueen | grep redqueen   

echo "===== TRACE TESTS ======"
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0x400000 0x427000  test_data/Unzip/data test_data/Unzip/trace 0x15b89190dfa291b2 trace | grep "trace file" 
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 7ffff7819000 7ffff79ca000 test_data/foo/data test_data/foo/trace 0xa76b8b44c4d3fe97 trace | grep "trace file" 
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0xffffffff81000000 ffffffff81e53ba0 test_data/Kernel/data test_data/Kernel/trace 0xd367d17ff52a23be trace | grep "trace file"  
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 00400000 4b6000 test_data/mruby/data test_data/mruby/trace 0x988aae1ab68920a4 trace | grep "trace file" 
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0xffffffff83a00000 0xffffffff83a26160 test_data/kafl_eval/data.pagecache test_data/kafl_eval/sample1_1x 0x235a1f04d22a003b trace | grep "trace file"  
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0xffffffff83a00000 0xffffffff83a26160 test_data/kafl_eval/data.pagecache test_data/kafl_eval/sample1_1x_sanx 0x235a1f04d22a003b trace | grep "trace file" 
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0x000555555554000 0x00555555d60000 test_data/qemu/page_cache test_data/qemu/trace1 0x22b4b12103609fd2 trace | grep "trace file"  
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0xffffffff81000000 0xffffffff81e53ba0 test_data/infinite_loop_linux/page_cache test_data/infinite_loop_linux/loop_trace_26023_0 0x8aac0cd463e0e675 trace | grep "trace file" 
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0x1000 0x7ffffffff000 test_data/dyn_test/page_cache test_data/dyn_test/sample_raw_0 0x0 trace | grep "trace file" 
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0x1000 0x7ffffffff000 test_data/avscript_32/page_cache test_data/avscript_32/failed_trace_12709_1 0x22792c81fccf4f70 trace | grep "trace file"  
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0x1000 0xfffffffffffff000 test_data/icelake_tip/pagecache test_data/icelake_tip/ptlog eb2a5b786860a79f trace | grep "trace file"  

echo "===== PERFORMANCE TESTS ======"
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0x400000 0x427000  test_data/Unzip/data test_data/Unzip/trace 0x0 performance
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 7ffff7819000 7ffff79ca000 test_data/foo/data test_data/foo/trace 0x0 performance
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0xffffffff81000000 ffffffff81e53ba0 test_data/Kernel/data test_data/Kernel/trace 0x0 performance
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 00400000 4b6000 test_data/mruby/data test_data/mruby/trace 0x0 performance
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0xffffffff83a00000 0xffffffff83a26160 test_data/kafl_eval/data.pagecache test_data/kafl_eval/sample1_1x 0x0 performance
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0xffffffff83a00000 0xffffffff83a26160 test_data/kafl_eval/data.pagecache test_data/kafl_eval/sample1_1x_sanx 0x0 performance
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0x000555555554000 0x00555555d60000 test_data/qemu/page_cache test_data/qemu/trace1 0x0 performance
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0xffffffff81000000 0xffffffff81e53ba0 test_data/infinite_loop_linux/page_cache test_data/infinite_loop_linux/loop_trace_26023_0 0x0 performance
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0x1000 0x7ffffffff000 test_data/avscript_32/page_cache test_data/avscript_32/failed_trace_12709_1 0x0 performance 
ASAN_OPTIONS=detect_leaks=1 LD_LIBRARY_PATH=./libxdc/build/ ./libxdc/build/tester_static 0x1000 0xfffffffffffff000 test_data/icelake_tip/pagecache test_data/icelake_tip/ptlog eb2a5b786860a79f performance 
