#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/types.h>
#include <sys/sem.h>
#include <signal.h>

#define SHM_KEY_CLOCK 1234
#define SHM_KEY_PROCESS_TABLE 5678
#define SEM_KEY 9876
#define MAX_CHILDREN 20
#define SHMEM_KEY 12345
#define MAX_PROCESSES 20
#define SIGALRM 14

struct Clock
{
    int seconds;
    int nanoseconds;
};

struct PCB
{
    int occupied;
    pid_t pid;
    int startSeconds;
    int startNano;
};

int shmid_clock;
int shmid_process_table;
int semid;

struct PCB processTable[MAX_CHILDREN];
int stillChildrenToLaunch = 1;

void timeout_handler(int signal_num);
void ctrlc_handler(int sig);
void launch_child_processes(int num_children);
void wait_for_child_processes();

pid_t child_pids[MAX_CHILDREN];
int num_children = 0;

void incrementClock();
void outputProcessTable();
void checkIfChildHasTerminated();
void updatePCBOfTerminatedChild();
void possiblyLaunchNewChild();

int main()
{
    signal(SIGALRM, timeout_handler);
    signal(SIGINT, ctrlc_handler);

    alarm(60);

    while (1)
    {

        if (num_children < MAX_CHILDREN)
        {
            launch_child_processes(1);
        }

        wait_for_child_processes();
    }

    while (stillChildrenToLaunch)
    {

        incrementClock();

        outputProcessTable();

        checkIfChildHasTerminated();

        possiblyLaunchNewChild();

        return 0;
    }
}

void incrementClock()
{
}

void outputProcessTable()
{
}

void checkIfChildHasTerminated()
{
    int status;
    pid_t pid = waitpid(-1, &status, WNOHANG);
    if (pid > 0)
    {

        updatePCBOfTerminatedChild();
    }
}

void updatePCBOfTerminatedChild()
{
}

void possiblyLaunchNewChild()
{
}

void timeout_handler(int signal_num)
{

    for (int i = 0; i < num_children; i++)
    {
        kill(child_pids[i], SIGTERM);
    }

    exit(EXIT_SUCCESS);
}

void ctrlc_handler(int sig)
{

    for (int i = 0; i < num_children; i++)
    {
        kill(child_pids[i], SIGTERM);
    }

    exit(EXIT_SUCCESS);
}
