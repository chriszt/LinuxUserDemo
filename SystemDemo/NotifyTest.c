#include "NotifyTest.h"
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <poll.h>
#include <sys/inotify.h>

////////////////////////////////////////////////////////////

static char g_buf[BUFSIZ];

void NotifyTest00()
{
    const char *logFile = "/var/log/auth.log";

    int logFd = open(logFile, O_RDONLY | O_NONBLOCK);
    if (logFd == -1) {
        fprintf(stderr, "open %s failed(%d): %s\n", logFile, errno, strerror(errno));
        return;
    }
    fprintf(stdout, "open %s succeeded: logFd(%d)\n", logFile, logFd);
    
    if (lseek(logFd, 0, SEEK_END) == -1) {
        fprintf(stderr, "seek to %s end failed(%d): %s\n", logFile, errno, strerror(errno));
        close(logFd);
        return;
    }
    fprintf(stdout, "seek to %s end succeeded\n", logFile);
    
    int notifyFd = inotify_init1(IN_NONBLOCK);
    if (notifyFd == -1) {
        fprintf(stderr, "init inotify failed(%d): %s\n", errno, strerror(errno));
        close(logFd);
        return;
    }
    fprintf(stdout, "init inotify succeeded: notifyFd(%d)\n", notifyFd);

    int wd = inotify_add_watch(notifyFd, logFile, IN_MODIFY);
    if (wd == -1) {
        fprintf(stderr, "add watch failed(%d): %s\n", errno, strerror(errno));
        close(notifyFd);
        close(logFd);
        return;
    }
    fprintf(stdout, "add watch succeeded: wd(%d)\n", wd);

    struct pollfd fds[2] = {0};
    fds[0].fd = notifyFd;
    fds[0].events = POLLIN;
    
    int iread;
    while (1) {
        if (poll(fds, 1, -1) == -1) {
            fprintf(stderr, "poll failed(%d): %s\n", errno, strerror(errno));
            continue;
        }
        if (fds[0].revents & POLLIN) {
            char *cmov = g_buf;
            struct inotify_event e = {0};
            // do {
            iread = read(notifyFd, &e, sizeof(e));
            // } while(iread > 0);
            while (e.wd) {
                iread = read(logFd, cmov, sizeof(g_buf));
                if (iread > 0) {
                    fprintf(stdout, "reading %s...\n", logFile);
                } else {
                    fprintf(stdout, "reading %s finished\n", logFile);
                    break;
                }
            }
        }
    }

    close(notifyFd);
    close(logFd);
}

/////////////////////////////////////////////////////////////

static void HandleEvent(int notifyFd, int *wd, int argc, char *argv[])
{
    char buf[4096] __attribute__((aligned(__alignof__(struct inotify_event))));
    struct inotify_event *event = NULL;
    ssize_t len = 0;
    while (1) {
        len = read(notifyFd, buf, sizeof(buf));
        if (len == -1 && errno != EAGAIN) {
            fprintf(stderr, "read failed(%d): %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
        if (len <= 0) {
            break;
        }
        for (char *ptr = buf; ptr < buf + len; ptr += sizeof(struct inotify_event) + event->len) {
            event = (struct inotify_event *)ptr;
            if (event->mask & IN_OPEN) {
                fprintf(stdout, "IN_OPEN: ");
            }
            if (event->mask & IN_CLOSE_NOWRITE) {
                fprintf(stdout, "IN_CLOSE_NOWRITE: ");
            }
            if (event->mask & IN_CLOSE_WRITE) {
                fprintf(stdout, "IN_CLOSE_WRITE: ");
            }
            for (int i = 1; i < argc; i++) {
                if (wd[i] == event->wd) {
                    fprintf(stdout, "%s/", argv[i]);
                    break;
                }
            }
            if (event->len) {
                fprintf(stdout, "%s", event->name);
            }
            if (event->mask & IN_ISDIR) {
                fprintf(stdout, "[directory]\n");
            } else {
                fprintf(stdout, "[file]\n");
            }
        }
    }
}

void NotifyTest01(int argc, char *argv[])
{
    if (argc < 2) {
        fprintf(stdout, "Usage: %s PATH [PATH ...]\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    fprintf(stdout, "Press ENTER key to terminate.\n");

    int notifyFd = inotify_init1(IN_NONBLOCK);
    if (notifyFd == -1) {
        fprintf(stderr, "init inotify failed(%d): %s\n", errno, strerror(errno));
        exit(EXIT_FAILURE);
    }

    int *wd = (int *)malloc(argc * sizeof(int));
    if (wd == NULL) {
        fprintf(stderr, "allocate memory for watch descriptor failed\n");
        exit(EXIT_FAILURE);
    }

    for (int i = 1; i < argc; i++) {
        wd[i] = inotify_add_watch(notifyFd, argv[i], IN_OPEN | IN_CLOSE);
        if (wd[i] == -1) {
            fprintf(stderr, "cannot watch '%s'(%d): %s\n", argv[i], errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
    }

    nfds_t nfds = 2;
    struct pollfd fds[2] = {0};
    fds[0].fd = STDIN_FILENO;
    fds[0].events = POLLIN;
    fds[1].fd = notifyFd;
    fds[1].events = POLLIN;
    
    fprintf(stdout, "Listening for events.\n");
    while (1) {
        int pollNum = poll(fds, nfds, -1);
        if (pollNum == -1) {
            fprintf(stderr, "poll failed(%d): %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
        if (pollNum > 0) {
            if (fds[0].revents & POLLIN) {
                char chr;
                while (read(STDIN_FILENO, &chr, 1) > 0 && (chr != '\n')) {
                    continue;
                }
                break;
            }
            if (fds[1].revents & POLLIN) {
                HandleEvent(notifyFd, wd, argc, argv);
            }
        }
    }

    fprintf(stdout, "Listening for events stopped.\n");

    close(notifyFd);
    free(wd);
}

/////////////////////////////////////////////////////////////

void NotifyTest02()
{
    const char *filePath = "/home/yl/proj/LinuxDemo/dir1";
    uint32_t mask = 0;
    mask |= IN_ALL_EVENTS;
    // mask |= IN_OPEN;
    // mask |= IN_CLOSE;

    int notifyFd = inotify_init1(IN_NONBLOCK);
    if (notifyFd == -1) {
        fprintf(stderr, "init inotify failed(%d): %s\n", errno, strerror(errno));
        return;
    }
    fprintf(stdout, "init inotify succeeded: notifyFd(%d)\n", notifyFd);

    int notifyWatch = inotify_add_watch(notifyFd, filePath, mask);
    if (notifyWatch == -1) {
        fprintf(stderr, "add watch to [%s] failed(%d): %s\n", filePath, errno, strerror(errno));
        return;
    }
    fprintf(stdout, "add watch to [%s] succeeded: notifyWatch(%d)\n", filePath, notifyWatch);

    struct pollfd fds[2] = {0};
    fds[0].fd = notifyFd;
    fds[0].events = POLLIN;
    int countOfRead = 0;

    while (1) {
        if (poll(fds, 2, -1) == -1) {
            fprintf(stderr, "poll failed(%d): %s\n", errno, strerror(errno));
            continue;
        }
        if (fds[0].revents & POLLIN) {
            for (;;) {
                char buf[4096] = {0};
                int bytesOfRead = read(fds[0].fd, buf, sizeof(buf));
                countOfRead++;
                fprintf(stdout, "[%d] bytesOfRead=%d\n", countOfRead, bytesOfRead);
                if ((bytesOfRead == -1) && (errno != EAGAIN)) {
                    fprintf(stderr, "  read failed(%d): %s\n", errno, strerror(errno));
                    break;
                }
                if (bytesOfRead <= 0) {
                    fprintf(stdout, "  read again\n");
                    break;
                }
                struct inotify_event *evt = NULL;
                for (char *ptr = buf; ptr < buf + bytesOfRead; ptr += sizeof(struct inotify_event) + evt->len) {
                    evt = (struct inotify_event *) ptr;
                    fprintf(stdout, "  inotify event: wd(%d), mask(0x%x), cookie(%u), len(%u), name(%s)\n",
                        evt->wd, evt->mask, evt->cookie, evt->len, evt->name);
                }
            }
        }
    }

    close(notifyFd);
}

/////////////////////////////////////////////////////////////

void NotifyTest03()
{

}

/////////////////////////////////////////////////////////////
