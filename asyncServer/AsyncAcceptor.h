/*************************************************************************
	> File Name: AsyncAcceptor.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年04月17日 星期一 20时40分14秒
 ************************************************************************/

#ifndef _ASYNCACCEPTOR_H
#define _ASYNCACCEPTOR_H

#include<functional>
#include"../Channel.h"


namespace unet
{
    namespace net
    {
        class AsyncAcceptor final
        {
            typedef std::function<Channel* (int sockfd)> NewConnectionCallBack;
            typedef std::function<void(Channel*)> AddInServerLoopCallBack;

            public:
                explicit AsyncAcceptor(unet::net::InetAddress* addr_);
                AsyncAcceptor(const AsyncAcceptor& lhs) = delete;
                AsyncAcceptor& operator=(const AsyncAcceptor& lhs) = delete;
                ~AsyncAcceptor();
//public interface
                void listen();
                bool listened() {return listening;};

                void setNewConnectionCallBack(const NewConnectionCallBack& cb)
                {newconnectioncallback = cb;};
                
                void setAddInServerLoopCallBack(const AddInServerLoopCallBack& cb)
                {addinserverloop = cb;};
            

            private:
                void handleRead();

                unet::net::InetAddress* serveraddr;
                int listenfd;
                Channel* listenchannel;
                bool listening;
                NewConnectionCallBack newconnectioncallback;
                AddInServerLoopCallBack addinserverloop;                    
                Channel* channel_;
        };
    }
}



















#endif

