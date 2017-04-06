/*************************************************************************
	> File Name: SocketTextServer.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月17日 星期五 19时36分18秒
 ************************************************************************/

#include"../mutilServer/MutilTcpServer.h"

using namespace unet;
using namespace unet::net;

int main(int argc,char** argv)
{
    int listenfd = unet::net::socket::socket();
    InetAddress addr(7777);

    unet::net::socket::bind(listenfd,&addr);
    unet::net::socket::listen(listenfd);
    std::cout << "listenfd: " << listenfd << std::endl;

    int confd = unet::net::socket::accept(listenfd);
    std::cout << "confd: " << confd << std::endl;

/*    
    int epollfd = ::epoll_create(5);
    std::cout << "epollfd: " << epollfd << std::endl;
    
    struct epoll_event event,event_;
    bzero(&event,sizeof(event));
    bzero(&event_,sizeof(event_));
    event.events = EPOLLIN && EPOLLOUT;
    event.data.fd = confd;
    
    int n = ::epoll_ctl(epollfd,EPOLL_CTL_ADD,confd,&event);
    if(n < 0)
        std::cout << "epoll_ctl error!" << std::endl;
    else
        std::cout << "epoll_ctl success!" << std::endl;
    
    int count = ::epoll_wait(epollfd,&event_,5,-1);
    std::cout << "count: " << count << std::endl; 

    
    if(count == 1)
    {
        if(event_.events & EPOLLIN)
            std::cout << "Server confd can read!" << std::endl;
        else if(event_.events & EPOLLOUT)
            std::cout << "Server confd can write!" << std::endl;
        else
            std::cout << "error!" << std::endl;
    }
    else
        std::cout << "nothing happened!" << std::endl;
*/

  
    char buf[16];
    struct pollfd pfd;
    pfd.fd = confd;
    pfd.events = POLLOUT && POLLIN;

        int count = ::poll(&pfd,1,64000);
        
        if(count == 1)
        {
            if(pfd.revents & POLLIN)
            {
                std::cout << "Client confd can read!" << std::endl;
                ::read(confd,buf,16);
                std::cout << buf << std::endl;
                bzero(buf,16);
            }
            else if(pfd.revents & POLLOUT)
            {
                std::cout << "Client confd can write!" << std::endl;
            }
            else if((pfd.revents & POLLERR) || (pfd.revents & POLLHUP) || (pfd.revents & POLLNVAL))
            {
                std::cout << "confd alredy close!" << std::endl;
            }
            else
            {
                std::cout << "error!" << std::endl;
            }
        }
        else
            std::cout << "nothing happened!" << std::endl;
    
        count = ::poll(&pfd,1,64000);
        
        if(count == 1)
        {
            if(pfd.revents & POLLIN)
            {
                std::cout << "Client confd can read!" << std::endl;
                int n = ::read(confd,buf,16);
                std::cout << "read n: " << n << std::endl;
                if(n == 0)
                    std::cout << "client confd alreay close!" << std::endl;
            }
            else if(pfd.revents & POLLOUT)
            {
                std::cout << "Client confd can write!" << std::endl;
            }
            else if((pfd.revents & POLLERR) || (pfd.revents & POLLHUP) || (pfd.revents & POLLNVAL))
            {
                std::cout << "confd alredy close!" << std::endl;
            }
            else
            {
                std::cout << "error!" << std::endl;
            }
        }
        else
            std::cout << "nothing happened!" << std::endl;

    sleep(10);
    unet::net::socket::close(listenfd);
    std::cout << "listen socket close" << std::endl;
    unet::net::socket::close(confd);
    std::cout << "server confd close" << std::endl;
    return 0;
}

































