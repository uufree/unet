/*************************************************************************
	> File Name: EventLoop.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月10日 星期五 00时53分33秒
 ************************************************************************/

#include"EventLoop.h"
#include<assert.h>
#include<iostream>

namespace unet 
{
    namespace net
    {
        EventLoop::EventLoop() : looping(false),quit(false),eventhandling(false)
        {};

        void EventLoop::loop()
        {
            std::cout << "I'm in looping!" << std::endl;
            std::cout << "----------------------" << std::endl;
            looping = true;
            quit = false;
            while(!quit)
            {
                activechannels.clear();
                
                if(activecallback)
                    activecallback(&activechannels);
                
                std::cout << "activechannels: " << activechannels.size() << std::endl;
                
                sleep(1);
                
                if(!activechannels.empty())
                {
                    eventhandling = true;
                    for(ChannelList::iterator iter=activechannels.begin();iter!=activechannels.end();++iter)
                    {
                        (*iter)->handleEvent();
                    }
                    eventhandling = false;
                }
                else
                {
                    std::cout << "nothing happend!" << std::endl;
                }

                if(!functorlist.empty())
                {
                    for(auto iter=functorlist.begin();iter!=functorlist.end();++iter)
                        (*iter)();
                }
                functorlist.clear();
            }
            looping = false;
            std::cout << "I'm in looping!" << std::endl;
        }

        void EventLoop::setQuit()
        {
            quit = true;
        };

        typedef std::vector<Channel*> ChannelList;//WTF
        ChannelList* EventLoop::getChannelList()
        {
            activechannels.clear();
            return &activechannels;
        }

    }
}
        
        

