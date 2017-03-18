/*************************************************************************
	> File Name: TcpServer.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月11日 星期六 19时18分24秒
 ************************************************************************/

#ifndef _TCPSERVER_H
#define _TCPSERVER_H

class Buffer;

namespace unet
{
    namespace net
    {
        class TcpServer final
        {
            public:
                TcpServer(EventLoop* eventloop_);
                TcpServer(const TcpServer& lhs) = delete;
                TcpServer& operator(const TcpServer& lhs) = delete;
                ~TcpServer() {};
//public interface
                EventLoop* getEventLoop() {return loop;};
                void start();
                void setReadCallBack(const MessageCallBack& cb)
                {readcallback = cb;}

                void setWriteCallBack(const MessageCallBack& cb)
                {writecallback = cb;};

                void checkMapIndex();

            private:
                typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
                typedef std::function<void(Buffer* inputbufer_,Buffer* outputbuffer_)> MessageCallBack;
                typedef std::map<int,TcpConnectionPtr> TcpConnectionPtrMap;

                Channel* newConnectionCallBack(int fd_,InetAddr& clientaddr);
                
                void changeTcpMapIndex(int fd)
                {   
                    assert(connectionptrmap[fd] != connectionptrmap.end());
                    TcpConnectionPtrMap::iterator iter = find(fd);
                    connectionmap.erase(iter);
                    connectionmap.insert({-iter->first,iter->second});
                }


                std::map<int,TcpConnectionPtr> connectionptrmap;
                std::unique<Acceptor> acceptoruptr;
                EventLoop* loop;
                InetAddr* serveraddr;
                MessageCallBack readcallback,writecallback;
        };

    }
}






































#endif

