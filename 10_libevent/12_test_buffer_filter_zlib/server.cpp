#include <event2/listener.h>
#include <event2/event.h>
#include <event2/bufferevent.h>
#include <event2/buffer.h>
#include <iostream>
#include <string.h>
#include <zlib.h>

using namespace std;

#define SPORT (5001)

struct Status 
{
    bool start = false;
    string filename;
    FILE *fp = 0;
    z_stream *p;
    int recvNum = 0;
    int writeNum = 0;
    ~Status()
    {
        
        if (fp){
            fclose(fp);
        }
        if (p){
            inflateEnd(p);
        }
        delete p;
    }
};

bufferevent_filter_result filter_in(evbuffer *s, evbuffer *d, ev_ssize_t limit, bufferevent_flush_mode mode, void *arg)
{
    
    Status * status = (Status*)arg;
#if  (1)
    if (!status->start)
    {
#endif

        // 接收客户端发送的文件名 
        char data[1024] = {0};
        int len = evbuffer_remove(s, data, sizeof(data)-1);

        cout << "server filter_in: " << len << endl;

        // 回复OK
        evbuffer_add(d, data, len);
        return BEV_OK;
#if  (1)
    }

    // 解压
    evbuffer_iovec v_in[1];
    // 读取数据 不清理缓冲
    int n = evbuffer_peek(s, -1, 0, v_in, 1);
    if (n <= 0)
        return BEV_NEED_MORE;
    
    cout << "client n: " << n << endl;

    // 解压上下文
    z_stream *p = status->p;

    // zlib 输入数据大小
    p->avail_in = v_in[0].iov_len;
    // zlib 输出数据地址
    p->next_in = (Byte*)v_in[0].iov_base;

    // 申请输出空间大小
    evbuffer_iovec v_out[1];
    evbuffer_reserve_space(d, 4096, v_out, 1);

    // zlib 输出空间大小
    p->avail_out = v_out[0].iov_len;
    
    // zlib 输出空间地址
    p->next_out = (Byte*)v_out[0].iov_base;


    int re = inflate(p, Z_SYNC_FLUSH);
    // 解压
    if (re != Z_OK)
    {
        cerr << "deflate failed: " << re << endl;
    }
    // 解压用了多少数据 从source evbuffer中移除
    int nread = v_in[0].iov_len - p->avail_in;

    // 解压后数据大小
    int nwrite = v_out[0].iov_len - p->avail_out;

    // 移除source evbuffer中数据
    evbuffer_drain(s, nread);

    v_out[0].iov_len = nwrite;
    evbuffer_commit_space(d, v_out, 1);
    cout << "server nread=" << nread << " nwrite=" << nwrite << endl;

    status->recvNum += nread;
    status->writeNum += nwrite;

    return BEV_OK;
#endif
}

void read_cb(bufferevent *bev, void *arg)
{
    cout << "server read_cb" << endl;
    char data[1024] = {0};
    Status * status = (Status*)arg;
    if (!status->start)
    {
        // 001 接收文件名
        bufferevent_read(bev, data, sizeof(data)-1);
        status->filename = data;
        // 打开写入
        string out = "out/";
        out += data;
        status->fp = fopen(out.c_str(), "wb");

        if (!status->fp){
            cout << "server open " << out << "failed!" << endl;
            return;
        }else{
            cout << "server open " << out << "success!" << endl;
        }


        bufferevent_write(bev, "OK", 2);
        status->start = true;
        return;
    }

    cout << "server_start_read " << endl;

    do
    {
        // 读取数据
        int len = bufferevent_read(bev, data, sizeof(data)-1);
        if (len >= 0)
        {
            cout << "server read_cb len: " << len << endl;
            fwrite(data, 1, len, status->fp);
            fflush(status->fp);
        }
    } while (evbuffer_get_length(bufferevent_get_input(bev)));

}

void event_cb(bufferevent *bev, short events, void *arg)
{
    cout << "server event_cb" << endl;
    Status *status = (Status*)arg;
    if (events & BEV_EVENT_EOF)
    {
        delete status;
        bufferevent_free(bev);
        cout << "server recv:" << status->recvNum << " send: " << status->writeNum << endl;
    }
}

void listen_cb(struct evconnlistener *, evutil_socket_t s, struct sockaddr *a, int socklen, void *arg)
{
    cout << "enter listen_cb function\n";

    event_base *base = (event_base*)arg;

    // 1. 创建bufferevent用于通信
    bufferevent *bev = bufferevent_socket_new(base, s, BEV_OPT_CLOSE_ON_FREE);

    Status *status = new Status();

    status->p = new z_stream();
    inflateInit(status->p);

    // 2. 添加过滤 并设置输入回调
    bufferevent *bev_filter = bufferevent_filter_new(bev, 
        filter_in,
        0,    // 输出过滤
        BEV_OPT_CLOSE_ON_FREE,  // 关闭filter同时关闭bufferevent
        0, // 清理回调
        status   // 传递参数
    );

    // 3. 设置回调 读取 事件(处理连接断开 超时等) 输入过滤回到
    bufferevent_setcb(bev_filter, read_cb, 0, event_cb, status);
    

    // 4.
    bufferevent_enable(bev_filter, EV_READ | EV_WRITE); 
}

void Server(event_base *base)
{
    cout << "Begin server" << endl;
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

}