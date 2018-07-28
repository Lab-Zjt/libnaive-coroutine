#ifndef NAIVECORO_CONTEXT_H
#define NAIVECORO_CONTEXT_H

#include "scheduler.h"
#include "manager.h"
#include <ucontext.h>
#include <functional>
#include <map>

struct ucontext_t;

class Context;

extern std::map<std::function<void()> *, Context *> fp_to_ctx;
#define divptr(p, p1, p2) p2 = (int)((p)&0x00000000ffffffff);\
p1 = (int)(((p)>>32)&0x00000000ffffffff)
#define makeptr(p1, p2) ((((std::uint64_t)(p1))<<32)|(((std::uint64_t)(p2))&0x00000000ffffffff))

const int kStackSize = 32768;

class Context {
  typedef char *char_p;
private:
  ucontext_t *ucp = nullptr;
  std::function<void()> fn;
  int fn_ptr[2];
  int this_ptr[2];
  static void ucontext_helper(int fn_ptr1, int fn_ptr2, int this_ptr1, int this_ptr2);
public:
  enum class Status {
    running, ready, IOblocking, finished,syscalling
  };
  Status status = Status::ready;
  typedef void(*helper)();
  Context() = default;
  ~Context();
  template<typename Func, typename ...ARGS>
  explicit Context(Func &&func, ARGS &&...args) :
    fn(std::bind(func, args...)) {
    divptr(reinterpret_cast<std::intptr_t>(&fn), fn_ptr[0], fn_ptr[1]);
    divptr(reinterpret_cast<std::intptr_t>(this), this_ptr[0], this_ptr[1]);
    ucp = new ucontext_t;
    getcontext(ucp);
    ucp->uc_stack.ss_sp = new char[32768];
    ucp->uc_stack.ss_size = 32768;
    auto mng = Scheduler::getCurrentManager();
    if (mng == nullptr) {
      ucp->uc_link = nullptr;
    } else {
      printf("set uc_link at %p\n", mng->main()->ucp);
      ucp->uc_link = mng->main()->ucp;
    }
    fp_to_ctx[&fn] = this;
    makecontext(ucp, helper(ucontext_helper), 4, fn_ptr[0], fn_ptr[1], this_ptr[0], this_ptr[1]);
  };
  void resume(Context *from);
  void setlink(ucontext_t *uc_link);
  ucontext_t *ucontext() {return ucp;}
};

template<typename Func, typename ...ARGS>
void go(Func &&func, ARGS &&...args) {
  _scheduler->push_func(std::bind(func, args...));
};

#endif
