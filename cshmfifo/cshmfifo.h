
#pragma once

#include <sys/ipc.h>
#include <sys/shm.h>

typedef struct shmhead
{
    int wr_index; // 当前共享内存写到哪个位置
    int rd_index; // 当前共享内存读取到哪个位置
    int blocks;     // 当前共享内存有多少块
    int blksize;    // 每个块多大
} shmhead_t;


class CShmFifo
{
    public:
        CShmFifo(int key, int blocks, int blksize);

        int sem_preoducer_id; // 生产者信号量；可以生产的数量  // 向共享内存中存数据
        int sem_consumer_id; // 消费者信号量；可以消费的数量   // 从共享内存中取数据

        int sem_mutex_id; // 互斥信号量 保证对同一块内存 只能进行读或者写 不能同时进行 值智能为1

        int shm_id; // 共享内存id;

        shmhead_t * shm_start_addr; // 共享内存的启始地址
        char* shm_data_addr; // 共享内存存放真实数据地址

        



        void shmfifo_read(char* buf);
        void shmfifo_write(char* buf);

};