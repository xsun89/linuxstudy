#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
struct msg_buf {
    long mtype;       /* message type, must be > 0 */
    char mtext[1024];    /* message data */
};

int main()
{
    key_t key;
    int msgid;
    int ret;
    struct msg_buf msgbuf;
    int msgtype = getpid();
    printf("getpid(): %d \n", getpid());
    key = ftok("/tmp/", 'a');
    printf("key=[%x]\n", key);
    msgid = msgget(key, IPC_CREAT | IPC_EXCL | 0666);
    if(msgid == -1)
    {
        if(msgid == EEXIST){
            printf("EEXIST:.....\n");
            key = ftok("./msgfile", 'a');
            msgid = msgget(key, IPC_CREAT | 0666);
        }else{
            printf("create error\n");
            perror("mget errro:\n");
            return -1;
        }
    }

    printf("msgid:%d \n", msgid);
    msgbuf.mtype = msgtype;
    strcpy(msgbuf.mtext, "test data");
    ret = msgsnd(msgid, &msgbuf, sizeof(msgbuf.mtext), IPC_NOWAIT);
    if(ret == -1)
    {
        printf("send message error\n");
        perror("send error");
        return -1;
    }
    sleep(1);
    memset(&msgbuf, 0, sizeof(msgbuf));
    ret = msgrcv(msgid, &msgbuf, sizeof(msgbuf.mtext), msgtype, IPC_NOWAIT);
    if(ret == -1)
    {
        printf("recv message error\n");
        perror("recv error");
        return -1;
    }

    printf("revc msg =[%s]\n", msgbuf.mtext);
    return 0;
}
