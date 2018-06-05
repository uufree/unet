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

namespace unet
{
    /*epoll关注的所有事件总数，在Linux2.8之后没有用处了，由内核负责调度*/
    static const int EPOLL_MAX_WATCH = 65536 * 4;
    static const int EPOLL_TIMEOUT = 20;
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
        
            /*事件分发器抽象出来的四个调用接口*/
            void addEvent(int,int) override;
            void delEvent(int) override;
            void poll(const EventMap&,std::vector<std::shared_ptr<Event>>&) override;
            void resetEvent(int) override;
            
            /*是否启动ET选项*/
            void openET(){u_openET = true;};
            void closeET(){u_openET = false;};
            int getFd() const{return u_epollfd;};

        private:
            uint32_t switchTo(int);     
            void addEventCore();
            void delEventCore();

        private:
            int u_epollfd;
            std::map<int,struct epoll_event*> u_eventMap;/*保存所有的对象*/ 
            std::vector<struct epoll_event> u_activeList;
            bool u_openET;

            base::MutexLock u_mutex;
            std::set<int> u_stopSet;    /**/

            base::MutexLock u_admutex;
            std::vector<std::pair<int,int>> u_addList;
            std::vector<int> u_eraseList;
    };
}


#endif

