all: tabstop.o mystdiolib.o
	gcc -g -o tabstop tabstop.o mystdiolib.o

tabstop.o: tabstop.c
	gcc -c -g tabstop.c

mystdiolib.o: mystdiolib.c mystdiolib.h
	gcc -c -g mystdiolib.c

clean: 
	rm -rf *.o tabstop