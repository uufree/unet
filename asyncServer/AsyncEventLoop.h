/*************************************************************************
	> File Name: AsyncEventLoop.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月01日 星期三 14时24分39秒
 ************************************************************************/

#ifndef _ASYNCEVENTLOOP_H
#define _ASYNCEVENTLOOP_H

#include"../Epoller.h"

namespace unet
{
    namespace net
    { 
        class AsyncEventLoop final
        {
            typedef std::vector<Channel*> ChannelList;
            typedef std::function<void(ChannelList*)> GetActiveChannelsCallBack;
            typedef std::function<void()> TaskFunc;
            typedef ChannelList::iterator Iter;
            typedef std::vector<Iter> IterList;
            typedef IterList::iterator IterIter;

            public:                                    
                AsyncEventLoop();
                AsyncEventLoop(const AsyncEventLoop&) = delete;
                AsyncEventLoop& operator=(const AsyncEventLoop&) = delete;
                ~AsyncEventLoop() {};
//punlic interface
                void loop();
                void setQuit();
                void setGetActiveChannelsCallBack(const GetActiveChannelsCallBack& cb)
                {activecallback = cb;};
                
                void setHandleActiveChannelsCallBack(const GetActiveChannelsCallBack& cb)
                {handleactivechannels = cb;}
                
                ChannelList* getChannelList();
                
            private:
                bool looping;
                bool quit;
                bool eventhandling;
                ChannelList activechannels;
                GetActiveChannelsCallBack activecallback,handleactivechannels;
                Iter iter;
                IterList iterlist;
                IterIter iteriter;
        };
    }
}

#endif

