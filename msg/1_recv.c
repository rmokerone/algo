
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <errno.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

struct  msg_st
{
    /* data */
    long int msg_type;
    char text[BUFSIZ];
};


int main()
{
    int msgid = -1;
    struct msg_st data;
    long int msgtype = 0; 

    // 1. 建立消息队列
    msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
    if (msgid == -1){
        perror("msgget error: ");
        exit(EXIT_FAILURE);
    }
    printf("msgid = %d\n", msgid);

    // 2. 从队列中获取消息 直到遇到end结束
    while (1) {
        if (msgrcv(msgid, (void*)&data, BUFSIZ, msgtype, 0) == -1)
        {
            perror("msgrcv error: ");
            exit(EXIT_FAILURE);
        }
        printf("recv: %s\n", data.text);
        if (strncmp(data.text, "end", 3) == 0){
            break;
        }
    }

    // 3. 删除消息队列
    if (msgctl(msgid, IPC_RMID, 0) == -1){
        perror("msgctl error: ");
        exit(EXIT_FAILURE);
    }

    return 0;
}