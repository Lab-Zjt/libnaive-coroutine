# naive coroutine

试着写下协程库。只在linux下适用。G++ 7.3.0,ubuntu 18.04下编译通过。

## 说明

目前功能非常不完善，hook系统调用还没完成，施工中。

基于信号机制与定时器实现调度，由于有系统调度可能会进入“不可抢占”状态，如果此时强行通过信号处理程序切换上下文会导致错误，所以这个库开不开发得下去还是个问题。。。目前hook了printf，在协程调用printf前将其设置为syscalling，避免调度器调度，似乎能正常运行，但是别的系统调用能不能正常运行就不知道了。。。所以随时可能弃坑。

切换上下文是通过ucontext库进行。

调用是`go(func,args...)`的方式。不过不同于go，所有协程任务全部结束时程序才会结束。

main函数已经被#define掉了，程序入口改为Coro_Main(argc,argv)宏。

## 注意事项

目前hook的系统调用：
read,write,open,close,socket,connect,bind,listen,accept,printf,malloc,free.