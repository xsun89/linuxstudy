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
#include <stddef.h>
#include <bootstrap.h>
#include <Python/Python.h>
#include <ForceFeedback/ForceFeedback.h>
#include <sys/errno.h>

typedef struct _data
{
    int buflen;
    char buf[1024];
}data;

void handler(int num)
{
    printf("parent received signal\n");
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

ssize_t recv_peek(int sockfd, void *buf, size_t len)
{
    while(1)
    {
        int ret = recv(sockfd, buf, MSG_PEEK);
        if(ret == -1 && errno == EINTR){
            continue;
        }
        return ret;
    }
}

ssize_t readline(int sockfd, void *buf, size_t maxline)
{
    int ret;
    int nread;
    char *bufp = (char *)buf;
    int nleft = maxline;
    while(nleft > 0)
    {
        ret = recv_peek(sockfd, bufp, nleft);
        if(ret < 0)
            return ret;
        else if(ret == 0)
            return 0;

        nread = ret;
        int i;
        for(i=0; i<nread; i++){
            if(bufp[i] == '\n') {
                ret = readn(sockfd, bufp, i + 1);
                if( ret != i+1)
                    exit(-1);

                return ret;
            }
        }
        if(nread > nleft)
            exit(-1);

        nleft -= nread;
        ret = readn(sockfd, bufp, nread);
        if(ret != nread)
            exit(-1);

        bufp += nread;

    }

    return -1;
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
    //inet_aton("192.168.30.188", &inAddr);
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int optionVal = 1;
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&optionVal, sizeof(optionVal)) < 0)
    {
        perror("setsockopt");
        exit(0);
    }
    if(bind(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("func bind");
        exit(0);
    }
    if(listen(sockfd, 5) < 0)
    {
        perror("func listen");
        exit(0);
    }

    struct sockaddr_in peerAddr;
    socklen_t peerAddrLen = sizeof(peerAddr);
    int conn = 0;
    if ((conn = accept(sockfd, (struct sockaddr *) &peerAddr, (socklen_t * ) & peerAddrLen)) < 0) {
        perror("func accept");
        exit(0);
    }
    printf("peer address:%s \t peerport:%d\n", inet_ntoa(peerAddr.sin_addr), ntohs(peerAddr.sin_port));
    pid_t pid = fork();
    if(pid == 0)
    {
        data d;
        memset(&d, 0, sizeof(d));
        int readlen;
        int n;
        while (1) {
            readlen = readn(conn, (void *)&d.buflen, 4);
            printf("server read length %d\n", readlen);
            if (readlen == 0) {
                printf("peer reset");
                break;
            } else if (readlen < 0) {
                perror("read fail");
                break;
            }
            n = ntohl(d.buflen);
            readlen = readn(conn, (void *)d.buf, n);
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
        close(conn);
        kill(getppid(), SIGUSR1);
        exit(0);
    }

    if(pid > 0)
    {
        data d;
        int n;
        memset(&d, 0, sizeof(d));
        while(fgets(d.buf, sizeof(d.buf), stdin) != NULL) {
            n = strlen(d.buf);
            d.buflen = htonl(n);
            writen(conn, (void *)&d, 4+n);
            memset(&d, 0, sizeof(d));
        }
    }
    close(sockfd);
    return 0;
}
