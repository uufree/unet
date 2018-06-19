/*************************************************************************
	> File Name: BufferTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月30日 星期三 19时39分57秒
 ************************************************************************/

#include<iostream>
#include"../UsrBuffer.h"

using namespace unet::base;

/*基本的输入输出测试，不包含socket测试*/
/*测试必须依赖于Sokcet*/
int main(int argc,char** argv)
{
    Buffer buffer(10);
    std::cout << "=======================================" << std::endl;
    std::cout << "Init: " << std::endl;
    std::cout << "Buffer Read Free Size: " << buffer.readFreeSize() << std::endl;
    std::cout << "Buffer Write Free Size: " << buffer.writeFreeSize() << std::endl;
    std::cout << "=======================================" << std::endl;
   
    for(int i=0;i<10;i++)
    {
        std::cout << "========================================" << std::endl;
        std::cout << "Write: " << std::endl;
        buffer.writeInBuffer("uuchen",6);
        std::cout << "Buffer Read Free Size: " << buffer.readFreeSize() << std::endl;
        std::cout << "Buffer Write Free Size: " << buffer.writeFreeSize() << std::endl;
        std::cout << "========================================" << std::endl;
    }
    return 0;
}
