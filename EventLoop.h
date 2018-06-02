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

            void start(){if(!u_start) u_thread.start();u_start=true;};
            void stop(){if(u_start) u_thread.stop();u_start=false;};
            bool isStart() const{return u_start;};

            void setGetActiveChannelsCallBack(const SeparationEventCallBack& cb)
            {u_callBack = cb;};
        
        private:
            void ThreadFunction();

        private:
            bool u_start;
            base::Thread u_thread;       
            SeparationEventCallBack u_callBack;
    };
}

#endif
