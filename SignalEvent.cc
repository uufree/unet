/*************************************************************************
	> File Name: SignalEvent.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年04月01日 星期日 02时08分38秒
 ************************************************************************/

#include<cstring>
#include<sys/types.h>
#include<signal.h>
#include<sys/socket.h>
#include<netinet/in.h>
#include<arpa/inet.h>

#include"SignalEvent.h"
#include"base/Global.h"

namespace unet
{
    int SignalEvent::u_readSignalfd = -1;
    int SignalEvent::u_writeSignalfd = -1;

    SignalEvent::SignalEvent() :
        u_wsignal(0),
        u_rsignal(0)
    {init();};

    SignalEvent::SignalEvent(int signal) :
        u_wsignal(signal),
        u_rsignal(0)
    {init();};

    SignalEvent::~SignalEvent()
    {
        if(u_readSignalfd)
            ::close(u_readSignalfd);
        if(u_writeSignalfd)
            ::close(u_writeSignalfd);
    }
    
    void SignalEvent::init()
    {
        //open signalfd
        int pipefd[2];
        if(socketpair(PF_UNIX,SOCK_STREAM,0,pipefd) < 0)
        {
            perror("Socket Pair opened Error!\n");
            unet::handleError(errno);
        }
        
        u_readSignalfd = pipefd[0];
        u_writeSignalfd = pipefd[1];
        
        //add sys singal
        addSignal(U_SIGHUP);
        addSignal(U_SIGPIPE);
        addSignal(U_SIGURG);
        addSignal(U_SIGCHLD);
        addSignal(U_SIGTERM);
        addSignal(U_SIGINT);
        
        //add usr signal
        addSignal(U_USRINT);
        addSignal(U_USRSTOP);
        addSignal(U_USRRESTART);
    }

    void SignalEvent::sendSignal(int sig)
    {
        if(::send(u_writeSignalfd,&sig,4,0) < 0)
        {
            perror("send sig error!\n");
            unet::handleError(errno);
        }
    }
    
    int SignalEvent::switchTo(int usig)
    {
        int sig = 0;
        if(usig & U_SIGHUP)
            sig = SIGHUP;
        else if(usig & U_SIGPIPE)
            sig = SIGPIPE;
        else if(usig & U_SIGURG)
            sig = SIGURG;
        else if(usig & U_SIGCHLD)
            sig = SIGCHLD;
        else if(usig & U_SIGTERM)
            sig = SIGTERM;
        else if(usig & U_SIGINT)
            sig = SIGINT;

        return sig;
    }
    
    void sendSignal(int sig)
    {
        SignalEvent::sendSignal(sig);
    }

    void SignalEvent::addSignal(int usig)
    {
        int ssig = usig < 0x00010000 ? switchTo(usig) : usig;
        if(!(usig & U_USRINT) && !(usig & U_USRSTOP) && !(usig & U_USRRESTART))
        {
            struct sigaction sa;
            memset(&sa,'\0',sizeof(sa));
            sa.sa_handler = sendSignal;
            sa.sa_flags |= SA_RESTART;
            sigfillset(&sa.sa_mask);
            if(sigaction(ssig,&sa,NULL) < 0)
            {
                perror("signal resgistre error!\n");
                unet::handleError(errno);
            }
        }

        u_wsignal |= usig;
    }
    
    void SignalEvent::eraseSignal(int usig)
    {
        int ssig = usig < 0x00010000 ? switchTo(usig) : usig;
        if(ssig < 0x00010000)
        {
            sigset_t set;
            sigemptyset(&set);
            sigaddset(&set,ssig);
            
            if(sigprocmask(SIG_BLOCK,&set,NULL) < 0)
            {
                perror("erase signal error!\n");
                unet::handleError(errno);
            }
        }

        u_wsignal &= ~usig;
    }
   
    void SignalEvent::handleSignal()
    {
        while(1)
        {
            int usig = 0;
            if(::recv(u_readSignalfd,(char*)&usig,sizeof(int),0) < 0)
                return;
        
            if(!hasSingal(usig))
                continue;
        
            u_rsignal |= usig;
            
            if((usig & U_SIGTERM) || (usig & U_SIGINT) || (usig & U_USRINT))
            {
                if(u_quitCallBack)
                    u_quitCallBack();
                break;
            }
            
            if(usig & U_SIGHUP)
            {
                if(u_readConfigCallBack)
                    u_readConfigCallBack();
                continue;
            }
            
            if(usig & U_USRSTOP)
            {
                if(u_stopCallBack)
                    u_stopCallBack();
                continue;
            }
            
            if(usig & U_USRRESTART)
            {
                if(u_restartCallBack)
                    u_restartCallBack();
                continue;
            }
            
            if((usig & U_SIGPIPE) || (usig & U_SIGURG))
            {
                /*如何处理往socket中写入数据的错误与带外数据到达的问题?*/
                continue;
            }
        }
    }
}
    
