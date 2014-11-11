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
int main()
{
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
    inet_aton("127.0.0.1", &inAddr);
    addr.sin_addr = inAddr;
    
    if(connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("func connect");
        exit(0);
    }
    char recvbuf[1024] = {0};
    char sendbuf[1024] = {0};
    while(fgets(sendbuf, sizeof(sendbuf), stdin) != NULL)
    {
        write(sockfd, sendbuf, sizeof(sendbuf));
        read(sockfd, recvbuf, sizeof(recvbuf));

        fputs(recvbuf, stdout);
        memset(recvbuf, 0, sizeof(recvbuf));
        memset(sendbuf, 0, sizeof(sendbuf));
    }

    close(sockfd);

    return 0;
}
