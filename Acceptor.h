/*************************************************************************
	> File Name: Acceptor.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月10日 星期五 08时45分48秒
 ************************************************************************/

#ifndef _ACCEPTOR_H
#define _ACCEPTOR_H

namespace unet
{
    namespace net
    {
        class Acceptor final
        {
            public:
                Acceptor(EventLoop* loop_,const& InetAddress& addr_);
                Acceptor(const Acceptor& lhs) = delete;
                Acceptor& operator(const Acceptor& lhs) = delete;
                ~Acceptor() {};
//public interface
                void listen();
                bool listening() {return listening};
                void getActiveChannels();

                void setNewConnectionCallBack(const newConnectionCallBack& cb)
                {newconnectioncallback = cb;};


            private:
                typedef std::vector<Channel*> ChannelList;
                typedef std::function<Channel* (int sockfd,const InetAddr& lhs)> NewConnectionCallBack;
                typedef std::vector<Channel*> ChannelList;

                void handleRead();

                EventLoop* loop;
                Socket listenfd;
                Channel listenchannel;
                Epoller epoller;
                bool listening;
                NewConnectionCallBack newconnectioncallback;
        };

    }
}















#endif

