/*************************************************************************
	> File Name: EventSeparation.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月19日 星期六 19时31分36秒
 ************************************************************************/

#ifndef _EVENTSEPARATION_H
#define _EVENTSEPARATION_H

#include<vector>
#include<memory>
#include<map>

#include<sys/epoll.h>
#include<sys/select.h>
#include<poll.h>


namespace unet
{
    class Event;
    class EventMap;

    class EventDemultiplexer 
    {
        public:
            explicit EventDemultiplexer();
            EventDemultiplexer(const EventDemultiplexer&) = delete;
            EventDemultiplexer& operator=(const EventDemultiplexer&) = delete;
            EventDemultiplexer(EventDemultiplexer&&);
            EventDemultiplexer& operator=(EventDemultiplexer&&);
            virtual ~EventDemultiplexer(){u_start = false;};
            
            bool operator==(const EventDemultiplexer& event)const {return u_wfds==event.u_wfds && u_rfds==event.u_rfds;};

            bool start() const{return u_start;};
            void start(){u_start = true;};
            void stop(){u_start = false;};
            int watchFds() const{return u_wfds;};
            int activeFds() const{return u_rfds;};

            virtual void addEvent(int,int){};
            virtual void delEvent(int){};
            virtual void poll(const EventMap&,std::vector<std::shared_ptr<Event>>&){};
    
        protected:
            bool u_start;
            int u_wfds;     //watch fds
            int u_rfds;     //active fds
    };

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

    class Selecter final : public EventDemultiplexer
    {
        public:
            explicit Selecter();
            Selecter(const Selecter&) = delete;
            Selecter& operator=(const Selecter&) = delete;
            Selecter(Selecter&&);
            Selecter& operator=(Selecter&&);
            ~Selecter() override;
        
            void addEvent(int,int) override;
            void delEvent(int) override;
            void poll(const EventMap&,std::vector<std::shared_ptr<Event>>&) override;
    
        private:
            fd_set u_readSet,u_writeSet,u_exceptionSet;
            std::vector<int> u_watchfds;     
    };

    class Poller final : public EventDemultiplexer
    {
        public:
            explicit Poller();
            Poller(const Poller&) = delete;
            Poller& operator=(const Poller&) = delete;
            Poller(Poller&&);
            Poller& operator=(Poller&&);
            ~Poller() override;
        
            void addEvent(int,int) override;
            void delEvent(int) override;
            void poll(const EventMap&,std::vector<std::shared_ptr<Event>>&) override;
    
        private:
            std::vector<struct pollfd> u_eventList;
    };
}

#endif
