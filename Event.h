/*************************************************************************
	> File Name: Event.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月18日 星期五 17时18分45秒
 ************************************************************************/

#ifndef _EVENT_H
#define _EVENT_H

#include<cstdint>
#include<memory>

#include"type.h"
enum EventType{SOCKET,CLOCK,SIGNAL};

#define U_EPOLL_ET 0x01
#define U_EPOLL_LT 0x02

struct SocketEvent
{
    int _socketfd;
    SocketType _type;
    int _event;
    int _revent;
};

struct SignalEvent
{
    int _signalfd;  //管道描述符的读端
    int _event;     //关注的信号事件
    int _revent;    //实际发生的信号事件
};

struct TimerEvent
{
    int _timerfd;
    int _event;
};

//将事件统一起来，为每一个事件提供一个全局唯一的描述符(对描述符进行虚拟化)
//这个结构将会传递给Epoll进行处理，随后返回
class Event final
{
    public:
        Event(int fd,int type) noexcept;
        Event(const Event&) = delete;
        Event& operator=(const Event&) = delete;
        Event(Event&&);
        Event& operator=(Event&&);
        ~Event();
    
        int getFd() const{return _fd;};
        int getEvent() const{return w_event;};
        void setWEvent(int event) {w_event = event;};
        void setREvent(int event) {r_event = event;}; 
        bool eventHappened();
        
    private:
        const int _fd;
        EventType _type;
        int w_event;    //关注的事件
        int r_event;    //发生的事件

        union
        {
            std::weak_ptr<SocketEvent> socket;
            std::weak_ptr<SignalEvent> signal;
            std::weak_ptr<TimerEvent> timer;
        } _eventPtr;
};

#endif
