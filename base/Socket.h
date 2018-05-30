/*************************************************************************
	> File Name: Socket.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年02月27日 星期一 23时56分55秒
 ************************************************************************/

/*需要封装的Socket选项：
 * SO_REUSEADDR：重用处于TIME_WAIT的地址（bind之前调用）            bit0
 * SO_RCVBUF：发送缓冲区
 * SO_SNDBUF：接收缓冲区
 * SO_RCVLOWAT：接收低水位标记
 * SO_SNDLOWAT：发送低水位标记
 * SO_LINGER：使用异常关闭，清理资源。用于服务端主动关闭（RST）     bit1
 * SO_KEEPALIVE：保活，可以被应用层的心跳协议替代                   bit2
 * TCP_NODELAY：关闭延迟算法                                        bit3
 * O_NONBLOCK and FD_CLOEXEC：非阻塞以及FORK关闭                    bit4
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

#include"Global.h"
#include"InetAddress.h"

/*2018.05.29 测试完成*/

namespace unet
{
    namespace base
    {
        enum SocketType{CONNECT,LISTEN};
        const static int COMMSIZE = 4096;
        
        class Socket final
        {
            public:
                explicit Socket(SocketType type) noexcept;
                explicit Socket(int connectfd) noexcept;
                Socket(const Socket& socketfd_) = delete;
                Socket(Socket&& socket);
                Socket& operator=(const Socket& lhs) = delete;
                Socket& operator=(Socket&& lhs);
                ~Socket() noexcept;
                
                bool operator==(const Socket& socket) {return u_socketfd == socket.u_socketfd;};

                bool isUsed() const {return u_open;};
                bool isReuseAddr() const{return u_bit & (1 << 0);};
                bool isLinger() const{return u_bit & (1 << 1);};
                bool isKeepAlive() const {return u_bit & (1 << 2);};
                bool isNoDelay() const{return u_bit & (1 << 3);};
                bool isNonBlockAndCloseOnExec() const {return u_bit & (1 << 4);};
                
                int setReuseAddr();
                
                /*Functionality:
                 *      调整Send/Recv Buffer为MSS的偶数倍。在Listen Socket上调整
                 *      Buffer Size才会起作用。当前版本无此限制，可随时调整。
                 *Parameters：
                 *       1）[in]:MSS的倍数
                 *Returned Value:
                 *       None
                 */
                int setSendBuf(int);    
                int setRecvBuf(int);
                int getSendBuf();
                int getRecvBuf();
                
                int getRecvLowAt();
                int getSendLowAt();
                int setCloseRST();
                int setCloseDelay(int);
                int setCloseNormal();
                int setKeepAlive();
                int setNodelay();
                int setNonBlockAndCloseOnExec();
                
                int getFd() const {return u_socketfd;}
                SocketType getType() const {return u_type;}

                int socket(int family=AF_INET,int type=SOCK_STREAM,int protocol=IPPROTO_TCP);
                int listen();
                int accept();
                int connect(InetAddress& addr);
                int bind(InetAddress& addr);
                int close();
                
                int read(char* buf,size_t size){return ::read(u_socketfd,buf,size);};
                int write(const char* buf,size_t size){return ::write(u_socketfd,buf,size);};
                int blockRead(char* buf,size_t buflen) {return readn(u_socketfd,buf,buflen);};
                int blockWrite(const char* buf,size_t buflen) {return writen(u_socketfd,buf,buflen);};
            
            private:
                void setReuseAddrBit(){u_bit |= (1 << 0);}
                void setLingerBit(){u_bit |= (1 << 1);};
                void setKeepAliveBit(){u_bit |= (1 << 2);}
                void setNoDelayBit(){u_bit |= (1 << 3);}
                void setNonBlockAndCloseOnExecBit(){u_bit |= (1 << 4);}
                
                void delReuseAddrBit(){u_bit &= ~(1 << 0);}
                void delLingerBit(){u_bit &= ~(1 << 1);};
                void delKeepAliveBit(){u_bit &= ~(1 << 2);}
                void delNoDelayBit(){u_bit &= ~(1 << 3);}
                void delNonBlockAndCloseOnExecBit(){u_bit &= ~(1 << 4);}

            private:
                bool u_open;
                SocketType u_type;
                unsigned char u_bit;
                mutable int u_socketfd;
                static const int MSS = 1460;
        };

    }
}



#endif

