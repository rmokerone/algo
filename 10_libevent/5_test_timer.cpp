#include <event2/event.h>
#include <iostream>
#include <signal.h>

using namespace std;


static timeval t1 = {1, 0};

void timer1(int sock, short which, void *arg)
{
    cout << "[timer1]" << endl;

    // 判决状态  再次天剑
    event *ev = (event*)arg;
    if (!evtimer_pending(ev, &t1))
    {
        evtimer_del(ev);
        evtimer_add(ev, &t1);
    }
}

void timer2(int sock, short which, void *arg)
{
    cout << "[timer2]" << endl;
}


void timer3(int sock, short which, void *arg)
{
    cout << "[timer3]" << endl;
}

int main()
{
    std::cout << "test timer!\n" ;

    event_base *base = event_base_new();
    if (base){
        std::cout << "event_base success\n";
    }

    // 1. 非持久定时器
    event *ev1 = evtimer_new(base, timer1, event_self_cbarg());
    if (!ev1){
        cout << "evtimer_new timer1 failed!" << endl;
        return -1;
    }

    evtimer_add(ev1, &t1);

    // 2. 持久化定时器
    static timeval t2;
    t2.tv_sec = 0;
    t2.tv_usec = 200 * 1000;
    event *ev2 = event_new(base, -1, EV_PERSIST, timer2, 0);
    event_add(ev2, &t2);

    // 3. 超时优化 性能优化
    // 默认event 使用二叉堆存储(完全二叉树) 插入删除 O(logn)
    // 优化到双向队列 插入删除 O(1)
    event *ev3 = event_new(base, -1, EV_PERSIST, timer3, 0);
    static timeval tv_in = {3, 0};
    const timeval *t3;
    t3 = event_base_init_common_timeout(base, &tv_in);
    event_add(ev3, t3);  // 插入性能 O(1)


    // 进入事件主循环
    event_base_dispatch(base);

    // 释放资源
    event_base_free(base);


    
    return 0;
}