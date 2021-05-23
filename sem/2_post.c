
#include <stdlib.h>
#include <stdio.h>
#include <fcntl.h>           /* For O_* constants */
#include <sys/stat.h>        /* For mode constants */
#include <semaphore.h>
#include <unistd.h>

/* man 7 sem_overview */

int main()
{
    // 1. 创建信号量 并打开

    char *name = "/test";
    unsigned int value = 0;
    sem_t *sem = sem_open(name, O_RDWR | O_CREAT | O_EXCL, 0777, value);
    // 如果创建失败 尝试打开
    if (sem == SEM_FAILED) {
        perror("sem_open create failed");

        // 尝试打开之前别的进程创建好的信号量文件
        sem = sem_open(name, O_RDWR);
        // 打开失败
        if (sem == SEM_FAILED){
            perror("sem_open open failed");
            exit(EXIT_FAILURE);
        }
    }
    printf("sem_open %s success\n", name);

    int max_post = 5;
    int gvalue = 0;
    for (int i = 0; i < max_post; i ++){
        if (sem_post(sem) != -1) {
            printf("sem_post success, ");
        }
        if (sem_getvalue(sem, &gvalue) != -1){
            printf("the semaphore value = %d\n", gvalue);
        }
    }

    sem_close(sem);

    // if (sem_unlink(name) != -1)
    //     printf("sem_unlink %s success\n", name);


    return 0;

}