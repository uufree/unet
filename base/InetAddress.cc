/*************************************************************************
	> File Name: InetAddress.cc
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2018年05月29日 星期二 17时08分10秒
 ************************************************************************/

#include"InetAddress.h"

/*封装的也比较“实用”，具体的细节内容后续继续封装*/
namespace unet
{
    namespace base
    {
        InetAddress::InetAddress(int port,const std::string& ip,InetAddressType type):
            u_ip(ip),
            u_type(type)
        {init(static_cast<uint16_t>(port),type);};
                
        InetAddress::InetAddress(uint16_t port,const std::string& ip,InetAddressType type) :
            u_ip(ip),
            u_type(type)
        {init(port,type);};
                
        InetAddress::InetAddress(const InetAddress& lhs) :
            u_addr(lhs.u_addr),
            u_ip(lhs.u_ip),
            u_type(lhs.u_type)
        {};

        InetAddress::InetAddress(InetAddress&& lhs) :
            u_addr(std::move(lhs.u_addr)),
            u_ip(std::move(lhs.u_ip)),
            u_type(lhs.u_type)
        {};

        InetAddress& InetAddress::operator=(const InetAddress& lhs)
        {
            if(*this == lhs)
                return *this;
            
            u_addr = lhs.u_addr;
            u_ip = lhs.u_ip;
            u_type = lhs.u_type;
            return *this;
        }

        InetAddress& InetAddress::operator=(InetAddress&& lhs)
        {
            if(*this == lhs)
                return *this;
            
            u_addr = std::move(lhs.u_addr);
            u_ip = std::move(lhs.u_ip);
            u_type = lhs.u_type;
            return *this;
        }

        bool InetAddress::operator==(const InetAddress& addr)
        {
            if(addr.u_type != u_type)
                return false;
            return u_type==IPV4 ? 
                addr.u_addr.u_addrv4.sin_port==u_addr.u_addrv4.sin_port : 
                addr.u_addr.u_addrv6.sin6_port == addr.u_addr.u_addrv6.sin6_port;
        };

        void InetAddress::init(uint16_t port,InetAddressType type)
        {
            if(type == IPV4)
            {
                u_addr.u_addrv4.sin_port = htons(port);
                u_addr.u_addrv4.sin_family = AF_INET;
                
                if(u_ip == "INADDR_ANY")
                    u_addr.u_addrv4.sin_addr.s_addr = htonl(INADDR_ANY);
                else
                    inet_pton(AF_INET,u_ip.c_str(),&u_addr.u_addrv4.sin_addr);
            }
            else
            {
                /*暂时不关注IPV6的初始化*/
                u_addr.u_addrv6.sin6_port = htons(port);
                u_addr.u_addrv6.sin6_family = AF_INET6;
            }
        }
    }
}


