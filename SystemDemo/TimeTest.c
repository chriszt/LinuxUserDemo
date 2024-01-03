#include "TimeTest.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>
#include <sys/times.h>

////////////////////////////////////////////////////////////

void TimeTest00()
{
    // returns  the  time  as  the  number of seconds since the Epoch,
    //   1970‐01‐01 00:00:00 +0000 (UTC).
    time_t curTime = time(NULL);
    fprintf(stdout, "time(..)=%lu\n", curTime);  // GMT, curTime=1692295569

    char curTimeStr[50] = {0};

    // GMT
    struct tm *curGmtTm = gmtime(&curTime);  
    fprintf(stdout, "asctime(gmtime(..))=%s", asctime(curGmtTm));

    // GMT  -->  GMT + 8
    struct tm *curLocTm = localtime(&curTime);
    fprintf(stdout, "asctime(localtime(..))=%s", asctime(curLocTm));
    fprintf(stdout, "%d-%02d-%02d %02d:%02d:%02d\n",
        curLocTm->tm_year + 1900, curLocTm->tm_mon + 1, curLocTm->tm_mday,
        curLocTm->tm_hour, curLocTm->tm_min, curLocTm->tm_sec);
    
    strftime(curTimeStr, 50, "%Y-%m-%d %H:%M:%S", curLocTm);
    fprintf(stdout, "%s\n", curTimeStr);
    strftime(curTimeStr, 50, "%D %T", curLocTm);
    fprintf(stdout, "%s\n", curTimeStr);

    // GMT  -->  GMT + 8
    fprintf(stdout, "ctime(time(..))=%s", ctime(&curTime));

    // GMT or GMT + 8  -->  GMT
    fprintf(stdout, "mktime(..)=%lu\n", mktime(curGmtTm));
    fprintf(stdout, "mktime(..)=%lu\n", mktime(curLocTm));
}

////////////////////////////////////////////////////////////

void TimeTest01()
{
    struct timeval time1, time2;

    // gives the number of seconds and microseconds since the Epoch,
    //   1970‐01‐01 00:00:00 +0000 (UTC).
    if (gettimeofday(&time1, NULL) == -1) {
        fprintf(stderr, "get time1 of day failed(%d): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "time1=%ld(s), %ld(us)\n", time1.tv_sec, time1.tv_usec);
    
    if (gettimeofday(&time2, NULL) == -1) {
        fprintf(stderr, "get time2 of day failed(%d): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "time2=%ld(s), %ld(us)\n", time2.tv_sec, time2.tv_usec);
}

////////////////////////////////////////////////////////////

void TimeTest02()
{
    long clockTicks = sysconf(_SC_CLK_TCK);
    fprintf(stdout, "clockTicks=%ld\n", clockTicks);

    struct tms t1;
    if (times(&t1) == -1) {
        fprintf(stderr, "times failed(%d): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "user cpu time is: %lf, system cpu time is: %lf\n",
        (double)t1.tms_utime / clockTicks, (double)t1.tms_stime / clockTicks);

    for (int i = 0; i < 1000000; i++) {
        getpid();
    }

    struct tms t2;
    if (times(&t2) == -1) {
        fprintf(stderr, "times failed(%d): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "user cpu time is: %lf, system cpu time is: %lf\n",
        (double)t2.tms_utime / clockTicks, (double)t2.tms_stime / clockTicks);
}

////////////////////////////////////////////////////////////

void TimeTest03()
{
    clock_t clock1 = clock();
    fprintf(stdout, "clock1=%ld\n", clock1);
    fprintf(stdout, "CLOCKS_PER_SEC=%ld\n", CLOCKS_PER_SEC);

    for (int i = 0; i < 1000000; i++) {
        getpid();
    }
    
    clock_t clock2 = clock();
    fprintf(stdout, "clock2=%ld\n", clock2);
    fprintf(stdout, "cpu running time is: %lf\n",
        (double)(clock2 - clock1) / CLOCKS_PER_SEC);
}

////////////////////////////////////////////////////////////

void TimeTest04()
{
    struct timespec tp;
    if (clock_gettime(CLOCK_REALTIME, &tp) == -1) {
        fprintf(stderr, "get time failed(%d): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "realtime=%ld, %ld\n", tp.tv_sec, tp.tv_nsec);

    if (clock_gettime(CLOCK_MONOTONIC, &tp) == -1) {
        fprintf(stderr, "get time failed(%d): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "monotonic=%ld, %ld\n", tp.tv_sec, tp.tv_nsec);

    sleep(1);

    if (clock_gettime(CLOCK_PROCESS_CPUTIME_ID, &tp) == -1) {
        fprintf(stderr, "get time failed(%d): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "process cpu time ID=%ld, %ld\n", tp.tv_sec, tp.tv_nsec);

    if (clock_gettime(CLOCK_THREAD_CPUTIME_ID, &tp) == -1) {
        fprintf(stderr, "get time failed(%d): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "thread cpu time ID=%ld, %ld\n", tp.tv_sec, tp.tv_nsec);
}

////////////////////////////////////////////////////////////

void TimeTest05()
{
    struct timeval tv;
    tv.tv_sec = 1613214177;
    tv.tv_usec = 236563;
    struct tm *timeTm = localtime(&tv.tv_sec);
    fprintf(stdout, "%d\n", timeTm->tm_year);
}

////////////////////////////////////////////////////////////
