/*************************************************************************
	> File Name: EventLoopThreadPool.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月28日 星期二 16时56分01秒
 ************************************************************************/

#include"EventLoopThreadPool.h"

namespace unet
{
    namespace thread
    {
        EventLoopThreadPool::EventLoopThreadPool(int size) :
            threadsize(size)
        {
            thread.setThreadCallBack();
        };
        
        EventLoopThreadPool::~EventLoopThreadPool()
        {
            for(auto iter=threadlist.begin();iter!=threadlist.end();++iter)
                ::pthread_detach(*iter);
        }

        void EventLoopThreadPool::start()
        {
            for(int i=0;i<threadsize;++i)
            {
                thread.start();
                threadlist.push_back(thread.getThreadId());
                epollerlist.push_back(thread.getEpoller());
            }
        }

        void EventLoopThreadPool::joinAll()
        {
            for(auto iter=threadlist.begin();iter!=threadlist.end();++iter)
                ::pthread_join(*iter,NULL);
        }

        void EventLoopThreadPool::addInChannelMap(unet::net::Channel* channel)
        {
            EpollerList::iterator iter = epollerlist.begin();
            
            for(auto it=epollerlist.begin();it!=epollerlist.end();++it)
            {
                if((*iter)->getConSize() <= (*it)->getConSize())
                    iter = it;
            }

            (*iter)->addInChannelMap(channel);
        }
    }
}
            





