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
    ret = sckClient_connect(handle, "127.0.0.1", 8008);
    ret = sckClient_send(handle, (unsigned char *)data, datalen);
	ret = sckClient_rcv(handle, (unsigned char *)buf, &outlen);
	printf("received=%d, content=%s\n", outlen, buf);

	printf("ret=%d\n", ret);
    return ret;
}
