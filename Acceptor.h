/*************************************************************************
	> File Name: Acceptor.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月10日 星期五 08时45分48秒
 ************************************************************************/

#ifndef _ACCEPTOR_H
#define _ACCEPTOR_H

#include"Epoller.h"

using namespace unet::rapid;

namespace unet
{
    namespace net
    {
        class Acceptor final
        {
            public:          
                explicit Acceptor(EventLoop* loop_,const InetAddress* addr_);
                Acceptor(const Acceptor& lhs) = delete;
                Acceptor& operator=(const Acceptor& lhs) = delete;
                ~Acceptor() {};
//public interface
                void listen();
                bool listened() {return listening;};
                void getActiveChannels();

                void setNewConnectionCallBack(const NewConnectionCallBack& cb)
                {newconnectioncallback = cb;};
            private:

                void handleRead();

                EventLoop* loop;
                InetAddress* serveraddr;
                socket::Socket listenfd;
                Channel listenchannel;
                Epoller* epoller;
                bool listening;
                NewConnectionCallBack newconnectioncallback;
        };

    }
}















#endif

