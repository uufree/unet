/*************************************************************************
	> File Name: Epoller.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月09日 星期四 00时00分59秒
 ************************************************************************/

#ifndef _EPOLLER_H
#define _EPOLLER_H

#include"Channel.h"
#include<iostream>
#include"Mutex.h"

//struct epoll_event;

struct pollfd;

namespace unet
{
    namespace net
    { 
        class Epoller final
        {
            static const int timeoutMs = 200;
            static const int KNoneEvent = 0;

//            typedef std::vector<struct epoll_event> EventList;
            typedef std::vector<struct pollfd> EventList;
            typedef std::map<int,Channel*> ChannelMap;
            typedef std::vector<Channel*> ChannelList;

            public:    
                Epoller();
                Epoller(const Epoller&) = delete;
                Epoller operator=(const Epoller&) = delete;
                ~Epoller();
//public interface 
                void epoll(ChannelList* channels);
                bool hasChannel(Channel* channel_);
                void removeChannel(Channel* channel_);
                void updateChannel(Channel* channel_); 
                void addInChannelMap(Channel* channel_);
                
                int getConSize()
                {return channelmap.size();};

                void getInfo();
            
            private:

                void update(Channel* channel_);
                void getActiveEvents(int activeEvents,ChannelList* channels);

                EventList eventlist;//保存epollfd的数组
//                int epollfd;//内核维护的epollfd表
                ChannelMap channelmap;//保存Channel的Map
                unet::thread::MutexLock mutex;
        };    
    }
}


#endif

