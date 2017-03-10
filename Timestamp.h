/*************************************************************************
	> File Name: Time.h
	> Author: uuchen
	> Mail: 1319081676@qq.com
	> Created Time: 2017年02月27日 星期一 16时17分32秒
 ************************************************************************/

#ifndef _TIME_H
#define _TIME_H

namespace unet
{
    namespace time
    {
        class Timestamp final
        {
            public:
                explicit Timestamp()
                {
                    struct timeval tv;
                    ::gettimeofday(&tv,NULL);
                    microseconds = tv.sec*KMicroseconds + tv.tv_usec;
                }

                explicit Timestamp(int64_t microseconds_) : 
                    microseconds(microseconds_)
                {};

                explicit Timestamp(Timestamp& rhs) :
                    microseconds(rhs.microseconds)
                {};

                void swap(Timestamp& rhs)
                {
                    std::swap(microseconds,rhs.microseconds);
                };
                    
                int64_t getTime() const
                {
                    return microseconds;
                }
                
            static const int KMicroseconds = 1000000;
            
            private:
                int64_t microseconds;
        };

        inline bool operator<(const Timestamp& lhs,const Timestamp& rhs)
        {
            return lhs.getTime() < rhs.getTime();
        }

        inline bool operator==(const Timestamp& lhs,const Timestamp& rhs)
        {
            return lhs.getTime() == rhs.getTime();
        }
        
        inline Timestamp addTime(Timestamp& lhs,double seconds)
        {
            return Timestamp(lhs.getTime()+seconds*KMicroseconds);
        }
        
        inline double timedifference(const Timestamp& lhs,const Timestamp& rhs)
        {
            int64_t diff = lhs.getTime() - rhs.getTime();
            return static<double>(diff/Timestamp::KMicroseconds);
        }
    }
}

#endif

