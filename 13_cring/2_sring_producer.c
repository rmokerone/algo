#include <stdio.h>
#include "sring.h"

int main(void)
{
    int retval = 0;
    cring_frame *put_frm;
    sring_context *ctx = sring_create(111, 10, 1024, CRING_MEDIA_AUDIO, 1);
    if (ctx == 0){
        printf("sring_create error!\n");
        return -1;
    }else{
        printf("sring_create success!\n");
    }

    put_frm = cring_frame_malloc(ctx->shm_start_addr);

    put_frm->frame_type = CRING_FRAME_AAC;
    
    put_frm->data[0] = 123;
    put_frm->frame_size = 1;

    retval = sring_put(ctx, put_frm);
    printf("retval = %d\n", retval);

    getchar();


    return 0;
}