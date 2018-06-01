/*************************************************************************
	> File Name: Socket.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月21日 星期二 00时12分57秒
 ************************************************************************/

#include"Socket.h"

namespace unet
{
    namespace base
    {
        Socket::Socket(SocketType type) noexcept :
            u_open(false),
            u_type(type),
            u_bit(0x00),
            u_socketfd(socket())
        {
            if(u_socketfd)
                u_open = true;
        };

        /*这种初始化方式是给服务端的Connect Socket准备的*/
        Socket::Socket(int connectfd) noexcept :
            u_open(false),
            u_type(CONNECT),
            u_bit(0x00),
            u_socketfd(connectfd)
        {
            if(u_socketfd)
                u_open = true;
            setKeepAlive();
        };

        Socket::Socket(Socket&& socket) :
            u_open(false),
            u_type(socket.u_type),
            u_bit(socket.u_bit)
        {
            /*dup只能用来操作FILE* */
            if(socket.u_open)
                u_socketfd = socket.u_socketfd;
            if(u_socketfd < 0)
                unet::handleError(errno);
            u_open = true;

            socket.u_open = false;
            socket.u_socketfd = -1;
        };
                
        Socket& Socket::operator=(Socket&& socket)
        {
            if(socket == *this)
                return *this;
            if(socket.u_open)
            {
                if(u_open)
                    close();
                u_open = false;
                
                u_type = socket.u_type;
                u_socketfd = socket.u_socketfd;
                if(u_socketfd < 0)
                    unet::handleError(errno);
                u_bit = socket.u_bit;
                u_open = true;

                socket.u_open = false;
                socket.u_socketfd = -1;
            }

            return *this;
        }

        Socket::~Socket() noexcept
        {
            if(u_open)
                close();
        };
        
        
        int Socket::setReuseAddr()
        {
            int m = 1;
            int n = ::setsockopt(u_socketfd,SOL_SOCKET,SO_REUSEADDR,&m,static_cast<socklen_t>(sizeof(int)));
                
            if(n == -1)
                unet::handleError(errno);
            setReuseAddrBit();
            return 0;
        }
        
        /*Buffer的设置需要在linten/connect之前*/
        int Socket::setRecvBuf(int multiple)
        {
            /*将RecvBuffer调整为MSS的偶数倍*/
            int recvbuf;
            multiple%2 ? recvbuf=(multiple-1)*MSS : recvbuf=multiple*MSS;
            int n = ::setsockopt(u_socketfd,SOL_SOCKET,SO_SNDBUF,&recvbuf,sizeof(recvbuf));
            if(n < 0)
                unet::handleError(errno);
            return 0;
        }

        int Socket::setSendBuf(int multiple)
        {
            /*将SendBuffer调整为MSS的偶数倍*/
            int sendbuf;
            multiple%2 ? sendbuf=(multiple-1)*MSS : sendbuf=multiple*MSS;
            int n = ::setsockopt(u_socketfd,SOL_SOCKET,SO_RCVBUF,&sendbuf,sizeof(sendbuf));
            if(n < 0)
                unet::handleError(errno);
            return 0;
        }

        int Socket::getRecvBuf() const
        {
            int recvbuf = 0;
            int len = sizeof(recvbuf);
            int n = ::getsockopt(u_socketfd,SOL_SOCKET,SO_SNDBUF,&recvbuf,(socklen_t*)&len);
            if(n < 0)
                unet::handleError(errno);
            return recvbuf;
        }

        int Socket::getSendBuf() const
        {
            int sendbuf = 0;
            int len = sizeof(sendbuf);
            int n = ::getsockopt(u_socketfd,SOL_SOCKET,SO_RCVBUF,&sendbuf,(socklen_t*)&len);
            if(n < 0)
                unet::handleError(errno);
            return sendbuf;
        }
        
        /*对低水位标记的设置需要在listen/connect之前*/
        /*在当前版本中，lowat = 1*/
        /*低水位标记不可调整*/
        int Socket::getRecvLowAt() const
        {
            int recv = 0;
            int len = sizeof(recv);
            int n = ::getsockopt(u_socketfd,SOL_SOCKET,SO_RCVLOWAT,&recv,(socklen_t*)&len);
            if(n < 0)
                unet::handleError(errno);
            return recv;
        }
        
        int Socket::getSendLowAt() const
        {
            int send = 0;
            int len = sizeof(send);
            int n = ::getsockopt(u_socketfd,SOL_SOCKET,SO_RCVLOWAT,&send,(socklen_t*)&len);
            if(n < 0)
                unet::handleError(errno);
            return send;
        }
        
        int Socket::setCloseRST()
        {
            struct linger lin;
            lin.l_onoff = 1;
            lin.l_linger = 0;
            
            int n = ::setsockopt(u_socketfd,SOL_SOCKET,SO_LINGER,&lin,sizeof(lin));
            if(n < 0)
                unet::handleError(errno);
            setLingerBit();
            return 0;
        }

        int Socket::setCloseDelay(int delay)
        {
            struct linger lin;
            lin.l_onoff = 1;
            lin.l_linger = delay;
            
            int n = ::setsockopt(u_socketfd,SOL_SOCKET,SO_LINGER,&lin,sizeof(lin));
            if(n < 0)
                unet::handleError(errno);
            setLingerBit();
            return 0;
        }
        
        int Socket::setCloseNormal()
        {
            struct linger lin;
            lin.l_onoff = 0;
            lin.l_linger = 1;
            
            int n = ::setsockopt(u_socketfd,SOL_SOCKET,SO_LINGER,&lin,sizeof(lin));
            if(n < 0)
                unet::handleError(errno);
            delLingerBit();
            return 0;
        }

        int Socket::setKeepAlive()
        {
            int m = 1;                
            int n = ::setsockopt(u_socketfd,SOL_SOCKET,SO_KEEPALIVE,&m,static_cast<socklen_t>(sizeof(int)));
                
            if(n == -1)
                unet::handleError(errno);
            setKeepAliveBit();
            return 0;
        }

        int Socket::setNodelay()
        {
            int m = 1;
            int n = ::setsockopt(u_socketfd,IPPROTO_TCP,TCP_NODELAY,&m,static_cast<socklen_t>(sizeof(int)));
                
            if(n == -1)
                unet::handleError(errno);
            setNoDelayBit();
            return 0;
        }

        int Socket::setNonBlockAndCloseOnExec()
        {
            int flag = ::fcntl(u_socketfd,F_GETFL,0);
            flag |= O_NONBLOCK;
            flag |= FD_CLOEXEC;
            int n = ::fcntl(u_socketfd,F_SETFL,flag);

            if(n == -1)
                unet::handleError(errno);
            setNonBlockAndCloseOnExecBit();
            return 0;
        }
        
        int Socket::delNonBlockAndCloseOnExec()
        {
            int flag = ::fcntl(u_socketfd,F_GETFL,0);
            flag &= ~O_NONBLOCK;
            int n = ::fcntl(u_socketfd,F_SETFL,flag);

            if(n == -1)
                unet::handleError(errno);
            delNonBlockAndCloseOnExecBit();
            return 0;
        }

        int Socket::socket(int family,int type,int protocol)
        {
            int n = ::socket(family,type,protocol);
            if(n  < 0)
                unet::handleError(errno);
            return n;
        }

        int Socket::listen()
        {
            /*对listen socket设置KeepAlive,ReuseAddr*/
            setKeepAlive();
            setReuseAddr();

            if(u_type == LISTEN)
            {
                /*COMMSIZE表示处于完全连接的socket上限(内核2.2之后)*/
                /*COMMSIZE表示处于半连接和完全连接的上限（内核2.2之前）*/
                int n = ::listen(u_socketfd,COMMSIZE);
                if(n < 0)
                    unet::handleError(errno);
            }
            else
                return -1;
            return 0;
        }

        int Socket::accept()
        {
            if(u_type == LISTEN)
            {
                int connectfd = ::accept(u_socketfd,nullptr,nullptr);
                if(connectfd < 0)
                    unet::handleError(errno);
                return connectfd;
            }
            else
                return -1;
        }
            
        int Socket::connect(InetAddress& addr)
        {
            setKeepAlive();

            if(u_type == CONNECT)
            {
                sockaddr* s_addr = addr.getSocketAddr();
                int n = ::connect(u_socketfd,s_addr,static_cast<socklen_t>(sizeof(struct sockaddr_in)));
                if(n < 0)
                    unet::handleError(errno);
            }
            else
                return -1;
            return 0;
        }
            
        int Socket::bind(InetAddress& addr)
        {
            if(u_type == LISTEN)
            {
                sockaddr* s_addr = addr.getSocketAddr();                
                int n = ::bind(u_socketfd,s_addr,static_cast<socklen_t>(sizeof(struct sockaddr_in)));
                if(n < 0)
                    unet::handleError(errno);
            }
            else
                return -1;
            return 0;
        }
            
        int Socket::close()
        {
            if(u_open)
            {
                int n = ::close(u_socketfd);
                if(n < 0)
                    unet::handleError(errno);
            }
            u_open = false;
            return 0;
        }

    }
}
