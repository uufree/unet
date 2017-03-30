一个基于muduo源码风格，Reactor模式的现代C++网络库（非阻塞）

unet的文件结构：

unet
\-- mutilServer                     多线程TcpServer
|   |-- Current.{h,cc}              中间类，结合Acceptor和EventLoop，主要作用是形成回调
|   |-- EventLoopThread.{h,cc}      含有EventLoop（实际是Current）的线程类
|   |-- EventLoopThreadPool.{h,cc}
|   |-- MutilAcceptor.{h,cc}              
|   |-- MutilTcpServer.{h,cc}       
|   \-- test                        mutilServer的测试文件夹    
|-- Singleserver                    单线程TcpServer
|   |-- Acceptor.{h,cc}             
|   \-- TcpServer.{h,cc}            
|-- client
|   |-- Connector.{h,cc}
|   \-- TcpClient.{h,cc}
|-- test                            整体的测试文件夹
|-- sample                          使用例程
|-- document                        细节说明文档
|-- BlockingQueue.{h}               
|-- Buffer.{h,cc}                   封装的缓冲区
|-- Channel.{h,cc}                  
|-- Condition.{h}
|-- CountDownLatch.{h}
|-- Epoller.{h,cc}
|-- EventLoop.{h,cc}
|-- File.{h}
|-- InetAddress.{h}
|-- Mutex.{h}
|-- Socket.{h,cc}
|-- TcpConnection.{h,cc}            表示连接的对象
|-- Thread.{h.cc}
|-- ThreadPool.{h,cc}
|-- TimeLoop.{h,cc}                 定时器设施
|-- Timer.{h}                       
|-- TimeQueue.{h}
|-- TimeStamp.{h}           
|-- Typedef.{h}                     整体的头文件包含

详细的细节说明在document中~
