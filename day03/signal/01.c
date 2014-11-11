#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <signal.h>
#include <errno.h>
#include <signal.h>

void myhandle(int num)
{
	printf("number is %d\n", num);
}
int main(void)
{
	sig_t oldHandle;
	printf("main ....begin\n");
	oldHandle = signal(SIGINT, myhandle);
	if (oldHandle == SIG_ERR)
	{
		perror("func signal err\n");
		return 0;
	}
    printf("if u enter a, reset signal \n");
	while(getchar() != 'a')
	{
		;
	}
    //键入a以后，恢复默认函数
	/*
	if (signal(SIGINT, oldHandle) == SIG_ERR)
	{
		perror("func signal err\n");
		return 0;
	}
	*/
	if (signal(SIGINT, SIG_DFL) == SIG_ERR)
	{
		perror("func signal err\n");
		return 0;
	}
    
	while(1) ;
	return 0;

}
