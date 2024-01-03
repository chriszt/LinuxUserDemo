#include "SigTest.h"
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <pthread.h>

////////////////////////////////////////////////////////////

void SigTest00()
{
    alarm(5);
    int count = 1;
    while (1) {
        fprintf(stdout, "count=%d\n", count++);
    }
}

////////////////////////////////////////////////////////////

static void PrintSigSet01(const sigset_t *set)
{
    for (int i = 1; i <= 31; i++) {
        if (sigismember(set, i)) {
            putchar('1');
        } else {
            putchar('0');
        }
    }
    putchar('\n');
}

void SigTest01()
{
    fprintf(stdout, "pid=%d\n", getpid());
    sigset_t oldMaskSet, newMaskSet;
    sigemptyset(&newMaskSet);
    sigaddset(&newMaskSet, SIGINT);
    sigprocmask(SIG_BLOCK, &newMaskSet, &oldMaskSet);

    fprintf(stdout, "old mask set:\n");
    PrintSigSet01(&oldMaskSet);
    fprintf(stdout, "new mask set:\n");
    PrintSigSet01(&newMaskSet);
    
    fprintf(stdout, "pending set:\n");
    while (1) {
        sigset_t pendingSet;
        sigpending(&pendingSet);
        PrintSigSet01(&pendingSet);
        sleep(1);
    }
}

////////////////////////////////////////////////////////////

static void PrintSigSet02(const sigset_t *set)
{
    for (int i = 1; i <= 31; i++) {
        if (sigismember(set, i)) {
            putchar('1');
        } else {
            putchar('0');
        }
    }
    putchar('\n');
}

static void PrintMaskSet02()
{
    sigset_t maskSet;
    sigprocmask(SIG_BLOCK, NULL, &maskSet);
    fprintf(stdout, "mask set:\n");
    PrintSigSet02(&maskSet);
}

static void PrintPendingSet02()
{
    sigset_t pendingSet;
    sigpending(&pendingSet);
    fprintf(stdout, "pending set:\n");
    PrintSigSet02(&pendingSet);
}

static void Alarm02(int sigNum)
{
    fprintf(stdout, "[Alarm02] thread: 0x%lx, sigNum(%d)\n", pthread_self(), sigNum);
    PrintMaskSet02();
    PrintPendingSet02();
}

void SigTest02()
{
    fprintf(stdout, "[main] thread: 0x%lx\n", pthread_self());
    PrintMaskSet02();

    struct sigaction newSigAct, oldSigAct;
    newSigAct.sa_handler = Alarm02;
    // newSigAct.sa_sigaction = NULL;  // NOTE: can NOT be NULL
    sigemptyset(&newSigAct.sa_mask);
    newSigAct.sa_flags = 0;
    newSigAct.sa_restorer = NULL;
    if (sigaction(SIGALRM, &newSigAct, &oldSigAct) == -1) {
        fprintf(stderr, "sigaction failed(%d): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "sigaction succeeded\n");
    PrintMaskSet02();
    
    while (1) {
        alarm(2);
        int ret = pause();
        fprintf(stdout, "ret=%d, errno=%d, strerror=%s\n", ret, errno, strerror(errno));
        PrintMaskSet02();
        PrintPendingSet02();
    }
}

////////////////////////////////////////////////////////////

void sig_alrm(int signo)
{
    /* nothing to do */
    printf("****\n");
}

unsigned int mysleep(unsigned int nsecs)
{
    struct sigaction newact, oldact;
    // unsigned int unslept;

    newact.sa_handler = sig_alrm;
    sigemptyset(&newact.sa_mask);
    newact.sa_flags = 0;
    sigaction(SIGALRM, &newact, &oldact);

    alarm(nsecs);
    pause();

    // unslept = alarm(0);
    // sigaction(SIGALRM, &oldact, NULL);

    return 0;
}


void SigTest03()
{
    mysleep(2);
    printf("Two seconds passed\n");
}

////////////////////////////////////////////////////////////

void SigTest04()
{
    sigset_t maskSet;
    sigemptyset(&maskSet);
    int ret = sigsuspend(&maskSet);
    fprintf(stdout, "ret=%d, errno=%d: %s\n", ret, errno, strerror(errno));
}

////////////////////////////////////////////////////////////
