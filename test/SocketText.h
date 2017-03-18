/*************************************************************************
	> File Name: SocketText.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月17日 星期五 22时33分06秒
 ************************************************************************/

#ifndef _SOCKETTEXT_H
#define _SOCKETTEXT_H

#include<iostream>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<assert.h>
#include<netinet/tcp.h>
#include<fcntl.h>
#include<unistd.h>

using namespace std;


    namespace net
    {
        class InetAddress final
        {   
            public:
                static const int IPV4SIZE = 16;
                static const int IPV6SIZE = 32;
                
                InetAddress(const InetAddress& addr) = delete;
                InetAddress& operator=(const InetAddress& addr_) = delete;
                ~InetAddress() {};

                explicit InetAddress(uint16_t port)
                {
                    bzero(&addr,sizeof(addr));
                    addr.sin_port = htons(port);
                    addr.sin_family = AF_INET;
                    addr.sin_addr.s_addr = htonl(INADDR_ANY);
                };

                explicit InetAddress(const std::string& ip,uint16_t port) : ip_(ip)
                {  
                    bzero(&addr,sizeof(addr));
                    addr.sin_port = htons(port);
                    addr.sin_family = AF_INET;
                    inet_pton(AF_INET,ip.c_str(),&addr.sin_addr.s_addr);
//                    addr.sin_addr.s_addr = htonl(addr.sin_addr.s_addr);
                };   

                std::string getIpString()
                {
                    return ip_;
                };
            
                uint32_t getIp() 
                {
                    in_addr_t addr_ = ntohl(addr.sin_addr.s_addr);
                    return static_cast<uint32_t>(addr_);
                };   

                uint16_t getPort() 
                {

                    in_port_t port = ntohs(addr.sin_port);
                    return static_cast<uint16_t>(port);
                };

                sockaddr_in getSockaddr() const
                {
                    return addr;
                }

            private:
                struct sockaddr_in addr;
                std::string ip_ = "INADDR_ANY";//网络字节序的准换有问题，所以只能这么写
        };
            
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

            int accept(int sockfd)
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


#endif

