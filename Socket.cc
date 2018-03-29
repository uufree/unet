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
        int Socket::socket(int family,int type,int protocol)
        {
            int n = ::socket(family,type,protocol);
            if(n  < 0)
                unet::handleError(errno);
            return n;
        }

        int Socket::listen()
        {//以下设置针对服务器端
            setKeepAlive();
            setReuseAddr();
            setReusePort();

            if(_type == LISTEN)
            {
                int n = ::listen(_socketfd,COMMSIZE);
                if(n < 0)
                    unet::handleError(errno);
            }
            else
                return -1;
            return 0;
        }

        int Socket::accept()
        {
            if(_type == LISTEN)
            {
                int connectfd = ::accept(_socketfd,nullptr,nullptr);
                if(connectfd < 0)
                    unet::handleError(errno);
                return connectfd;
            }
            else
                return -1;
        }
            
        int Socket::connect(InetAddress& addr)
        {//针对客户端
            setKeepAlive();

            if(_type == CONNECT)
            {
                sockaddr* s_addr = addr.getSocketAddr();
                int n = ::connect(_socketfd,s_addr,static_cast<socklen_t>(sizeof(struct sockaddr_in)));
                if(n < 0)
                    unet::handleError(errno);
            }
            else
                return -1;
            return 0;
        }
            
        int Socket::bind(InetAddress& addr)
        {
            if(_type == LISTEN)
            {
                sockaddr* s_addr = addr.getSocketAddr();                
                int n = ::bind(_socketfd,s_addr,static_cast<socklen_t>(sizeof(struct sockaddr_in)));
                if(n < 0)
                    unet::handleError(errno);
            }
            else
                return -1;
            return 0;
        }
            
        int Socket::close()
        {
            int n = ::close(_socketfd);
            if(n < 0)
                unet::handleError(errno);
            return 0;
        }

        int Socket::setKeepAlive()
        {
            int m = 1;                
            int n = ::setsockopt(_socketfd,SOL_SOCKET,SO_KEEPALIVE,&m,static_cast<socklen_t>(sizeof(int)));
                
            if(n == -1)
                unet::handleError(errno);
            setKeepAliveBit();
            return 0;
        }

        int Socket::setNodelay()
        {
            int m = 1;
            int n = ::setsockopt(_socketfd,IPPROTO_TCP,TCP_NODELAY,&m,static_cast<socklen_t>(sizeof(int)));
                
            if(n == -1)
                unet::handleError(errno);
            setNoDelayBit();
            return 0;
        }

        int Socket::setNonBlockAndCloseOnExec()
        {
            int flag = ::fcntl(_socketfd,F_GETFL,0);
            flag |= O_NONBLOCK;
            flag |= FD_CLOEXEC;
            int n = ::fcntl(_socketfd,F_SETFL,flag);

            if(n == -1)
                unet::handleError(errno);
            setNonBlockAndCloseOnExecBit();
            return 0;
        }
            
        int Socket::setReuseAddr()
        {
            int m = 1;
            int n = ::setsockopt(_socketfd,SOL_SOCKET,SO_REUSEADDR,&m,static_cast<socklen_t>(sizeof(int)));
                
            if(n == -1)
                unet::handleError(errno);
            setReuseAddrBit();
            return 0;
        }

        int Socket::setReusePort()
        {
            int m = 1;
            int n = ::setsockopt(_socketfd,SOL_SOCKET,SO_REUSEPORT,&m,static_cast<socklen_t>(sizeof(int)));
                
            if(n == -1)
                unet::handleError(errno);
            setReusePortBit();
            return 0;
        }

        int Socket::setRecvBuf(int multiple)
        {
            int recvbuf;
            multiple%2 ? recvbuf=(multiple-1)*MSS : recvbuf=multiple*MSS;
            int n = ::setsockopt(_socketfd,SOL_SOCKET,SO_SNDBUF,&recvbuf,sizeof(recvbuf));
            if(n < 0)
                unet::handleError(errno);
            return 0;
        }

        int Socket::setSendBuf(int multiple)
        {
            int sendbuf;
            multiple%2 ? sendbuf=(multiple-1)*MSS : sendbuf=multiple*MSS;
            int n = ::setsockopt(_socketfd,SOL_SOCKET,SO_RCVBUF,&sendbuf,sizeof(sendbuf));
            if(n < 0)
                unet::handleError(errno);
            return 0;
        }

        int Socket::getRecvBuf()
        {
            int recvbuf = 0;
            int len = sizeof(recvbuf);
            int n = ::getsockopt(_socketfd,SOL_SOCKET,SO_SNDBUF,&recvbuf,(socklen_t*)&len);
            if(n < 0)
                unet::handleError(errno);
            return recvbuf;
        }

        int Socket::getSendBuf()
        {
            int sendbuf = 0;
            int len = sizeof(sendbuf);
            int n = ::getsockopt(_socketfd,SOL_SOCKET,SO_RCVBUF,&sendbuf,(socklen_t*)&len);
            if(n < 0)
                unet::handleError(errno);
            return sendbuf;
        }
    }
}
