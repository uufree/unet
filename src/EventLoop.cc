/*************************************************************************
	> File Name: EventLoopX.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年06月13日 星期二 19时58分23秒
 ************************************************************************/

#include"../include/EventLoop.h"

namespace unet
{
    EventLoop::EventLoop() :
        u_start(false),
        u_thread(),
        u_callBack()
    {
        u_thread.setThreadCallBack(std::bind(&EventLoop::ThreadFunction,this));
    };

    EventLoop::EventLoop(EventLoop&& loop) :
        u_start(loop.u_start),
        u_thread(std::move(loop.u_thread)),
        u_callBack(std::move(loop.u_callBack))
    {};

    EventLoop& EventLoop::operator=(EventLoop&& loop)
    {
        if(loop == *this)
            return *this;
        u_start = loop.u_start;
        u_thread = std::move(loop.u_thread);
        u_callBack = std::move(loop.u_callBack);

        return *this;
    }
    
    EventLoop::~EventLoop()
    {
        if(u_start)
            u_thread.stop();
    }
    
    void EventLoop::ThreadFunction()
    {
        if(!u_callBack)
        {
            perror("There is no registered u_callBack!\n");
            return;
        }

        while(u_start)
            u_callBack();
    }
}

