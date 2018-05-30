/*************************************************************************
	> File Name: FileTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月29日 星期二 09时01分03秒
 ************************************************************************/

#include<iostream>
#include"../File.h"

using namespace unet::base;

int main(int argc,char** argv)
{
    File file1("/home/uuchen/uuchen.txt",WRITE);
    File file = std::move(file1);
    file.blockWrite("uuchen",6);
    std::cout << "filename: " << file.getFilename() << std::endl;
    std::cout << "filename: " << file.getGlobalFilename() << std::endl;
    std::cout << "fd: " << file.getFd() << std::endl;
    std::cout << "isOpen: " << file.isOpened() << std::endl;
    std::cout << "filesize: " << file.getFileSize() << std::endl;
    
    file.switchType(READ);

    char buf[50];
    memset(buf,'\0',50);
    int size = file.blockRead(buf,50);
    std::cout << "read size: " << size << std::endl;
    std::cout << buf << std::endl;
    return 0;
}
