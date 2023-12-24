#include "SigTest.h"
#include <iostream>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <signal.h>

////////////////////////////////////////////////////////////

static void PrintSigSet00(const sigset_t &set)
{
    for (int i = 1; i <= 31; i++) {
        if (sigismember(&set, i)) {
            std::cout << "1";
        } else {
            std::cout << "0";
        }
    }
    std::cout << std::endl;
}

void SigTest00()
{
    sigset_t oldMaskSet, newMaskSet;
    sigemptyset(&newMaskSet);
    sigaddset(&newMaskSet, SIGINT);
    sigprocmask(SIG_BLOCK, &newMaskSet, &oldMaskSet);
    // sigprocmask(SIG_BLOCK, nullptr, &oldMaskSet);
    // PrintSigSet00(oldMaskSet);
    std::fprintf(stdout, "old mask set:\n");
    PrintSigSet00(oldMaskSet);
    std::fprintf(stdout, "new mask set:\n");
    PrintSigSet00(newMaskSet);

    std::fprintf(stdout, "pending set:\n");
    while (true) {
        sigset_t pendingSet;
        sigpending(&pendingSet);
        PrintSigSet00(pendingSet);
        sleep(1);
    }
}

////////////////////////////////////////////////////////////

static void InterruptHandler(int sigNo)
{
    std::cout << "InterruptHandler()" << std::endl;
}

static void AlarmHandler(int sigNo)
{
    std::cout << "AlarmHandler()" << std::endl;
}

void SigTest01()
{
    std::cout << "current pid: " << getpid() << std::endl;
    struct sigaction newSigAct, oldSigAct;
    newSigAct.sa_handler = InterruptHandler;
    // newSigAct.sa_handler = AlarmHandler;
    sigemptyset(&newSigAct.sa_mask);
    newSigAct.sa_flags = 0;
    newSigAct.sa_restorer = nullptr;
    sigaction(SIGINT, &newSigAct, &oldSigAct);
    // sigaction(SIGALRM, &newSigAct, &oldSigAct);
    while (true) {
        // alarm(2);
        // int ret = pause();
        // std::fprintf(stdout, "ret=%d, errno=%d, strerror=%s\n", ret, errno, strerror(errno));
        std::string input;
        std::cin >> input;
        std::cout << input << std::endl;
    }
}

////////////////////////////////////////////////////////////

void SigTest02()
{
    char *p = nullptr;
    *p = 1;
}

////////////////////////////////////////////////////////////
