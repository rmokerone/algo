#include <event2/event.h>
#include <iostream>
#include <signal.h>

using namespace std;

bool isexit = false;

/**
 * @brief 
 * 
 * @param sock 文件描述符 
 * @param which 事件类型
 * @param arg 传递的参数
 */
static void ctrl_c(int sock, short which, void *arg)
{
    event_base *base = (event_base*)arg;
    cout << "enter ctrl_c function" << endl;
    // 执行完当前处理的事件 函数就退出
    // event_base_loopbreak(base);

    // 运行所有的活动事件再退出 事件循环没有运行时也要等待一次后再退出
    timeval t = {3, 0};
    event_base_loopexit(base, &t);
}

static void kill(int sock, short which, void *arg)
{
    cout << "enter kill function" << endl;

    event *ev = (event*)arg;
    // 如果处于非待决状态
    if (!evsignal_pending(ev, NULL)){
        event_del(ev);
        event_add(ev, NULL);
    }
}

int main()
{
    std::cout << "test signal!\n" ;

    event_base *base = event_base_new();
    if (base){
        std::cout << "event_base success\n";
    }

    // 添加信号 ctrl+c
    // evsignal_new 隐藏的状态 EV_SIGNAL | EV_PERSIST
    event *csig = evsignal_new(base, SIGINT, ctrl_c, base);
    if (!csig){
        cerr << "SIGINT evsignal_new failed!" << endl;
        return -1;
    }

    // 添加事件到pending
    if (event_add(csig, 0) != 0)
    {
        cerr << "SIGINT event_add failed!" << endl;
        return -1;
    }

    // 添加kill 信号
    // 非持久只进入一次  参数传入event
    event *ksig = event_new(base, SIGTERM, EV_SIGNAL, kill, event_self_cbarg());
    if (!ksig){
        cerr << "SIGTERM event_new failed!" << endl;
        return -1;
    }

    // 添加事件到pending
    if (event_add(ksig, 0) != 0)
    {
        cerr << "SIGTERM event_add failed!" << endl;
        return -1;
    }

    event_base_dispatch(base);

    // 进入事件主循环
    // EVLOOP_ONCE 等待一个事件运行 直到没有活动事件就退出
    // event_base_loop(base, EVLOOP_ONCE);
    
    // EVLOOP_NONBLOCK 有活动就处理  没有就返回
    // while (!false){
    //     event_base_loop(base, EVLOOP_NONBLOCK);
    // }

    // EVLOOP_NO_EXIT_ON_EMPTY 没有注册事件也不返回 用于事件后期多线程增加
    // event_base_loop(base, EVLOOP_NO_EXIT_ON_EMPTY);


    // 释放资源
    event_free(csig);
    event_base_free(base);


    
    return 0;
}