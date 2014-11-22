#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include "commsocket.h"

void handle(int num){
    int pid = 0;
    while(pid = waitpid(-1, NULL, WNOHANG)){
        printf("child pid=%d\n", pid);
        fflush(stdout);
    }
}
int main()
{
    int ret = 0;
    int listenfd;
    signal(SIGCHLD, handle);
    sckServer_init(&listenfd);

}
