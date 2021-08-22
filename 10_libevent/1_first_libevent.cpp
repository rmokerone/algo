#include <event2/event.h>
#include <iostream>
#include <signal.h>


using namespace std;

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


    event_base_dispatch(base);

    event_base_free(base);
    
    
    return 0;
}