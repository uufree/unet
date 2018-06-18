# unet
- 一个非阻塞多线程网络编程框架的简易实现,整体使用事件进行驱动。
- 本次是第三次重构，修改了事件处理的方式

## unet的大体架构：
 - 底层：
    - 描述：封装最底层的系统调用，大多数使用RAII手法管理资源
    - 规划：随着编程经验的增加，可以在保留接口不变的情况下修改内部资源
    - 构件组成：
        - Alloc:创建一个内存池，用以维护LogBuffer与UserbBuffer，在Log或者TcpConnection需要内存时，无需陷入系统调用
        - Buffer:维护UsrBuffer，在初始时，将4个UsrBuffer使用链表连接起来，使用readv与writev的方式来操作缓冲区，减少在Buffer需要扩张时的数据copy
        - Condition:封装条件变量，存在即初始化
        - InetAddress:封装IPV4地址与IPV6地址（暂时没用到）
        - RDMutex,Mutex,SpinLock:使用存在即初始化，离开即析构的方式封装三种类型的锁
        - Thread:线程对象，与Qt中的线程对象的使用基本类似。在细节上，使用cannel控制线程的生命周期，要求线程不可分离。
        - Timer:定时器事件的底层对象，使用Timer封装事件，启动时即加入定时器处理框架
        - ThreadPool:
 - 中间层：
    - 描述：根据框架需求，设计一些处理框架需求的对象，每种对象都有自己独立的作用，核心上：只处理数据，不保存数据
    - 规划：根据框架需求，逐渐增加
    - 构件组成：
        - EventDemultiplexer:I/O多路复用器，统一下述的三种类型的事件检测方式。维护四个统一的接口：add，erase，reset，poll
        - Selecter,EPoller,Poller:根据各个I/O多路复用的方式维护自己的数据
        - Event:将Signal，Socket，Timer三种事件使用union的方式统一起来，将Event交由TaskPool线程进行处理
        - SignalEvent:在unet中，我们将信号事件作为全局事件进行处理，使用pipe，将主线程阻塞在read上，信号发生时由主线程进行处理。可以处理系统信号和用户自定义信号
        - SocketEvent:将Listen Socket与Connection Socket统一起来
        - TimerEvent:全局唯一的定时器事件框架，使用timerfd与优先级队列通知定时器到期
        - TaskPool:底层使用ThreadPool维护一个任务处理队列，处理EventList中的Event对象
        - TcpConnection:描述一个Tcp连接对象，不维护状态，TcpConnection在获取Connet Fd时已经处理已连接状态
        - EventLoop:事件分离线程，将事件分离出来，交由TaskPool进行处理
 - 分离层：
    - 描述：分离中间层，对数据与操作数据的对象进行分离，使用STL管理数据，使数据的流动可控制。
    - 规划：为随后数据结构与算法层面的优化指明方向
    - 构件组成:
        - EventMap:保存全局的EventMap，只有一份，保存在TcpServer中
        - TcpConnectionMap:保存全局的TcpConnetion，只存在一份，保存在TcpServer中
 - 应用层：
    - 描述：使用上述构件集成出适应业务的模型，在应用层设计构件之间的关联回调
    - 规划：自由组合，自由组合，自由组合
    - 构件组成：
        - TcpServer:使用上述层次的构件维护一个TcpServer对象，在使用时，只需向TcpServer注册读写处理事件即可
        - Accepter:全局唯一的Accepter对象，处理Listen Socket事件，纳入事件处理框架

## 对于Log设计的思考：
 在多线程Log中，一条Log写入文件中需要两次加锁：多线程向后台Buffer中写Log时，加锁一次；使用Timer到期或者Buffer已满的情况下，将Buffer中的数据写入LogFile，加锁一次。虽然说临界代码段很短，基本可以不考虑锁带来的开销。但是，有没有只加锁一次的方式呢？我们在unet中尝试使用一些激进的方式取消锁带来的开销：为每个线程维护一个Buffer，这样减少了一次加锁，但是多线程的Buffer写入LogFile时会造成LogFile中事件不同步的问题。

## 本次重构反思：
### 正面：
 - 统一了事件源与事件处理的方式
 - 在信号事件的处理上加上了自己的思考，扩展信号处理的方式，使的主线程可以接收子线程中的自定义信号，实时的调整框架资源
 - 在Socket选项上维护了：SO-REUSEADDR,SO-RCVBUF,SO-SNDBUF,SO-RCVLOWAT,SO-SNDLOWAT,SO-LINGER,SO-KEEPALIVE,TCP-NODELAY,O-NONBLOCK,FD-CLOSEXEC选项
 - 在Signal中维护了：SIGHUP,SIGPIPE,SIGURG,SIGCHLD,SIGTERM,SIGINT,SIGKILL以及用户自定义的一些信号   

### 反面：
 - 事件处理方式存在缺陷：在成熟的网络编程框架中，one thread per loop是成熟的做法，libevent与muduo都是如此，将I/O事件固定在一个线程中。但是在unet的设计上，因为资源的统一管理，I/O事件发生之后封装成Event对象，在逻辑线程中一并进行处理，这样做适用于ET模式，在LT中会造成事件的频繁触发。因此为了维护三种事件检测方式的统一性，引入的reset方法：在每次检测到事件之后，为了防止事件的再次触发，从事件分发器中取消检测这个事件，从而造成了事假处理的低效。在下次重构时，将统一管理的资源分成n等分，每分资源交由一个线程单独管理，从而将I/O事件均匀的分布到所有线程中去，避免每次在不同的线程中处理I/O事件。这个问题属于设计上的失误。
 - 依旧没有引入成熟的异常处理机制，在C语言的errno与C++ try-catch-throw中无法抉择
