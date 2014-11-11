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
#include <sys/wait.h>
#include <stddef.h>

int pidarray[10];
void myhandle(int num, siginfo_t *st, void *p)
{
    printf("recv num:%d, data=%d\n", num, st->si_value.sival_int);
}
int main()
{
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    sigaddset(&act.sa_mask, SIGQUIT);

    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = myhandle;
    if(sigaction(SIGINFO, &act, NULL) < 0)
    {
        perror("sigaction failed");
    }
    if(sigaction(SIGUSR1, &act, NULL) < 0)
    {
        perror("sigaction2 failed");
    }
    if(sigaction(SIGUSR2, &act, NULL) < 0)
    {
        perror("sigaction3 failed");
    }

    int i = 0;
    int pid = 0;
    for(i=0; i<2; i++)
    {
        pidarray[i] = pid = fork();
        if(pid == 0)
        {
            break;
        }
    }
    if(pid == 0 && i==0)
    {
        printf("child 1\n");
        exit(0);
    }
    if(pid == 0 && i==1)
    {
        printf("child 2\n");
        exit(0);
    }
    if(pid > 0)
    {
        printf("parent \n");
    }
    int childpid = 10;
    while((childpid = wait(NULL)) > 0)
    {
        printf("child pid:%d ended\n", childpid);
    }
    return 0;
}
