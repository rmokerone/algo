
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include "cshmfifo.h"

int main()
{
    CShmFifo* f = new CShmFifo(5678, 10, 1024);

    char buf[1024] = {0};

    for (int i = 0; i < 1000; i ++){
        
        f->shmfifo_read(buf);
        printf("%s\n", buf);
        memset(buf, 0, 1024);
        usleep(500*1000);
    }
}