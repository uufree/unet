/*************************************************************************
	> File Name: TcpConnectionMap.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年06月12日 星期一 22时34分35秒
 ************************************************************************/

#include"TcpConnectionMap.h"

namespace unet
{

    TcpConnectionMap::TcpConnectionMap(TcpConnectionMap&& lhs) :
        _tcpConnectionMap(std::move(lhs._tcpConnectionMap))
    {};

    TcpConnectionMap& TcpConnectionMap::operator=(TcpConnectionMap&& lhs)
    {
        _tcpConnectionMap = std::move(lhs._tcpConnectionMap);
        return *this;
    }

    void TcpConnectionMap::insert(int fd)
    {
        base::MutexLockGuard guard(_mutex);
        TcpConnectionPtr tcp(new TcpConnection(fd));
        _tcpConnectionMap.insert({fd,tcp});
    }

    void TcpConnectionMap::insert(const TcpConnectionPtr& lhs)
    {
        base::MutexLockGuard guard(_mutex);
        _tcpConnectionMap.insert({lhs->getFd(),lhs});
    }

    void TcpConnectionMap::erase(int fd) 
    {
        base::MutexLockGuard guard(_mutex);
        _tcpConnectionMap.erase(fd);
    }
}

