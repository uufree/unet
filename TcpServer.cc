/*************************************************************************
	> File Name: TcpServer.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月11日 星期六 19时18分41秒
 ************************************************************************/

#include"Buffer.h"
#include"Channel.h"
#include"InetAddress.h"
#include"EventLoop.h"
#include"TcpServer.h"

namespace unet
{
    namespace net
    {
        TcpServer::TcpServer(EventLoop* loop_,const InetAddress* serveraddr_) : loop(loop_),serveraddr(serveraddr_),acceptor(loop_,serveraddr)
        {//将处理连接的方式传递进acceptor，为的是可以将创建的对象中的TcpConnection对象放进TcpConnectionMap中去
            acceptoruptr->setNewConnectionCallBack(std::bind(&newConnectionCallBack,this,_1,_2));
        };
    
        Channel* TcpServer::newConnectionCallBack(int fd_,InetAddress& clientaddr)
        {
              Channel* channel = new Channel(fd_,true);//创建一个Channel对象
              TcpConnectionPtr ptr(channel->getTcpConnectionPtr());//得到Channel对象中的TcpConnectionPtr对象（重点在于让Channel中的TcpConnectionPtr对象将控制权交出来，只保留弱指针）
              ptr->resetChannelPtr();//同上述步骤，放弃控制权
              ptr->setReadCallBack(readcallback);//设置处理消息的方式
              ptr->setWriteCallBack(writecallback);
              ptr->setChangeTcpMapIndex(std::bind(&TcpServer::changeTcpMapIndex,this,std::placeholders::_1));//控制最外面的TcpConnectionMap，区别对待残余的连接
              connectionptrmap.insert(make_pair(fd_,ptr));//将得到的结果插入map中
              return channel;//返回的Channel交由Epoller处理（放入Epoller的map中）
        }

        void TcpServer::start()
        {//启动Loop循环
            loop.loop();
        }

        
        void checkMapIndex()
        {//遍历map处理将死未死的连接
            for(auto iter : connectionptrmap)
            {
                if(iter->first < 0)
                {
                    if(iter->second->handleWriteForTcpServer())
                        connectionptrmap.erase(iter);
                }
            }
        }


    }
}
        


