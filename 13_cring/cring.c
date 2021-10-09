
#include "cring.h"

unsigned int cring_full_size(unsigned int size, unsigned int capacity, CRING_MEDIA_TYPE type)
{
    return size * (capacity+sizeof(cring_frame_head)) + sizeof(cring_context);
}

/**
 * @brief 创建cring环形缓冲区
 * 
 * @param size 缓冲区中能够存放的frame数量
 * @param capacity 每个frame的最大容量，单位字节
 * @param type 缓冲区的类型
 * @return cring_context* 成功返回结构体指针，失败返回NULL
 */
cring_context* cring_create(unsigned int size, unsigned int capacity, CRING_MEDIA_TYPE type)
{
    unsigned int full_size = size * (capacity+sizeof(cring_frame_head)) + sizeof(cring_context);
    
    unsigned char* buf = (unsigned char*)malloc(full_size);
    if (buf == NULL){
        perror("cring_create ");
        return NULL;
    }


    return cring_create_not_malloc(buf, size, capacity, type);
}

cring_context* cring_create_not_malloc(unsigned char* buf, unsigned int size, unsigned int capacity, CRING_MEDIA_TYPE type)
{
    if (buf == NULL) return NULL;
    cring_context* ctx = (cring_context*)buf;

    ctx->in = 0;
    ctx->out = 0;
    ctx->n = 0;
    ctx->size = size;
    ctx->frame_capacity = capacity;
    ctx->type = type;

    unsigned char *frm_start_addr = (unsigned char*)(ctx+1);

    ctx->frm_full_size =  size*capacity;

    ctx->frm_per_size = capacity+sizeof(cring_frame_head);

    memset(frm_start_addr , 0, ctx->frm_full_size);

    return ctx;

}

/**
 * @brief 创建frame结构体
 * 
 * @param ctx cring结构体，提供信息给frame
 * @return cring_frame* 成功返回结构体指针，失败返回NULL
 */
cring_frame *cring_frame_malloc(cring_context *ctx)
{
    if (ctx == NULL) return NULL;
    
    cring_frame *frm = (cring_frame *)malloc(sizeof(cring_frame));

    frm->data = (unsigned char*)malloc(ctx->frame_capacity);

    frm->frame_capacity = ctx->frame_capacity;
    frm->frame_size = 0;
    frm->frame_type = CRING_FRAME_UNKNOWN;
    return frm;
}

int cring_addring(cring_context *ctx, int i)
{
    return (i + 1) == ctx->size ? 0 : i + 1;
}


/**
 * @brief 将一帧frame放入到cring缓冲区中
 * 
 * @param ctx cring结构体
 * @param frm frame结构体
 * @return int <0 失败，>=0 frame结构中数据区的大小
 */
int cring_put(cring_context *ctx, cring_frame *frm)
{
    if (ctx == NULL || frm == NULL) return -1;
    
    if (ctx->n >= ctx->size){
        return -1;
    }

    unsigned char *frm_start_addr = (unsigned char*)(ctx+1);

    unsigned char *dst_ptr = frm_start_addr + ctx->in * ctx->frm_per_size;

    printf("dst_ptr = %p\n", dst_ptr);

    unsigned int src_len = sizeof(cring_frame) + frm->frame_size;

    memset(dst_ptr, 0, ctx->frm_per_size);

    memcpy(dst_ptr, frm, sizeof(cring_frame_head));
    memcpy(dst_ptr+sizeof(cring_frame_head), frm->data, frm->frame_size);

    ctx->in = cring_addring(ctx, ctx->in);

    ctx->n += 1;

    return frm->frame_size;
}

/**
 * @brief 将裸数据放入到cring缓冲区中
 * 
 * @param ctx cring结构体
 * @param buf 裸数据地址
 * @param buf_size 裸数据大小
 * @param type 裸数据类型
 * @return int <0 失败，>=0 frame结构中数据区的大小
 */
int cring_put_raw(cring_context *ctx, unsigned char* buf, unsigned int buf_size, CRING_FRAME_TYPE type)
{
    if (ctx == NULL) return -1;

    if (ctx->n >= ctx->size){
        return -1;
    }

    unsigned char *frm_start_addr = (unsigned char*)(ctx+1);

    unsigned char *dst_ptr = frm_start_addr + ctx->in * ctx->frm_per_size;

    cring_frame_head *head = (cring_frame_head *)dst_ptr;
    head->frame_capacity = ctx->frame_capacity;
    head->frame_size = buf_size;
    head->frame_type = type;

    memcpy(dst_ptr+sizeof(cring_frame_head), buf, buf_size);

    ctx->in = cring_addring(ctx, ctx->in);

    ctx->n += 1;

    return buf_size;
}

/**
 * @brief 从cring中取出一帧数据，注意不是直接传出来指针，而是将cring中的数据通过memcpy复制到frm->data中; 
 *          frm 传入前必须经过cring_frame_malloc函数初始化;
 * 
 * @param ctx 初始化后的cring_context结构体；
 * @param frm 初始化后的cring_frame结构体
 * @return int <0 获取出错， >=0 获取成功
 */
int cring_get(cring_context *ctx, cring_frame *frm)
{
    if (ctx == NULL || frm == NULL) return -1;

    if (ctx->n <= 0){
        return -1;
    }
    unsigned int pos = ctx->out;
    ctx->out = cring_addring(ctx, ctx->out);
    ctx->n -= 1;

    unsigned char *frm_start_addr = (unsigned char*)(ctx+1);
    
    unsigned char *src_ptr = frm_start_addr + pos * ctx->frm_per_size;

    printf("src_ptr = %p\n", src_ptr);

    memcpy(frm, src_ptr, sizeof(cring_frame_head));
    unsigned int frame_size = ((cring_frame*)src_ptr)->frame_size;
    printf("in cring_get frame_size=%d\n", frame_size);
    memcpy(frm->data, src_ptr+sizeof(cring_frame_head), frame_size);

    return frm->frame_size;
}

/**
 * @brief 清空cring缓冲区中的数据
 * 
 * @param ctx cring_context结构体
 */
void cring_reset(cring_context *ctx)
{
    if (ctx == NULL) return;

    ctx->in = 0;
    ctx->out = 0;
    ctx->n = 0;
}


/**
 * @brief 释放frame结构体
 * 
 * @param frm cring_frame结构体
 */
void cring_frame_free(cring_frame *frm)
{
    if (frm == NULL){
        return ;
    }
    if (frm->data != NULL){
        free(frm->data);
    }

    free(frm);
}

/**
 * @brief 销毁cring_context结构体
 * 
 * @param ctx cring_context结构体
 */
void cring_destroy(cring_context *ctx)
{
    if (ctx == NULL){
        return;
    }
    free(ctx);
}