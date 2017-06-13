/*************************************************************************
	> File Name: EventLoopX.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年06月13日 星期二 19时58分23秒
 ************************************************************************/

#include"EventLoop.h"

namespace unet
{
    namespace net
    {
        EventLoop::EventLoop() : 
            looping(false),
            quit(false),
            eventHandling(false)
        {};

        EventLoop::EventLoop(EventLoop&& lhs) : 
            looping(lhs.looping),
            quit(lhs.quit),
            eventHandling(lhs.eventHandling),
            activeEventsList(std::move(lhs.activeEventsList)),
            getActiveEventsCallBack(std::move(lhs.getActiveEventsCallBack)),
            handleEventsCallBack(std::move(lhs.handleEventsCallBack))
        {};

        EventLoop& EventLoop::operator=(EventLoop&& lhs)
        {
            looping = lhs.looping;
            quit = lhs.quit;
            eventHandling = lhs.eventHandling;

            activeEventsList = std::move(lhs.activeEventsList);
            getActiveEventsCallBack = std::move(lhs.getActiveEventsCallBack);
            handleEventsCallBack = std::move(lhs.handleEventsCallBack);

            return *this;
        }
        
        void EventLoop::setQuit()
        {
            quit = true;
        }

        void EventLoop::loop()
        {
            looping = true;
            quit = false;
            
            if(!getActiveEventsCallBack)
            {
                perror("没有注册getActiveEventsCallBack!\n");
                exit(1);
            }

            if(!handleEventsCallBack)
            {
                perror("没有注册handleEventsCallBack!\n");
                exit(1);
            }

            while(!quit)
            {
                activeEventsList.clear();

                getActiveEventsCallBack();
                handleEventsCallBack();
            }
        }
        
        void EventLoop::setGetActiveChannelsCallBack(const GetActiveEventsCallBack& lhs)
        {
            getActiveEventsCallBack = lhs;
        }

        void EventLoop::setHandleEventsCallBack(const HandleEventsCallBack& lhs)
        {
            handleEventsCallBack = lhs;
        }
        
    }
}

