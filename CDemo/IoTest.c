#include "IoTest.h"
#include <stdio.h>
#include <poll.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <errno.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>

////////////////////////////////////////////////////////////

void IoTest00(int argc, char *argv[])
{
    int ready;
    char buf[10];
    nfds_t numOpenFds, nfds;
    ssize_t s;
    struct pollfd *pfds;

    if (argc < 2) {
        fprintf(stderr, "usage: %s file...\n", argv[0]);
        exit(EXIT_FAILURE);
    }

    numOpenFds = nfds = argc - 1;
    pfds = calloc(nfds, sizeof(struct pollfd));
    if (!pfds) {
        fprintf(stderr, "malloc memory failed\n");
        exit(EXIT_FAILURE);
    }

    for (nfds_t j = 0; j < nfds; j++) {
        pfds[j].fd = open(argv[j + 1], O_RDONLY);
        if (pfds[j].fd == -1) {
            fprintf(stderr, "open %s failed\n", argv[j + 1]);
            exit(EXIT_FAILURE);
        }
        fprintf(stdout, "opened \"%s\" on fd %d\n", argv[j + 1], pfds[j].fd);
        pfds[j].events = POLLIN;
    }

    while (numOpenFds > 0) {
        fprintf(stdout, "about to poll()\n");
        ready = poll(pfds, nfds, -1);
        if (ready == -1) {
            fprintf(stderr, "poll failed(%d): %s\n", errno, strerror(errno));
            exit(EXIT_FAILURE);
        }
        fprintf(stdout, "ready: %d\n", ready);
        for (nfds_t j = 0; j < nfds; j++) {
            if (pfds[j].revents) {
                fprintf(stdout, "  fd=%d; events: %s %s %s\n", pfds[j].fd,
                    (pfds[j].revents & POLLIN) ? "POLLIN" : "",
                    (pfds[j].revents & POLLHUP) ? "POLLHUP" : "",
                    (pfds[j].revents & POLLERR) ? "POLLERR" : "");
                if (pfds[j].revents & POLLIN) {
                    s = read(pfds[j].fd, buf, sizeof(buf));
                    if (s == -1) {
                        fprintf(stderr, "read failed(%d): %s\n", errno, strerror(errno));
                        exit(EXIT_FAILURE);
                    }
                    fprintf(stdout, "    read %zd bytes: %.*s\n", s, (int)s, buf);
                } else {
                    fprintf(stdout, "    closing fd %d\n", pfds[j].fd);
                    if (close(pfds[j].fd) == -1) {
                        fprintf(stderr, "close failed(%d): %s\n", errno, strerror(errno));
                        exit(EXIT_FAILURE);
                    }
                    numOpenFds--;
                }
            }
        }
    }
    fprintf(stdout, "all file descriptors closed; bye\n");
    exit(EXIT_FAILURE);
}

////////////////////////////////////////////////////////////