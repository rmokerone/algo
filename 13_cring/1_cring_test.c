
#include <stdio.h>
#include "cring.h"

int main(void)
{
    int retval = 0;
    
    cring_context *ctx;

    cring_frame *put_frm;
    cring_frame *get_frm;



    ctx = cring_create(10, 1024, CRING_MEDIA_VIDEO);


    put_frm = cring_frame_malloc(ctx);
    get_frm = cring_frame_malloc(ctx);

    put_frm->frame_type = CRING_FRAME_AAC;
    
    printf("put_frm=%p\n", put_frm);

    printf("put_frm->frame_type=%d\n", put_frm->frame_type);

    int i = 0;
    for (int i = 0; i < 10; i ++){
        put_frm->data[0] = i;
        put_frm->frame_size = 1;
        retval = cring_put(ctx, put_frm);
        if (retval < 0){
            printf("cring_put %d error!\n", i);
        }else{
            printf("cring_put %d success!, retval=%d\n", i, retval);
        }
    }

    cring_get(ctx, get_frm);

    put_frm->data[0] = 123;
    put_frm->frame_size = 1;

    cring_put(ctx, put_frm);

    // char buf[1];
    // buf[0] = 138;

    // cring_put_raw(ctx, buf, 1, CRING_FRAME_AAC);


    for (int i = 0; i < 10; i ++){
        retval = cring_get(ctx, get_frm);
        if (retval < 0){
            printf("cring_get %d error!\n", i);
        }else{
            printf("cring_get %d success!, retval=%d\n", i, retval);
            printf("get_frm[%d]->data=%d, %d, %p\n", i, get_frm->data[0], get_frm->frame_type, get_frm->data);
        }
    }

    printf("put_frm=%p\n", put_frm);
    cring_frame_free(put_frm);
    cring_frame_free(get_frm);

    cring_destroy(ctx);



    return 0;
}