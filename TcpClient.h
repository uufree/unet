/*************************************************************************
  > File Name: TcpClient.h
  > Author: uuchen
  > Mail: 1319081676@qq.com
  > Created Time: 2017年03月21日 星期二 20时32分11秒
 ************************************************************************/

#ifndef _TCPCLIENT_H
#define _TCPCLIENT_H

#include"Connector.h"

namespace unet
{
    namespace net
    {
        class TcpClient final
        {
            typedef std::function<Buffer*,Buffer*> MessageCallBack;
            typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

            public:
            explicit TcpClient(const InetAddress& serveraddr_);
            TcpClient(const TcpClient& lhs) = delete;
            TcpClient& operator=(const TcpClient& lhs) = delete;
            ~TcpClient();

            void setReadCallBack(const MessageCallBack& cb)
            {readcallback = cb;};

            void setWriteCallBack(const MessageCallBack& cb)
            {writecallback = cb;};

            void handleDiedTcpConnection(TcpConnection* con)
            {ptr.reset();};

            void start()
            {
                connector->start();
            }

            private:
            InetAddress serveraddr;
            std::unique_ptr<Connector> connector;
            TcpConnectionPtr ptr;
            MessageCallBack readcallback,writecallback;
        };
    }
}
















































#endif

