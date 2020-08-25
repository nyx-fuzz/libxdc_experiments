gcc -c -ggdb -Ofast -flto linux_ipt_instrumentation.c -o linux_ip_instrumentation.o
gcc -ggdb -Ofast -flto main.c linux_ip_instrumentation.o -o tester