#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>

void mysigaction(int num, siginfo_t *info, void *p)
{
    if(num == SIGINT)
    {
        printf("got SIGIN\n");
        int data = info->si_value.sival_int;
        printf("got data=%d\n", data);
    }
}

int main()
{
    struct sigaction act;
    act.sa_sigaction = mysigaction;
    act.sa_flags = SA_SIGINFO;
    int ret = sigaction(SIGINT, &act, NULL);
    if(ret == -1)
    {
        perror("sigaction error");
        exit(-1);
    }

    for(;;)
    {
        sleep(1);
    }
    return 0;
}
