/*************************************************************************
	> File Name: Epoller.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月09日 星期四 00时00分59秒
 ************************************************************************/

#ifndef _EPOLLER_H
#define _EPOLLER_H

#include<sys/epoll.h>
#include<vector>

#include"ChannelMap.h"
#include"TcpConnectionMap.h"
#include"EventList.h"
#include"error.h"

namespace unet
{
    namespace net
    {
        static const int timeoutMs = 200;//默认poll阻塞200ms
        
        class Epoller final
        {
            typedef std::vector<ChannelPtr> ChannelList;
            
            public:    
                explicit Epoller(EventList& events);
                Epoller(const Epoller&) = delete;
                Epoller& operator=(const Epoller&) = delete;
                Epoller(Epoller&& lhs);
                Epoller& operator=(Epoller&& lhs);
                ~Epoller();

                void epoll(ChannelList& channelList,ChannelMap& channelMap,TcpConnectionMap& tcpconnectionMap);
                
                int getEpollfd() const
                {return epollfd;};
            
            private:
                void getActiveEvents(int activeEvents,ChannelList& channelList,ChannelMap& channelMap,TcpConnectionMap& tcpConnectionMap);

            private:
                EventList& eventList;//保存epollfd的数组
                int epollfd;//内核维护的epollfd表
        };    
    }
}


#endif

