CFLAGS=-march=native -Wall -std=gnu99 -g -O2
LIBS=-lpthread -lrt

MY_SOURCE=actor_in_c.c
MY_FILE=actor_in_c.o

all: actor

clean:
	rm lib/*.a lib/*.o $(MY_FILE)

actor: $(MY_SOURCE)
	gcc $(CFLAGS) $(MY_SOURCE) $(LIBS) -o $(MY_FILE)	