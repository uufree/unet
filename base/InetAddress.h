/*************************************************************************
	> File Name: InetAddress.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年02月27日 星期一 20时58分25秒
 ************************************************************************/

#ifndef _INETADDRESS_H
#define _INETADDRESS_H

#include<string.h>
#include<string>
#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>

/*2018.05.29 测试完成*/

namespace unet
{
    namespace base
    {
        enum InetAddressType{IPV6,IPV4}; 
        
        /*暂时没有提供IPV6版本*/
        class InetAddress final
        {   
            public:
                explicit InetAddress(int port,const std::string& ip = "INADDR_ANY",InetAddressType type = IPV4);
                explicit InetAddress(uint16_t port,const std::string& ip = "INADDR_ANY",InetAddressType type = IPV4);
                InetAddress(const InetAddress&);
                InetAddress(InetAddress&& lhs);
                InetAddress& operator=(const InetAddress& lhs);
                InetAddress& operator=(InetAddress&& lhs);
                ~InetAddress() noexcept {};
                
                bool operator==(const InetAddress& addr);

                const std::string& getIpString() const {return u_ip;};
                
                sockaddr* getSocketAddr() 
                {return u_type==IPV4 ? (sockaddr*)&u_addr.u_addrv4 : (sockaddr*)&u_addr.u_addrv6;}
                
                uint32_t getIp() const
                {return static_cast<uint32_t>(ntohl(u_addr.u_addrv4.sin_addr.s_addr));};   
                uint16_t getPort() 
                {return static_cast<uint16_t>(ntohs(u_addr.u_addrv4.sin_port));};
                
                InetAddressType getType() const {return u_type;};

            private:
                void init(uint16_t port,InetAddressType type);
                
            private:
                union Inet 
                {
                    struct sockaddr_in u_addrv4;
                    struct sockaddr_in6 u_addrv6;
                } u_addr;
                std::string u_ip;
                InetAddressType u_type;
        };
    }
}

#endif
