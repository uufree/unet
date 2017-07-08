/*************************************************************************
	> File Name: AsyncTcpServer.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年04月17日 星期一 16时27分51秒
 ************************************************************************/

#ifndef _ASYNCTCPSERVER_H
#define _ASYNCTCPSERVER_H

#include"AsyncAcceptor.h"
#include"../TcpConnectionMap.h"
#include"../ChannelMap.h"
#include"../EventList.h"
#include"../Epoller.h"
#include"../EventLoop.h"
#include"../ThreadPool.h"
#include"../ThreadPool.h"

namespace unet
{
    namespace net
    {
        class AsyncTcpServer final
        {
            typedef std::function<void(Buffer*,Buffer*)> MessageCallBack;
            typedef std::vector<Channel&> ChannelList;

            public:
                explicit AsyncTcpServer(socket::InetAddress& server,int size = 2);
                AsyncTcpServer(const AsyncTcpServer& lhs) = delete;
                AsyncTcpServer& operator=(const AsyncTcpServer& lhs) = delete;
                AsyncTcpServer(AsyncTcpServer&& lhs);
                AsyncTcpServer& operator=(AsyncTcpServer&& lhs);
                ~AsyncTcpServer();

                inline void setReadCallBack(const MessageCallBack& cb);
                inline void setWriteCallBack(const MessageCallBack& cb);
                void start();
             
            private:
                void InsertChannel(Channel&& channel);
                void EraseChannel(int fd);
                void GetActiveChannels();

            private:
                socket::InetAddress& serveraddr;

                TcpConnectionMap tcpconnectionMap;
                ChannelMap channelMap;
                EventList eventList;
                ChannelList channelList;
                thread::ThreadPool pool;                

                Epoller epoller;
                EventLoop eventLoop;
                AsyncAcceptor asyncAcceptor;

                MessageCallBack readCallBack,writeCallBack;
        };
    }
}

#endif

