#include <event2/event.h>
#include <iostream>
#include <signal.h>

#include <errno.h>
#include <string.h>

using namespace std;

#define SPORT 5001

void client_cb(evutil_socket_t s, short w, void *arg)
{
    event *ev = (event*)arg;
    char buf[1024] = {0};

    if (w & EV_TIMEOUT){
        cout << "timeout" << endl;
        event_free(ev);
        evutil_closesocket(s);
        return;
    }


    int len = recv(s, buf, sizeof(buf)-1, 0);
    if (len > 0){
        cout << buf << endl;
        // 数据回应
        send(s, "ok", 2, 0);
    }else{
        // 需要清理event
        cout << ".";
        event_free(ev);
        evutil_closesocket(s);
    }
}

void listen_cb(evutil_socket_t s, short w, void *arg)
{
    cout << "enter listen_cb function" << endl;

    event_base *base = (event_base*)arg;

    // 读取连接信息
    sockaddr_in sin;
    socklen_t size = sizeof(sin);
    evutil_socket_t client = accept(s, (sockaddr*)&sin, &size);
    char ip[16] = {0};
    evutil_inet_ntop(AF_INET, &sin.sin_addr, ip , sizeof(ip)-1);
    cout << "client ip is " << ip << " port is: " << sin.sin_port << endl;

    // 客户端数据读取时间
    event *ev = event_new(base, client, EV_READ | EV_PERSIST, client_cb, event_self_cbarg());
    // 边缘触发 只触发一次 如果单次没有处理完成，容易丢数据
    // event *ev = event_new(base, client, EV_READ | EV_PERSIST | EV_ET, client_cb, event_self_cbarg());
    timeval t = {10, 0};
    event_add(ev, &t);
}

int main()
{
    std::cout << "test event server!\n" ;

    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR)
        return 1;



    event_base *base = event_base_new();
    if (base){
        std::cout << "event_base success\n";
    }


    evutil_socket_t sock = socket(AF_INET, SOCK_STREAM, 0); 
    if (sock < 0){
        cout << "socket error: " << strerror(errno);
    }

    // 设置地址复用和非阻塞
    evutil_make_listen_socket_reuseable(sock);
    evutil_make_socket_nonblocking(sock);

    sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(SPORT);

    // 绑定与监听
    int re = ::bind(sock, (sockaddr*)&sin, sizeof(sin));
    if (re != 0){
        cerr << "bind error : " << strerror(errno) << endl;
        return -1;
    }
    listen(sock, 10);

    // 开始接收连接事件 默认为水平触发
    event* ev = event_new(base, sock, EV_READ | EV_PERSIST, listen_cb, base);
    event_add(ev, 0);


    // 进入事件主循环
    event_base_dispatch(base);

    evutil_closesocket(sock);

    // 释放资源
    event_base_free(base);


    
    return 0;
}