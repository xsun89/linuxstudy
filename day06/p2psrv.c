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
#include <bootstrap.h>
#include <signal.h>
#include <sys/signal.h>
#include <search.h>
#include <GSS/GSS.h>
#include <ForceFeedback/ForceFeedback.h>
#include <Python/Python.h>
#include <Kernel/string.h>
#include <stddef.h>

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
            if(errno == ENTER)
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
        nread = write(fd, tmpBuf, nleft);
        if(nread < 0)
        {
            if(errno == ENTER) {
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
        char buf[1024] = {0};
        int buflen = 0;
        while (1) {
            buflen = readn(conn, buf, 4);
            if (buflen == 0) {
                printf("peer reset");
                break;
            } else if (buflen < 0) {
                perror("read fail");
                break;
            }else if(buflen != 4)
            {
                break;
            }
            int actLen = (int)buf;
            memset(buf, 0, sizeof(buf));
            buflen = readn(conn, buf, actLen);
            fputs(buf, stdout);
            memset(buf, 0, sizeof(buf));
        }
        close(conn);
        kill(getppid(), SIGUSR1);
        exit(0);
    }

    if(pid > 0)
    {
        char sendbuf[1024] = {0};
        int sendbufLen = 0;
        while(fgets(sendbuf, sizeof(sendbuf), stdin) != NULL) {
            sendbufLen = strlen(sendbuf);
            write(conn, (void *)sendbufLen, 4);
            write(conn, sendbuf, sendbufLen);
            memset(sendbuf, 0, sizeof(sendbuf));
        }
    }
    close(sockfd);
    return 0;
}
