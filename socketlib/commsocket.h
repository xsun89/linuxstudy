#ifndef _SCK_CLINT_H_
#define _SCK_CLINT_H_

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

#ifdef __cplusplus
extern 'C'
{
#endif
#define SCK_OK                 0
#define SCK_BASEERR            3000
#define SCK_ERRPARM            (SCK_BASEERR+1)
#define SCK_ERRTIMEOUT         (SCK_BASEERR+2)
#define SCK_ERRPEERCLOSED      (SCK_BASEERR+3)
#define SCK_ERRMALLOC          (SCK_BASEERR+4)
#define ERR_EXIT(m) \
	do \
	{ \
		perror(m); \
		exit(EXIT_FAILURE); \
	} \
	while (0)

int sckClient_init(void **handle, int connTime, int sendTime, int recvTime, int connNumber);
int sckClient_connect(void *handle, const char *addr, int port);
int sckClient_send(void *handle, unsigned char *data, int datalen);
int sckClient_rcv(void *handle, unsigned char *out, int *outlen);
int sckClient_closeConn(int conn);
int sckClient_destroy(void *handle);


int sckServer_init(int *listenfd);
int sckServer_accept(int listenfd, int *connfd, int timeout);
int sckServer_send(int connfd, unsigned char *data, int datalen, int timeout);
int sckServer_rcv(int connfd, unsigned char *out, int *outlen, int timeout);

#ifdef __cplusplus
}
#endif

#endif