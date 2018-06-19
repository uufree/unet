/*************************************************************************
	> File Name: Poller.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月20日 星期日 12时32分18秒
 ************************************************************************/

#ifndef _POLLER_H
#define _POLLER_H

#include"EventDemultiplexer.h"
#include"Mutex.h"
#include<set>
#include<map>

/*Poller中出现close事件时出现exception事件*/

namespace unet
{
    static const int POLL_TIMEOUT = 20;    /*Poll的阻塞时间为20ms*/

    class Poller final : public EventDemultiplexer
    {
        public:
            explicit Poller();
            Poller(const Poller&) = delete;
            Poller& operator=(const Poller&) = delete;
            Poller(Poller&&);
            Poller& operator=(Poller&&);
            ~Poller() override;
            
            bool operator==(const Poller& poll){return u_eventList.begin() == poll.u_eventList.begin();};
            
            /*抽象出来的统一的接口*/
            void addEvent(int,int) override;
            void delEvent(int) override;
            void poll(const EventMap&,std::vector<std::shared_ptr<Event>>&) override;
            void resetEvent(int) override; 
        private:
            /*因为设计问题，需要异步的关闭事件*/
            int switchEvent(int usrEvent);
            void addEventCore();
            void deleteEventCore();

        private:
            std::vector<struct pollfd> u_eventList; /*pollfd数组*/
            std::set<int> u_set;    /*已添加的fd的统计*/
            
            base::MutexLock u_mutex;
            std::map<int,int> u_stopMap;    /*重置事件使用*/
            
            base::MutexLock u_admutex;
            std::vector<struct pollfd> u_addEventList;  /*异步的添加与删除事件*/
            std::vector<int> u_deleteEventList;
    };
}

#endif
