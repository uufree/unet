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
            tcpconnectionMap(),
            channelMap(),
            eventMap(),
            channelList(),
            pool(size,tcpconnectionMap),
            epoller(),
            eventLoop(),
            asyncAcceptor(serveraddr) 
        {
            eventLoop.setGetActiveChannelsCallBack(std::bind(&AsyncTcpServer::GetActiveChannels,this));
            asyncAcceptor.setEraseChannelCallBack(std::bind(&AsyncTcpServer::EraseChannel,this,std::placeholders::_1));
            asyncAcceptor.setInsertChannelCallBack(std::bind(&AsyncTcpServer::InsertChannel,this,std::placeholders::_1));
        }
        
        AsyncTcpServer::AsyncTcpServer(AsyncTcpServer&& lhs) :
            serveraddr(lhs.serveraddr),
            tcpconnectionMap(std::move(lhs.tcpconnectionMap)),
            channelMap(std::move(lhs.channelMap)),
            eventMap(std::move(lhs.eventMap)),
            channelList(std::move(lhs.channelList)),
            pool(std::move(lhs.pool)),
            epoller(std::move(lhs.epoller)),
            eventLoop(std::move(lhs.eventLoop)),
            asyncAcceptor(std::move(lhs.asyncAcceptor)),
            readCallBack(std::move(lhs.readCallBack))
        {};

        AsyncTcpServer& AsyncTcpServer::operator=(AsyncTcpServer&& lhs)
        {
            tcpconnectionMap = std::move(lhs.tcpconnectionMap);
            channelMap = std::move(lhs.channelMap);
            eventMap = std::move(lhs.eventMap);
            channelList = std::move(lhs.channelList);
            pool = std::move(lhs.pool);
            epoller = std::move(lhs.epoller);
            eventLoop = std::move(lhs.eventLoop);
            asyncAcceptor = std::move(lhs.asyncAcceptor);
            readCallBack = std::move(lhs.readCallBack);
            
            return *this;
        }

        AsyncTcpServer::~AsyncTcpServer()
        {
            std::cout << "~AsyncTcpServer" << std::endl;
        };

        void AsyncTcpServer::InsertChannel(ChannelPtr&& channel)
        {
            channel->setCloseCallBack(std::bind(&AsyncTcpServer::EraseChannel,this,std::placeholders::_1));
            
            TcpConnectionPtr connection(new TcpConnection(channel->getFd()));
            connection->setReadCallBack(readCallBack);
            
            tcpconnectionMap.insert(connection);
            channelMap.insert(std::move(channel));
            eventMap.insert(channel->getFd(),channel->getEvent(),epoller.getEpollfd());
        }

        void AsyncTcpServer::EraseChannel(int fd)
        {
            tcpconnectionMap.erase(fd);
            channelMap.erase(fd);
            eventMap.erase(fd,epoller.getEpollfd());
        }

        void AsyncTcpServer::GetActiveChannels()
        {
            channelList.clear();
            epoller.epoll(channelList,channelMap,tcpconnectionMap);
            pool.addInTaskQueue(channelList); 
        }

        void AsyncTcpServer::start()
        {
            asyncAcceptor.listen();
            eventLoop.loop();
        }
    }
}



