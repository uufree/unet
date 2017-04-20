/*************************************************************************
	> File Name: AsyncTcpServer.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年04月17日 星期一 16时27分51秒
 ************************************************************************/

#ifndef _ASYNCTCPSERVER_H
#define _ASYNCTCPSERVER_H

#include"AsyncEventLoop.h"
#include"../Epoller.h"
#include"TaskThreadPool.h"
#include"AsyncAcceptor.h"

namespace unet
{
    namespace net
    {
        class AsyncTcpServer final
        {
            typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
            typedef std::map<int,TcpConnectionPtr> TcpConnectionPtrMap;
            typedef std::function<void(Buffer*,Buffer*)> MessageCallBack;
            typedef std::vector<Channel*> ChannelList;

            public:
                AsyncTcpServer(InetAddress* addr_,int size = 2);
                AsyncTcpServer(const AsyncTcpServer& lhs) = delete;
                AsyncTcpServer& operator=(const AsyncTcpServer& lhs) = delete;
                ~AsyncTcpServer(){};

                void setReadCallBack(const MessageCallBack& cb)
                {readcallback = cb;};

                void setWriteCallBack(const MessageCallBack& cb)
                {writecallback = cb;};

                void setDrivedCallBack(const MessageCallBack& cb)
                {drivedcallback = cb;};

                void handleDiedTcpConnection(int fd);
                void start();
                
                void getActiveChannels(ChannelList* channels)
                {epoller->epoll(channels);};
                
                void addChannelInPool(ChannelList* channels)
                {
                    pool->addInChannelQueue(channels);
                };
            
            private:
                Channel* newConnectionCallBack(int fd_);
                void addInServerLoop(Channel* channel);

                InetAddress* serveraddr;
                std::unique_ptr<AsyncEventLoop> loop;
                std::unique_ptr<Epoller> epoller;
                std::unique_ptr<AsyncAcceptor> acceptor;
                std::unique_ptr<unet::thread::TaskThreadPool> pool;
                TcpConnectionPtrMap tcpconnectionptrmap;
                MessageCallBack readcallback,writecallback,drivedcallback;
                thread::MutexLock lock;
        };
    }
}

#endif

