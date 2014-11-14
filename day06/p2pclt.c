#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

typedef struct _data
{
    int buflen;
    char buf[1024];
}data;


void handler(int num)
{
    printf("child received signal\n");
    exit(0);
}

ssize_t writen(int fd, const void *buf, size_t len)
{
    size_t nleft = len;
    ssize_t nwritten = 0;
    char *tmpBuf = (char *)buf;
    while(nleft >0)
    {
        nwritten = write(fd, tmpBuf, nleft);
        if(nwritten < 0)
        {
            if(errno == EINTR)
                continue;
            return -1;
        }else if(nwritten == 0)
        {
            continue;
        }
        tmpBuf = tmpBuf + nwritten;
        nleft -= nwritten;
    }

    return len;
}

ssize_t readn(int fd, void *buf, size_t len)
{
    size_t nleft = len;
    ssize_t nread = 0;
    char *tmpBuf = (char *)buf;
    while(nleft >0)
    {
        nread = read(fd, tmpBuf, nleft);
        if(nread < 0)
        {
            if(errno == EINTR) {
                continue;
            }else {
                return -1;
            }
        }else if(nread == 0)
        {
            return len-nleft;
        }
        tmpBuf = tmpBuf + nread;
        nleft -= nread;
    }

    return len;

}
int main()
{
    signal(SIGUSR1, handler);
    int sockfd = 0;
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if(sockfd < 0)
    {
        perror("func sock");
        exit(0);
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    struct in_addr inAddr;
    memset(&inAddr, 0, sizeof(inAddr));
    addr.sin_family = AF_INET;
    addr.sin_port = htons(8008);
    inet_aton("127.0.0.1", &inAddr);
    addr.sin_addr = inAddr;
    if(connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("func connect");
        exit(0);
    }
    pid_t pid = fork();
    if(pid == 0)
    {
        data d;
        memset(&d, 0, sizeof(d));
        int n;
        while(fgets(d.buf, sizeof(d.buf), stdin) != NULL) {
            n = strlen(d.buf);
            d.buflen = htonl(n);
            printf("client sending length=%d buf=%s\n", n, d.buf);
            writen(sockfd, (void *)&d, 4+n);
            memset(&d, 0, sizeof(d));
        }
    }

    if(pid > 0)
    {
        data d;
        memset(&d, 0, sizeof(d));
        int readlen;
        while (1) {
            readlen = readn(sockfd, (void *)&d.buflen, 4);
            if (readlen == 0) {
                printf("server reset");
                break;
            } else if (readlen < 0) {
                perror("read fail");
                break;
            }
            int actlen = ntohl((int)d.buflen);
            printf("client read actual length=%d\n", actlen);
            readlen = readn(sockfd, (void *)d.buf, actlen);
            if (readlen == 0) {
                printf("peer reset");
                break;
            } else if (readlen < 0) {
                perror("read fail");
                break;
            }
            fputs(d.buf, stdout);
            memset(&d, 0, sizeof(d));
        }
        close(sockfd);
        kill(pid, SIGUSR1);
        exit(0);
    }

    close(sockfd);

    return 0;
}
