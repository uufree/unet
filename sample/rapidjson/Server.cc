/*************************************************************************
	> File Name: Server.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年04月15日 星期六 17时12分40秒
 ************************************************************************/

#include"../../mutilServer/MutilTcpServer.h"
#include"../../rapidjson/document.h"
#include"../../rapidjson/writer.h"
#include"../../rapidjson/stringbuffer.h"
#include"../../rapidjson/filereadstream.h"

using namespace unet::net;
using namespace unet::thread;
using namespace unet::thread;
using namespace rapidjson;

void readCallBack(Buffer* inputbuffer,Buffer* outputbuffer)
{
    char* message = NULL;
    
    inputbuffer->readInSocket();
    inputbuffer->getCompleteMessageInBuffer(message); 
    
    if(message != nullptr)
    {
        Document docu;
        docu.Parse(message);
        Value& s = docu["temp"];
        std::cout << s.GetInt() << std::endl;
    }
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
    unet::net::MutilTcpServer server(&serveraddr,2);
    
    server.setReadCallBack(std::bind(&readCallBack,std::placeholders::_1,std::placeholders::_2));
    server.setWriteCallBack(std::bind(&writeCallBack,std::placeholders::_1,std::placeholders::_2));
    server.setDrivedCallBack(std::bind(&drivedCallBack,std::placeholders::_1,std::placeholders::_2));

    server.start();
    return 0;
}

