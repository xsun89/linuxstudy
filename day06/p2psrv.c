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
#include <bootstrap.h>
#include <signal.h>
#include <sys/signal.h>
void handler(int num)
{
    printf("parent received signal\n");
    exit(0);
}
int main()
{
    signal(SIGUSR1, handler);
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
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    int optionVal = 1;
    if(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, (void *)&optionVal, sizeof(optionVal)) < 0)
    {
        perror("setsockopt");
        exit(0);
    }
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
    if ((conn = accept(sockfd, (struct sockaddr *) &peerAddr, (socklen_t * ) & peerAddrLen)) < 0) {
        perror("func accept");
        exit(0);
    }
    printf("peer address:%s \t peerport:%d\n", inet_ntoa(peerAddr.sin_addr), ntohs(peerAddr.sin_port));
    pid_t pid = fork();
    if(pid == 0)
    {
        char buf[1024] = {0};
        while (1) {
            int ret = read(conn, buf, sizeof(buf));
            if (ret == 0) {
                printf("peer reset");
                break;
            } else if (ret < 0) {
                perror("read fail");
                break;
            }

            fputs(buf, stdout);
            memset(buf, 0, sizeof(buf));
        }
        close(conn);
        kill(getppid(), SIGUSR1);
        exit(0);
    }

    if(pid > 0)
    {
        char sendbuf[1024] = {0};
        while(fgets(sendbuf, sizeof(sendbuf), stdin) != NULL) {
            write(conn, sendbuf, sizeof(sendbuf));
            memset(sendbuf, 0, sizeof(sendbuf));
        }
    }
    close(sockfd);
    return 0;
}
