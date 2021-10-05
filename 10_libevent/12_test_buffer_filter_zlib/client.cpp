#include "client.h"
#include <iostream>
#include <string.h>
#include <zlib.h>

using namespace std;

#define SPORT (5001)
#define FILEPATH "001.txt"

struct ClientStatus
{
    FILE *fp = 0;
    bool fileend = false;
    bool startsend = false;
    z_stream *z_output = 0;

    int readNum = 0;
    int sendNum = 0;

    ~ClientStatus()
    {
        if (fp)
            fclose(fp);
        if (z_output){
            deflateEnd(z_output);
            delete z_output;
            z_output = 0;
        }
    }
};


bufferevent_filter_result client_filter_out(evbuffer *s, evbuffer *d, ev_ssize_t limit, bufferevent_flush_mode mode, void *arg)
{
    cout << "client_filter_out" << endl;

    ClientStatus *ctx = (ClientStatus*)arg;



    // 发送消息 001 去掉
    if (!ctx->startsend)
    {
        char data[1024] = {0};
        int len = evbuffer_remove(s, data, sizeof(data));
        evbuffer_add(d, data, len);
        return BEV_OK;
    }

    // 开始压缩文件
    // 1. 取出buffer中数据的引用
    evbuffer_iovec v_in[1];
    int n = evbuffer_peek(s, -1, 0, v_in, 1);
    if (n <= 0)
    {
        cout << "client_filter_out n = 0 | fileend="<< ctx->fileend << endl;
        // 调用write回调 清理空间
        if (ctx->fileend)
        {
            return BEV_OK;
        }
        // 没有数据
        return  BEV_NEED_MORE;
    }
    // zlib上下文
    z_stream *p = ctx->z_output;
    if (!p){
        return BEV_ERROR;
    }

    // 设置设置数据大小
    p->avail_in = v_in[0].iov_len;
    p->next_in = (Byte*)v_in[0].iov_base;

    // 申请输出空间大小
    evbuffer_iovec v_out[1];
    evbuffer_reserve_space(d, 4096, v_out, 1);

    // 传递给zlib

    // 输出空间大小
    p->avail_out = v_out[0].iov_len;

    // 输出空间地址
    p->next_out = (Byte*)v_out[0].iov_base;

    // zlib 压缩
    int re = deflate(p, Z_SYNC_FLUSH);
    if (re != Z_OK)
    {
        cerr << "deflate failed!" << endl;
    }

    // 压缩用了多少数据 从source evbuffer中移除
    // p->avail_in 未处理的数据大小
    int nread = v_in[0].iov_len - p->avail_in;

    // 压缩后的数据大小
    // p->avail_out 剩余的空间大小
    int nwrite = v_out[0].iov_len - p->avail_out;

    // 移除source eventbuffer中的数据
    evbuffer_drain(s, nread);

    v_out[0].iov_len = nwrite;

    // 传入 des eventbuffer
    evbuffer_commit_space(d, v_out, 1);

    cout << "nread=" << nread << " nwrite=" << nwrite << endl;

    ctx->readNum += nread;
    ctx->sendNum += nwrite;

    return BEV_OK;
}

void client_read_cb(bufferevent *bev, void *arg)
{
    // 002 接收服务端发送的回复

    ClientStatus *ctx = (ClientStatus*)arg;
    
    char data[1024] = {0};
    int len = bufferevent_read(bev, data, sizeof(data)-1);
    cout << "client_read_cb: "<< data << endl;
    if (strcmp(data, "OK") == 0)
    {
        cout << data << endl;

        ctx->startsend = true;

        // 开始发送文件
        bufferevent_trigger(bev, EV_WRITE, 0); // 启动write_cb
    }else{
        bufferevent_free(bev);
    }
}

void client_write_cb(bufferevent *bev, void *arg)
{
    cout << "client_write_cb" << endl;

    ClientStatus *s = (ClientStatus*)arg;

    FILE *fp = s->fp;
    if (!fp) return;

    // 判断是否到结尾
    if (s->fileend)
    {
        // 判断缓冲是否有数据  如果有则刷新
        bufferevent *be = bufferevent_get_underlying(bev);

        evbuffer *evb = bufferevent_get_output(be);
        int len = evbuffer_get_length(evb);
        if (len <= 0) 
        {

            cout << "client read " << s->readNum << endl;
            cout << "client send " << s->sendNum << endl;

            bufferevent_free(bev);
            
            delete s;
            return;
        }

        // 刷新缓冲
        bufferevent_flush(bev, EV_WRITE, BEV_FINISHED);
        return;
    }


    // 发送文件
    char data[1024] = {0};
    int len = fread(data, 1, sizeof(data), fp);
    if (len <= 0)
    {
        s->fileend = true;

        bufferevent_flush(bev, EV_WRITE, BEV_FINISHED);

        return;
    }

    // 发送文件
    bufferevent_write(bev, data, len);
}

void client_event_cb(bufferevent *bev, short events, void *arg)
{
    cout << "client_event_cb" << endl;
}

void connect_event_cb(struct bufferevent *be, short what, void *ctx)
{
    cout << "client_event_cb " << what << endl;

    if (what & BEV_EVENT_CONNECTED)
    {
        cout << "BEV_EVENT_CONNECTED" << endl;

        // 001 发送文件名
        bufferevent_write(be, FILEPATH, strlen(FILEPATH));



        FILE *fp = fopen(FILEPATH, "rb");
        if (!fp)
        {
            cout << "open file " << FILEPATH << "failed!" << endl;
            return;
        }

        ClientStatus *s = new ClientStatus();
        s->fp = fp;
        s->fileend = false;

        // 初始化zlib上下文
        s->z_output = new z_stream();
        deflateInit(s->z_output, Z_DEFAULT_COMPRESSION); // 默认压缩参数

        // 创建输出过滤
        bufferevent *bev = bufferevent_filter_new(be, 0, client_filter_out, BEV_OPT_CLOSE_ON_FREE | BEV_OPT_DEFER_CALLBACKS, 0, s);

        // 设置读取、写入和事件回调
        bufferevent_setcb(bev, client_read_cb, client_write_cb, client_event_cb, s);

        bufferevent_enable(bev, EV_READ | EV_WRITE); 

    }
}

void Client(event_base *base)
{
    cout << "Begin Client" << endl;

    // 连接服务端 发送文件 接收回复确认OK

    sockaddr_in sin;
    memset(&sin, 0, sizeof(sin));
    sin.sin_family = AF_INET;
    sin.sin_port = htons(SPORT);

    evutil_inet_pton(AF_INET, "127.0.0.1", &sin.sin_addr.s_addr);

    bufferevent *bev = bufferevent_socket_new(base, -1, BEV_OPT_CLOSE_ON_FREE);

    bufferevent_enable(bev, EV_READ | EV_WRITE);
    // 只绑定事件回调  用来确认连接成功

    bufferevent_setcb(bev, 0, 0, connect_event_cb, 0);

    bufferevent_socket_connect(bev, (sockaddr*)&sin, sizeof(sin));
}