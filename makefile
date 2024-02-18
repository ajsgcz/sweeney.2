CC = gcc
CFLAGS = -std=c99

all: oss worker

oss: oss.c shared.h
    $(CC) $(CFLAGS) -o oss oss.c -lrt

worker: worker.c shared.h
    $(CC) $(CFLAGS) -o worker worker.c -lrt

clean:
    rm -f oss worker
