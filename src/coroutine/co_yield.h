#ifndef CORONET_CO_YILED_H
#define CORONET_CO_YILED_H

#include "manager.h"
#include "context.h"
#define co_yield do{\
auto mng = Scheduler::get_current_manager();\
auto cur = mng->current();\
cur->set_status(Context::Status::ready);\
mng->manager()->resume(cur);\
}while(false)
#endif
