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
        
        Channel::Channel(int fd_,bool hasconnection) : fd(fd_),index(-1),event(0),revent(0),isnewchannel(true),isinepoll(false),handleeventing(false),hasconnection(hasconnection_)
        {
            if(hasconnection_)
                connectionptr(new TcpConnection());
        };

        Channel::~Channel()
        {
            assert(handleeventing);
            assert(!isinepoll);
        }

        void Channel::handleEvent()
        {
            if(hasconnection)
            {//have connectionptr handle ways
                handleeventing = true;
                if((revent & EPOLLHUP) && !(revent & POLLIN))
                {
                    connectionptr->handleClose();
                }
                if(revent & (EPOLLERR | EPOLLNVAL))
                {   
                    connectionptr->handleError();
                }
                if(revent & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
                {
                        connectionptr->handleRead();
                }
                if(revent & EPOLLOUT)
                {
                        connecionptr->handleWrite();
                }
                handleeventing = false;
            }
            else
            {//timefd and listenfd handle ways
                handleeventing = true;
                if((revent & EPOLLHUP) && !(revent & POLLIN))
                {
                    if(closecallback)
                        closecallback();
                }
                if(revent & (EPOLLERR | EPOLLNVAL))
                {   
                    if(errorcallback)
                        errorcallback();
                }

                if(revent & (EPOLLIN | EPOLLPRI | EPOLLRDHUP))
                {
                    if(readcallback)
                        readcallback();
                }

                if(revent & EPOLLOUT)
                {
                    if(writecallback)
                        writecallback();
                }
                handleeventing = false;
            }
        }
        
        void Channel::remove()
        {   
            removecallback(this);
        }
    }
}





        
            
