#include "ctask.h"
#include <iostream>

using namespace std;

CTask::CTask(int i)
:num(i)
{

}

CTask::~CTask()
{

}

void CTask::run()
{
    cout << "num=" << num << endl;
}