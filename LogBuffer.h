/*************************************************************************
	> File Name: LogBuffer.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月26日 星期六 06时05分42秒
 ************************************************************************/

#ifndef _LOGBUFFER_H
#define _LOGBUFFER_H

#include<functional>
#include<memory>
#include<list>
#include"base/Mutex.h"

namespace unet
{
    /*设计目的：
     * 细化LogBuffer锁的粒度，所有的Queue又共享一个WriteBackQueue。
     * 每个工作线程会向线程专属所属的LogBuffer写入日志，定时清理LogBuffer。
     * 定时清理WriteBackQueue。
     */

    class Timer;
    struct LogBuffer;
    class LogFile;    

    class LogBufferQueue final
    {
        typedef std::shared_ptr<Timer> TimerPtr;

        public:
            LogBufferQueue(int length = 2);
            LogBufferQueue(const LogBufferQueue&) = delete;
            LogBufferQueue& operator=(const LogBufferQueue&) = delete;
            LogBufferQueue(LogBufferQueue&&);
            LogBufferQueue& operator=(LogBufferQueue&&);
            ~LogBufferQueue();
            
            bool operator==(const LogBufferQueue& log){return u_bufferList.front() == log.u_bufferList.front();};

            void start();
            void stop();
            static void startWriteBack();
            static void stopWriteBack();

        private:
            void handleSwapEvent();
            void handleWriteBackEvent();
            void initWriteBack(); 

        private:
            bool u_start;
            int u_length;
            std::list<LogBuffer*> u_bufferList;
            base::MutexLock u_mutex;
            TimerPtr u_timer;
            
            static bool u_writeBackInit;
            static bool u_writeBackStart;
            static std::list<LogBuffer*> u_writeBackList;
            static TimerPtr u_writeBackTimer;
            static base::MutexLock u_writeBackMutex;
            static LogFile u_logFile;
    };
}

#endif
