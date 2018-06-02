/*************************************************************************
	> File Name: EventdemuTest.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年06月02日 星期六 18时13分22秒
 ************************************************************************/

#include<iostream>
#include"../Selecter.h"
#include"../Poller.h"
#include"../EPoller.h"
#include"../EventDemultiplexer.h"

using namespace unet;

int main(int argc,char** argv)
{
    EventDemultiplexer* selecter = new Selecter;
    EventDemultiplexer* poller = new Poller;
    EventDemultiplexer* epoller = new EPoller;
    
    delete selecter;
    delete poller;
    delete epoller;

    return 0;
}
