/*************************************************************************
	> File Name: FileTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月31日 星期五 16时52分54秒
 ************************************************************************/

#include"../File.h"
#include"../Buffer.h"
#include<iostream>
#include<string.h>

int main(int argc,char** argv)
{
    unet::File uuchen("/home/uuchen/uuchen.jpeg");
    unet::File chenuu("/home/uuchen/chenuu.jpeg");
    Buffer inputbuffer(uuchen.getFd());
    Buffer outputbuffer(chenuu.getFd());

    std::cout << "uuchen: " << uuchen.getFd() << std::endl;
    std::cout << "chenuu: "  << chenuu.getFd() << std::endl;

    char buf[1024];
    ::bzero(buf,1024);
    while(1)
    {
        uuchen.readn(buf,1024);
        if(uuchen.getReadSize() > 0)
            chenuu.writen(buf,uuchen.getReadSize());
        else
            break;
    }
    return 0;
}

