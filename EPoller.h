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

#include"EventDemultiplexer.h"
#include"base/Mutex.h"

/*epoll在检测到close时会出现：read和exception事件*/

namespace unet
{
    /*epoll关注的所有事件总数，在Linux2.8之后没有用处了，由内核负责调度*/
    static const int EPOLL_MAX_WATCH = 65536 * 4;
    static const int EPOLL_TIMEOUT = 20;    /*EPoll的超时时间，20ms*/
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
            uint32_t switchTo(int); /*自定义事件到sys事件的转换*/    
            void addEventCore();    /*异步添加与删除事件*/
            void delEventCore();

        private:
            int u_epollfd;  /*epollfd*/
            std::map<int,struct epoll_event*> u_eventMap;/*保存所有的对象*/ 
            std::vector<struct epoll_event> u_activeList;/*临时用于保存事件结果*/
            bool u_openET;

            /*重置操作在工作线程中进行*/
            base::MutexLock u_mutex;
            /*由于设置了ONESHOT，摆设*/
            std::set<int> u_stopSet;    /*临时用于保存暂停关注的事件*/

            /*添加与删除事件是异步进行的，需要使用锁进行保护*/
            base::MutexLock u_admutex;
            std::vector<std::pair<int,int>> u_addList;
            std::vector<int> u_eraseList;
    };
}


#endif

