CC = gcc
CFLAGS = -Wall

all: dircont

hashes.o: hashes.c hashes.h
	$(CC) $(CFLAGS) -c hashes.c

hashtab.o: hashtab.c hashtab.h
	$(CC) $(CFLAGS) -c hashtab.c

dircont.o: dircont.c dircont.h
	$(CC) $(CFLAGS) -c dircont.c

dircont: hashes.o hashtab.o dircont.o

clean:
	rm -f *~ *.o dircont core
