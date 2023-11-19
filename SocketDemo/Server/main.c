#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <poll.h>
#include <errno.h>
#include <fcntl.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>
#include <ctype.h>
#include <poll.h>

#define MAXLINE 80
#define SERV_PORT 8000
#define CLIENT_SIZE 100

int setnoblock(int fd)
{
    int oldopt = fcntl(fd, F_GETFL);
    int newopt = oldopt | O_NONBLOCK;
    fcntl(fd, F_SETFL, newopt);
    return oldopt;
}

// https://www.cnblogs.com/zuofaqi/p/9631601.html
int main(int argc, char *argv[])
{
    struct sockaddr_in servaddr, cliaddr;
    socklen_t cliaddr_len;
    int listenfd, connfd;
    char buf[MAXLINE];
    char str[INET_ADDRSTRLEN];
    int i, n;
    struct pollfd fds[CLIENT_SIZE];

    listenfd = socket(AF_INET, SOCK_STREAM, 0);

    bzero(&servaddr, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    servaddr.sin_port = htons(SERV_PORT);
    
    bind(listenfd, (struct sockaddr *)&servaddr, sizeof(servaddr));

    listen(listenfd, 20);

    fds[0].fd = listenfd;
    fds[0].events = POLLIN | POLLERR;
    fds[0].revents = 0;
    int clientCount = 0;

    printf("Accepting connections ...\n");
    while (1) {
        int ret = poll(fds, clientCount + 1, -1);
        if (ret == -1) {
            fprintf(stderr, "poll: %d, %s\n", errno, strerror(errno));
            exit(1);
        }
        for (int i = 0; i < clientCount + 1; i++) {
            if ((fds[i].revents & POLLHUP) || (fds[i].revents & POLLERR)) {
                int fd = fds[i].fd;
                fds[i] = fds[clientCount];
                i--;
                clientCount--;
                close(fd);
                printf("delete connection: %d\n", fd);
            } else if ((fds[i].fd == listenfd) && (fds[i].revents & POLLIN)) {
                cliaddr_len = sizeof(cliaddr);
                connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);
                printf("get connection %d from %s:%d\n",
                    connfd, inet_ntoa(cliaddr.sin_addr), cliaddr.sin_port);
                clientCount++;
                setnoblock(connfd);
                fds[clientCount].fd = connfd;
                fds[clientCount].events = POLLIN | POLLHUP | POLLERR;
                fds[clientCount].revents = 0;
            } else if (fds[i].revents & POLLIN) {
                int lenrecv = recv(fds[i].fd, buf, MAXLINE-1, 0);
                if (lenrecv > 0) {
                    fprintf(stdout, "reading...: %s\n", buf);
                    fds[i].events &= (~POLLIN);
                    fds[i].events |= POLLOUT;
                } else if (lenrecv == 0) {
                    printf("------- client %d exit (not print) --------\n", fds[i].fd);
                } else {
                    fprintf(stderr, "recv: %d, %s\n", errno, strerror(errno));
                    exit(1);
                }
            } else if (fds[i].revents & POLLOUT) {
                for (int j = 0; j < strlen(buf); j++) {
                    buf[j] = toupper(buf[j]);
                }
                send(fds[i].fd, buf, strlen(buf), 0);
                fds[i].events &= (~POLLOUT);
                fds[i].events |= POLLIN;
            }
        }
        // cliaddr_len = sizeof(cliaddr);
        // connfd = accept(listenfd, (struct sockaddr *)&cliaddr, &cliaddr_len);

        // n = read(connfd, buf, MAXLINE);
        // printf("received from %s at PORT %d\n",
        //     inet_ntop(AF_INET, &cliaddr.sin_addr, str, sizeof(str)),
        //     ntohs(cliaddr.sin_port));
    
        // for (i = 0; i < n; i++) {
        //     buf[i] = toupper(buf[i]);
        // }
        // write(connfd, buf, n);
        // close(connfd);
    }

    return 0;
}
