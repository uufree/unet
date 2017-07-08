/*************************************************************************
	> File Name: EventLoopX.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年06月13日 星期二 19时58分23秒
 ************************************************************************/

#include"EventLoop.h"
#include"Channel.h"

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
            activeChannelList(std::move(lhs.activeChannelList)),
            getActiveChannelsCallBack(std::move(lhs.getActiveChannelsCallBack)),
            handleChannelsCallBack(std::move(lhs.handleChannelsCallBack))
        {};

        EventLoop& EventLoop::operator=(EventLoop&& lhs)
        {
            looping = lhs.looping;
            quit = lhs.quit;
            eventHandling = lhs.eventHandling;

            activeChannelList = std::move(lhs.activeChannelList);
            getActiveChannelsCallBack = std::move(lhs.getActiveChannelsCallBack);
            handleChannelsCallBack = std::move(lhs.handleChannelsCallBack);

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
            
            if(!getActiveChannelsCallBack)
            {
                perror("没有注册getActiveEventsCallBack!\n");
                exit(1);
            }

            if(!handleChannelsCallBack)
            {
                perror("没有注册handleEventsCallBack!\n");
                exit(1);
            }

            while(!quit)
            {
                activeChannelList.clear();

                getActiveChannelsCallBack();
                handleChannelsCallBack();
            }
        }
        
        void EventLoop::setGetActiveChannelsCallBack(const GetActiveChannelsCallBack& lhs)
        {
            getActiveChannelsCallBack = lhs;
        }

        void EventLoop::setHandleChannelsCallBack(const HandleChannelsCallBack& lhs)
        {
            handleChannelsCallBack = lhs;
        }
        
    }
}

