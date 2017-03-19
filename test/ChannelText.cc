/*************************************************************************
	> File Name: ChannelText.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月19日 星期日 12时20分46秒
 ************************************************************************/

#include"../Channel.h"
#include"../EventLoop.h"

int main(int argc,char** argv)
{
    unet::net::EventLoop loop;
    unet::net::Channel(&loop,3);
    return 0;
}

