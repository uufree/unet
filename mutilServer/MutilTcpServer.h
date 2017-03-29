/*************************************************************************
	> File Name: MutilTcpServer.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月29日 星期三 02时30分52秒
 ************************************************************************/

#ifndef _MUTILTCPSERVER_H
#define _MUTILTCPSERVER_H

#include"MutilAcceptor.h"

namespace unet
{   
    namespace net
    {
        class MutilTcpServer final
        {
            typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
            typedef std::map<int,TcpConnectionPtr> TcpConnectionPtrMap;//保存描述连接的智能指针，索引是连接中保存的fd
            typedef std::function<void(Buffer*,Buffer*)> MessageCallBack;
            public:
                





            private:
                Channel* newConnectionCallBack(int fd_);//设置新连接到来时的处理方式
                
                InetAddress* serveraddr;
                std::unique_ptr<MutilAcceptor> acceptor;
                TcpConnectionPtrMap tcpconnectionptrmap;
                MessageCallBack readcallback,writecallback;
        };

#endif

