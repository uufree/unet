/*************************************************************************
  > File Name: TcpClient.h
  > Author: uuchen
  > Mail: 1319081676@qq.com
  > Created Time: 2017年03月21日 星期二 20时32分11秒
 ************************************************************************/

#ifndef _TCPCLIENT_H
#define _TCPCLIENT_H

#include"Connector.h"
#include<functional>
#include<iostream>

namespace unet
{
    namespace net
    {
        class TcpClient final
        {
            typedef std::function<void(Buffer*,Buffer*)> MessageCallBack;
            typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

            public:
                explicit TcpClient(InetAddress* serveraddr_);
                TcpClient(const TcpClient& lhs) = delete;
                TcpClient& operator=(const TcpClient& lhs) = delete;
                ~TcpClient() {};

                void setReadCallBack(const MessageCallBack& cb)
                {ptr->setReadCallBack(cb);};

                void setWriteCallBack(const MessageCallBack& cb)
                {ptr->setWriteCallBack(cb);};
            
                void setDrivedCallBack(const MessageCallBack& cb)
                {ptr->setDrivedCallBack(cb);};

                void handleDiedTcpConnection(int fd)
                {ptr.reset();};

                void start()
                {
                    connector->start();
                }

                void setTcpConnectionPtr(TcpConnectionPtr&& ptr_)
                {
                    ptr.reset(ptr_.get());
                }

                int getFd()
                {return ptr->getFd();};

            private:
                InetAddress* serveraddr;
                std::unique_ptr<Connector> connector;
                TcpConnectionPtr ptr;
        };
    }
}
















































#endif

