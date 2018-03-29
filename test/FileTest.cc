/*************************************************************************
	> File Name: FileTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年07月08日 星期六 13时45分13秒
 ************************************************************************/

#include"../File.h"
#include<iostream>

using namespace unet::base;

int main(int argc,char** argv)
{
    std::string mess("Hello,World!");
    std::string message("I'm uuchen!");
    File uuchen("/home/uuchen/uuchen.txt",unet::base::READ);
    std::cout << uuchen.getFd() << std::endl;
    std::string buf;
    uuchen.blockRead(buf,100);
    std::cout << buf << std::endl;
    return 0;
}

