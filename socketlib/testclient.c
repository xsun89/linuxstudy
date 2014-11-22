#include "commsocket.h"

int main()
{
    int ret = 0;
    void *handle = NULL;
    char *data = "abcdefghijklmn";
    int datalen = 10;
    ret = sckClient_init(&handle, 5, 5, 5, 10);
    ret = sckClient_connect(handle, "127.0.0.1", 8001);
    ret = sckClient_send(handle, data, datalen);
    return 0;
}