#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "ckfifo.h"

int main()
{
    CKFifo* f = new CKFifo(5678, 1024);

    char buf[1024] = {0};
    int ret = 0;
    for (int i = 0; i < 1000; i ++){
        sprintf(buf, "my id = %03d", i);
        ret = f->ckfifo_put_chunk((unsigned char*)buf, strlen(buf));
        printf("%s | ret = %d\n", buf, ret);
        memset(buf, 0, 1024);
        usleep(500*1000);
    }
}