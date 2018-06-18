/*************************************************************************
	> File Name: SignalEvent.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年04月01日 星期日 02时08分32秒
 ************************************************************************/

#ifndef _SIGNALEVENT_H
#define _SIGNALEVENT_H

#include<functional>
#include<iostream>

/*signal event分成两部分：一是系统原生类型，需要在主线程进行处理；
 * 二是自定义类型，子线程向主线程发送字节类型的数据。
 * 主线程根据用户定义的类型进行处理，有较大的灵活性*/

#define U_SIGHUP 0x01   //sys->主线程，用来重新读取配置文件（稍后实现）
#define U_SIGPIPE 0x02  //sys->主线程，往一个错误的socket写数据（忽略）
#define U_SIGURG 0x04   //sys->主线程，带外数据到达（忽略）
#define U_SIGCHLD 0x08  //sys->主线程，父进程捕捉已经终止的子进程，调用wait()函数
#define U_SIGTERM 0x10  //sys->主线程，安全终止进程（被动）
#define U_SIGINT 0x20   //sys->主线程，安全终止进程（被动）
#define U_SIGKILL 0x40  //sys->主线程，安全终止进程（被动）(SIGKILL不能被用户处理)

#define U_USRINT 0x00010000     //子线程->主线程，安全结束任务（主动）
#define U_USRSTOP 0x00020000    //子线程->主线程，停止任务
#define U_USRRESTART 0x00040000 //子线程->主线程，从上次停止的位置启动任务  

/*
 * 设计理念：
 *信号是一个全局的事件，为了避免在线程间出现信号的传递，我们让主线程阻塞在pipe[0]
 *上，出现信号时，处理信号，然后继续阻塞，在遇到一些无法处理的错误时，发送终止信号
 *，可以保证程序在销毁资源后，安全的退出。同时，因为全程在主线程中进行，所以不需要
 *锁，并且需要在对象创建时就向SignalEvent对象注册好需要处理的信号。
 *
*/

/*2018.06.01 测试完成*/

namespace unet
{
    class SignalEvent final
    {
        typedef std::function<void()> QuitCallBack;
        typedef std::function<void()> StopCallBack;
        typedef std::function<void()> RestartCallBack;
        typedef std::function<void()> ReadConfigCallBack;

        public:
            explicit SignalEvent();
            SignalEvent(int signal);
            SignalEvent(const SignalEvent& sig) = delete;
            SignalEvent(SignalEvent&& sig) = delete;
            SignalEvent& operator=(const SignalEvent& sig) = delete;
            SignalEvent& operator=(SignalEvent&& sig) = delete;
            ~SignalEvent();

            bool operator==(const SignalEvent& signal){return u_readSignalfd==signal.u_readSignalfd && u_writeSignalfd==signal.u_writeSignalfd;};

            void addSignal(int);
            void eraseSignal(int);
            bool hasSingal(int sig) const{return sig & u_wsignal;};
        
            void handleEvent();
            static void sendSignal(int sig);
            
            void setQuitCallBack(const QuitCallBack& quit){u_quitCallBack=quit;};
            void setStopCallBack(const StopCallBack& stop){u_stopCallBack=stop;};
            void setRestartCallBack(const RestartCallBack& restart){u_restartCallBack=restart;};
            void setReadConfigCallBack(const ReadConfigCallBack& config){u_readConfigCallBack=config;};

        private:
            void init();
            int switchTo(int sig);

        private:
            int u_wsignal;   //watch event
            int u_rsignal;   //happened event
            static int u_readSignalfd;     //pipe[0]
            static int u_writeSignalfd;    //pipe[1]
        
            QuitCallBack u_quitCallBack;/*根据用户规定的信号设置可能出现的回调*/
            StopCallBack u_stopCallBack;
            RestartCallBack u_restartCallBack;
            ReadConfigCallBack u_readConfigCallBack;
    };
    

    void sendSignal(int sig);   /*全局函数，可以在工作线程中向主线程中发送信号*/
    /*从而根据需求动态的调整资源*/
}

#endif
