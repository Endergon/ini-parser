@echo off
gcc -DWINDOWS_HEAP_ALLOC -O3 -c ini.c
gcc -O3 -c getval.c
ar cr libini.a ini.o getval.o
ranlib libini.a
pause
