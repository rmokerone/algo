
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>

#define MAXSIZE 1024*1024*16
#define FILENAME "shm.test"

int main() 
{
    // 1. 创建共享内存区对象
    int fd = shm_open(FILENAME, O_CREAT | O_TRUNC | O_RDWR, 0777);
    if (fd == -1){
        perror("shm_open failed: ");
        exit(-1);
    }

    // 2. 调整共享内存区对象大小
    if (ftruncate(fd, MAXSIZE) == -1){
        perror("ftruncate failed: ");
        exit(1);
    }

    // 3. 获取属性
    struct stat buf;
    if (fstat(fd, &buf) == -1){
        perror("fstat failed: ");
        exit(1);
    }

    printf("the shm object size is %ld\n", buf.st_size);

    sleep(30);
    
    // 如果引用计数为0，系统释放共享内存对象
    if (shm_unlink(FILENAME) == -1){
        perror("shm_unlink failed: ");
        exit(1);
    }
    printf("shm_unlink %s success\n", FILENAME);

}