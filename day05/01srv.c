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
    //inet_aton("192.168.30.188", &inAddr);
    addr.sin_addr.s_addr = inet_addr("192.168.30.188");

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
    if((conn = accept(sockfd, (struct sockaddr *)&peerAddr, (socklen_t *)&peerAddrLen)) < 0)
    {
        perror("func accept");
        exit(0);
    }
    printf("peer address:%s \t peerport:%d\n", inet_ntoa(peerAddr.sin_addr), ntohs(peerAddr.sin_port));
    char buf[1024] = {0};
    while(1)
    {
        int ret = read(conn, buf, sizeof(buf));
        if(ret == 0)
        {
            printf("peer reset");
            exit(0);
        }else if(ret < 0)
        {
            perror("read fail");
        }

        fputs(buf, stdout);
        write(conn, buf, sizeof(buf));
    }

    close(conn);
    close(sockfd);
    return 0;
}
