/*************************************************************************
	> File Name: TcpServer.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年06月02日 星期六 21时14分28秒
 ************************************************************************/

#include"TcpServer.h"
#include"EPoller.h"
#include"Poller.h"
#include"Selecter.h"

namespace unet
{
    /*定时器事件依旧需要纳入事件处理事件处理*/
    std::shared_ptr<Event> TcpServer::u_timerEventPtr(new Event(U_TIMER));
    /*信号事件由主线程单独管理*/
//    std::shared_ptr<SignalEvent> TcpServer::u_signalEventPtr(new SignalEvent());

    TcpServer::TcpServer(base::InetAddress& server,int size) :
        u_start(false),
        u_serverAddr(server),
        u_connectionMap(),
        u_eventMap(),
        u_eventDemuType(U_POLL),
        u_eventDemu(new Poller()),
        u_loop(),
        u_accepter(server),
        u_taskPool(size),
        u_taskSize(size),
        u_eventPtrList()
    {
        u_loop.setSeparationEventCallBack(std::bind(&TcpServer::SeparationEvent,this));
        u_accepter.setSaveListenCallBack(std::bind(&TcpServer::SaveListen,this,std::placeholders::_1));
        u_accepter.setEraseListenCallBack(std::bind(&TcpServer::SaveListen,this,std::placeholders::_1));
        u_accepter.setSaveConnectCallBack(std::bind(&TcpServer::SaveConnect,this,std::placeholders::_1)); 
        
        /*向loop注册TimerEvent*/
        u_eventMap.insert(u_timerEventPtr);
        u_eventDemu->addEvent(u_timerEventPtr->getFd(),u_timerEventPtr->getWEvent());
    };
    
    TcpServer::TcpServer(TcpServer&& tcp) :
        u_start(tcp.u_start),
        u_serverAddr(std::move(tcp.u_serverAddr)),
        u_connectionMap(std::move(tcp.u_connectionMap)),
        u_eventMap(std::move(tcp.u_eventMap)),
        u_eventDemuType(tcp.u_eventDemuType),
        u_eventDemu(std::move(tcp.u_eventDemu)),
        u_loop(std::move(tcp.u_loop)),
        u_accepter(std::move(tcp.u_accepter)),
        u_taskPool(std::move(tcp.u_taskPool)),
        u_taskSize(std::move(tcp.u_taskSize)),
        u_eventPtrList(std::move(tcp.u_eventPtrList)),
        u_readCallBack(std::move(tcp.u_readCallBack)),
        u_writeCallBack(std::move(tcp.u_writeCallBack))
    {};
    
    TcpServer& TcpServer::operator=(TcpServer&& server)
    {
        if(*this == server)
            return *this;
        if(u_start)
            stop();
        
        u_start = server.u_start;
        u_serverAddr = std::move(server.u_serverAddr);
        u_connectionMap = std::move(server.u_connectionMap);
        u_eventMap = std::move(server.u_eventMap);
        u_eventDemuType = server.u_eventDemuType;
        u_eventDemu = std::move(server.u_eventDemu);
        u_loop = std::move(server.u_loop);
        u_accepter = std::move(server.u_accepter);
        u_taskPool = std::move(server.u_taskPool);
        u_taskSize = server.u_taskSize;
        u_eventPtrList = std::move(server.u_eventPtrList);
        u_readCallBack = std::move(server.u_readCallBack);
        u_writeCallBack = std::move(server.u_writeCallBack);

        return *this;
    }

    TcpServer::~TcpServer()
    {
        if(u_start)
            stop();
    }

    void TcpServer::SaveListen(int fd)
    {
        std::shared_ptr<Event> lptr(new Event(U_LISTEN_SOCKET,fd));
        lptr->setListenReadCallBack(std::bind(&TcpServer::AccepterHandleRead,this));
        lptr->setListenCloseCallBack(std::bind(&TcpServer::EraseListen,this,std::placeholders::_1));
        u_eventMap.insert(lptr);
        u_eventDemu->addEvent(fd,U_READ|U_EXCEPTION);
    }

    void TcpServer::EraseListen(int fd)
    {
        u_eventDemu->delEvent(fd);
        u_eventMap.erase(fd);
    }

    void TcpServer::SaveConnect(int fd)
    {
        std::shared_ptr<TcpConnection> ptr(std::make_shared<TcpConnection>(fd));
        ptr->setReadCallBack(u_readCallBack);
        ptr->setWriteCallBack(u_writeCallBack);
        ptr->setCloseCallBack(std::bind(&TcpServer::EraseConnect,this,std::placeholders::_1));
        
        std::shared_ptr<Event> eptr(new Event(U_CONNECT_SOCKET,fd));
        eptr->setTcpConnectionPtr(ptr);  

        u_eventMap.insert(eptr);
        u_connectionMap.insert(ptr);
        u_eventDemu->addEvent(fd,U_READ|U_WRITE|U_EXCEPTION);
    }
    
    void TcpServer::EraseConnect(int fd)
    {
        u_connectionMap.erase(fd);
        u_eventMap.erase(fd);
        u_eventDemu->delEvent(fd);
    }
    
    void TcpServer::SeparationEvent()
    {
        u_eventPtrList.clear();
        u_eventDemu->poll(u_eventMap,u_eventPtrList);
        if(!u_eventPtrList.empty())
            u_taskPool.addInTaskEvent(u_eventPtrList);
    }
    
    /*重置事件，由事件处理的线程重置事件，需要使用锁进行同步*/
    void TcpServer::ResetEvent(int fd)
    {
        u_eventDemu->resetEvent(fd);
    }
    
    void TcpServer::start()
    {
        if(u_start)
            return;
        u_accepter.startListen();
//        u_taskPool.start();
        u_loop.start();
//        u_timerEventPtr->startTimerEvent();
        u_start = true;
//        u_signalEventPtr->handleEvent();
    }
    
    void TcpServer::stop()
    {
        if(!u_start)
            return;
        u_accepter.stopListen();
        u_taskPool.stop();
        u_loop.stop();
        u_timerEventPtr->stopTimerEvent();
        u_start = false;
    }
}

