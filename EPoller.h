/*************************************************************************
	> File Name: Epoller.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月09日 星期四 00时00分59秒
 ************************************************************************/

#ifndef _EPOLLER_H
#define _EPOLLER_H

#include<sys/epoll.h>
#include<map>

#include"EventDemultiplexer.h"

/* 注意：EPoller
 * 为了保证在多线程中的安全性，设置了EPOLLONESHOT属性，注意在使用之后再次填充事件
 */

namespace unet
{
    class EPoller final : public EventDemultiplexer
    {
        public:
            explicit EPoller();
            EPoller(const EPoller&) = delete;
            EPoller& operator=(const EPoller&) = delete;
            EPoller(EPoller&&);
            EPoller& operator=(EPoller&&);
            ~EPoller() override;
            
            bool operator==(const EPoller& epoller){return u_epollfd==epoller.u_epollfd;};

            void addEvent(int,int) override;
            void delEvent(int) override;
            void poll(const EventMap&,std::vector<std::shared_ptr<Event>>&) override;
            void resetEvent(int) override;
            void openET(){u_openET = true;};
            void closeET(){u_openET = false;};

        private:
            uint32_t switchTo(int);

        private:
            int u_epollfd;
            std::map<int,struct epoll_event*> u_eventMap; 
            std::vector<struct epoll_event> u_activeList;
            bool u_openET;
    };
}


#endif

