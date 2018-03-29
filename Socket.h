/*************************************************************************
	> File Name: Socket.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年02月27日 星期一 23时56分55秒
 ************************************************************************/

/*      封装套接字及其属性
 *在Socket类中用8位来标志socket属性，最高的两位保留，从低到高依次为：
 * used
 * keepalive
 * nodelay
 * noblock and closeonexec 
 * reuseaddr
 * reuseport
 */

#ifndef _SOCKET_H
#define _SOCKET_H

#include<assert.h>
#include<netinet/in.h>
#include<string.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<assert.h>
#include<netinet/tcp.h>
#include<fcntl.h>
#include<unistd.h>

#include"global.h"
#include"InetAddress.h"

namespace unet
{
    namespace base
    {
        enum SocketType{CONNECT,LISTEN};
        const static int COMMSIZE = 4096;
        
        class Socket final
        {//用RAII处理sockfd
            public:
                explicit Socket(SocketType type) noexcept :
                    _type(type),
                    _socketfd(socket()),
                    _bit(0x00)
                {};

                explicit Socket(int connectfd) noexcept :
                    _type(CONNECT),
                    _socketfd(connectfd),
                    _bit(0x00)
                {};

                explicit Socket(const Socket& socketfd_) = delete;
                explicit Socket(Socket&& socket) :
                    _type(std::move(socket._type)),
                    _socketfd(std::move(socket._socketfd)),
                    _bit(std::move(socket._bit))
                {};
                Socket& operator=(const Socket& lhs) = delete;
                Socket& operator=(Socket&& lhs)
                {
                    if(lhs == *this)
                        return *this;
                    close();
                    _type = std::move(lhs._type);
                    _socketfd = std::move(lhs._socketfd);
                    _bit = std::move(lhs._bit);
                    return *this;
                }

                ~Socket() noexcept{close();};
                
                bool operator==(const Socket& socket) {return _socketfd == socket._socketfd;};

                int getFd() const {return _socketfd;}
                SocketType getType() const {return _type;}
                bool isUsed() const {return _bit & (1 << 5);};
                void setUsedBit(){_bit |= (1 << 5);}
                bool isKeepAlive() const {return _bit & (1 << 4);};
                void setKeepAliveBit(){_bit |= (1 << 4);}
                bool isNoDelay() const{return _bit & (1 << 3);};
                void setNoDelayBit(){_bit |= (1 << 3);}
                bool isNonBlockAndCloseOnExec() const {return _bit & (1 << 2);};
                void setNonBlockAndCloseOnExecBit(){_bit |= (1 << 2);}
                bool isReuseAddr() const{return _bit & (1 << 1);};
                void setReuseAddrBit(){_bit |= (1 << 1);}
                bool isReusePort() const{return _bit & (1 << 0);};
                void setReusePortBit(){_bit |= (1 << 0);}
                
                int setKeepAlive();
                int setNodelay();
                int setNonBlockAndCloseOnExec();
                int setReuseAddr();
                int setReusePort();
                int setSendBuf(int sendbuf);    
                int setRecvBuf(int recvbuf);
                
                int socket(int family=AF_INET,int type=SOCK_STREAM,int protocol=IPPROTO_TCP);
                int listen();
                int accept();
                int connect(InetAddress& addr);
                int bind(InetAddress& addr);
                int close();

            private:
                SocketType _type;
                mutable int _socketfd;
                unsigned char _bit;
        };

    }
}



#endif

