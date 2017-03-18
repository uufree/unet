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
        namespace socket
        {
            const static int COMMSIZE = 256;

            int socket(int family=AF_INET,int type=SOCK_STREAM,int protocol=IPPROTO_TCP)
            {
                int n = ::socket(family,type,protocol);
                assert(n > 0);
                return n;
            }

            void setNodelay(int sockfd)
            {
                int m = 1;
                int n = ::setsockopt(sockfd,IPPROTO_TCP,TCP_NODELAY,&m,static_cast<socklen_t>(sizeof(int)));
                assert(n >= 0);
            }

            void setNonBlockAndCloseOnExec(int sockfd)
            {
                int flag = ::fcntl(sockfd,F_GETFL,0);
                flag |= O_NONBLOCK;
                flag |= FD_CLOEXEC;
                int n = ::fcntl(sockfd,F_SETFL,flag);
                assert(n >= 0);
            }

            void listen(int sockfd)
            {
                int n = ::listen(sockfd,COMMSIZE);
                assert(n >= 0);
            }

            int accept(int sockfd,InetAddress& clientaddr)
            {
                int connetfd = ::accept(sockfd,nullptr,nullptr);         
                assert(connetfd > 0);
                return connetfd;
            }

            void connect(int sockfd,InetAddress* addr)
            {
                sockaddr_in addr_ = addr->getSockaddr();
                int n = ::connect(sockfd,(sockaddr*)(&addr_),static_cast<socklen_t>(sizeof(struct sockaddr_in)));
                
                assert(n >= 0);
            }

            void bind(int sockfd,InetAddress* addr)
            {
                sockaddr_in addr_ = addr->getSockaddr();                
                int n = ::bind(sockfd,(sockaddr*)(&addr_),static_cast<socklen_t>(sizeof(struct sockaddr_in)));
                assert(n >= 0);
            }

            void setKeepAlive(int sockfd)
            {
                int m = 1;
                int n = ::setsockopt(sockfd,SOL_SOCKET,SO_KEEPALIVE,&m,static_cast<socklen_t>(sizeof(int)));
                assert(n >= 0);
            }

            void shutdownWrite(int sockfd)
            {
                int n = ::shutdown(sockfd,SHUT_WR);
                assert(n >= 0);
            }

            void shutdownRead(int sockfd)
            {
                int n = ::shutdown(sockfd,SHUT_RD);
                assert(n >= 0);
            }

            void close(int sockfd)
            {
                int n = ::close(sockfd);
                assert(n >= 0);
            }

            class Socket final
            {//用RAII处理sockfd
                public:
                    explicit Socket(int sockfd_) : sockfd(sockfd_),opened(true)
                    {};
                
                    Socket(const Socket&) = delete;
                    Socket(Socket&&) = delete;

                    ~Socket()
                    {
                        if(opened)
                            assert(::close(sockfd) == 0);
                    }

                    int getFd() const
                    {   
                        return sockfd;
                    }

                private:
                    const int sockfd;
                    bool opened;
            };
            
        }
    }
}



#endif

