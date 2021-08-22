#include <event2/event.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <iostream>
#include <signal.h>
#include <sys/types.h>          /* See NOTES */
#include <sys/socket.h>
#include <string.h>


using namespace std;

// 错误 超时 连接断开
void event_cb(bufferevent *be, short events, void *arg)
{
    cout << "[event_cb]" << endl;
    // 读取超时时间发生后 数据读取停止
    if ((events & BEV_EVENT_TIMEOUT) && (events & BEV_EVENT_READING)){
        cout << "BEV_EVENT_TIMEOUT" << endl;
        // bufferevent_enable(be, EV_READ);
        bufferevent_free(be);
    }else if (events & BEV_EVENT_ERROR){
        bufferevent_free(be);
    }else{
        cout << "OTHER" << endl;
    }
}

void write_cb(bufferevent *be, void *arg)
{
    cout << "[write_cb]" << endl;
}

void read_cb(bufferevent *be, void *arg)
{
    cout << "[read_cb]" << endl;
    char data[1024] = {0};

    int len = bufferevent_read(be, data, sizeof(data)-1);
    cout << "[" << data << "]" << endl;
    if (len  <= 0) return;
    if (strstr(data, "quit") != NULL){
        cout << "quit" << endl;
        // 退出并关闭socket  BEV_OPT_CLOSE_ON_FREE
        bufferevent_free(be);
    }

    // 发送数据 写入到输出缓冲区
    bufferevent_write(be, "OK", 3);
}

void listen_cb(evconnlistener *ev, evutil_socket_t s, sockaddr *sin, int slen, void *arg)
{
    cout << "enter listen_cb function" << endl;

    event_base *base = (event_base*)arg;

    // 创建bufferevent 上下文 BEV_OPT_CLOSE_ON_FREE 清理bufferevent的时候关闭socket
    bufferevent *bev = bufferevent_socket_new(base, s, BEV_OPT_CLOSE_ON_FREE);

    // 开启bufferevent读写权限
    bufferevent_enable(bev, EV_READ | EV_WRITE);

    // 设置水位
    bufferevent_setwatermark(bev, EV_READ,
        5, // 低水位
        10   // 高水位  0 表示无限制
    );

    // 超时时间设置
    timeval t1 = {3, 0};
    bufferevent_set_timeouts(bev, &t1, 0);

    // 设置回调函数
    bufferevent_setcb(bev, read_cb, write_cb, event_cb, base);
}

int main()
{
    if (signal(SIGPIPE, SIG_IGN) == SIG_ERR){
        return 1;
    }
    std::cout << "test libevent!\n" ;

    event_base *base = event_base_new();
    if (!base){
        std::cout << "event_base failed\n";
        return -1;
    }

    // 创建网络服务器

    // 设定监听的端口的地址
    sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(5001);

    evconnlistener *ev = evconnlistener_new_bind(base,
        listen_cb,      // 回调函数
        base,           // 回调函数的参数arg
        LEV_OPT_REUSEABLE | LEV_OPT_CLOSE_ON_FREE,
        10,  // list back
        (sockaddr*)&sin,
        sizeof(sin)
    );

    event_base_dispatch(base);

    evconnlistener_free(ev);

    event_base_free(base);
    

    
    return 0;
}