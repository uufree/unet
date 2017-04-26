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
                iterlist.clear();

                if(activecallback)
                    activecallback(&activechannels);

                if(!activechannels.empty())
                {
                    eventhandling = true;
    
                    for(iter=activechannels.begin();iter!=activechannels.end();++iter)
                    {
                        if((*iter)->getType() == LISTEN)
                        {
                            (*iter)->handleEvent();
                            iterlist.push_back(iter); 
                        }
                        else if((*iter)->getType() == CONNECT)
                        {   
                            if((*iter)->asyncRead() == 0)
                                iterlist.push_back(iter);
                        }
                        else//留给CLOCK
                        {}
                    }
                    
                    if(!iterlist.empty())
                    {
                        for(iteriter=iterlist.begin();iteriter!=iterlist.end();++iteriter)
                            activechannels.erase(*iteriter);
                    }
                    
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
        
        

