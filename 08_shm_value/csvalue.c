#include "csvalue.h"

#include <stdio.h>
#include <string.h>
#include "ipc.h"

// 打开csvalue
st_csvalue* csvalue_open(int key, int user_data_size)
{
    // 创建context
    st_csvalue* context = (st_csvalue*)malloc(sizeof(st_csvalue));

    context->user_data_size = user_data_size;

    context->shm_id = shmget(key, 0, 0);
    if (context->shm_id == -1){
        // 打开失败 创建贡献内存
        context->shm_id = shmget(key, user_data_size, IPC_CREAT | 0666);

        // 指定用户数据地址
        context->user_data = (void*)shmat(context->shm_id, NULL, 0);

        // 创建信号量
        context->sem_mutex_id = sem_create(key);

        // 给信号量赋值
        sem_setval(context->sem_mutex_id, 1);
    }else{
        // 打开成功
        context->user_data =  (void*)shmat(context->shm_id, NULL, 0);
        
        // 获取信号量
        context->sem_mutex_id = sem_open(key);
    }

    return context;
}

// 更新csvalue
void csvalue_push(st_csvalue* context, void* user_data)
{
    sem_p(context->sem_mutex_id);

    memcpy(context->user_data, user_data, context->user_data_size);

    sem_v(context->sem_mutex_id);
}

// 获取csvalue
void csvalue_pull(st_csvalue* context, void* user_data)
{
    sem_p(context->sem_mutex_id);

    memcpy(user_data, context->user_data, context->user_data_size);

    sem_v(context->sem_mutex_id);
}

void csvalue_close(st_csvalue* context)
{
    sem_d(context->sem_mutex_id);

    shmctl(context->shm_id, IPC_RMID, NULL);

    free(context);
}
