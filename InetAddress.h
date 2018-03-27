/*************************************************************************
	> File Name: InetAddress.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年02月27日 星期一 20时58分25秒
 ************************************************************************/

//封装IP地址

#ifndef _INETADDRESS_H
#define _INETADDRESS_H

#include<sys/types.h>
#include<netinet/in.h>
#include<arpa/inet.h>
#include<sys/socket.h>
#include<string.h>
#include<string>

namespace unet
{
    namespace base
    {
        enum InetAddressType{IPV6,IPV4}; 

        class InetAddress final
        {   
            public:
                explicit InetAddress(int port,const std::string& ip = "INADDR_ANY",InetAddressType type = IPV4):
                    _ip(ip),
                    _type(type)
                {init(static_cast<uint16_t>(port),type);};
                
                explicit InetAddress(uint16_t port,const std::string& ip = "INADDR_ANY",InetAddressType type = IPV4) :
                    _ip(ip),
                    _type(type)
                {init(port,type);};
                
                InetAddress(const InetAddress& lhs) :
                    _addr(lhs._addr),
                    _ip(lhs._ip),
                    _type(lhs._type)
                {};

                InetAddress(InetAddress&& lhs) :
                    _addr(std::move(lhs._addr)),
                    _ip(std::move(lhs._ip)),
                    _type(std::move(lhs._type))
                {};

                InetAddress& operator=(const InetAddress& lhs)
                {
                    if(*this == lhs)
                        return *this;
                    _addr = lhs._addr;
                    _ip = lhs._ip;
                    _type = lhs._type;
                    return *this;
                }

                InetAddress& operator=(InetAddress&& lhs)
                {
                    if(*this == lhs)
                        return *this;
                    _addr = lhs._addr;
                    _ip = lhs._ip;
                    _type = lhs._type;
                    return *this;
                }

                ~InetAddress() noexcept {};
                
                bool operator==(const InetAddress& addr)
                {
                    if(addr._type != _type)
                        return false;
                    return _type==IPV4 ? addr._addr._addrv4.sin_port==_addr._addrv4.sin_port : addr._addr._addrv6.sin6_port == addr._addr._addrv6.sin6_port;
                };

                const std::string& getIpString() const {return _ip;};
                sockaddr* getSocketAddr() 
                {return _type==IPV4 ? (sockaddr*)&_addr._addrv4 : (sockaddr*)&_addr._addrv6;}
                
            //这两个函数没有提供IPV6版本             
                uint32_t getIp() const
                {return static_cast<uint32_t>(ntohl(_addr._addrv4.sin_addr.s_addr));};   
                uint16_t getPort() 
                {return static_cast<uint16_t>(ntohs(_addr._addrv4.sin_port));};
                
                InetAddressType getType() const
                {return _type;};

            private:
                void init(uint16_t port,InetAddressType type)
                {
                    if(type == IPV4)
                    {
                        _addr._addrv4.sin_port = htons(port);
                        _addr._addrv4.sin_family = AF_INET;
                        _ip=="INADDR_ANY" ? _addr._addrv4.sin_addr.s_addr=htonl(INADDR_ANY) : inet_pton(AF_INET,_ip.c_str(),&_addr._addrv4.sin_addr.s_addr);
                        if(_ip == "INADDR_ANY")
                            _addr._addrv4.sin_addr.s_addr = htonl(INADDR_ANY);
                        else
                        {
                            inet_pton(AF_INET,_ip.c_str(),&_addr._addrv4.sin_addr);
                            _addr._addrv4.sin_addr.s_addr = htonl(_addr._addrv4.sin_addr.s_addr);
                        }
                    }
                    else
                    {
                        _addr._addrv6.sin6_port = htons(port);
                        _addr._addrv6.sin6_family = AF_INET6;
                        //不知如何初始化IPV6，反正也用不到... 
                    }
                }
                
            private:
                union Inet 
                {
                    struct sockaddr_in _addrv4;
                    struct sockaddr_in6 _addrv6;
                } _addr;
                std::string _ip;
                InetAddressType _type;
        };
    }
}

#endif
