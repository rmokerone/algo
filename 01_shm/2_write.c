
#include <sys/mman.h>
#include <sys/stat.h>        /* For mode constants */
#include <fcntl.h>           /* For O_* constants */
#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <stdlib.h>
#include <string.h>

#define MAXSIZE 1024*4
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

    // 4. 建立映射关系
    char *ptr = (char*)mmap(NULL, MAXSIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (ptr == MAP_FAILED) {
        perror("mmap failed: ");
        exit(1);
    }

    printf("mmap %s success\n", FILENAME);
    close(fd);

    // 5. 写入数据
    char *content = "hello world";
    strncpy(ptr, content, strlen(content));

    sleep(30);

    return 0;
}