#include <event2/event.h>
#include <event2/listener.h>
#include <iostream>
#include <signal.h>
#include <string.h>
#include <zlib.h>

#include "server.h"
#include "client.h"

using namespace std;



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

 

    Server(base);

    Client(base);

    // 事件分发处理
    event_base_dispatch(base);

    // 清理
    // if (ev)
    //     evconnlistener_free(ev);

    if (base)
        event_base_free(base);
    
    return 0;
}