
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "ckfifo.h"

int main()
{
    CKFifo* f = new CKFifo(5678, 1024);

    char buf[1024] = {0};

    for (int i = 0; i < 1000; i ++){
        
        f->ckfifo_get_chunk((unsigned char*)buf, 11);
        printf("%s\n", buf);
        memset(buf, 0, 1024);
        usleep(500*1000);
    }
}