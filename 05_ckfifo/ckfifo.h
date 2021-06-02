
#pragma once

#include <sys/ipc.h>
#include <sys/shm.h>

typedef struct ckfifo {
    unsigned int size;      /* the size of the allocated buffer */
    unsigned int in;        /* data is added at offset (in % size) */
    unsigned int out;       /* data is extracted from off. (out % size) */
} ckfifo_t;

class CKFifo
{
    public:
        CKFifo(int key, unsigned int size);

        int sem_mutex_id; // 互斥信号量 保证对同一块内存 只能进行读或者写 不能同时进行 值只能为1

        int shm_id; // 共享内存id

        ckfifo_t* shm_start_addr; // 共享内存的启始地址

        unsigned char* shm_buffer; // 共享数据存放真实数据地址

        unsigned int ckfifo_put_chunk(unsigned char *buffer, unsigned int len);

        unsigned int ckfifo_get_chunk(unsigned char *buffer, unsigned int len);

        unsigned int ckfifo_len();

};