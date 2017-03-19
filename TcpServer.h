/*************************************************************************
	> File Name: TcpServer.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月11日 星期六 19时18分24秒
 ************************************************************************/

#ifndef _TCPSERVER_H
#define _TCPSERVER_H

#include<assert.h>
#include<algorithm>
#include"Acceptor.h"

namespace unet
{
    namespace net
    {
        class TcpServer final
        {
            typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
            typedef std::map<int,TcpConnectionPtr> TcpConnectionPtrMap;//保存描述连接的智能指针，索引是连接中保存的fd
            public:     
                TcpServer(EventLoop* eventloop_,InetAddress* serveraddr_);
                TcpServer(const TcpServer& lhs) = delete;
                TcpServer& operator=(const TcpServer& lhs) = delete;
                ~TcpServer() {};
//public interface
                EventLoop* getEventLoop() {return loop;};
                void start();//TcpServer开始工作
                void setReadCallBack(const MessageCallBack& cb)
                {readcallback = cb;}

                void setWriteCallBack(const MessageCallBack& cb)
                {writecallback = cb;};

                void checkMapIndex();

            private:
                Channel* newConnectionCallBack(int fd_,InetAddress& clientaddr);//设置新连接到来时的处理方式
                
//这个函数的作用是当已经删除掉Epoller中的channel时，因为Buffer中的数据还没有处理完成，所以将fd设置为-fd，特殊处理将要关闭但是还没有关闭的事件                
                void changeTcpMapIndex(int fd)
                {   
                    assert(tcpconnectionptrmap[fd] != tcpconnectionptrmap.end());
                    TcpConnectionPtrMap::iterator iter = find(fd);
                    tcpconnectionptrmap.erase(iter);
                    map.insert({-iter->first,iter->second});
                }

                EventLoop* loop;
                InetAddress* serveraddr;
                std::unique_ptr<Acceptor> acceptoruptr;//持有的acceptor
                std::map<int,TcpConnectionPtr> tcpconnectionptrmap;//保存连接的容器
                MessageCallBack readcallback,writecallback;//处理消息的回调函数，将要注册给TcpConnection
        };

    }
}






































#endif

