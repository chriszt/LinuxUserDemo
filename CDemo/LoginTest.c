#include "LoginTest.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <pwd.h>
#include <utmp.h>

#define PATH_BTMP "/var/log/btmp"

static void GetNameByPid(pid_t pid, char *name)
{
    char procPidPath[260] = {0};
    char buf[260];
    sprintf(procPidPath, "/proc/%d/status", pid);
    FILE *fp = fopen(procPidPath, "r");
    if (!fp) {
        fprintf(stderr, "open %s failed\n", procPidPath);
        return;
    }
    if (!fgets(buf, sizeof(buf), fp)) {
        fprintf(stderr, "read %s failed\n", procPidPath);
        fclose(fp);
        return;
    }
    fclose(fp);
    sscanf(buf, "%*s %s", name);
}

static pid_t GetPpidByPid(pid_t pid)
{
    char procPidPath[260] = {0};
    char buf[260];
    sprintf(procPidPath, "/proc/%d/status", pid);
    FILE *fp = fopen(procPidPath, "r");
    if (!fp) {
        fprintf(stderr, "open %s failed\n", procPidPath);
        return 0;
    }
    pid_t ppid;
    while (fgets(buf, sizeof(buf), fp)) {
        if (!strncmp(buf, "PPid:", 5)) {
            char ppidBuf[50] = {0};
            sscanf(buf, "%*s %s", ppidBuf);
            ppid = strtol(ppidBuf, NULL, 10);
            break;
        }
    }
    fclose(fp);
    return ppid;
}

static void ShowUtent(const struct utmp *ut)
{
    // id
    char id[5] = {0};
    memcpy(id, ut->ut_id, sizeof(ut->ut_id));
    
    // ip
    int32_t addrInt = ut->ut_addr;
    unsigned char *addrPtr = (unsigned char *)&addrInt;
    char ipv4[16] = {0};
    sprintf(ipv4, "%d.%d.%d.%d", addrPtr[0], addrPtr[1], addrPtr[2], addrPtr[3]);

    // time
    time_t sec = ut->ut_tv.tv_sec;
    struct tm *timeTm = localtime(&sec);
    char tmStr[20] = {0};
    strftime(tmStr, sizeof(tmStr), "%Y-%m-%d %H:%M:%S", timeTm);
    char timeStr[50] = {0};
    sprintf(timeStr, "%s.%d", tmStr, ut->ut_tv.tv_usec);

    // association
    pid_t pid = ut->ut_pid;
    pid_t ppid = GetPpidByPid(pid);
    char pname[50] = {0};
    char ppname[50] = {0};
    GetNameByPid(pid, pname);
    GetNameByPid(ppid, ppname);

    // output
    fprintf(stdout, "[%s] %s(%d), %s(%d), type=%d, line=%s, id=%s, user=%s, host=%s, addr=%s, session=%d, exit=<%d,%d>\n",
        timeStr, pname, pid, ppname, ppid, ut->ut_type, ut->ut_line, id, ut->ut_user, ut->ut_host, ipv4, ut->ut_session, ut->ut_exit.e_termination, ut->ut_exit.e_exit);
}

////////////////////////////////////////////////////////////

void LoginTest00()
{
    if (utmpname(_PATH_WTMP) == -1) {
        fprintf(stderr, "change ut file failed(%d): %s\n", errno, strerror(errno));
        return;
    }
    while (1) {
        struct utmp *ut = getutent();
        if (!ut) {
            fprintf(stderr, "error(%d): %s\n", errno, strerror(errno));
            break;
        }
        if (ut->ut_pid != 4155) {
            continue;
        }
        ShowUtent(ut);
    }
}

////////////////////////////////////////////////////////////

void LoginTest01()
{
    struct utmp entry;
    entry.ut_type = USER_PROCESS;
    entry.ut_pid = getpid();
    char *ttyName = ttyname(STDIN_FILENO);
    fprintf(stdout, "ttyName=%s\n", ttyName);
    strcpy(entry.ut_line, ttyName + strlen("/dev/"));
    strcpy(entry.ut_id, ttyName + strlen("/dev/tty"));
    time((time_t*)&entry.ut_time);
    uid_t uid = getuid();
    struct passwd *pwd = getpwuid(uid);
    fprintf(stdout, "%u --> %s\n", uid, pwd->pw_name);
    strcpy(entry.ut_user, pwd->pw_name);
    memset(entry.ut_host, 0, UT_HOSTSIZE);
    entry.ut_addr = 0;
    setutent();
    if (pututline(&entry) == NULL) {
        fprintf(stderr, "pututline() failed(%d): %s\n", errno, strerror(errno));
    }

    entry.ut_type = DEAD_PROCESS;
    memset(entry.ut_line, 0, UT_LINESIZE);
    entry.ut_time = 0;
    memset(entry.ut_user, 0, UT_NAMESIZE);
    setutent();
    pututline(&entry);

    endutent();
}

////////////////////////////////////////////////////////////

void LoginTest02()
{
    if (utmpname(_PATH_WTMP) == -1) {
        fprintf(stderr, "change ut file failed(%d): %s\n", errno, strerror(errno));
        return;
    }
    struct utmp ut;
    ut.ut_type = USER_PROCESS;
    memset(ut.ut_id, 0, sizeof(ut.ut_id));
    strcpy(ut.ut_id, "~~");
    while (1) {
        struct utmp *res = getutid(&ut);
        if (!res) {
            fprintf(stderr, "error(%d): %s\n", errno, strerror(errno));
            break;
        }
        ShowUtent(res);
    }
}

////////////////////////////////////////////////////////////

void LoginTest03()
{
    if (utmpname(_PATH_WTMP) == -1) {
        fprintf(stderr, "change ut file failed(%d): %s\n", errno, strerror(errno));
        return;
    }
    struct utmp ut;
    ut.ut_type = USER_PROCESS;
    memset(ut.ut_line, 0, UT_LINESIZE);
    strcpy(ut.ut_line, "pts/1");
    while (1) {
        struct utmp *res = getutline(&ut);
        if (!res) {
            fprintf(stderr, "error(%d): %s\n", errno, strerror(errno));
            break;
        }
        ShowUtent(res);
    }
}

////////////////////////////////////////////////////////////
