#ifndef __UTHREAD_H__
#define __UTHREAD_H__

#include <ucontext.h>
#include <vector>

#define DEFAULT_STACK_SIZE (1024*128)
#define MAX_UTHREAD_SIZE 1024

enum ThreadState{FREE, RUNNABLE, RUNNING, SUSPEND};

struct schedule_t;

typedef void (*Fun)(void *arg);

typedef struct uthread_t
{
    ucontext_t ctx;
    Fun func;
    void *arg;
    enum ThreadState state;
    char stack[DEFAULT_STACK_SIZE];
} uthread_t;

typedef struct schedule_t
{
    ucontext_t main;
    int runing_thread;
    uthread_t *threads;
    int max_index;

    schedule_t(): runing_thread(-1), max_index(0) {
        threads = new uthread_t[MAX_UTHREAD_SIZE];
        for (int i = 0; i < MAX_UTHREAD_SIZE; i ++){
            threads[i].state = FREE;
        }
    }

    ~schedule_t() {
        delete [] threads;
    }
} schedule_t;


int uthread_create(schedule_t &schedule, Fun func, void *arg);

void uthread_yield(schedule_t &schedule);

void uthread_resume(schedule_t &schedule, int id);

int schedule_finished(const schedule_t &schedule);


#endif