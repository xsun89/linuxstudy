#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <signal.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/errno.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                (Linux-specific) */
};
int sem_create(key_t key)
{
    int semid;
    semid = semget(key, 1, 0666 | IPC_CREAT | IPC_EXCL);
    if(semid == -1){
        perror("semget error");
        if(errno == EEXIST)
        {
            printf("sem already exist\n");
            return -2;
        }
        return -1;
    }

    return semid;
}

int sem_open(key_t key)
{
    int semid;
    semid = semget(key, 1, 0666) ;
    if(semid == -1){
        perror("semget error");
        return -1;
    }

    return semid;
}

int sem_setval(int semid, int val)
{
    union semun su;
    su.val = 1;
    int ret;
    ret = semctl(semid, 0, SETVAL, su);

    return ret;
}

int sem_getval(int semid, int *val)
{
    union semun su;
    *val = su.val;
    int ret;
    ret = semctl(semid, 0, GETVAL, su);
    return ret;
}
int main()
{
    int semid;
    int ret;
    ret = sem_create(0x1111);
    if(ret == -1)
    {
        perror("create error");
        return -1;
    }else if(ret == -2){
        printf("sem already exist\n");
        semid = sem_open(0x1111);
    }else{
        semid = ret;
    }

    sem_setval(semid, 1);
    int val;
    sem_getval(semid, &val);

    printf("sem get val=%d\n", val);
    return 0;
}
