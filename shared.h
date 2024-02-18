#ifndef SHARED_H
#define SHARED_H

#define SHMEM_KEY 12345
#define MAX_PROCESSES 20

struct Clock {
    int seconds;
    int nanoseconds;
};

struct PCB {
    int occupied;
    pid_t pid;
    int startSeconds;
    int startNano;
};

#endif
