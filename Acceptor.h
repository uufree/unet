/*************************************************************************
	> File Name: Acceptor.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月10日 星期五 08时45分48秒
 ************************************************************************/

#ifndef _ACCEPTOR_H
#define _ACCEPTOR_H

namespace unet
{
    namespace net
    {
        class Acceptor final
        {
            public:





            private:
                typedef std::unique_ptr<EventLoop> EventLoopUptr;
                typedef std::unique_ptr<Epoller> EpollerLoopUptr;
                typedef std::unique_ptr<Channel> ChannelUptr;
                
                EventLoopUptr eventloopuptr;
                EpollerLoopUptr epollerloopuptr;
                ChannelUptr listenchanneluptr;

                bool listening;















#endif

