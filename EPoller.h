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
#include<set>

#include<iostream>

#include"EventDemultiplexer.h"
#include"base/Mutex.h"

/* 注意：EPoller
 * 为了保证在多线程中的安全性，设置了EPOLLONESHOT属性，注意在使用之后再次填充事件
 */

namespace unet
{
    static const int EPOLL_MAX_WATCH = 65536 * 4;
    static const int EPOLL_TIMEOUT = 200;
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
            int getFd() const{return u_epollfd;};

        private:
            uint32_t switchTo(int);
            void addEventCore();
            void delEventCore();

        private:
            int u_epollfd;
            std::map<int,struct epoll_event*> u_eventMap; 
            std::vector<struct epoll_event> u_activeList;
            bool u_openET;

            base::MutexLock u_mutex;
            std::set<int> u_stopSet;

            base::MutexLock u_admutex;
            std::vector<std::pair<int,int>> u_addList;
            std::vector<int> u_eraseList;
    };
}


#endif

