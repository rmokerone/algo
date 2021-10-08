#include <stdio.h>
#include "sring.h"

int main(void)
{
    int retval = 0;
    cring_frame *get_frm;
    sring_context *ctx = sring_create(111, 0, 0, 0, 0);
    if (ctx == 0){
        printf("sring_create error!\n");
        return -1;
    }else{
        printf("sring_create success!\n");
    }

    get_frm = cring_frame_malloc(ctx->shm_start_addr);


    retval = sring_get(ctx, get_frm);
    if (retval < 0){
        printf("sring_get error!\n");
    }else{
        printf("sring_get success!, retval=%d\n", retval);
    }

    printf("get_frm->type=%d\n", get_frm->frame_type);

    getchar();


    return 0;
}