#include <event2/event.h>
#include <iostream>

int main()
{
    std::cout << "test libevent!\n" ;

    event_base *base = event_base_new();

    if (base){
        std::cout << "event_base success\n";
    }
    
    return 0;
}