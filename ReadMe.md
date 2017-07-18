# unet
- 一个基于muduo源码风格，Reactor模式的现代C++网络库（非阻塞),事件驱动的业务模型。
- 提供单线程server，多线程server，多线程异步server，多线程异步client等版本，可以根据不同的需求满足开发需求。
## 重构ing~
 - 想法：组件分离，组件分层。
 - 目的：组件在确保线程安全的情况下可以通过不同的回调函数适应新的需求。
 - 思路：
    - 最底层的系统调用封装，留下约定的接口以及充分的接口描述，以待后续更改。
    - 组件与STL容器结合，分为在这一层考虑线程安全。仅留下最简单的输入输出接口。
    - 考虑应用需求，构建成型组件。用回调函数增加成型组件的灵活性。

### 修改完成的构件
```
    Condition
    Directory
    File
    InetAddress
    Mutex
    Socket
    TaskPool
    Thread
    ThreadPool
    Buffer
    fix TcpConnectionMap has a problem
```

