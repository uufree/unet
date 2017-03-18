/*************************************************************************
	> File Name: TcpServer.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月11日 星期六 19时18分24秒
 ************************************************************************/

#ifndef _TCPSERVER_H
#define _TCPSERVER_H

#include<memory>
#include<functional>
#include<map>

class Buffer;
class Channel;
class InetAddress;
class EventLoop;

namespace unet
{
    namespace net
    {
        class TcpServer final
        {
            public:
                typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
                typedef std::function<void(Buffer* inputbufer_,Buffer* outputbuffer_)> MessageCallBack;//由使用者传入的函数，根据两个缓冲区内的数据进行处理
                typedef std::map<int,TcpConnectionPtr> TcpConnectionPtrMap;//保存描述连接的智能指针，索引是连接中保存的fd
                
                TcpServer(EventLoop* eventloop_);
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
                Channel* newConnectionCallBack(int fd_,InetAddr& clientaddr);//设置新连接到来时的处理方式
                
//这个函数的作用是当已经删除掉Epoller中的channel时，因为Buffer中的数据还没有处理完成，所以将fd设置为-fd，特殊处理将要关闭但是还没有关闭的事件                
                void changeTcpMapIndex(int fd)
                {   
                    assert(connectionptrmap[fd] != connectionptrmap.end());
                    TcpConnectionPtrMap::iterator iter = find(fd);
                    connectionmap.erase(iter);
                    connectionmap.insert({-iter->first,iter->second});
                }

                std::map<int,TcpConnectionPtr> tcpconnectionptrmap;//保存连接的容器
                std::unique<Acceptor> acceptoruptr;//持有的acceptor
                EventLoop* loop;
                InetAddress serveraddr;
                MessageCallBack readcallback,writecallback;//处理消息的回调函数，将要注册给TcpConnection
        };

    }
}






































#endif

