#include "ProcessTest.h"
#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <ctype.h>
#include <dirent.h>
#include <sys/wait.h>

////////////////////////////////////////////////////////////

void ProcessTest00()
{
    extern char **environ;
    for (int i = 0; environ[i] != NULL; i++) {
        fprintf(stdout, "%s\n", environ[i]);
    }
}

////////////////////////////////////////////////////////////

void ProcessTest01()
{
    pid_t pid;
    char *msg = NULL;
    int n;

    fprintf(stdout, "[main] ppid=%d pid=%d\n", getppid(), getpid());

    pid = fork();
    if (pid == -1) {
        fprintf(stderr, "fork failed(%d): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (pid == 0) { // child
        msg = (char*)"child";
        n = 6;
    } else { // parent
        msg = (char*)"parent";
        n = 3;
    }
    for (int i = 1; i <= n; i++) {
        fprintf(stdout, "%d. [%s] ppid=%d pid=%d <%d>\n", i, msg, getppid(), getpid(), pid);
        sleep(1);
    }
}

////////////////////////////////////////////////////////////

void ProcessTest02()
{
    // char *psArgv[] = {"ps", "-o", "pid,ppid,pgrp,session,tpgid,comm", NULL};
    // char *psEnvp[] = {"PATH=/bin:/usr/bin", "TERM=console", NULL};
    // execve("/usr/bin/ps", psArgv, psEnvp);
    // execl("/usr/bin/ps", "ps", "-o", "pid,ppid,pgrp,session,tpgid,comm", NULL);
    // execv("/usr/bin/ps", psArgv);
    // execle("/usr/bin/ps", "ps", "-o", "pid,ppid,pgrp,session,tpgid,comm", NULL, psEnvp);
    // execlp("ps", "ps", "-o", "pid,ppid,pgrp,session,tpgid,comm", NULL);
    // execvp("ps", psArgv);
    
    execlp("ls", "ps", "-o", "pid,ppid,pgrp,session,tpgid,comm", NULL);
    perror("exec ps");
    exit(EXIT_FAILURE);
}

////////////////////////////////////////////////////////////

void ProcessTest03()
{
    fprintf(stdout, "ppid=%d, pid=%d\n", getppid(), getpid());
    int ch;
    while ((ch = getchar()) != EOF) {
        putchar(toupper(ch));
    }
}

////////////////////////////////////////////////////////////

void ProcessTest04()
{
    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "fork failed(%d): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (pid > 0) {
        while (1) {
            fprintf(stdout, "parent\n");
            sleep(1);
        }
    }
    fprintf(stdout, "child\n");
}

////////////////////////////////////////////////////////////

void ProcessTest05()
{
    pid_t pid;
    pid = fork();
    if (pid < 0) {
        fprintf(stderr, "fork failed(%d): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    if (pid == 0) { // child
        for (int i = 1; i <= 22; i++) {
            fprintf(stdout, "child -- %d\n", i);
            sleep(1);
        }
        exit(3);
    } else { // parent
        int status;
        waitpid(pid, &status, 0);
        fprintf(stdout, "status=%d, ifExist=%d, exitStatus=%d, ifSignal=%d, signal=%d\n",
            status, WIFEXITED(status), WEXITSTATUS(status), WIFSIGNALED(status), WTERMSIG(status));
    }
}

////////////////////////////////////////////////////////////

void ProcessTest06()
{
    char msg[] = "This is a test\n";
    int fd = open("somefile", O_RDWR | O_CREAT, S_IRUSR | S_IWUSR); // fd == 3
    if (fd < 0) {
        fprintf(stderr, "create file failed(%d): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    fprintf(stdout, "create file succeeded: fd(%d)\n", fd);

    int saveFd = dup(STDOUT_FILENO);  // saveFd == 4
    dup2(fd, STDOUT_FILENO);  // close 1 --> fd == 3
    close(fd);  // close 3
    write(STDOUT_FILENO, msg, strlen(msg));

    dup2(saveFd, STDOUT_FILENO);
    write(STDOUT_FILENO, msg, strlen(msg));
    close(saveFd);
}

////////////////////////////////////////////////////////////

static void Daemonzie07()
{
    pid_t pid = fork();
    if (pid < 0) {
        fprintf(stderr, "fork failed(%d): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        exit(EXIT_SUCCESS);    // close parent process
    }
    pid_t sid = setsid();
    fprintf(stdout, "pid=%d, sid=%d\n", getpid(), sid);
    if (chdir("/") == -1) {
        fprintf(stderr, "change working directory failed(%d): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }
    close(STDIN_FILENO);
    open("/dev/null", O_RDWR);
    dup2(STDIN_FILENO, STDOUT_FILENO);
    dup2(STDIN_FILENO, STDERR_FILENO);
}

void ProcessTest07()
{
    Daemonzie07();
    while (1);
}

////////////////////////////////////////////////////////////

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

static void GetPidByName(pid_t *pid, char *name)
{
    DIR *dir = opendir("/proc");
    if (!dir) {
        fprintf(stderr, "open /proc failed\n");
        return;
    }
    struct dirent *ptr;
    while ((ptr = readdir(dir)) != NULL) {
        if (!strcmp(ptr->d_name, ".") || !strcmp(ptr->d_name, "..")) {
            continue;
        }
        if (ptr->d_type != DT_DIR) {
            continue;
        }
        char filePath[1024] = {0};
        sprintf(filePath, "/proc/%s/status", ptr->d_name);
        FILE *fp = fopen(filePath, "r");
        if (!fp) {
            fprintf(stderr, "open %s failed\n", filePath);
            continue;
        }
        char buf[260] = {0};
        if (!fgets(buf, sizeof(buf) - 1, fp)) {
            fprintf(stderr, "read %s failed\n", filePath);
            fclose(fp);
            continue;
        }
        char procName[260] = {0};
        sscanf(buf, "%*s %s", procName);
        if (!strcmp(name, procName)) {
            sscanf(ptr->d_name, "%d", pid);
        }
        fclose(fp);
    }
    closedir(dir);
}

static void GetPpidByPid(pid_t pid, pid_t *ppid)
{
    char procPidPath[260] = {0};
    char buf[260];
    sprintf(procPidPath, "/proc/%d/status", pid);
    FILE *fp = fopen(procPidPath, "r");
    if (!fp) {
        fprintf(stderr, "open %s failed\n", procPidPath);
        return;
    }
    while (fgets(buf, sizeof(buf), fp)) {
        if (!strncmp(buf, "PPid:", 5)) {
            char ppidBuf[50] = {0};
            sscanf(buf, "%*s %s", ppidBuf);
            *ppid = strtol(ppidBuf, NULL, 10);
            break;
        }
    }
    fclose(fp);
}

void ProcessTest08()
{
    pid_t pid = getpid();
    char name[50] = {0};
    GetNameByPid(pid, name);
    fprintf(stdout, "pid=%d, pname=%s\n", pid, name);

    GetPidByName(&pid, name);
    fprintf(stdout, "pname=%s, pid=%d\n", name, pid);

    pid_t ppid;
    GetPpidByPid(pid, &ppid);
    fprintf(stdout, "pid=%d, ppid=%d\n", pid, ppid);

    char parentName[50] = {0};
    GetNameByPid(ppid, parentName);
    fprintf(stdout, "ppid=%d, ppname=%s\n", ppid, parentName);
}

////////////////////////////////////////////////////////////

void ProcessTest09()
{
    pid_t pid = getpid();
    // pid_t pid = 1;
    char procFdDir[260] = {0};
    sprintf(procFdDir, "/proc/%d/fd", pid);
    DIR *dir = opendir(procFdDir);
    if (!dir) {
        fprintf(stderr, "open %s failed(%d): %s\n", procFdDir, errno, strerror(errno));
        return;
    }
    fprintf(stdout, "open %s succeeded\n", procFdDir);
    struct dirent *ent;
    while ((ent = readdir(dir)) != NULL) {
        if (ent->d_type != DT_LNK) {
            continue;
        }
        // fprintf(stdout, "name=%s, type=%u\n", ent->d_name, ent->d_type);
        char lnkPath[1024] = {0};
        sprintf(lnkPath, "%s/%s", procFdDir, ent->d_name);
        char lnkContent[260] = {0};
        if (readlink(lnkPath, lnkContent, sizeof(lnkContent)) == -1) {
            fprintf(stderr, "read link %s failed(%d): %s\n", lnkPath, errno, strerror(errno));
            break;
        }
        fprintf(stdout, "%s -> %s\n", lnkPath, lnkContent);
    }
    closedir(dir);
}

////////////////////////////////////////////////////////////

void ProcessTest10()
{

}

////////////////////////////////////////////////////////////
