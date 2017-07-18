/*************************************************************************
	> File Name: AsyncTcpServer.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年04月17日 星期一 16时27分23秒
 ************************************************************************/

#include"AsyncTcpServer.h"

namespace unet
{
    namespace net
    {
        AsyncTcpServer::AsyncTcpServer(socket::InetAddress& addr_,int size):
            serveraddr(addr_),
            pool(size,tcpconnectionMap),
            epoller(eventList),
            asyncAcceptor(serveraddr) 
        {
            eventLoop.setGetActiveChannelsCallBack(std::bind(&AsyncTcpServer::GetActiveChannels,this));
            
            asyncAcceptor.setEraseChannelCallBack(std::bind(&AsyncTcpServer::EraseChannel,this,std::placeholders::_1));
            asyncAcceptor.setInsertChannelCallBack(std::bind(&AsyncTcpServer::InsertChannel,this,std::placeholders::_1));
        }
        
        void AsyncTcpServer::InsertChannel(ChannelPtr&& channel)
        {
            channel->setCloseCallBack(std::bind(&AsyncTcpServer::EraseChannel,this,std::placeholders::_1));
            
            TcpConnectionPtr connection(new TcpConnection(channel->getFd()));
            connection->setReadCallBack(readCallBack);
            connection->setWriteCallBack(writeCallBack);
            
            tcpconnectionMap.insert(connection);
            channelMap.insert(std::move(channel));
            eventList.insert(channel->getFd(),channel->getEvent());
        }

        void AsyncTcpServer::EraseChannel(int fd)
        {
            tcpconnectionMap.erase(fd);
            channelMap.erase(fd);
            eventList.erase(fd);
        }

        void AsyncTcpServer::GetActiveChannels()
        {
            epoller.epoll(channelList,channelMap,tcpconnectionMap);
            pool.addInTaskQueue(channelList); 
        }
    }
}



