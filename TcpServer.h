/*************************************************************************
	> File Name: TcpServer.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月11日 星期六 19时18分24秒
 ************************************************************************/

#ifndef _TCPSERVER_H
#define _TCPSERVER_H

namespace unet
{
    namespace net
    {
        class TcpServer final
        {
            public:
                TcpServer(EventLoop* eventloop_);
                TcpServer(const TcpServer& lhs) = delete;
                TcpServer& operator(const TcpServer& lhs) = delete;
                ~TcpServer() {};
//public interface
                EventLoop* getEventLoop() {return loop;};
                void start();

            private:
                typedef std::weak_ptr<TcpConnection> TcpConnectionWptr;

                Channel* newConnectionCallBack(int fd_,InetAddr& clientaddr);

                std::map<int,TcpConnectionWptr> connectionwptrmap;
                std::unique<Acceptor> acceptoruptr;
                EventLoop* loop;
                InetAddr* serveraddr;
        };

    }
}






































#endif

