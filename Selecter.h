/*************************************************************************
	> File Name: Selecter.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月20日 星期日 12时27分35秒
 ************************************************************************/

#ifndef _SELECTER_H
#define _SELECTER_H

#include"EventDemultiplexer.h"
#include<set>

namespace unet
{
    class Selecter final : public EventDemultiplexer
    {
        public:
            explicit Selecter();
            Selecter(const Selecter&) = delete;
            Selecter& operator=(const Selecter&) = delete;
            Selecter(Selecter&&);
            Selecter& operator=(Selecter&&);
            ~Selecter() override;
            
            bool operator==(const Selecter& select)const {return maxfd==select.maxfd;};

            void addEvent(int,int) override;
            void delEvent(int) override;
            void poll(const EventMap&,std::vector<std::shared_ptr<Event>>&) override;

        private:
            fd_set u_readSet,u_writeSet,u_exceptionSet;
            fd_set u_readSetSave,u_writeSetSave,u_exceptionSetSave;
            int maxfd;
            std::set<int> u_set;
    };
}

#endif
