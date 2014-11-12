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
#include <sys/signal.h>

void handler(int num)
{
    printf("child received signal\n");
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
    inet_aton("127.0.0.1", &inAddr);
    addr.sin_addr = inAddr;
    
    if(connect(sockfd, (struct sockaddr *)&addr, sizeof(addr)) < 0)
    {
        perror("func connect");
        exit(0);
    }
    pid_t pid = fork();
    if(pid == 0)
    {
        char sendbuf[1024] = {0};
        while(fgets(sendbuf, sizeof(sendbuf), stdin) != NULL) {
            write(sockfd, sendbuf, sizeof(sendbuf));
            memset(sendbuf, 0, sizeof(sendbuf));
        }
    }

    if(pid > 0)
    {
        char recvbuf[1024] = {0};
        while(1) {
            int ret = read(sockfd, recvbuf, sizeof(recvbuf));
            if (ret == 0) {
                printf("peer reset");
                break;
            } else if (ret < 0) {
                perror("read fail");
                break;
            }
            fputs(recvbuf, stdout);
            memset(recvbuf, 0, sizeof(recvbuf));
        }
        close(sockfd);
        kill(pid, SIGUSR1);
        exit(0);
    }

    close(sockfd);

    return 0;
}
