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
            pool(new thread::ThreadPool(size)),
            epoller(eventList),
            asyncAcceptor(serveraddr) 
        {
            eventLoop.setGetActiveChannelsCallBack();
            eventLoop.setHandleChannelsCallBack();
            
            asyncAcceptor.setEraseChannelCallBack();
            asyncAcceptor.setInsertChannelCallBack();
        }
        

    }
}



