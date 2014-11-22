#include "commsocket.h"

int main()
{
    int ret = 0;
    void *handle = NULL;
    char *data = "abcdefghijklmn";
    int datalen = 10;
	char buf[1024];
	int outlen;
	memset(buf, 0, 1024);
    ret = sckClient_init(&handle, 10, 10, 10, 10);
	printf("init ret=%d\n", ret);
    ret = sckClient_connect(handle, "127.0.0.1", 8008);
	printf("connect ret=%d\n", ret);
    ret = sckClient_send(handle, (unsigned char *)data, datalen);
	printf("send ret=%d\n", ret);
	ret = sckClient_rcv(handle, (unsigned char *)buf, &outlen);
	printf("recv ret=%d\n", ret);
	printf("received=%d, content=%s\n", outlen, buf);

    return ret;
}
