/*************************************************************************
	> File Name: AsyncTcpServerTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年04月18日 星期二 17时19分56秒
 ************************************************************************/

#include"../AsyncTcpServer.h"

using namespace unet;
using namespace unet::thread;
using namespace unet::net;


void readCallBack(Buffer* inputbuffer,Buffer* outputbuffer)
{
/*    
    char message[16];
    bzero(message,16);
    inputbuffer->readInSocket();
    inputbuffer->getCompleteMessageInBuffer(message);

    printf("%s\n",message);

    outputbuffer->appendInBuffer("hello,client!");
    outputbuffer->writeInSocket();
*/
    std::cout << "1" << std::endl;
    inputbuffer->readInSocket();
}

void writeCallBack(Buffer* inputbuffer,Buffer* outputbuffer)
{}

void drivedCallBack(Buffer* inputbuffer,Buffer* outputbuffer)
{}


int main(int argc,char** argv)
{
    InetAddress serveraddr(7777);
    AsyncTcpServer server(&serveraddr);
    
    server.setReadCallBack(std::bind(&readCallBack,std::placeholders::_1,std::placeholders::_2));
    server.setWriteCallBack(std::bind(&writeCallBack,std::placeholders::_1,std::placeholders::_2));
    server.setDrivedCallBack(std::bind(&drivedCallBack,std::placeholders::_1,std::placeholders::_2));

    server.start();
}
    

    



