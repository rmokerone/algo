
#include "uthread.h"
#include <stdio.h>

static void uthread_body(schedule_t *ps);

int uthread_create(schedule_t &schedule, Fun func, void *arg)
{
    int id = 0;
    for (id = 0 ; id < schedule.max_index; id++)
    {
        if (schedule.threads[id].state == FREE){
            break;
        }
    }

    if (id == schedule.max_index){
        schedule.max_index++;
    }

    uthread_t *t = &(schedule.threads[id]);

    t->state = RUNNABLE;
    t->func = func;
    t->arg = arg;


    return id;
}
 

void uthread_body(schedule_t *ps)
{
    int id = ps->runing_thread;

    if (id != -1){
        uthread_t *t = &(ps->threads[id]);

        t->func(t->arg);

        t->state = FREE;

        ps->runing_thread = -1;
    }
}

void uthread_yield(schedule_t &schedule)
{
    if (schedule.runing_thread != -1){
        uthread_t *t = &(schedule.threads[schedule.runing_thread]);
        t->state = SUSPEND;

        schedule.runing_thread = -1;

        swapcontext(&(t->ctx), &(schedule.main));
    }
}

void uthread_resume(schedule_t &schedule, int id)
{
    if (id < 0 || id >= schedule.max_index){
        return;
    }

    uthread_t *t = &(schedule.threads[id]);
    
    switch (t->state)
    {
        case RUNNABLE:
            getcontext(&(t->ctx));

            t->ctx.uc_stack.ss_sp = t->stack;
            t->ctx.uc_stack.ss_size = DEFAULT_STACK_SIZE;
            t->ctx.uc_stack.ss_flags = 0;
            t->ctx.uc_link = &(schedule.main);
            t->state = RUNNING;
            schedule.runing_thread = id;

            makecontext(&(t->ctx), (void(*)(void))(uthread_body), 1, &schedule);

        case SUSPEND:
            swapcontext(&(schedule.main), &(t->ctx));
            break;
        default: ;
    }
}

int schedule_finished(const schedule_t &schedule)
{
    if (schedule.runing_thread != -1){
        return 0;
    } else {
        for (int i = 0; i < schedule.max_index; ++i){
            if (schedule.threads[i].state != FREE){
                return 0;
            }
        }
    }
    return 1;
}