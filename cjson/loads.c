

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "cJSON/cJSON.h"
#include "io/file.h"


int main()
{
    cJSON* cjson_test = NULL;

    // 1. 读取文件到堆区
    char *buf = read_file_to_buf("conf.json");
    if (NULL == buf){
        perror("read_file_to_buf error: ");
        exit(EXIT_FAILURE);
    }

    // 2. 处理数据
    cjson_test = cJSON_Parse(buf);

    // 获取基础键值
    cJSON* cjson_name = cJSON_GetObjectItem(cjson_test, "name");
    cJSON* cjson_age = cJSON_GetObjectItem(cjson_test, "age");
    cJSON* cjson_weight = cJSON_GetObjectItem(cjson_test, "weight");
    printf("name: %s\n", cjson_name->valuestring);
    printf("age: %d\n", cjson_age->valueint);
    printf("weight: %f\n", cjson_weight->valuedouble);

    // 解析嵌套的json数据
    cJSON* cjson_address = cJSON_GetObjectItem(cjson_test, "address");
    cJSON* cjson_address_country = cJSON_GetObjectItem(cjson_address, "country");
    cJSON* cjson_address_zipcode = cJSON_GetObjectItem(cjson_address, "zip-code");
    printf("address-country: %s\n", cjson_address_country->valuestring);
    printf("address-zip-code: %d\n", cjson_address_country->valueint);

    // 解析数组
    cJSON* cjson_skill_item;
    cJSON* cjson_skill = cJSON_GetObjectItem(cjson_test, "skill");
    int skill_array_size = cJSON_GetArraySize(cjson_skill);
    printf("skill:[");
    for (int i = 0; i < skill_array_size; i ++){
        cjson_skill_item = cJSON_GetArrayItem(cjson_skill, i);
        printf("%s, ", cjson_skill_item->valuestring);
    }
    printf("\b]\n");

    // 解析布尔型数据
    cJSON* cjson_student = cJSON_GetObjectItem(cjson_test, "student");
    if (cjson_student->valueint == 0){
        printf("student: false\n");
    }else{
        printf("student: true\n");
    }

    return 0;
}