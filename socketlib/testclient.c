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
#include <IMServicePlugIn/IMServicePlugIn.h>
#include <Foundation/Foundation.h>
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