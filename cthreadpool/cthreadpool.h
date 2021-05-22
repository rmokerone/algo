#pragma once

#include <list>
#include <queue>
#include <pthread.h>
#include "ctask.h"

class CThreadPool
{
    public:
        CThreadPool(int num);
        ~CThreadPool();
        
        void add_task(CTask *t);


        int start();

        // 条件变量
        pthread_cond_t cond;
        // 互斥锁
        pthread_mutex_t mutex;  // 用来保护整个 CThreadPool 对象的

        // 任务队列
        std::queue<CTask*> tasks;

        std::list<pthread_t> tids;

        // 结束标志
        bool is_runing;

    private:
        int thread_count;



};