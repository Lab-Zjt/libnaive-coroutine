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

class Context {
  typedef char *char_p;
private:
  ucontext_t *ucp = nullptr;
  std::function<void()> fn;
  static void ucontext_helper(std::function<void()> *pf);
public:
  typedef void(*helper)();
  Context() = default;
  ~Context();
  template<typename Func, typename ...ARGS>
  explicit Context(Func &&func, ARGS &&...args) :
    fn(std::bind(func, args...)) {
    ucp = new ucontext_t;
    getcontext(ucp);
    ucp->uc_stack.ss_sp = new char[8192];
    ucp->uc_stack.ss_size = 8192;
    auto mng = Scheduler::getCurrentManager();
    if (mng == nullptr) {
      ucp->uc_link = nullptr;
    } else {
      printf("set uc_link at %p\n", mng->main()->ucp);
      ucp->uc_link = mng->main()->ucp;
    }
    fp_to_ctx[&fn] = this;
    makecontext(ucp, helper(ucontext_helper), 1, &fn);
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
