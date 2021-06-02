
#pragma once

#include <sys/ipc.h>
#include <sys/shm.h>
#include <stdbool.h>

typedef struct{
    int sem_mutex_id;   // 互斥信号量 保证多个进程 同时只能有一个读或写
    int shm_id;     // 共享内存id
    int user_data_size;     // 用户数据的地址
    void* user_data;    // 存放用户数据的内存地址
} st_csvalue;

// 打开csvalue
st_csvalue* csvalue_open(int key, int user_data_size);

// 更新csvalue
void csvalue_push(st_csvalue* context, void* user_data);

// 获取csvalue
void csvalue_pull(st_csvalue* context, void* user_data);

void csvalue_close(st_csvalue* context);