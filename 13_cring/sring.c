
#include "sring.h"


sring_context* sring_create(int key, unsigned int size, unsigned int capacity, CRING_MEDIA_TYPE type, int force_unlock)
{
    sring_context* ctx = (sring_context*)malloc(sizeof(sring_context));
    ctx->key = key;
    ctx->shm_id = shmget(key, 0, 0);
    unsigned int full_size = cring_full_size(size, capacity, type);
    if (ctx->shm_id == -1){
        // 打开失败 创建共享内存
        ctx->shm_id = shmget(key, full_size, IPC_CREAT | 0666);
        unsigned char* buf = (unsigned char*)shmat(ctx->shm_id, NULL, 0);

        ctx->shm_start_addr =  cring_create_not_malloc(buf, size, capacity, type);
        if (ctx->shm_start_addr == NULL) return NULL;

        ctx->sem_mutex_id = sem_create(key);
        sem_setval(ctx->sem_mutex_id, 1);
    }else{
        printf("shm already create!\n");
        // 打开成功
        ctx->shm_start_addr = (cring_context*)shmat(ctx->shm_id, NULL, 0);
        ctx->sem_mutex_id = sem_open(key);
        if (force_unlock){
            sem_setval(ctx->sem_mutex_id, 1);
        }
    }
    return ctx;
}



int sring_put(sring_context *ctx, cring_frame *frm)
{
    int retval = -1;
    if (ctx == NULL) return -1;
    sem_p(ctx->sem_mutex_id);

    retval = cring_put(ctx->shm_start_addr, frm);

    sem_v(ctx->sem_mutex_id);

    return retval;

}

int sring_put_raw(sring_context *ctx, unsigned char* buf, unsigned int buf_size, CRING_FRAME_TYPE type)
{
    int retval = -1;
    if (ctx == NULL) return -1;
    sem_p(ctx->sem_mutex_id);

    retval = cring_put_raw(ctx->shm_start_addr, buf, buf_size, type);

    sem_v(ctx->sem_mutex_id);

    return retval;
}

int sring_get(sring_context *ctx, cring_frame *frm)
{
    int retval = -1;
    if (ctx == NULL) return -1;
    sem_p(ctx->sem_mutex_id);

    retval = cring_get(ctx->shm_start_addr, frm);

    sem_v(ctx->sem_mutex_id);

    return retval;
}



/**
 * @brief 清空sring缓冲区中的数据
 * 
 * @param ctx sring_context结构体
 */
void sring_reset(sring_context *ctx)
{
    if (ctx == NULL) return;
    sem_p(ctx->sem_mutex_id);

    cring_reset(ctx->shm_start_addr);

    sem_v(ctx->sem_mutex_id);
}


void sring_destroy(sring_context *ctx)
{
    if (ctx == NULL) return;

    free(ctx);
}