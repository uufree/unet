/*************************************************************************
	> File Name: SocketTextServer.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月17日 星期五 19时36分18秒
 ************************************************************************/

#include"../mutilServer/MutilTcpServer.h"

using namespace unet;
using namespace unet::net;
using namespace unet::thread;

void readCallBack(Buffer* inputbuffer,Buffer* outputbuffer)
{//服务端被动处理事务（适用于短连接）,只能在fd资源和内存资源中二选一
    inputbuffer->readInSocket();
    inputbuffer->getData();
}

void writeCallBack(Buffer* inputbuffer,Buffer* outputbuffer)
{

}

void drivedCallBack(Buffer* inputbuffer,Buffer* outputbuffer)
{

}


int main(int argc,char** argv)
{
    InetAddress serveraddr(7777);
    MutilTcpServer server(&serveraddr);

    server.setReadCallBack(std::bind(&readCallBack,std::placeholders::_1,std::placeholders::_2));
    server.setWriteCallBack(std::bind(&writeCallBack,std::placeholders::_1,std::placeholders::_2));
    server.setDrivedCallBack(std::bind(&drivedCallBack,std::placeholders::_1,std::placeholders::_2));
    
    server.start();

    return 0;
}



