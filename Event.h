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

/*
 * 再三思索，还是决定暂时不把SignalEvent加入事件处理框架，原因如下：
 *   Singal是一个全局事件，决定着整个库当前工作的平稳性，在事件线程进行处理的话，
 *   缺少全局视角，就算回调函数可以解决，但是又需要使用锁去维护Signal状态，但是
 *   Signal本身就具有随机性
 */

/* class设计目的：
 * 1.将SocketEvent与TimerEvent同一起来，在事件发生后，将Event交由事件线程处理
 * 2.Epoll会分离事件，处理的对象也是Event
 * 3.Event保持事件状态，SocketEvent和TimerEvent只负责处理事件
 */

/* 因为以union存储对象的复杂度，故我们使用普通的指针存储对象
 * 我们持有另一个对象的指针，并管理生命期，使用引用计数的话也不方便
 * 故，去掉Copy与Move操作
 */ 

namespace unet
{
    class SocketEvent;
    class SignalEvent;
    class TimerEvent;

    class Event final
    {
        public:
            explicit Event(int fd,int type,int wevent);
            Event(const Event&) = delete;
            Event& operator=(const Event&) = delete;
            Event(Event&&) = delete;
            Event& operator=(Event&&) = delete;
            ~Event();
            
            bool operator==(const Event& event){return u_fd==event.u_fd && u_type==event.u_type;};

            int getFd() const{return u_fd;};
            int getWEvent() const{return u_wevent;};
            void setWEvent(int event) {u_wevent = event;};
            void setREvent(int event) {u_revent = event;}; 
            bool eventHappened(){return u_wevent & u_revent;};
            
            void handleEvent();

        private:
            int u_fd;  //socketfd or timerfd   
            int u_type;      //Socket or Timer 
            int u_wevent;    //watch event
            int u_revent;    //happeded event
            
            union
            {
                SocketEvent* u_socket;
                SignalEvent* u_signal;
                TimerEvent* u_timer;
            } u_event;
    };
};

#endif
