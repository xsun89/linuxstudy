#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#define MSGMAX 8092
struct msgbuf
{
    long mtype;
    char mtext[MSGMAX];
};
void echo_cli(int msgid)
{
    int n;
    int pid;
    pid = getpid();
    struct msgbuf msg;
    memset(&msg, 0, sizeof(msg));
    *((int *)msg.mtext) = pid;
    msg.mtype = 1;

    while(fgets(msg.mtext+4, MSGMAX, stdin))
    {
        if(msgsnd(msgid, &msg, 4+strlen(msg.mtext+4), 0) < 0){
            perror("msgsnd error");
            return;
        }
        memset(msg.mtext+4, 0, MSGMAX-4);
        if((n = msgrcv(msgid, &msg, MSGMAX, pid, 0)) < 0)
        {
            perror("msgrcv err");
            return;
        }
        fputs(msg.mtext+4, stdout);
        memset(msg.mtext+4, 0, MSGMAX-4);
    }
}

int main()
{
    int msgid;
    msgid = msgget(0x1234, 0);
    if(msgid == -1)
    {
        perror("msgget err");
        return -1;
    }

    echo_cli(msgid);

    return 0;
}