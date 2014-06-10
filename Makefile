CFLAGS=-march=native -std=gnu99 -g -O2 -I./include -L./lib
LIBS=-lpthread -lrt -ltermpaper

MY_ACTOR_SOURCE=actor_in_c.c
MY_ACTOR_FILE=actor_in_c.out

MY_CLIENT_SOURCE=client.c
MY_CLIENT_FILE=client.out

all: actor client

clean:
	rm -fv lib/*.a 
	rm -fv lib/*.o 
	rm -fv $(MY_ACTOR_FILE) $(MY_CLIENT_FILE) 

actor: $(MY_ACTOR_SOURCE) lib/libtermpaper.a
	gcc $(CFLAGS) $(MY_ACTOR_SOURCE) $(LIBS) -o $(MY_ACTOR_FILE)
	
client: $(MY_CLIENT_SOURCE)
	gcc $(CFLAGS) $(MY_CLIENT_SOURCE) $(LIBS) -o $(MY_CLIENT_FILE)		

# shared libs
lib/termPaperLib.o: lib/termPaperLib.c include/termPaperLib.h
	gcc -c $(CFLAGS) lib/termPaperLib.c -o lib/termPaperLib.o

lib/concurrentLinkedList.o: lib/concurrentLinkedList.c lib/termPaperLib.o
	gcc -c $(CFLAGS) lib/concurrentLinkedList.c -o lib/concurrentLinkedList.o

lib/libtermpaper.a: lib/termPaperLib.o lib/concurrentLinkedList.o 
	ar crs lib/libtermpaper.a lib/termPaperLib.o lib/concurrentLinkedList.o 
