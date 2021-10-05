#include <event2/event.h>
#include <event2/buffer.h>
#include <event2/listener.h>
#include <event2/bufferevent.h>
#include <iostream>
#include <signal.h>
#include <string.h>

using namespace std;

#define SPORT (5001)

bufferevent_filter_result filter_in(evbuffer *s, evbuffer *d, ev_ssize_t limit,
    bufferevent_flush_mode mode, void *arg)
{
    cout << "filter_in" << endl;

    // 读取并清理源数据
    char data[1024] = {0};
    int len = evbuffer_remove(s, data, sizeof(data)-1);

    // 把所有字母转为大写
    for (int i = 0; i < len; i ++){
        data[i] = toupper(data[i]);
    }

    evbuffer_add(d, data, len);

    return BEV_OK;
}

bufferevent_filter_result filter_out(evbuffer *s, evbuffer *d, ev_ssize_t limit,
    bufferevent_flush_mode mode, void *arg)
{
    cout << "filter_out" << endl;


    char data[1024] = {0};
    int len = evbuffer_remove(s, data, sizeof(data)-1);

    // 增加头部消息
    string str = "";
    str += "==================\n";
    str += data;
    str += "==================\n";

    evbuffer_add(d, str.c_str(), str.size());

    return BEV_OK;
}

void read_cb(bufferevent *bev, void *arg)
{
    char data[1024] = {0};
    int len = bufferevent_read(bev, data, sizeof(data)-1);
    cout << data << endl;

    bufferevent_write(bev, data, len);
}

void write_cb(bufferevent *bev, void *arg)
{
    
}

void event_cb(bufferevent *bev, short events, void *arg)
{

}

void listen_cb(struct evconnlistener *, evutil_socket_t s, struct sockaddr *a, int socklen, void *arg)
{
    cout << "enter listen_cb function\n";

    event_base *base = (event_base*)arg;

    // 创建bufferevent 编订bufferevent filter
    bufferevent *bev = bufferevent_socket_new(base, s, BEV_OPT_CLOSE_ON_FREE);

    // 绑定bufferevent filter
    bufferevent *bev_filter = bufferevent_filter_new(bev,
        filter_in,   // 输入过滤函数
        filter_out,  // 输出过滤函数
        BEV_OPT_CLOSE_ON_FREE,  // 关闭filter时 同时关闭bufferevent
        0, // 清理的回到函数
        0   // 传给回调的参数
    );

    // 设置bufferevent的回调
    bufferevent_setcb(bev_filter, read_cb, write_cb, event_cb, NULL);

    // 开启读写权限
    bufferevent_enable(bev_filter, EV_READ | EV_WRITE);
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