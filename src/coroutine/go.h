#include "context.h"
#include "scheduler.h"
#include "../hook/hook.h"
#include "../net/tls_connection.h"
#include <csignal>


#define co_yiled do{\
auto mng = Scheduler::get_current_manager();\
auto cur = mng->current();\
cur->set_status(Context::Status::ready);\
mng->manager()->resume(cur);\
}while(false)

int coro_main(int argc, char *argv[]);

#define Main() \
int main(int argc,char* argv[]){\
  hook_all();\
  srlib::net::SSLInit();\
  _scheduler = new Scheduler;\
  _scheduler->start(coro_main,argc,argv);\
  return 0;\
}

#define Coro_Main(argc,argv) Main();\
int coro_main(int argc, char *argv[])
