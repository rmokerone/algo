

#include "cshmfifo.h"
#include "ipc.h"
#include <string.h>
#include <iostream>

CShmFifo::CShmFifo(int key, int blocks, int blksize)
{
    this->shm_id = shmget(key, 0, 0);
    int size = blocks * blksize + sizeof(shmhead_t);
    if (this->shm_id == -1)
    {
        // 打开失败 说明共享内存不存在
        this->shm_id = shmget(key, size, IPC_CREAT | 0666);

        this->shm_start_addr = (shmhead_t*)shmat(shm_id, NULL, 0);

        this->shm_data_addr = (char*)(shm_start_addr + 1);

        // 给头部幅值
        shm_start_addr->rd_index = 0;
        shm_start_addr->wr_index = 0;
        shm_start_addr->blksize = blksize;
        shm_start_addr->blocks = blocks;

        // 创建信号量
        this->sem_preoducer_id = sem_create(key);  // 创建生产者信号量
        this->sem_consumer_id = sem_create(key+1);  // 创建消费者信号量
        this->sem_mutex_id = sem_create(key+2);

        // 给信号量幅值
        sem_setval(this->sem_preoducer_id, blocks);  // 共享内存有多少个块，就有多大信号量，可以生产多少个blocks
        sem_setval(this->sem_consumer_id, 0);  // 可供消耗的信号量
        sem_setval(this->sem_mutex_id, 1);  // 对每块内存的操作 只能是读或者写
    }else{
        // 原来已经创建好共享内存和信号量
        this->shm_start_addr = (shmhead_t*)shmat(this->shm_id, NULL, 0);

        this->shm_data_addr = (char*)(shm_start_addr + 1);

        // 获取信号量
        this->sem_preoducer_id = sem_open(key);
        this->sem_consumer_id = sem_open(key+1);
        this->sem_mutex_id = sem_open(key+2);
    }
}

void CShmFifo::shmfifo_write(char* buf)
{
    // 先占用一个生产者的信号量
    sem_p(this->sem_preoducer_id);

    sem_p(this->sem_mutex_id);
    // 把buf写入到共享内存的其中一个块里面
    memcpy(this->shm_data_addr + this->shm_start_addr->wr_index * this->shm_start_addr->blksize, buf, this->shm_start_addr->blksize);

    // 写了之后 head中的wr_index+1 
    shm_start_addr->wr_index = (shm_start_addr->wr_index + 1) % shm_start_addr->blocks;
    sem_v(this->sem_mutex_id);

    sem_v(this->sem_consumer_id);
}

void CShmFifo::shmfifo_read(char* buf)
{
    // 把共享内存中对应位置的数据取出到buf

    sem_p(this->sem_consumer_id);

    sem_p(this->sem_mutex_id);
    memcpy(buf, this->shm_data_addr + this->shm_start_addr->rd_index * this->shm_start_addr->blksize, this->shm_start_addr->blksize);
    shm_start_addr->rd_index = (shm_start_addr->rd_index + 1) % shm_start_addr->blocks;

    sem_v(this->sem_mutex_id);

    sem_v(this->sem_preoducer_id); 
}