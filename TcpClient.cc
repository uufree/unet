/*************************************************************************
	> File Name: TcpClient.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月21日 星期二 20时32分19秒
 ************************************************************************/

#include"TcpClient.h"

namespace unet
{
    namespace net
    {
        TcpClient::TcpClient(const InetAddress& serveraddr_) :
            serveraddr(serveraddr_),
            connector(new connector(serveraddr_)),
            ptr(connector->getTcpConnectionPtr())
        {
            ptr->resetChannelPtr();
            ptr->setReadCallBack(readcallback);
            ptr->setWriteCallBack(writecallback);
            ptr->setHandleDiedTcpConnection(&TcpClient::handleDiedTcpConnection,this,std::placeholders::_1);
        };
        
        
            

        
