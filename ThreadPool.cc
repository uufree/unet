/*************************************************************************
	> File Name: ThreadPool.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月27日 星期一 23时31分49秒
 ************************************************************************/

#include"ThreadPool.h"

namespace unet
{
    ThreadPool::ThreadPool(int size) :
        _start(false),
        _threadSize(size),
        _threadListPtr(new base::Thread[size]),
        _mutex(),
        _cond(_mutex)
    {};
        
    ThreadPool::ThreadPool(int size,const ThreadFunc& cb) :
        _start(false),
        _threadSize(size),
        _threadListPtr(new base::Thread[size]),
        _threadFunc(cb),
        _mutex(),
        _cond(_mutex)
    {};

    ThreadPool::ThreadPool(ThreadPool&& lhs) : 
        _start(false),
        _threadSize(lhs._threadSize),
        _threadListPtr(new base::Thread[_threadSize]),
        _threadFunc(std::move(lhs._threadFunc)),
        _mutex(),
        _cond(_mutex)
        {};
        
    ThreadPool& ThreadPool::operator=(ThreadPool&& lhs)
    {
        joinAll();
        delete [] _threadListPtr;

        _start = false;
        _threadListPtr = new base::Thread[_threadSize];
        _threadFunc = std::move(lhs._threadFunc);
        return *this;
    }

    ThreadPool::~ThreadPool()
    {
        _start = false;
        for(int i=0;i<_threadSize;++i)
            ::pthread_detach(_threadListPtr[i].getThreadId());

        delete [] _threadListPtr;
    }
    
    void ThreadPool::start()
    {
        if(!_start)
        {
            for(int i=0;i<_threadSize;++i)
            {
                _threadListPtr[i].setThreadCallBack(_threadFunc);
                _threadListPtr[i].start();
            }
            _start = true;
        }
    }

    void ThreadPool::joinAll()
    {
        for(int i=0;i<_threadSize;++i)
            _threadListPtr[i].join();
    }
}
