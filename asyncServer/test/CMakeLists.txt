CMAKE_MINIMUM_REQUIRED(VERSION 2.8)

PROJECT(server)
PROJECT(client)


SET(CMAKE_C_COMPILER g++)
INCLUDE_DIRECTORIES(/usr/include/pthread)

SET(SERVER AsyncTcpServerTest.cc ../AsyncTcpServer.cc ../AsyncAcceptor.cc ../AsyncEventLoop.cc ../../Buffer.cc ../../Channel.cc ../../Epoller.cc ../../File.cc ../../Socket.cc ../../TcpConnection.cc ../../Thread.cc ../../ThreadPool.cc)
SET(CLIENT ClientTest.cc  ../../Buffer.cc ../../Channel.cc ../../Epoller.cc ../../File.cc ../../Socket.cc ../../TcpConnection.cc ../../Thread.cc ../../ThreadPool.cc)

ADD_DEFINITIONS("-g -Wall -o -lpthread -std=c++11")

ADD_EXECUTABLE(server ${SERVER})
ADD_EXECUTABLE(client ${CLIENT})

TARGET_LINK_LIBRARIES(server pthread)
TARGET_LINK_LIBRARIES(client pthread)
