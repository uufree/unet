/*************************************************************************
	> File Name: TcpClientText.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月24日 星期五 16时25分26秒
 ************************************************************************/

#include"../client/TcpClient.h"
#include<iostream>
//#include"../Buffer.h"
#include<string>

using namespace unet;
using namespace unet::net;
/*
void writeForServer(Buffer* inputbuffer,Buffer* outputbuffer)
{
    std::string str("hello,server!");
    outputbuffer->appendInBuffer(str.c_str());
};

void readForServer(Buffer* inputbuffer,Buffer* outputbuffer)
{
    std::cout << inputbuffer->getCompleteMessageInBuffer() << std::endl;
};
*/
int main(int argc,char** argv)
{
    unet::net::InetAddress addr(7777);
    unet::net::TcpClient client(&addr);
//    client.setReadCallBack(std::bind(&readForServer,std::placeholders::_1,std::placeholders::_2));
//    client.setWriteCallBack(std::bind(&writeForServer,std::placeholders::_1,std::placeholders::_2));

    client.start();
    return 0;
}


