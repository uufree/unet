/*************************************************************************
	> File Name: BufferTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年07月15日 星期六 19时54分51秒
 ************************************************************************/

#include<iostream>
#include"../Buffer.h"

using namespace unet::base;

int main(int argc,char** argv)
{
    StringBuffer buffer(10);
    buffer.appendInBuffer("uuchen");
    std::cout << "BufferSize: " << buffer.size() << std::endl;

    return 0;
}

