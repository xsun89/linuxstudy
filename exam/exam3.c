#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>

int pidarray[10];
void myhandle(int num, siginfo_t *st, void *p)
{
    printf("recv num:%d, data=%d\n", num, st->si_value.sival_int);
	if(num == SIGRTMIN + 1)
	{
		printf("child1 received signal\n");
		pidarray[1] = st->si_value.sival_int;
	}
    if(num == SIGRTMIN + 2)
    {
		printf("child2 received signal\n");
		pidarray[2] = st->si_value.sival_int;
    }

    if(num == SIGRTMIN + 3)
    {
		printf("parent received signal\n");
		printf("parent received %d\n", st->si_value.sival_int);
    }

}
int main()
{
    struct sigaction act;
    sigemptyset(&act.sa_mask);
    //sigaddset(&act.sa_mask, SIGQUIT);

    act.sa_flags = SA_SIGINFO;
    act.sa_sigaction = myhandle;
    if(sigaction(SIGRTMIN+1, &act, NULL) < 0)
    {
        perror("sigaction failed");
    }
    if(sigaction(SIGRTMIN+2, &act, NULL) < 0)
    {
        perror("sigaction2 failed");
    }
    if(sigaction(SIGRTMIN+3, &act, NULL) < 0)
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
		sleep(5);
        printf("child 1 wake up\n");
        union sigval mysigval;
		mysigval.sival_int = getpid()*2;
		int ret;
		ret = sigqueue(pidarray[1], SIGRTMIN+2, mysigval);

        exit(0);
    }
    if(pid == 0 && i==1)
    {
        printf("child 2\n");
		sleep(10);
        printf("child 2 wake up\n");
        union sigval mysigval;
		mysigval.sival_int = pidarray[2];
		int ret;
		ret = sigqueue(getppid(), SIGRTMIN+3, mysigval);
        exit(0);
    }
    if(pid > 0)
    {
        printf("parent \n");
		union sigval mysigval;
		mysigval.sival_int = pidarray[1];
		int ret;
		ret = sigqueue(pidarray[0], SIGRTMIN+1, mysigval);
    }
    int childpid = 0;
    sleep(5);
    while((childpid = waitpid(-1, NULL, WNOHANG)) >= 0)
    {
		sleep(1);
        printf("child pid:%d ended\n", childpid);
    }
    return 0;
}
