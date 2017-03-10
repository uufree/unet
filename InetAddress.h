/*************************************************************************
	> File Name: InetAddress.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年02月27日 星期一 20时58分25秒
 ************************************************************************/

#ifndef _INETADDRESS_H
#define _INETADDRESS_H

#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/sockets.h>

namespace unet
{
    class InetAddress final
    {
        public:
            const int IPV4SIZE = 16;
            const int IPV6SIZE = 32;
            explicit InetAddress(uint16_t port)//listen socket
            {
                bzero(&addr4,sizeof(addr4));
                addr4.sin_port = htons(port);
                addr4.sin_family = AF_INET;
                addr4.sin_addr.s_addr = htonl(INADDR_ANY);
            }

            explicit InetAddress(std::string& ip,uint16_t port);//socket
            {
                bzero(&addr4,sizeof(addr4));
                addr4.sin_family = AF_INET;
                addr4.sin_port = htons(port);
                inet_pton(AF_INET,ip.c_str(),&addr4.sin_addr);
                addr4.sin_addr.s_addr = htonl(addr4.sin_addr.s_addr);
            }

            explicit InetAddress(const InetAddress& addr4_) : addr4(addr4_) {};

            InetAddress(InetAddress&& addr4_) : addr4(addr4_) {};

            std::string getIpString()
            {
                return static_cast<string>(inet_ntop(addr4.sin_family,&addr4.sin_addr,NULL,IPV4SIZE));
            }
            
            uint32_t getIp()
            {
                return static_cast<uint32_t>(addr4.sin_addr.s_addr);
            }

            uint16_t getPort()
            {
                return static_cast<uint16_t>(addr4.sin_port);
            }
            
        private:
            struct sockaddr_in addr4;
    }
}



#endif

