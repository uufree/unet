/*************************************************************************
	> File Name: DirectoryTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月29日 星期二 16时51分01秒
 ************************************************************************/

#include<iostream>
#include"../Directory.h"

using namespace unet::base;

int main(int argc,char** argv)
{
    Directory dir("/home/uuchen");
    std::cout << dir.getDirBuffer() << std::endl;
    return 0;
}
