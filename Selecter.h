/*************************************************************************
	> File Name: Selecter.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月20日 星期日 12时27分35秒
 ************************************************************************/

#ifndef _SELECTER_H
#define _SELECTER_H

#include"EventDemultiplexer.h"
#include"base/Mutex.h"

#include<set>
#include<map>

namespace unet
{
    /*默认延迟时间为20s*/
    static const int SELECT_TIMEOUT = 20 * 1000;

    class Selecter final : public EventDemultiplexer
    {
        public:
            explicit Selecter();
            Selecter(const Selecter&) = delete;
            Selecter& operator=(const Selecter&) = delete;
            Selecter(Selecter&&);
            Selecter& operator=(Selecter&&);
            ~Selecter() override;
            
            bool operator==(const Selecter& select)const {return u_maxfd==select.u_maxfd;};
            
            /*统一的抽象接口*/
            void addEvent(int,int) override;
            void delEvent(int) override;
            void poll(const EventMap&,std::vector<std::shared_ptr<Event>>&) override;
            void resetEvent(int) override;
        
        private:
            void addEventCore();/*异步的添加与删除事件*/
            void eraseEventCore();

        private:
            /*以为select会主动的清空set，所以保存两个set，需要的时候进行赋值*/
            fd_set u_readSet,u_writeSet,u_exceptionSet;
            fd_set u_readSetSave,u_writeSetSave,u_exceptionSetSave;
            int u_maxfd;
            std::set<int> u_set;
            struct timeval* u_timeout;

            base::MutexLock u_mutex;
            std::map<int,int> u_stopMap;/*fd,event*/

            base::MutexLock u_admutex;
            std::vector<std::pair<int,int>> u_addList;/*fd,event*/
            std::vector<int> u_eraseList;
    };
}

#endif
