/*************************************************************************
	> File Name: Acceptor.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月10日 星期五 08时45分54秒
 ************************************************************************/

namespace unet
{
    namespace net
    {
        Acceptor::Acceptor(EventLoop* loop_,const InetAddr& addr_)
            : loop(loop_),listenfd(socket::socket(....)),
            listenchannel(loop,listenfd.getSocket()),
            epoller(loop_),listening(false)
        {
            socket::bind(listenfd.getSocket(),addr_);
            loop->setActiveCallBack(std::bind(&getActiveChannels,this));
            listenchannel->setReadCallBack(std::bind(&handleRead,this)); 
        }

        void Acceptor::listen()
        {
            listening = true;
            socket::listen(listenfd.getSocket());
            epoller->addInChannelMap(&listenchannel);
        }

        void Acceptor::getActiveChannels()
        {
            ChannelList* channels = loop->getActiveChannels();
            epoller->epoll(channels);
        }
        
        void Acceptor::handleRead()
        {
            InetAddr clientaddr;
            int confd = sockfd::accept(listenfd.getSocket(),clientaddr);
            assert(confd >= 0);
            
            newconnectioncallback(confd,clientaddr);//将connection加入TcpServer
            epoller->addInChannelList(Channel(loop,confd));//主动将channel加入channelList
        }
    }
}

         
        
         
            


