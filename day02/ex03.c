#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

void handle(int num)
{
    printf("signal arrived\n");
    if(num == SIGUSR1)
    {
        printf("got SIGUSR1\n");
    }
    else if(num == SIGUSR2)
    {
        sigset_t set;
        sigemptyset(&set);
        sigaddset(&set, SIGUSR1);
        sigprocmask(SIG_UNBLOCK, &set, NULL);
    }
}
int main()
{
    sigset_t set;
    sigemptyset(&set);
    sigaddset(&set, SIGUSR1);
    sigprocmask(SIG_BLOCK, &set, NULL);

    pid_t pid = fork();

    if(pid > 0)
    {
        if(signal(SIGUSR1, handle) == SIG_ERR)
        {
            perror("signal error");
            exit(-1);
        }

        if(signal(SIGUSR2, handle) == SIG_ERR)
        {
            perror("signal error");
            exit(-1);
        }
    }

    if(pid == 0)
    {
        printf("send SIGUSR1 first time\n");
        kill(getppid(), SIGUSR1);
        sleep(100);
        printf("send SIGUSR2\n");
        kill(getppid(), SIGUSR2);\
        sleep(300);
        printf("send SIGUSR1 second time\n");
        kill(getppid(), SIGUSR1);
        exit(0);
    }

    for(;;)
    {
        pause();
    }

    return 0;
}