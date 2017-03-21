/*************************************************************************
	> File Name: TcpClient.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月21日 星期二 20时32分11秒
 ************************************************************************/

#ifndef _TCPCLIENT_H
#define _TCPCLIENT_H

namespace unet
{
    namespace net
    {
        class TcpClient final
        {
            typedef std::function<Buffer*,Buffer*> MessageCallBack;
            typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;

            public:
                TcpClient(const InetAddress& clientaddr)




















    }
}




























#endif

