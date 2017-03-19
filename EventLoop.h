/*************************************************************************
	> File Name: EventLoop.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月01日 星期三 14时24分39秒
 ************************************************************************/

#ifndef _EVENTLOOP_H
#define _EVENTLOOP_H

#include"Epoller.h"
#include<functional>

class Channel;

namespace unet
{
    namespace net
    { 
        class EventLoop final
        {
            typedef std::vector<Channel*> ChannelList;
            typedef std::function<void(ChannelList*)> GetActiveChannelsCallBack;

            public:                                
                EventLoop();
                EventLoop(const EventLoop&) = delete;
                EventLoop& operator=(const EventLoop&) = delete;
                ~EventLoop() {};
//punlic interface
                void loop();
                void setQuit();
                ChannelList* getChannelList();
                void setGetActiveChanneslCallBack(const GetActiveChannelsCallBack& cb)
                {activecallback = cb;};//由Epoller注册

            private:
                bool looping;
                bool quit;
                bool eventhandling;
                ChannelList activechannels;
                GetActiveChannelsCallBack activecallback;
        };
    }
}

#endif

