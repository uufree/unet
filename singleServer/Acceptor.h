/*************************************************************************
	> File Name: Acceptor.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月10日 星期五 08时45分48秒
 ************************************************************************/

#ifndef _ACCEPTOR_H
#define _ACCEPTOR_H

#include"../Epoller.h"
#include"../EventLoop.h"

namespace unet
{
    namespace net
    {
        class Acceptor final
        {
            typedef std::function<Channel* (int sockfd)> NewConnectionCallBack;
            typedef std::vector<Channel*> ChannelList;
            typedef std::function<void()> Functor;

            public:          
                explicit Acceptor(InetAddress* addr_);
                Acceptor(const Acceptor& lhs) = delete;
                Acceptor& operator=(const Acceptor& lhs) = delete;
                ~Acceptor() {::close(listenfd);};
//public interface
                void listen();
                bool listened() {return listening;};
                void startLoop() {loop->loop();};
                void getActiveChannels(ChannelList* channels);

                void setNewConnectionCallBack(const NewConnectionCallBack& cb)
                {newconnectioncallback = cb;};
                
                void addFunctorInLoop(const Functor& fun)
                {
                    loop->addFunctorInLoop(fun);
                }

            private:

                void handleRead();

                std::unique_ptr<EventLoop> loop;
                InetAddress* serveraddr;
                int listenfd;
                Channel* listenchannel;
                std::unique_ptr<Epoller> epoller;
                bool listening;
                NewConnectionCallBack newconnectioncallback;
        };

    }
}















#endif

