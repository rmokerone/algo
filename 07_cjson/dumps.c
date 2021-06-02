
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON.h"


int main(void)
{
    cJSON* cjson_test = NULL;
    cJSON* cjson_address = NULL;
    cJSON* cjson_skill = NULL;
    char* str;

    // 创建一个JSOn数据对象(链接头结点)
    cjson_test = cJSON_CreateObject();

    // 添加一个字符串类型的json数据
    cJSON_AddStringToObject(cjson_test, "name", "vroot");

    // 添加一个整数类型的json数据
    cJSON_AddNumberToObject(cjson_test, "age", 32);

    // 添加一个浮点类型的json数据
    cJSON_AddNumberToObject(cjson_test, "weight", 55.5);

    // 添加一个嵌套的json数据
    cjson_address = cJSON_CreateObject();
    cJSON_AddStringToObject(cjson_address, "country", "China");
    cJSON_AddNumberToObject(cjson_address, "zip-code", 111111);
    cJSON_AddItemToObject(cjson_test, "address", cjson_address);

    // 添加一个数组类型的json数据
    cjson_skill = cJSON_CreateArray();
    cJSON_AddItemToArray(cjson_skill, cJSON_CreateString("C"));
    cJSON_AddItemToArray(cjson_skill, cJSON_CreateString("Java"));
    cJSON_AddItemToArray(cjson_skill, cJSON_CreateString("Python"));
    cJSON_AddItemToObject(cjson_test, "skill", cjson_skill);

    // 添加一个值为False的json数据
    cJSON_AddFalseToObject(cjson_test, "student");

    // 打印整个json对象
    str = cJSON_Print(cjson_test);
    printf("%s\n", str);
    

    FILE* fp = fopen("conf.json", "w+");
    if (fp == NULL){
        perror("fopen error: ");
        exit(EXIT_FAILURE);
    }

    fwrite(str, strlen(str), 1, fp);

    fclose(fp);

    // 释放json对象空间
    // cJSON_Delete(cjson_skill);
    // cJSON_Delete(cjson_address);
    cJSON_Delete(cjson_test);

    // 释放char* 空间
    free(str);


    return 0;
}