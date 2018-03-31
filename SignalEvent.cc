/*************************************************************************
	> File Name: SignalEvent.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年04月01日 星期日 02时08分38秒
 ************************************************************************/

#include"SignalEvent.h"

namespace unet
{
    SignalEvent::SignalEvent() :
        _bitmap()
    {
        if(::socketpair(PF_UNIX,SOCK_STREAM,0,_pipefd) < 0)
            handleError(errno);
        _bitmap.reset();
    }

    SignalEvent::~SignalEvent()
    {
        _bitmap.reset();

    }
}
    
