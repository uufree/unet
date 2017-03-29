/*************************************************************************
	> File Name: MutilAcceptor.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月29日 星期三 01时29分42秒
 ************************************************************************/

#ifndef _MUTILACCEPTOR_H
#define _MUTILACCEPTOR_H

#include"EventLoopThreadPool.h"

namespace uent
{
    namespace net
    {
        class MutilAcceptor final
        {
            typedef std::unique_ptr<unet::net::EventLoopThreadPool> PoolPtr;
            typedef std::function<Channel* (int sockfd)> NewConnectionCallBack;
            typedef std::function<void(Channel*)> AddInServerLoop;

            public:
                explicit MutilAcceptor();
                MutilAcceptor(const MutilAcceptor& lhs) = delete;
                MutilAcceptor& operator=(const MutilAcceptor& lhs) = delete;
                ~MutilAcceptor();
//public interface
                void listen();
                bool listened() {return listening;};

                void setNewConnectionCallBack(const NewConnectionCallBack& cb)
                {newconnectioncallback = cb;};
                
                void setAddInServerLoopCallBack(const AddInServerLoop& cb)
                {addinserverloop = cb;};

                void startLoop()
                {pool->start();};

            private:
                void handleRead();

                PoolPtr pool;
                InetAddress* serveraddr;
                int listenfd;
                Channel* listenchannel;
                bool listening;
                NewConnectionCallBack newconnectioncallback;
                AddInServerLoop addinserverloop;
                    
        };
    }
}
















#endif

