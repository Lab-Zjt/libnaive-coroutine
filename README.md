# Naive Coroutine

试着写下协程库。只在linux下适用。G++ 7.3.0,ubuntu 18.04下编译通过。

## 说明

目前功能非常不完善，hook系统调用还没完成，施工中。

尝试了几次决定放弃基于信号和定时器的自动yiled机制，因为信号实在是不稳定，随着内容的增多有可能出现一些奇奇怪怪的bug。

切换上下文是通过ucontext库进行。

调用是`go(func,args...)`的方式。不过不同于go，所有协程任务全部结束时程序才会结束。

可以通过`go<SIZE>(func,args...)`为协程分配大小为SIZE的栈空间。

main函数已经被#define掉了，程序入口改为Coro_Main(argc,argv)宏。

实现了Channel。

实现了Mutex，ConditionVariable，Barrier，目前来看似乎能正常工作。

### literal

B,KB,MB,GB

分别对应2^0,2^10,2^20,2^30

### go

Usage:

```C++
go<4MB>([](){
  srlib::println("hello,world");
});
```

模板参数为栈大小，默认为4KB（可以通过corodef.h修改）。第一个参数为新协程的入口函数，之后的参数为入口函数的参数。

### channel

Usage:

```C++
Channel<int> channel(1);// Channel<T>(buffer_size);
go([](){
  for(int i = 0; i < 100 ; ++i){
    channel << i;
    srlib::println("Send",i);
  }
}); 
go([](){
  for(int i = 0; i < 100 ; ++i){
    //auto t = channel();
    int t;
    t << channel;
    srlib::println("Recv",t);
  }
});
```

如果当前channel中没有数据，从当前channel中读取数据将会导致当前协程阻塞。

如果当前channel的缓冲区已满，向当前channel写入数据将会导致当前协程阻塞。

## 注意事项

目前hook的系统调用：
`read`,`write`,`connect`,`accept`,`recv`,`send`.

虽然不是系统调用但是会阻塞线程的：
`sleep`,`usleep`,`nanosleep`.

当前协程阻塞时，程序将会继续执行其他协程。

`getaddrinfo()`函数会通过`alloca()`在栈上分配缓冲区，所以对会执行`getaddrinfo()`的协程请至少分配约16KB的缓冲区。