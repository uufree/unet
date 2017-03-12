/*************************************************************************
	> File Name: TcpServer.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月11日 星期六 19时18分41秒
 ************************************************************************/

namespace unet
{
    namespace net
    {
        TcpServer::TcpServer(EventLoop* loop_,const InetAddr* serveraddr_) : loop(loop_),serveraddr(serveraddr_),acceptor(loop_,serveraddr)
        {
            acceptoruptr->setNewConnection(std::bind(&newconnectionCallBack,this,_1,_2));
        };
    














//put TcpConnection in conwptr and return Channel*
        Channel* newConnectionCallBack(int fd_,InetAddr& clientaddr)
        {
              Channel* channel = new Channel(fd_,true);
              TcpConnectionWptr wptr = channel->getTcpConnectionPtr();
              connectionwptrlist.insert(fd_,wptr);
              return channel;
        }
        


