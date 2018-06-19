/*************************************************************************
	> File Name: StaticThreadTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年06月03日 星期日 21时24分20秒
 ************************************************************************/

#include<iostream>
#include"../Thread.h"

using namespace unet::base;

class sta
{
    public:
        static void start(){thread.start();};
        static void stop(){thread.stop();};
        static void func()
        {
            while(1)
            {
                std::cout << "1" << std::endl;
            }
        }
    private:
        static Thread thread;
};

Thread sta::thread(std::bind(&sta::func));

int main()
{
    sta::start();
    while(1);
}

