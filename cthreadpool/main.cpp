
#include <iostream>
#include <unistd.h>
#include "ctask.h"
#include "cthreadpool.h"

using namespace std;

int main()
{
    CThreadPool pool(10);
    pool.start();
    sleep(1);
    for (int i = 0; i < 100; i ++){
        CTask* t = new CTask(i);
        pool.add_task(t);
        usleep(1000);
    }

    sleep(2);
}