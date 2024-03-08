libini.a: ini.o getval.o
	@ar cr libini.a ini.o getval.o
	@ranlib libini.a
ini.o:
	@gcc -DPOSIX_MMAP -O3 -c ini.c
getval.o:
	@gcc -O3 -c getval.c
