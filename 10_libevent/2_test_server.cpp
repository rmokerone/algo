#include <event2/event.h>
#include <event2/listener.h>
#include <iostream>
#include <signal.h>
#include <string.h>

using namespace std;

#define SPORT (5001)


void listen_cb(struct evconnlistener *, evutil_socket_t, struct sockaddr *a, int socklen, void *arg)
{
    cout << "enter listen_cb function\n";
}

int main()
{

#ifdef _WIN32
#else
    // 忽略管道信号  发送数据给已关闭的socket
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        return -1;
#endif

    cout << "test server!\n" ;

    event_base *base = event_base_new();

    if (base){
        cout << "event_base_new success\n";
    }

    // 监听端口 socket, bind, listen
    sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(SPORT);


    evconnlistener *ev =  evconnlistener_new_bind(base,  
                                                listen_cb, 
                                                base,
                                                LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE, // 地址重用， listen关闭同时关闭socket
                                                10,  // 连接队列大小
                                                (sockaddr*)&sin, sizeof(sin)    // 绑定地址和端口
                                            );

    // 事件分发处理
    event_base_dispatch(base);

    // 清理
    if (ev)
        evconnlistener_free(ev);

    if (base)
        event_base_free(base);
    
    return 0;
}