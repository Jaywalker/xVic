CC=gcc
CFLAGS=-I/opt/local/include/ -L/opt/local/lib/
all:
	$(CC) -o xvic -m32 $(CFLAGS) xvic.c
	$(CC) -o dumpimages $(CFLAGS) -lgd dumpimages.c

clean:
	rm xvic dumpimages

