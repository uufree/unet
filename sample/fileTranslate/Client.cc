/*************************************************************************
	> File Name: Client.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月31日 星期五 16时28分42秒
 ************************************************************************/

//这个例子处理两种情况的事件

//服务端主动给多个客户端发送文件
//多个客户端主动给服务端发送文件

#include"../../client/TcpClient.h"
#include"../../File.h"

using namespace unet;
using namespace unet::thread;
using namespace unet::net;

void readCallBack(Buffer* inputbuffer,Buffer* outputbuffer)
{//被动的接受服务端传送的文件
    char* buf;
    inputbuffer->readInSocket();
    buf = inputbuffer->getCompleteMessageInBuffer();

    if(buf != nullptr)
    {
        File chenuu(argv[1]);
        chenuu.writen(buf,strlen(buf));
        ::free(buf);
    }
}

void writeCallBack(Buffer* inputbuffer,Buffer* outputbuffer)
{


}

void drivedCallBack(Buffer* inputbuffer,Buffer* outputbuffer)
{//服务端主动发送文件，及时服务
/*    char buf[1024];
    bzero(buf,1024);
    File uuchen("/home/uuchen/uuchen.jpeg");

    while(1)
    {
        uuchen.readn(static_cast<void*>(buf),1024);
        if(uuchen.getReadSize() > 0)
        {
            outputbuffer->appendInBuffer(static_cast<const void*>(buf));
            outputbuffer->writeInSocket();
        }
        else
            break;
    }
*/}

int main(int argc,char** argv)
{
    InetAddress addr(7777);
    TcpClient client(&addr);
    
    client.setReadCallBack(std::bind(&readCallBack,std::placeholders::_1,std::placeholders::_2));
    client.setWriteCallBack(std::bind(&writeCallBack,std::placeholders::_1,std::placeholders::_2));
    client.setDrivedCallBack(std::bind(&drivedCallBack,std::placeholders::_1,std::placeholders::_2));
    
    client.start();

    return 0;
}

