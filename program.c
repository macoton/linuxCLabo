#include <stdio.h>
#include <stdlib.h>
#include <signal.h>
#include <unistd.h>
#include <string.h>
#include <pthread.h>

#define ARRAY_SIZE(x) (sizeof(x) / sizeof((x)[0]))
#define MIN_VALUE(a, b) ((a) < (b) ? (a) : (b))
#define MAX_VALUE(a, b) ((a) > (b) ? (a) : (b))

enum {
    PROCESS,
    THREAD
};

int mode = PROCESS;
int pids[5];
pthread_t tids[ARRAY_SIZE(pids)];

void sigint_handler(int sig) {
    printf("Received SIGINT signal...%d. Exiting...\n", getpid());
    switch (mode) {
        case PROCESS:
            for (int index = 0; index < ARRAY_SIZE(pids); index++) {
                printf("Sended SIGKILL signal process...%d.\n", pids[index]);
                kill(pids[index], SIGKILL);
            }
            break;
        case THREAD:
            for (int index = 0; index < ARRAY_SIZE(tids); index++) {
                printf("Sended SIGKILL signal thread...%ld\n", tids[index]);
                pthread_kill(tids[index], SIGKILL);
            }
            break;
    }
    exit(0);
}

void process_handler(void) {
    int no = 1;
    while (1) {
        printf("No. %d Sleep process...%d.\n", no, getpid());
        usleep(100000);
        no++;
    }
}

void *thread_handler(void *arg) {
    int no = 1;
    while (1) {
        printf("No. %d Sleep thread...%ld.\n", no, pthread_self());
        usleep(100000);
        no++;
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);
    if (argc > 1) {
        mode = atoi(argv[1]);
        mode = MAX_VALUE(mode, PROCESS);
        mode = MIN_VALUE(mode, THREAD);
    }
    switch (mode) {
        case PROCESS:
            memset(pids, 0, sizeof(pids));
            for (int index = 0; index < ARRAY_SIZE(pids); index++) {
                if ((pids[index] = fork()) == 0) {
                    process_handler();
                }
            }
            break;
        case THREAD:
            memset(tids, 0, sizeof(tids));
            for (int index = 0; index < ARRAY_SIZE(tids); index++) {
                pthread_create(&tids[index], NULL, thread_handler, NULL);
            }
            break;
    }
    signal(SIGINT, sigint_handler);
    process_handler();
}
