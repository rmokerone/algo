#ifndef __SRING_H__
#define __SRING_H__

#include <sys/ipc.h>
#include <sys/shm.h>

#include "cring.h"
#include "ipc.h"

typedef struct
{
    int key;
    int sem_mutex_id;
    int shm_id;
    cring_context* shm_start_addr;
} sring_context;

sring_context* sring_create(int key, unsigned int size, unsigned int capacity, CRING_MEDIA_TYPE type, int force_unlock);


int sring_put(sring_context *ctx, cring_frame *frm);

int sring_get(sring_context *ctx, cring_frame *frm);


void sring_destroy(sring_context *ctx);

#endif 