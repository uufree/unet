/*************************************************************************
	> File Name: Channel.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月09日 星期四 20时56分25秒
 ************************************************************************/

namespace unet
{
    namespace net
    {
        const int Channel::KNoneEvent = 0;
        const int Channel::KReadEvent = EPOLLIN | EPOLLPRI;
        const int Channel::KWriteEvent = EPOLLOUT;

        Channel::Channel(EventLoop* loop_):loop(loop_),
           fd(socket::socket()),index(-1),event(0),revent(0),
           isnewchannel(true),addinepoller(false),fdclose(true),
           handleeventing(false)
        {};

        Channel::~Channel()
        {
            assert(!addinpoller);
            if(!fdclose)
                socket::close(fd);
        }

        void Channel::update()
        {
            addinepoller = true;
            loop->updateChannel(this);
        }

        void Channel::remove()
        {
            assert(isNoneEvent());
            addinepoller = false;
            loop->removeChannel(this);
        }

        void Channel::handleEvent()
        {
            handleeventing = true;
            if((revent & EPOLLHUP) && !(revent & POLLIN))
                if(CloseCallBack)
                    CloseCallBack();

            if(revent & (EPOLLERR | EPOLLNVAL))
            {
                if(ErrorCallBack)
                    ErrorCallBack();
            }

            if(revent & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
            {
                if(ReadCallBack)
                    ReadCalBack();
            }

            if(revent & EPOLLOUT)
            {
                if(WriteCallBack)
                    WriteCallBack();
            }
            handleeventing = false;
        }

        ChannelCon::ChannelCon(EventLoop* loop_) : Channel(loop_)
        {};
    }
}





        
            
