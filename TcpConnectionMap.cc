/*************************************************************************
	> File Name: TcpConnectionMap.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年06月12日 星期一 22时34分35秒
 ************************************************************************/

#include"TcpConnectionMap.h"

namespace unet
{
    bool operator==(const TcpConnectionMap& lhs,const TcpConnectionMap& rhs)
    {return lhs.size() == rhs.size();};

    TcpConnectionMap::TcpConnectionMap(TcpConnectionMap&& lhs) :
        u_tcpConnectionMap(std::move(lhs.u_tcpConnectionMap))
    {};

    TcpConnectionMap& TcpConnectionMap::operator=(TcpConnectionMap&& lhs)
    {
        if(lhs == *this)
            return *this;
        u_tcpConnectionMap = std::move(lhs.u_tcpConnectionMap);
        return *this;
    }

    void TcpConnectionMap::insert(int fd)
    {
        base::MutexLockGuard guard(u_mutex);
        TcpConnectionPtr tcp(new TcpConnection(fd));
        u_tcpConnectionMap.insert({fd,tcp});
    }

    void TcpConnectionMap::insert(const TcpConnectionPtr& lhs)
    {
        base::MutexLockGuard guard(u_mutex);
        u_tcpConnectionMap.insert({lhs->getFd(),lhs});
    }

    void TcpConnectionMap::erase(int fd) 
    {
        base::MutexLockGuard guard(u_mutex);
        u_tcpConnectionMap.erase(fd);
    }

    TcpConnectionMap::TcpConnectionPtr& TcpConnectionMap::find(int fd)
    {
        base::MutexLockGuard guard(u_mutex);
        return u_tcpConnectionMap[fd];
    }
}

