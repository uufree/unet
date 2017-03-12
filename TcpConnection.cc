/*************************************************************************
	> File Name: TcpConnection.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月12日 星期日 09时50分36秒
 ************************************************************************/

namespace unet
{
    namespace net
    {
        TcpConnection::TcpConnection(EventLoop* loop_,int fd_,const InetAddr& serveraddr_,const InetAddr& clientaddr_) :
            loop(loop_),consocket(fd_),serveraddr(serveraddr_),     clientaddr(clientaddr_)
        {};

//if inputbuffer over highwater,handle it and put it in outputbuffer,otherwize,update inputbuffer and outputbuffer
        void TcpConnection::handleRead()
        {   
            

        }

        void TcpConnection::handleWrite()
        {
        }

        void TcpConnection::handleError()
        {

        }

        void TcpConnection::handleClose()
        {

        }
    }
}





