/*************************************************************************
	> File Name: EventLoop.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月01日 星期三 14时24分39秒
 ************************************************************************/

#include<vector>
#include<functional>
#include<memory>
#include<iostream>

class Channel; 

namespace unet
{
    namespace net
    {
        class EventLoop final
        {
            private:
                typedef std::vector<Channel*> ChannelList;
                typedef std::function<void(ChannelList*)> GetActiveChannelsCallBack;
                
                typedef std::vector<int> ChannelList;
                bool looping;
                bool quit;
                bool eventhandling;
                ChannelList activechannels;
                GetActiveChannelsCallBack activecallback;
        
            public:
                EventLoop() : looping(false),quit(false),eventhandling(false)
        {};
                EventLoop(const EventLoop&) = delete;
                EventLoop& operator=(const EventLoop&) = delete;
                ~EventLoop() {};
            
                void loop()
                {
                    looping = true;
                    quit = false;
                    while(!quit)
                    {
                        activechannels.push_back(1);
                        activecallback(&activechannels);
                        if(!activechannels.empty())
                        {   
                            eventhandling = true;
                            for(ChannelList::iterator iter=activechannels.begin();iter!=activechannels.end();++iter)
                            {
                                *iter->handleEvent();
                            }
                            eventhandling = false;
                        }
                        else
                        {
                            std::cout << "nothing happend!" << std::endl;
                        }
                    }
                    looping = false;
                };

                void setQuit()
                {
                    quit = true;
                };

                ChannelList* getChannelList()
                {
                    activechannels.clear();
                    return &activechannels;
                };
            };
    }
}   


int main(int argc,char** argv)
{
    unet::net::EventLoop loop;
    loop.loop();

    return 0;
}






































