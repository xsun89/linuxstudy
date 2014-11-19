#ifndef _SCK_CLINT_H_
#define _SCK_CLINT_H_

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

int sckClient_init(void **handle, int connTime, int sendTime, int recvTime);

int sckClient_connect(void **handle);

int sckClient_send(void *handle, unsigned char *data, int datalen);

int sckClient_rcv(void *handle, unsigned char *out, int *outlen);

int sckClient_destroy(void *handle);

#ifdef __cplusplus
}
#endif

#endif