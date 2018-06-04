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
#include"base/Condition.h"
#include"base/Thread.h"
#include"base/Alloc.h"
#include"LogFile.h"


/*关于LogBuffer Queue的一些思考：
 * 如果使用了Timer，就意味出现了异步的情况，必然会加重锁的负担。但是，如果不引入
 * Timer，Buffer中的日志消息得不到定时的清理，即使对日志消息进行计数，也无法满足
 * 日志消息的安全性。如果一个线程的日志消息量很小，一直小于计数量，日志消息便一
 * 直存储在内存中，无法写入文件。
 * 在这个Log Buffer Queue的设计中，日志消息只能由一般对象传递给Write Buffer，无
 * 法被Write Buffer反向收集。只能说是：设计上有硬伤。
 * 其实，说到底，根本问题还是Timer没办法融入TimerEvent。*/

/*接上，反过来讨论，即使是使用了Timer，就需要给Timer维护一个Mutex，各有利弊。*/

/*当前版本：为每个Log Buffer Queue维护一个计数，消息到达一定的计数，主动通知写入*/
/*多线程无锁日志的弊端：各个线程的日志时间不匹配*/

namespace unet
{
    static const int MAX_LOG = 2048;

    class LogBufferQueue final
    {
        public:
            LogBufferQueue(int length = 2);
            LogBufferQueue(const LogBufferQueue&) = delete;
            LogBufferQueue& operator=(const LogBufferQueue&) = delete;
            LogBufferQueue(LogBufferQueue&&);
            LogBufferQueue& operator=(LogBufferQueue&&);
            ~LogBufferQueue();
            
            bool operator==(const LogBufferQueue& log){return u_bufferList.front() == log.u_bufferList.front();};

            void append(const char* message);
            void clear();
            static void startWriteThread(){u_writeThread.start();};
            static void stopWriteThread(){u_writeThread.stop();};
            static void WriteBackThread();

        private:
            void handleSwapEvent();
            void initWriteBack(); 

        private:
            int u_length;   /*某个工作线程的LogBuffers长度*/
            int u_logNumbers;   /*维护一个Log计数，到达一定的Log量，写入后台*/
            std::list<base::LogBuffer*> u_bufferList; /*某个工作线程对应的LogBuffers*/
        
            static std::list<base::LogBuffer*> u_writeBackList;
            static base::MutexLock u_writeBackMutex;
            static base::Condition u_writeBackCond;
            static LogFile u_logFile;   /*回写文件*/
            static base::Thread u_writeThread;     /*回写线程*/
    };
}

#endif
