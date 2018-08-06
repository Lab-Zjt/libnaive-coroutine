# Naive Coroutine

试着写下协程库。只在linux下适用。G++ 7.3.0,ubuntu 18.04下编译通过。

## 说明

目前功能非常不完善，hook系统调用还没完成，施工中。

基于信号机制与定时器实现调度，由于有系统调度可能会进入“不可抢占”状态，如果此时强行通过信号处理程序切换上下文会导致错误，所以这个库开不开发得下去还是个问题。。。目前计划hook系统调用，在进入系统调用前设置当前context的状态为syscalling，避免调度器调度，似乎能正常运行，但是别的系统调用能不能正常运行就不知道了。。。所以随时可能弃坑。

切换上下文是通过ucontext库进行。

调用是`go(func,args...)`的方式。不过不同于go，所有协程任务全部结束时程序才会结束。

main函数已经被#define掉了，程序入口改为Coro_Main(argc,argv)宏。

## 注意事项

目前hook的系统调用：
read,write,open,close,socket,connect,bind,listen,accept,malloc,free,recv,send.

SSL_read()可能不能正常工作，例如`httpsGET("www.baidu.com")`的时候，其每次读取的大小远小于16384字节,而httpsGET其他大部分网站时均是每次读取16384字节直至无内容可读，故目前判断`tlsRead()`结束的条件为读取字节数小于16384。由于`SSL_read()`是阻塞的（目未找到能在当前设计下能用的使`SSL_read()`不阻塞的解决方法），故只能通过`SSL_read()<16384`的方法来判断结束。