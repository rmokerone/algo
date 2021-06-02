
#include "cthreadpool.h"
#include <stdio.h>
#include <unistd.h>
#include <errno.h>

void *start_routine(void *arg)
{
    CThreadPool *pool = (CThreadPool*)arg;

    while (pool->is_runing){
        while (pthread_mutex_trylock(&pool->mutex) != 0){
            if (!pool->is_runing) {
                return nullptr;
            }
        }  

        // 等待任务队列不为空
        while (pool->tasks.empty()){
            if (!pool->is_runing) {
                pthread_cond_signal(&pool->cond);
                pthread_mutex_unlock(&pool->mutex);
                return nullptr;
            }
            pthread_cond_wait(&pool->cond, &pool->mutex); 
        }

        // 获取一个任务
        CTask *task = pool->tasks.front();
        pool->tasks.pop();

        pthread_mutex_unlock(&pool->mutex);

        // 执行任务
        task->run();

    }

    return nullptr;
}

CThreadPool::CThreadPool(int num)
:is_runing(false), thread_count(num) 
{
    // 初始化 条件变量 互斥量
    pthread_cond_init(&cond, NULL);

    pthread_mutex_init(&mutex, NULL);
}

CThreadPool::~CThreadPool()
{
    is_runing = false;

    pthread_cond_signal(&cond);
    for (auto it = tids.cbegin(); it != tids.cend(); ++it) {
        
        pthread_join(*it, NULL);
        printf("thread %lu exit...\n", *it);
    }

    pthread_cond_destroy(&cond);
    pthread_mutex_destroy(&mutex);
}

int CThreadPool::start() 
{
    is_runing = true;
    // 创建thread_count个线程 并让其等待
    for (int i = 0 ; i < thread_count; i ++){
        pthread_t tid;
        int ret = pthread_create(&tid, NULL, start_routine, this);
        if (ret != 0){
            perror("create thread error: ");
            return -1;
        }
        printf("thread %lu create...\n", tid);

        tids.push_back(tid);
    }
    return thread_count;
}

void CThreadPool::add_task(CTask *t)
{
    pthread_mutex_lock(&mutex);

    tasks.push(t);

    pthread_cond_signal(&cond);

    pthread_mutex_unlock(&mutex);


}


