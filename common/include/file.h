#pragma once

#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <string.h>
#include <stdlib.h>

size_t get_file_size(const char *filepath)
{
    /*check input para*/
    if(NULL == filepath)
        return 0;
    struct stat filestat;
    memset(&filestat,0,sizeof(struct stat));
    /*get file information*/
    if(0 == stat(filepath,&filestat))
        return filestat.st_size;
    else
        return 0;
}

char *read_file_to_buf(const char *filepath)
{
    /*check input para*/
    if(NULL == filepath)
    {
        return NULL;
    }
    /*get file size*/
    size_t size = get_file_size(filepath);
    if(0 == size)
        return NULL;
        
    /*malloc memory*/
    char *buf = (char*)malloc(size+1);
    if(NULL == buf)
        return NULL;
    memset(buf,0,size+1);
    
    /*read string from file*/
    FILE *fp = fopen(filepath,"r");
    size_t readSize = fread(buf,1,size,fp);
    if(readSize != size)
    {
        /*read error*/
        free(buf);
        buf = NULL;
    }

    buf[size] = 0;
    return buf;
}