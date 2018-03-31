/*************************************************************************
	> File Name: Buffer.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年03月14日 星期二 19时28分46秒
 ************************************************************************/

#include"Buffer.h"

namespace unet
{
    namespace base
    {
        int StringBuffer::readInSocket()
        {
            char extraBuffer[65536];
            int nbytes = 0;
            while(1)
            {
                memset(extraBuffer,'\0',65536);
                nbytes = ::read(_confd,extraBuffer,65536); 
                if(nbytes < 0)
                {
                    if(errno==EAGAIN || errno==EWOULDBLOCK)
                        break;
                }
                else if(nbytes > 0)
                    appendInBuffer(extraBuffer,nbytes);
                else
                {};
            }
            
            return nbytes;
        }

        int StringBuffer::blockReadInSocket()
        {
            char extraBuffer[65536];
            int nbytes = readn(_confd,extraBuffer,14);
            if(nbytes > 0)
                appendInBuffer(extraBuffer);
            return nbytes;
        }

        int StringBuffer::writeInSocket() 
        {
            int n = ::write(_confd,_buffer.c_str(),_buffer.size());
            if(n > 0)
                _buffer.erase(0,n);
            else if(n < 0)
                handleError(errno);
            else
            {};
            
            return n;
        }
                
        void StringBuffer::appendInBuffer(const char* message,size_t nbytes) 
        {
            _buffer.append(message,nbytes);
            _buffer.append("\r\n");
        }

        void StringBuffer::appendInBuffer(const std::string& message)
        {
            _buffer.append(message);
            _buffer.append("\r\n");
        }

        void StringBuffer::getInBuffer(std::string& message)
        {
            size_t index = _buffer.find_first_of("\r\n");
            if(index != std::string::npos)
            {
                message.append(_buffer,0,index);
                _buffer.erase(0,index+2);
            }
        }
    }
}
