
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


int main(){
    int msgid = -1;
    char buffer[BUFSIZ];
    msgid = msgget((key_t)1234, 0666 | IPC_CREAT);
    struct msg_st data;

    if (msgid == -1){
        perror("msgget error: ");
        exit(EXIT_FAILURE);
    }
    printf("msgid = %d\n", msgid);

    while (1){
        printf("Enter some text: ");
        fgets(buffer, BUFSIZ, stdin);
        data.msg_type = 1;
        strcpy(data.text, buffer);
        // 向队列发送数据
        if (msgsnd(msgid, (void*)&data, BUFSIZ, 0) == -1){
            perror("msgsnd error: ");
            exit(EXIT_FAILURE);
        }

        if (strncmp(buffer, "end", 3) == 0){
            break;
        }
    }
    
    return 0;
}