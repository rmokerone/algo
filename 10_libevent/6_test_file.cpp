#include <event2/event.h>
#include <iostream>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <sys/types.h>
#include <unistd.h>

using namespace std;

void read_file(evutil_socket_t fd, short which, void *arg)
{
    char buf[1024] = {0};
    int len = read(fd, buf, sizeof(buf)-1);
    if (len > 0){
        cout << buf << endl;
    }else{
        cout << ".";
        usleep(500*1000);
    }
}

int main()
{
    std::cout << "test libevent!\n" ;

    event_config *conf = event_config_new();
    
    // 设置支持文件描述符)
    event_config_require_features(conf, EV_FEATURE_FDS);
    event_base *base = event_base_new_with_config(conf);
    event_config_free(conf);

    if (!base){
        std::cout << "event_base failed\n";
        return -1;
    }

    // 打开文件只读 非阻塞
    int sock = open("/var/log/auth.log", O_RDONLY|O_NONBLOCK, 0);
    if (sock <= 0){
        cerr << "open /var/log/auth.log failed!" << endl;
        return -2;
    }

    // 移动到文件结尾处
    lseek(sock, 0, SEEK_END);

    // 监听文件数据
    event *fev = event_new(base, sock, EV_READ|EV_PERSIST, read_file, 0);

    event_add(fev, NULL);

    event_base_dispatch(base);

    event_base_free(base);
    
    return 0;
}