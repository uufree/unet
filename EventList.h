/*************************************************************************
	> File Name: EventList.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年07月07日 星期五 13时04分02秒
 ************************************************************************/

#ifndef _EVENTLIST_H
#define _EVENTLIST_H


#include<vector>
#include<sys/epoll.h>
#include"Mutex.h"
#include<memory>

namespace unet
{
    namespace net
    {
        class Channel;
        typedef std::shared_ptr<Channel> ChannelPtr;

        class EventList final
        {
            public:
                explicit EventList();
                EventList(const EventList& lhs) = delete;
                EventList(EventList&& lhs);
                EventList& operator=(const EventList& lhs) = delete;
                EventList& operator=(EventList&& lhs);
                ~EventList();
            
                inline void swap(EventList& lhs);

                int size() const
                {return eventList.size();};

                bool empty() const
                {return eventList.empty();};

                void insert(const ChannelPtr& channel);
                void erase(int fd);
                std::vector<struct epoll_event>& getEventList();

            private:
                thread::MutexLock mutex;
                std::vector<struct epoll_event> eventList;
                std::vector<int> eventFdList;
        };
    }
}




























#endif
