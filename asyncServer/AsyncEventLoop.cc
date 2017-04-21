/*************************************************************************
	> File Name: AsyncEventLoop.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月10日 星期五 00时53分33秒
 ************************************************************************/

#include"AsyncEventLoop.h"

namespace unet 
{
    namespace net
    {
        AsyncEventLoop::AsyncEventLoop() : looping(false),quit(false),eventhandling(false)
        {};

        void AsyncEventLoop::loop()
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
                
                if(!activechannels.empty())
                {
                    eventhandling = true;

                    handleactivechannels(&activechannels);
                    
                    eventhandling = false;
                }
            }
            looping = false;
        }

        void AsyncEventLoop::setQuit()
        {
            quit = true;
        }

        typedef std::vector<Channel*> ChannelList;//WTF
        ChannelList* AsyncEventLoop::getChannelList()
        {
            activechannels.clear();
            return &activechannels;
        }

    }
}
        
        

