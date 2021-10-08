

#ifndef __C_IPC_H__
#define __C_IPC_H__

#ifdef __cplusplus
extern "C" {
#endif

#include <stdio.h>
#include <errno.h>

#include <stdlib.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/sem.h>



#define ERR_EXIT(m) \
    do  \
    {   \
        perror(m);   \
        exit(EXIT_FAILURE);  \
    } while(0) 

union semun {
    int val; // value for SETVAL
    struct semid_ds *buf;   // buffer for IPC_STAT, IPC_SET
    unsigned short *array;  // array for GETALL, SETALL

    // Linux specific part
    struct seminfo *_buf;   // buffer for IPC_INFO
};

int sem_create(key_t key);
int sem_open(key_t key);

int sem_p(int semid);
int sem_v(int semid);
int sem_d(int semid);
int sem_setval(int semid, int val);
int sem_getval(int semid);

#ifdef __cplusplus
}
#endif

#endif 