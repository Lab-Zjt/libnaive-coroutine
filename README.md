# Naive Coroutine

试着写下协程库。只在linux下适用。G++ 7.3.0,ubuntu 18.04下编译通过。

## 说明

目前功能非常不完善，hook系统调用还没完成，施工中。

尝试了几次决定放弃基于信号和定时器的自动yiled机制，因为信号实在是不稳定，随着内容的增多有可能出现一些奇奇怪怪的bug。

切换上下文是通过ucontext库进行。

调用是`go(func,args...)`的方式。不过不同于go，所有协程任务全部结束时程序才会结束。

可以通过`go<SIZE>(func,args...)`为协程分配大小为SIZE的栈空间。

main函数已经被#define掉了，程序入口改为Coro_Main(argc,argv)宏。

## 注意事项

目前hook的系统调用：
read,write,connect,accept,recv,send.

虽然不是系统调用但是会阻塞线程的：
sleep,usleep,nanosleep.

sleep类函数均已hook成令当前协程睡眠一段时间，期间可以执行其他协程。


SSL_read()可能不能正常工作，例如`httpsGET("www.baidu.com")`的时候，其每次读取的大小远小于16384字节,而httpsGET其他大部分网站时均是每次读取16384字节直至无内容可读，故目前判断`tlsRead()`结束的条件为读取字节数小于16384。由于`SSL_read()`是阻塞的（目未找到能在当前设计下能用的使`SSL_read()`不阻塞的解决方法），故只能通过`SSL_read()<16384`的方法来判断结束。