#include <event2/event.h>
#include <iostream>
#include <signal.h>

using namespace std;

/**
 * @brief 
 * 
 * @param sock 文件描述符 
 * @param which 事件类型
 * @param arg 传递的参数
 */
static void ctrl_c(int sock, short which, void *arg)
{
    cout << "enter ctrl_c function" << endl;
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

    // 进入事件主循环
    event_base_dispatch(base);

    // 释放资源
    event_free(csig);
    event_base_free(base);


    
    return 0;
}