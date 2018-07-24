#include "src/context.h"

extern ContextManager *mng;
int coro_main(int argc, char *argv[]);

#define Main() \
int main(int argc,char* argv[]){\
 mng = new ContextManager;\
 auto ctx = new Context(coro_main, argc, argv);\
 mng->push(ctx);\
 mng->start();\
 return 0;\
}

#define Coro_Main() Main();\
int coro_main(int argc, char *argv[])
