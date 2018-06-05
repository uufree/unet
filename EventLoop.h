/*************************************************************************
	> File Name: EventLoop.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年06月13日 星期二 19时05分58秒
 ************************************************************************/

#ifndef _EVENTLOOP_H
#define _EVENTLOOP_H

#include<functional>
#include<vector>
#include<utility>

#include"base/Thread.h"

/*2018.06.03 测试完成*/
/*事件分离线程，根据需求将事件分离*/
namespace unet
{
    class EventLoop final
    {
        typedef std::function<void()> SeparationEventCallBack;

        public:
            explicit EventLoop();
            EventLoop(const EventLoop&) = delete;
            EventLoop(EventLoop&&);
            EventLoop& operator=(const EventLoop&) = delete;
            EventLoop& operator=(EventLoop&&);
            ~EventLoop();
            
            bool operator==(const EventLoop& loop){return u_thread==loop.u_thread;};

            void start(){u_start = true;u_thread.start();};
            void stop(){if(u_start) u_thread.stop();u_start=false;};
            bool isStart() const{return u_start;};

            void setSeparationEventCallBack(const SeparationEventCallBack& cb)
            {u_callBack = cb;};
        
        private:
            void ThreadFunction();

        private:
            bool u_start;
            base::Thread u_thread;       
            SeparationEventCallBack u_callBack; /*启用事件分离线程*/
    };
}

#endif
