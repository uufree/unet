/*************************************************************************
	> File Name: EventLoop.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月01日 星期三 14时24分39秒
 ************************************************************************/

#ifndef _EVENTLOOP_H
#define _EVENTLOOP_H

#include<vector>
#include<functional>
#include<memory>


//这个loop只认Channel，loop中不阻塞。如果有事件处理事件，没有事件空转
namespace unet
{
    namespace net
    {
        class Channel;
        class EventLoop final
        {
            public:
//                typedef std::function<void()> Functor;
                typedef std::vector<Channel*> ChannelList;

                EventLoop();
                EventLoop(const EventLoop&) = delete;
                EventLoop& operator=(const EventLoop&) = delete;
                ~EventLoop();

//punlic interface
                void loop();
                void quit();
                void assertInLoopThread();
                void writeInLoop();

            private:
                void abortNotInLoopThread();
//                void doPendingFunctors();
                
                bool looping;
                bool quit;
                bool eventHandling;
//                bool callingPendingFunctor;
                const pid_t threadId;
                ChannelList activeChanels;//其他部分向loop中写东西
                
//                mutable MutexLock mutex;
//                std::vector<Functor> pendingFunctor;
        };
    }
}

#endif

