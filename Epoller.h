/*************************************************************************
	> File Name: Epoller.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月09日 星期四 00时00分59秒
 ************************************************************************/

#ifndef _EPOLLER_H
#define _EPOLLER_H

#include<vector>
#include<map>
#include<functional>

class Channel;
struct epoll_event;

namespace unet
{
    namespace net
    {
        class Epoller final
        {
            public:
                typedef std::vector<struct epoll_event> EventList;
                typedef std::map<int,Channel*> ChannelMap;
                typedef std::vector<Channel*> ChannelList;
                
                Epoller(EventLoop* loop_);
                Epoller(const Epoller&) = delete;
                Epoller operator(const Epoller&) = delete;
                ~Epoller();
//public interface 
                void epoll(ChannelList* channels);
                bool hasChannle(Channel* channel_);
                void removeChannel(Channel* channel_);
                void updateChannel(Channel* channel_); 
                void addInChannelMap(Channel* channel_);

            private:

                void update(int operation,Channel* channel_);
                void getActiveEvents(ChannelList* channels);

                EventList eventlist;//保存epollfd的数组
                int epollfd;//内核维护的epollfd表
                ChannelMap channelmap;//保存Channel的Map
        };    
    }
}


#endif

