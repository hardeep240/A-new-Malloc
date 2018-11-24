myprogram:mymain.c  libmem.so
	gcc mymain.c -lmem -L. -o myprogram
libmem.so:mem.o
	gcc -shared -o libmem.so mem.o
	setenv LD_LIBRARY_PATH ${LD_LIBRARY_PATH}:.
mem.o:
	gcc -c -fpic mem.c

