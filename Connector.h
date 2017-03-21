/*************************************************************************
	> File Name: Connector.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月21日 星期二 20时36分08秒
 ************************************************************************/

#ifndef _CONNECTOR_H
#define _CONNECTOR_H

#include"Epoller.h"

namespace unet
{
    namespace net
    {
        class Connector final
        {
            typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

            public:
                Connector();
                Connector(const Connector& lhs) = delete;
                Connector& operator=(const Connector& lhs) = delete;
                ~Connector(){};





            private:
                socket::Socket confd;
                InetAddress clientaddr;
                Channel* connectchannel;
        };
    }
}
            





        





















#endif

