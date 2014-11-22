#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <sys/signal.h>
#include "commsocket.h"

void handle(int num){
    int pid = 0;
    while((pid = waitpid(-1, NULL, WNOHANG))){
        printf("child pid=%d\n", pid);
        fflush(stdout);
    }
}
int main()
{
    int ret = 0;
    int listenfd;
    signal(SIGCHLD, SIG_IGN);
    signal(SIGPIPE, SIG_IGN);

    ret = sckServer_init(&listenfd, 8008);
    if(ret != 0){
        printf("sckServer_init() err:%d\n", ret);
        return ret;
    }
    while(1) {
        int connfd = 0;
        ret = sckServer_accept(listenfd, &connfd, 10);
        if (ret == SCK_ERRTIMEOUT) {
            printf("timeout...\n");
            continue;
        }
        int pid = fork();
        if (pid == 0) {
            unsigned char recvbuf[1024];
            int recvbuflen = 1024;
            close(listenfd);
			while(1){
				memset(recvbuf, 0, sizeof(recvbuf));
				ret = sckServer_rcv(connfd, recvbuf, &recvbuflen, 10);
				if (ret != 0) {
					printf("testserver func sckServer_recv() err:%d\n", ret);
                    break;
                }
				printf("received buf=%s\n", recvbuf);
                ret = sckServer_send(connfd, recvbuf, recvbuflen, 10);
                if (ret != 0) {
                    printf("testserver func sckServer_send() err:%d\n", ret);
                    break;
                }
            }
            close(connfd);
            exit(ret);
        } else if (pid > 0) {
            close(connfd);
        }
    }
    return ret;
}
