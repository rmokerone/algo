
#ifndef __CRING_H__
#define __CRING_H__

#include <stdio.h>
#include <string.h>
#include <stdlib.h>

typedef enum  {
    CRING_MEDIA_AUDIO,
    CRING_MEDIA_VIDEO,
} CRING_MEDIA_TYPE;

typedef enum  
{
    CRING_FRAME_H264_I, 
    CRING_FRAME_H264_P, 
    CRING_FRAME_H264_B,
    CRING_FRAME_AAC,
    CRING_FRAME_UNKNOWN
} CRING_FRAME_TYPE;

typedef struct {
    unsigned int frame_capacity;
    unsigned int frame_size;
    CRING_FRAME_TYPE frame_type;
} cring_frame_head;

typedef struct {
    unsigned int frame_capacity;
    unsigned int frame_size;
    CRING_FRAME_TYPE frame_type;
    unsigned char *data;
} cring_frame;

typedef struct {
    unsigned int size;  // 总共有多少个frame
    unsigned int in;    // 数据进来的位置 
    unsigned int out;   // 输出出去的位置
    unsigned int n;     // cring当前有几个frame
    CRING_MEDIA_TYPE type;
    unsigned int frame_capacity;   // 每帧的最大的容量是多少
    // unsigned char* frm_start_addr;
    unsigned int frm_full_size;
    unsigned int frm_per_size;
} cring_context;

/**
 * @brief 创建cring环形缓冲区
 * 
 * @param size 缓冲区中能够存放的frame数量
 * @param capacity 每个frame的最大容量，单位字节
 * @param type 缓冲区的类型
 * @return cring_context* 成功返回结构体指针，失败返回NULL
 */
cring_context *cring_create(unsigned int size, unsigned int capacity, CRING_MEDIA_TYPE type);

cring_context* cring_create_not_malloc(unsigned char* buf, unsigned int size, unsigned int capacity, CRING_MEDIA_TYPE type);
/**
 * @brief 创建frame结构体
 * 
 * @param ctx cring结构体，提供信息给frame
 * @return cring_frame* 成功返回结构体指针，失败返回NULL
 */
cring_frame *cring_frame_malloc(cring_context *ctx);

/**
 * @brief 将一帧frame放入到cring缓冲区中
 * 
 * @param ctx cring结构体
 * @param frm frame结构体
 * @return int <0 失败，>=0 frame结构中数据区的大小
 */
int cring_put(cring_context *ctx, cring_frame *frm);

/**
 * @brief 将裸数据放入到cring缓冲区中
 * 
 * @param ctx cring结构体
 * @param buf 裸数据地址
 * @param buf_size 裸数据大小
 * @param type 裸数据类型
 * @return int <0 失败，>=0 frame结构中数据区的大小
 */
int cring_put_raw(cring_context *ctx, unsigned char* buf, unsigned int buf_size, CRING_FRAME_TYPE type);


/**
 * @brief 从cring中取出一帧数据，注意不是直接传出来指针，而是将cring中的数据通过memcpy复制到frm->data中; 
 *          frm 传入前必须经过cring_frame_malloc函数初始化;
 * 
 * @param ctx 初始化后的cring_context结构体；
 * @param frm 初始化后的cring_frame结构体
 * @return int <0 获取出错， >=0 获取成功
 */
int cring_get(cring_context *ctx, cring_frame *frm); 


/**
 * @brief 释放frame结构体
 * 
 * @param frm cring_frame结构体
 */
void cring_frame_free(cring_frame *frm);

/**
 * @brief 销毁cring_context结构体
 * 
 * @param ctx cring_context结构体
 */
void cring_destroy(cring_context *ctx);


unsigned int cring_full_size(unsigned int size, unsigned int capacity, CRING_MEDIA_TYPE type);


#endif 