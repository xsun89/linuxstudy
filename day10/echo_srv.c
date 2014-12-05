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

void echo_srv(int msgid)
{
    int n;
    struct msgbuf msg;
    memset(&msg, 0, sizeof(msg));
    while(1)
    {
        if((n = msgrcv(msgid, &msg, MSGMAX, 1, 0)) < 0)
        {
            perror("msgrcv err");
            return;
        }
        int pid;
        pid = *((int *)msg.mtext);
        msg.mtype = pid;
        fputs(msg.mtext+4, stdout);
        if(msgsnd(msgid, &msg, n, 0) < 0){
            perror("msgsnd error");
            return;

        }
    }
}

int main()
{
    int msgid;
    msgid = msgget(0x1234, IPC_CREAT | 0666);
    if(msgid == -1)
    {
        perror("msgget err");
        return -1;
    }

    echo_srv(msgid);

    return 0;
}