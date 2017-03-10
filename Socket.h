/*************************************************************************
	> File Name: Socket.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年02月27日 星期一 23时56分55秒
 ************************************************************************/

#ifndef _SOCKET_H
#define _SOCKET_H

#include<assert.h>
#include"InetAddress.h"

namespace unet
{
    namespace net
    {
        int Sockets(int family,int type,int protocol)
        {
            int n = ::socket(family,type,protocol);
            assert(n > 0);
            return n;
        }

        class Socket final
        {
            public:

                static int COMMSIZE = 16;

                explicit Socket(const InetAddress& addr_) : addr(addr_),sockfd(Sockets(AF_INET,SOCK_STREAM,IPPROTO_TCP)),opened(true)
                {};
                
                Socket(const Socket&) = delete;
                Socket(Socket&&) = delete;

                ~Socket()
                {
                    if(opened)
                        assert(::close(sockfd) == 0);
                }

                void close()
                {
                    int n = ::close(sockfd);
                    assert(n >= 0);
                }

                void shutDownWrite()
                {
                    int n = ::shutdown(sockfd,SHUT_WR);
                    assert(n >= 0);
                }

                int getSocket() const
                {
                    return sockfd;
                }
/*
                void listenNonBlock()
                {
                    setNonBlockAndOnExec();
                    int n = ::listen(sockfd,COMMSIZE);
                    assert(n >= 0);
                }
*/
                void listen()
                {
                    int n = ::listen(sockfd,COMMSIZE);
                    assert(n >= 0);
                }

                int accept()
                {
                    int connetfd = ::accept(sockfd,&addr,static_cast<socklen_t>(sizeof(sockaddr_in)));         
                    assert(connetfd > 0);
                    return connetfd;
                }

                void connect();
                {
                   int n = ::connect(sockfd,&addr,static_cast<socklen_t>(sizeof(sockaddr_in)));
                   assert(n >= 0);
                }

                void bind()
                {
                    int n = ::bind(sockfd,&addr,static_cast<socklen_t>(sizeof(struct sockaddr_in)));
                    assert(n >= 0);
                }

                void setKeepAlive()
                {

                    int m = 1;
                    int n = ::setsockopt(sockfd,SOL_SOCKET,SO_KEEPALIVE,&m,static_cast<socklen_t>(sizeof(int)));
                    assert(n >= 0);
                }

//                void setSocketBuf(int size);

                void setNodelay()
                {
                    int m = 1;
                    int n = ::setsockopt(sockfd,IPPROTO_TCP,TCP_NODELAY,&m,static_cast<socklen_t>(sizeof(int)));
                    assert(n >= 0);
                }

                void setNonBlockAndCloseOnExec()
                {
                    int flag = ::fcntl(sockfd,F_GETFL,0);
                    flag |= O_NONBLOCK;
                    flag |= FD_CLOEXEC;
                    int n = ::fcntl(sockfd,F_SETFL,flag);
                    assert(n >= 0);
                }

            private:
                bool opened;
                InetAddress addr;
                const int sockfd;
        }
    }

}



#endif

