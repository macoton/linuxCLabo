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
    MODE_MIN = 1,
    MODE_PROCESS = MODE_MIN,
    MODE_MAX,
    MODE_THREAD = MODE_MAX 
};

enum {
    MODE2_MIN = 1,
    MODE2_SLEEP = MODE_MIN,
    MODE2_MAX,
    MODE2_TIMER = MODE2_MAX 
};

int mode = 0;
int mode2 = 0;
int pidsize = 0;
int *pids = NULL;
int tidsize = 0;
pthread_t *tids = NULL;

void sigint_handler(int sig) {
    printf("Received SIGINT signal...%d. Exiting...\n", getpid());
    switch (mode) {
        case MODE_PROCESS:
            for (int pidindex = 0; pidindex < pidsize; pidindex++) {
                printf("Sended SIGKILL signal process...%d.\n", pids[pidindex]);
                kill(pids[pidindex], SIGKILL);
            }
            free(pids);
            break;
        case MODE_THREAD:
            for (int tidindex = 0; tidindex < tidsize; tidindex++) {
                printf("Sended SIGKILL signal thread...%ld\n", tids[tidindex]);
                pthread_kill(tids[tidindex], SIGKILL);
            }
            free(tids);
            break;
    }
    exit(0);
}

void process_handler(void) {
    int no = 1;
    switch (mode2) {
        case MODE2_SLEEP:
            while (1) {
                printf("No. %d Sleep process...%d.\n", no, getpid());
                usleep(100000);
                no++;
            }
            break;
        case MODE2_TIMER:
            break;
    }
}

void *thread_handler(void *arg) {
    int no = 1;
    switch (mode2) {
        case MODE2_SLEEP:
            while (1) {
                printf("No. %d Sleep thread...%ld.\n", no, pthread_self());
                usleep(100000);
                no++;
            }
            break;
        case MODE2_TIMER:
            break;
    }
    return NULL;
}

int main(int argc, char *argv[]) {
    setvbuf(stdout, NULL, _IONBF, 0);
    mode = argc > 1 ? atoi(argv[1]) : 0;
    mode = MAX_VALUE(mode, MODE_MIN);
    mode = MIN_VALUE(mode, MODE_MAX);
    mode2 = argc > 2 ? atoi(argv[2]) : 0;
    mode2 = MAX_VALUE(mode2, MODE2_MIN);
    mode2 = MIN_VALUE(mode2, MODE2_MAX);
    switch (mode) {
        case MODE_PROCESS:
            pidsize = argc > 3 ? atoi(argv[3]) : 0;
            pidsize = MAX_VALUE(pidsize, 1);
            pidsize = MIN_VALUE(pidsize, 100);
            if ((pids = malloc(sizeof(pids[0]) * pidsize)) == NULL) {
                return 1;
            }
            memset(pids, 0, sizeof(pids));
            for (int pidindex = 0; pidindex < pidsize; pidindex++) {
                if ((pids[pidindex] = fork()) == 0) {
                    process_handler();
                }
            }
            break;
        case MODE_THREAD:
            tidsize = argc > 3 ? atoi(argv[3]) : 0;
            tidsize = MAX_VALUE(tidsize, 1);
            tidsize = MIN_VALUE(tidsize, 100);
            if ((tids = malloc(sizeof(tids[0]) * tidsize)) == NULL) {
                return 1;
            }
            memset(tids, 0, sizeof(tids));
            for (int tidindex = 0; tidindex < tidsize; tidindex++) {
                pthread_create(&tids[tidindex], NULL, thread_handler, NULL);
            }
            break;
    }
    signal(SIGINT, sigint_handler);
    process_handler();
}
