#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <signal.h>

#define SHM_KEY_CLOCK 1234

struct Clock {
    int seconds;
    int nanoseconds;
};

void handle_sigint(int sig) {
    printf("Worker received SIGINT. Terminating.\n");
    exit(EXIT_SUCCESS);
}

int main(int argc, char *argv[]) {
    if (argc != 3) {
        fprintf(stderr, "Usage: %s <seconds> <nanoseconds>\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    int seconds = atoi(argv[1]);
    int nanoseconds = atoi(argv[2]);

    key_t key = ftok(".", SHM_KEY_CLOCK);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    int shmid = shmget(key, sizeof(struct Clock), 0666);
    if (shmid == -1) {
        perror("shmget");
        exit(EXIT_FAILURE);
    }

    struct Clock *clock = (struct Clock *)shmat(shmid, NULL, 0);
    if (clock == (void *)-1) {
        perror("shmat");
        exit(EXIT_FAILURE);
    }

    // Calculate termination time
    int termSeconds = clock->seconds + seconds;
    int termNanoseconds = clock->nanoseconds + nanoseconds;
    if (termNanoseconds >= 1000000000) {
        termSeconds++;
        termNanoseconds -= 1000000000;
    }

    signal(SIGINT, handle_sigint);

    printf("WORKER PID:%d PPID:%d SysClockS: %d SysClockNano: %d TermTimeS: %d TermTimeNano: %d --Just Starting\n",
           getpid(), getppid(), clock->seconds, clock->nanoseconds, termSeconds, termNanoseconds);

    while (1) {
        if (clock->seconds > termSeconds || (clock->seconds == termSeconds && clock->nanoseconds >= termNanoseconds)) {
            printf("WORKER PID:%d PPID:%d SysClockS: %d SysClockNano: %d TermTimeS: %d TermTimeNano: %d --Terminating\n",
                   getpid(), getppid(), clock->seconds, clock->nanoseconds, termSeconds, termNanoseconds);
            break;
        }

        if (clock->nanoseconds % 1000000 == 0) {
            printf("WORKER PID:%d PPID:%d SysClockS: %d SysClockNano: %d TermTimeS: %d TermTimeNano: %d --%d seconds have passed since starting\n",
                   getpid(), getppid(), clock->seconds, clock->nanoseconds, termSeconds, termNanoseconds, clock->seconds - seconds);
        }
    }

    shmdt(clock);

    return 0;
}
