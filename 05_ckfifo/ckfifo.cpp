

#include "ckfifo.h"
#include "ipc.h"
#include <string.h>
#include <iostream>

#define min(x,y) ({ \
        typeof(x) _x = (x);     \
        typeof(y) _y = (y);     \
        (void) (&_x == &_y);            \
        _x < _y ? _x : _y; })

CKFifo::CKFifo(int key,  unsigned int size)
{
    this->shm_id = shmget(key, 0, 0);
    int full_size = size + sizeof(ckfifo_t);
    if (this->shm_id == -1){
        // 打开失败  共享内存 不存在
        this->shm_id = shmget(key, full_size, IPC_CREAT | 0666);

        this->shm_start_addr = (ckfifo_t*)shmat(shm_id, NULL, 0);

        this->shm_buffer = (unsigned char*)(shm_start_addr + 1);

        // 给ckfifo头赋值
        shm_start_addr->in = 0;
        shm_start_addr->out = 0;
        shm_start_addr->size = size;

        // 创建信号量
        sem_mutex_id = sem_create(key);

        // 给信号量赋值
        sem_setval(this->sem_mutex_id, 1); // 对每块内存的操作 只能是读或者写
    }else{
        this->shm_start_addr = (ckfifo_t*)shmat(this->shm_id, NULL, 0);
        this->shm_buffer = (unsigned char*)(shm_start_addr + 1);

        // 获取信号量
        this->sem_mutex_id = sem_open(key);
    }
}

unsigned int CKFifo::ckfifo_put_chunk(unsigned char *buffer, unsigned int len)
{

    unsigned int l;
    sem_p(this->sem_mutex_id);

    // 放不下，直接返回0
    if (len > this->shm_start_addr->size - this->shm_start_addr->in + this->shm_start_addr->out){
        sem_v(this->sem_mutex_id);
        return 0;
    }

    /* first put the data starting from fifo->in to buffer end */
    l = min(len, this->shm_start_addr->size - (this->shm_start_addr->in & (this->shm_start_addr->size - 1)));
    memcpy(this->shm_buffer + (this->shm_start_addr->in & (this->shm_start_addr->size - 1)), buffer, l);

    /* then put the rest (if any) at the beginning of the buffer */
    memcpy(this->shm_buffer, buffer + l, len - l);

    this->shm_start_addr->in += len;    

    sem_v(this->sem_mutex_id);

    return len;
}

unsigned int CKFifo::ckfifo_get_chunk(unsigned char *buffer, unsigned int len)
{
    unsigned int l;
    sem_p(this->sem_mutex_id);
    if (len > this->shm_start_addr->in - this->shm_start_addr->out){
        sem_v(this->sem_mutex_id);
        return 0;
    }
    /* first get the data from fifo->out until the end of the buffer */
    l = min(len, this->shm_start_addr->size - (this->shm_start_addr->out & (this->shm_start_addr->size - 1)));
    memcpy(buffer, this->shm_buffer + (this->shm_start_addr->out & (this->shm_start_addr->size - 1)), l);

    /* then get the rest (if any) from the beginning of the buffer */
    memcpy(buffer + l,  this->shm_buffer, len - l);

    this->shm_start_addr->out += len;

    sem_v(this->sem_mutex_id);

    return len;
}

unsigned int CKFifo::ckfifo_len()
{
    sem_p(this->sem_mutex_id);
    unsigned int len = this->shm_start_addr->in - this->shm_start_addr->out;
    sem_v(this->sem_mutex_id);
    return len;
}