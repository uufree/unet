/*************************************************************************
	> File Name: EventLoop.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月10日 星期五 00时53分33秒
 ************************************************************************/

#include"EventLoop.h"

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
                ::sleep(1);
                
                activechannels.clear();
                    
                if(activecallback)
                    activecallback(&activechannels);
                
                if(!activechannels.empty())
                {
                    eventhandling = true;
                    for(ChannelList::iterator iter=activechannels.begin();iter!=activechannels.end();++iter)
                    {
                        std::cout << "pthread_self1(): " << pthread_self() << std::endl;
                        (*iter)->handleEvent();
                        std::cout << "pthread_self2(): " << pthread_self() << std::endl;
                    }
                    eventhandling = false;
                }
            }
            looping = false;
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
        
        

