/*************************************************************************
	> File Name: TcpConnection.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月12日 星期日 09时50分36秒
 ************************************************************************/

#include"TcpConnection.h"

namespace unet
{
    TcpConnection::TcpConnection(int fd) : 
        u_confd(fd),
        u_buffer(std::make_shared<base::Buffer>(fd))
    {};
    
    TcpConnection::TcpConnection(base::Socket&& socket) :
        u_confd(std::move(socket)),
        u_buffer(std::make_shared<base::Buffer>(socket.getFd()))
    {};

    TcpConnection::TcpConnection(TcpConnection&& lhs):
        u_confd(std::move(lhs.u_confd)),
        u_buffer(std::move(lhs.u_buffer)),
        u_readCallBack(std::move(lhs.u_readCallBack)),
        u_writeCallBack(std::move(lhs.u_writeCallBack)),
        u_closeCallBack(std::move(lhs.u_closeCallBack))
    {};

    TcpConnection& TcpConnection::operator=(TcpConnection&& lhs)
    {
        if(*this == lhs)
            return *this;
        u_confd = std::move(lhs.u_confd);
        u_buffer = std::move(lhs.u_buffer);
        u_readCallBack = std::move(lhs.u_readCallBack);
        u_writeCallBack = std::move(lhs.u_writeCallBack);
        u_closeCallBack = std::move(lhs.u_closeCallBack);

        return *this;
    }

    /*步骤：将数据逻辑处理与IO在同一个线程中进行处理
     * 1.先从Socket中读取数据
     * 2.调用用户的逻辑处理函数
     */
    void TcpConnection::handleRead()
    { 
        /*读到0，就意味着连接被关闭*/
        int size = u_buffer->readInSocket();
        if(size == 0)
        {
            handleClose();
            return;
        }

        if(u_readCallBack)
            u_readCallBack(u_buffer);
        else
            perror("没有注册readcallback\n");
    }

    /*向Buffer中添加数据可能发生在两个地方；
     * 1.处理读请求时，直接向buffer中添加数据
     * 2.特意调用写回调函数
     * 3.最后向socket中写数据
     */
    void TcpConnection::handleWrite()
    {
        if(u_writeCallBack)
            u_writeCallBack(u_buffer);
        else
            perror("没有注册writecallback");
        u_buffer->writeInSocket();
    }
    
    /*出现异常的时候，会调用这个函数*/
    void TcpConnection::handleClose()
    {   
        if(u_closeCallBack)
            u_closeCallBack(u_confd.getFd());
        else
            perror("没有注册handlediedtcpconnection\n");
    }
}


