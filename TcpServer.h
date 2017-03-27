/*************************************************************************
  > File Name: TcpServer.h
  > Author: uuchen
  > Mail: 1319081676@qq.com
  > Created Time: 2017年03月11日 星期六 19时18分24秒
 ************************************************************************/

#ifndef _TCPSERVER_H
#define _TCPSERVER_H

#include<assert.h>
#include<algorithm>
#include"Acceptor.h"

namespace unet
{
    namespace net
    {
        class TcpServer final
        {
            typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
            typedef std::map<int,TcpConnectionPtr> TcpConnectionPtrMap;//保存描述连接的智能指针，索引是连接中保存的fd
            typedef std::function<void(Buffer*,Buffer*)> MessageCallBack;

            public:     
            explicit TcpServer(InetAddress* serveraddr_);
            TcpServer(const TcpServer& lhs) = delete;
            TcpServer& operator=(const TcpServer& lhs) = delete;
            ~TcpServer() {};
            //public interface
            void start();//TcpServer开始工作
            void setReadCallBack(const MessageCallBack& cb)
            {readcallback = cb;}

            void setWriteCallBack(const MessageCallBack& cb)
            {writecallback = cb;};

            void handleDiedTcpConnection(int fd);

            private:
            InetAddress* serveraddr;
            Channel* newConnectionCallBack(int fd_);//设置新连接到来时的处理方式
            std::unique_ptr<Acceptor> acceptor;
            TcpConnectionPtrMap tcpconnectionptrmap;
            MessageCallBack readcallback,writecallback;
        };

    }
}






































#endif

