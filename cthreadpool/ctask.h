

#pragma once

class CTask{
    public:
        CTask(int i);
        ~CTask();

        void run();

    private:
        int num;
};