/*************************************************************************
	> File Name: FileTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年07月08日 星期六 13时45分13秒
 ************************************************************************/

#include"../File.h"
#include<iostream>

using namespace unet::file;

int main(int argc,char** argv)
{
    std::string mess;
    std::string message("I'm uuchen!");
    File uuchen("/home/uuchen/uuchen.txt",N_WRITE);
    std::cout << "fd: " << uuchen.getFd() << std::endl;
    std::cout << "g_filename: " << uuchen.getGlobalFilename() << std::endl;
    std::cout << "filename: " << uuchen.getFilename() << std::endl;
    
    writen(uuchen,message,message.size());
    std::cout << "finish writen" << std::endl;
    readn(uuchen,mess,16);
    std::cout << "finish readn" << std::endl;
            
    std::cout << mess << std::endl;
    return 0;
}

