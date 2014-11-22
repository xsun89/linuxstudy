#include <unistd.h>
#include <sys/types.h>
#include <fcntl.h>
#include <sys/select.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <netdb.h>

#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <Python/Python.h>
#include "commsocket.h"

typedef struct _SckHandle
{
    int sockfd;
    int conntime;
    int sendtime;
    int recvtime;
}SckHandle;
void activate_nonblock(int fd)
{
    int ret;
    int flags = fcntl(fd, F_GETFD);
    if(flags == -1)
        ERR_EXIT("fcntl");
    flags |= O_NONBLOCK;
    ret = fcntl(fd, F_SETFL, flags);
    if(ret == -1)
        ERR_EXIT("fcntl");
}
void deactivate_nonblock(int fd)
{
    int ret;
    int flags = fcntl(fd, F_GETFD);
    if(flags == -1)
        ERR_EXIT("fcntl");
    flags &= ~O_NONBLOCK;
    ret = fcntl(fd, F_SETFL, flags);
    if(ret == -1)
        ERR_EXIT("fcntl");
}
int read_timeout(int fd, unsigned int wait_seconds)
{
    int ret = 0;
    if(wait_seconds > 0)
    {
        fd_set read_fdset;
        struct timeval timeout;
        FD_ZERO(&read_fdset);
        FD_SET(fd, &read_fdset);

        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;

        do{
            ret = select(fd+1, &read_fdset, NULL, NULL, &timeout);
        }while(ret < 0 && errno == EINTR);

        if(ret == 0){
            ret = -1;
            errno = ETIMEDOUT;
        }else if(ret == 1){
            ret = 0;
        }
    }
    return ret;
}
int write_timeout(int fd, unsigned int wait_seconds)
{
    int ret = 0;
    if(wait_seconds > 0)
    {
        fd_set write_fdset;
        struct timeval timeout;
        FD_ZERO(&write_fdset);
        FD_SET(fd, &write_fdset);

        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;

        do{
            ret = select(fd+1, &write_fdset, NULL, NULL, &timeout);
        }while(ret < 0 && errno == EINTR);

        if(ret == 0){
            ret = -1;
            errno = ETIMEDOUT;
        }else if(ret == 1){
            ret = 0;
        }
    }
    return ret;
}

int connect_timeout(int fd, struct sockaddr *addr, unsigned int wait_seconds)
{
    int ret = 0;
    socklen_t addrlen = sizeof(struct sockaddr_in);
    if(wait_seconds > 0)
        activate_nonblock(fd);
    ret = connect(fd, (struct sockaddr *)addr, addrlen);
    if(ret == -1 && errno == EINPROGRESS)
    {
        fd_set connect_fdset;
        FD_ZERO(&connect_fdset);
        FD_SET(fd, &connect_fdset);
        struct timeval timeout;
        timeout.tv_sec = wait_seconds;
        timeout.tv_usec = 0;
        do{
            ret = select(fd+1, NULL, &connect_fdset, NULL, &timeout);
        }while(ret == -1 && errno == EINTR);
        if(ret == 0){
            ret = -1;
            errno = ETIMEDOUT;
        }else if(ret < 0){
            ret = -1;
        }else if(ret == 1){
            int err;
            socklen_t socklen = sizeof(err);
            int sockoptret = getsockopt(fd, SOL_SOCKET, SO_ERROR, &err, &socklen);
            if(sockoptret == -1){
                return -1;
            }else if(err == 0){
                ret = 0;
            }else{
                errno = err;
                ret = -1;
            }
        }

    }
    if(wait_seconds > 0){
        deactivate_nonblock(fd);
    }
    return ret;
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
        int ret = recv(sockfd, buf, len, MSG_PEEK);
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
int sckClient_init(void **handle, int connTime, int sendTime, int recvTime, int connNumber)
{
    int ret = 0;
    if(handle == NULL || connTime < 0 || sendTime < 0 || recvTime < 0 )
    {
        ret = SCK_ERRPARM;
        printf("func sckClient_init() err\n");
        return ret;
    }

    SckHandle *tmp = (SckHandle *) malloc(sizeof(SckHandle));
    if(tmp == NULL)
    {
        ret = SCK_ERRMALLOC;
        printf("func sckClient_init() err: malloc %d\n", ret);
        return ret;
    }

    tmp->conntime = connTime;
    tmp->sendtime = sendTime;
    tmp->recvtime = recvTime;
    int sockfd;
    sockfd = socket(PF_INET, SOCK_STREAM, IPPROTO_TCP);
    if(sockfd < 0)
    {
        ret = errno;
        printf("func sckClient_init() err: %d\n", ret);
        return ret;
    }
    tmp->sockfd = sockfd;

    return ret;
}

int sckClient_connect(void *handle, const char *ip, int port)
{
    int ret = 0;
    if(handle == NULL || ip == NULL || port <0 || port > 61350){
        ret = SCK_ERRPARM;
        printf("func sckClient_init() err\n");
        return ret;
    }

    SckHandle *tmp = handle;
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(ip);

    ret = connect_timeout(tmp->sockfd, (struct sockaddr *)&servaddr, tmp->conntime);
    if(ret < 0)
    {
        if(ret == -1 && errno == ETIMEDOUT) {
            ret = SCK_ERRTIMEOUT;
            printf("func connect() err: %d\n", ret);
            return ret;
        }

        return ret;
    }

    return ret;
}

int sckClient_send(void *handle, unsigned char *data, int datalen)
{
    int ret = 0;
    SckHandle *tmp = handle;
    ret = write_timeout(tmp->sockfd, tmp->sendtime);
    if(ret == 0){
        int netDataLen = htonl(datalen);
        char *buf = malloc(4+datalen);
        if(buf == NULL){
            ret = SCK_ERRMALLOC;
            printf("sckClient_send() err: %d\n", ret);
            return ret;
        }
        memcpy(buf, &netDataLen, 4);
        memcpy(buf+4, data, datalen);
        int writenLen = writen(tmp->sockfd, buf, datalen + 4);
        if(writenLen < datalen + 4){
            if(buf != NULL){
                free(buf);
                buf = NULL;
                return writenLen;
            }
        }
        if(buf != NULL){
            free(buf);
            buf = NULL;
        }
        return writenLen;
    }
    if(ret < 0)
    {
        if(ret == -1 && errno == ETIMEDOUT) {
            ret = SCK_ERRTIMEOUT;
            printf("func send() err: %d\n", ret);
            return ret;
        }

        return ret;
    }

    return ret;
}

int sckClient_rcv(void *handle, unsigned char *out, int *outlen)
{
    int ret = 0;
    SckHandle *tmp = handle;
    ret = read_timeout(tmp->sockfd, tmp->recvtime);
    if(ret < 0){
        if(ret == -1 && errno == ETIMEDOUT){
            ret = SCK_ERRTIMEOUT;
            printf("sckClient_rcv() err: %d\n", ret);
            return ret;
        }

        return ret;
    }
    int netdatalen = 0;
    ret = readn(tmp->sockfd, &netdatalen, 4);
    if(ret == -1 ){
        printf("func sckClient_rcv() err: %d\n", ret);
        return ret;
    }else if(ret < 4){
        ret = SCK_ERRPEERCLOSED;
        printf("func sckClient_rcv() err: %d\n", ret);
        return ret;
    }
    int n;
    n = ntohl(netdatalen);
    ret = readn(tmp->sockfd, out, n);
    if(ret == -1 ){
        printf("func sckClient_rcv() err: %d\n", ret);
        return ret;
    }else if(ret < 4){
        ret = SCK_ERRPEERCLOSED;
        printf("func sckClient_rcv() err: %d\n", ret);
        return ret;
    }
    *outlen = n;

    return 0;
}
int sckClient_closeConn(int conn)
{
    if(conn > 0){
        close(conn);
    }

    return 0;
}
int sckClient_destroy(void *handle)
{
    if(handle != NULL){
        free(handle);
    }

    return 0;
}