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

namespace unet
{
    namespace net
    {
        class Channel;

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

                inline int size() const;
                inline bool empty() const;
                inline void insert(const Channel& channel);
                inline void erase(int fd);
                inline std::vector<struct epoll_event>& getEventList();

            private:
                std::vector<struct epoll_event&&> eventList;
                std::vector<int> eventFdList;
        };
    }
}




























#endif
