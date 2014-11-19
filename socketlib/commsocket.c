#include <unstd.h>
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
#include <sys/errno.h>
#include <sys/_endian.h>
#include "commsocket.h"

typedef struct _SckHandle
{
    int sockfd;
    int connfd;
    int conntime;
    int sendtime;
    int recvtime;
}SckHandle;

int sckClient_init(void **handle, char *ip, int port, int connTime, int sendTime, int recvTime)
{
    int ret;
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
    struct sockaddr_in servaddr;
    memset(&servaddr, 0, sizeof(servaddr));
    servaddr.sin_family = AF_INET;
    servaddr.sin_port = htons(port);
    servaddr.sin_addr.s_addr = inet_addr(ip);


    int connfd;
    connfd = connect(sockfd, (struct sockaddr*)&servaddr, sizeof(servaddr));
    if(connfd < 0)
    {
        ret = errno;
        printf("func connect() err: %d\n", ret);
        return ret;
    }
    tmp->connfd = connfd;
    return ret;
}

int sckClient_connect(void **handle)
{

}

int sckClient_send(void *handle, unsigned char *data, int datalen)
{

}

int sckClient_rcv(void *handle, unsigned char *out, int *outlen)
{

}

int sckClient_destroy(void *handle)
{

}