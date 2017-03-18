/*************************************************************************
	> File Name: EventLoop.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月01日 星期三 14时24分39秒
 ************************************************************************/

#ifndef _EVENTLOOP_H
#define _EVENTLOOP_H

#include<vector>
#include<functional>
#include<memory>

class Channel;

namespace unet
{
    namespace net
    {
        class EventLoop final
        {
            public:
                typedef std::vector<Channel*> ChannelList;
                typedef std::function<void(ChannelList*)> GetActiveChannelsCallBack;
                
                EventLoop();
                EventLoop(const EventLoop&) = delete;
                EventLoop& operator=(const EventLoop&) = delete;
                ~EventLoop();
//punlic interface
                void loop();
                void setQuit();
                ChannelList* getChannelList();
                void setGetActiveChannelsCallBack(const GetActiveChannelCallBack& cb)
                {activecallback = cb;};

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

