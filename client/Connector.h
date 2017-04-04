/*************************************************************************
	> File Name: Connector.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月21日 星期二 20时36分08秒
 ************************************************************************/

#ifndef _CONNECTOR_H
#define _CONNECTOR_H

#include"../Epoller.h"
#include"../EventLoop.h"

namespace unet
{
    namespace net
    {
        class Connector final
        {
            typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
            typedef std::function<void(TcpConnectionPtr&&)> ConnectionCallBack;
            typedef std::vector<Channel*> ChannelList;

            public:
                Connector(InetAddress* serveraddr_);
                Connector(const Connector& lhs) = delete;
                Connector& operator=(const Connector& lhs) = delete;
                ~Connector(){};
                
                void setConnectionCallBack(const ConnectionCallBack& cb)
                {connectioncallback = cb;};
                
                int createConnection();
                void createChannel();
                void start();
                void getActiveChannels(ChannelList* channels);

            private:
                socket::Socket confd;
                InetAddress* serveraddr;
                Channel* connectchannel;
                bool connected;
                ConnectionCallBack connectioncallback;
                std::unique_ptr<Epoller> epoller;
                std::unique_ptr<EventLoop> loop;
        };
    }
}
            





        





















#endif

