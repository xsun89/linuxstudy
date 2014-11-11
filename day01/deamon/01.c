#include <sys/types.h>
#include <unistd.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <signal.h>
#include <errno.h>
#include <signal.h>

int my_deamon(int change_dir, int change_tty)
{
	int i = 0;
	pid_t pid;
	pid = fork();
	if(pid == -1)
	{
		perror("fork err");
	}

	if(pid > 0)
	{
		exit(0);
	}

	pid = setsid();
	if(pid == -1)
	{
		perror("setsid err");
		exit(0);
	}

	if(change_tty != 0)
	{
		chdir("/");
	}

	if(change_dir != 0)
	{
		for(i=0; i<3; i++)
		{
			close(i);
		}
		open("/dev/null", O_RDWR);
		dup(0);
		dup(0);
	}
	
	for(;;)
		;

}
int main()
{
	my_deamon(1, 1);
}
