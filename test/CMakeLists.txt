CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(server)
PROJECT(client)

SET(CMAKE_C_COMPILER g++)
INCLUDE_DIRECTORIES(/usr/include/pthread)

SET(SERVER TcpServerTest.cc ../TcpServer.cc ../TcpConnection.cc ../Accepter.cc ../EPoller.cc ../Event.cc ../EventDemultiplexer.cc ../EventLoop.cc ../EventMap.cc ../Poller.cc ../Selecter.cc ../SignalEvent.cc ../SocketEvent.cc ../TaskPool.cc ../TcpConnection.cc ../TcpConnectionMap.cc ../ThreadPool.cc ../TimerEvent.cc ../base/Alloc.cc ../base/Buffer.cc ../base/Condition.cc ../base/Global.cc ../base/InetAddress.cc ../base/Mutex.cc ../base/Socket.cc ../base/SpinLock.cc ../base/Thread.cc ../base/Timer.cc)
SET(CLIENT TcpClientTest.cc ../TcpConnection.cc ../base/Alloc.cc ../base/Buffer.cc ../base/Socket.cc ../base/Alloc.cc ../base/Mutex.cc ../base/Global.cc ../base/InetAddress.cc)

ADD_DEFINITIONS("-g -Wall -o -lpthread -std=c++11")

ADD_EXECUTABLE(server ${SERVER})
ADD_EXECUTABLE(client ${CLIENT})


TARGET_LINK_LIBRARIES(server pthread)
TARGET_LINK_LIBRARIES(client pthread)
