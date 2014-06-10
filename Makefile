CFLAGS=-march=native -std=gnu99 -g -O2 -I./include -L./lib
LIBS=-lpthread -lrt -ltermpaper

MY_SOURCE=actor_in_c.c
MY_FILE=actor_in_c.out

all: actor

clean:
	rm -fv lib/*.a 
	rm -fv lib/*.o 
	rm -fv $(MY_FILE) 

actor: $(MY_SOURCE) lib/libtermpaper.a
	gcc $(CFLAGS) $(MY_SOURCE) $(LIBS) -o $(MY_FILE)	

# shared libs
lib/termPaperLib.o: lib/termPaperLib.c include/termPaperLib.h
	gcc -c $(CFLAGS) lib/termPaperLib.c -o lib/termPaperLib.o

lib/concurrentLinkedList.o: lib/concurrentLinkedList.c lib/termPaperLib.o
	gcc -c $(CFLAGS) lib/concurrentLinkedList.c -o lib/concurrentLinkedList.o

lib/libtermpaper.a: lib/termPaperLib.o lib/concurrentLinkedList.o 
	ar crs lib/libtermpaper.a lib/termPaperLib.o lib/concurrentLinkedList.o 
