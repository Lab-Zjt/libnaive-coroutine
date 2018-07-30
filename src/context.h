#ifndef NAIVECORO_CONTEXT_H
#define NAIVECORO_CONTEXT_H

#include "scheduler.h"
#include "manager.h"
#include <ucontext.h>
#include <functional>
#include <map>

struct ucontext_t;

class Context;

extern bool is_regular[1024];
#define divptr(p, p1, p2) p2 = (int)((p)&0x00000000ffffffff);\
p1 = (int)(((p)>>32)&0x00000000ffffffff)
#define makeptr(p1, p2) ((((std::uint64_t)(p1))<<32)|(((std::uint64_t)(p2))&0x00000000ffffffff))

const int kStackSize = 32768;

class Context {
public:
  typedef char *char_p;
  typedef Context *pointer;
  typedef Context &reference;
  typedef void(*helper)();
  enum class Status {
    running, ready, IOblocking, finished, syscalling
  };
private:
  ucontext_t *_ucp = nullptr;
  std::function<void()> _fn;
  int _fn_ptr[2];
  int _this_ptr[2];
  Status _status = Status::ready;
  static void ucontext_helper(int fn_ptr1, int fn_ptr2, int this_ptr1, int this_ptr2);
public:
  Context() = default;
  ~Context();
  //Create manager context.
  template<typename Func, typename ...ARGS>
  Context(int stack_size, Func &&func, ARGS &&...args) : _fn(std::bind(func, args...)) {
    divptr(reinterpret_cast<std::intptr_t>(&_fn), _fn_ptr[0], _fn_ptr[1]);
    divptr(reinterpret_cast<std::intptr_t>(this), _this_ptr[0], _this_ptr[1]);
    _ucp = new ucontext_t;
    getcontext(_ucp);
    _ucp->uc_stack.ss_sp = new char[stack_size];
    _ucp->uc_stack.ss_size = stack_size;
    auto mng = Scheduler::get_current_manager();
    if (mng == nullptr) {
      _ucp->uc_link = nullptr;
    } else {
      _ucp->uc_link = mng->manager()->_ucp;
    }
    makecontext(_ucp, helper(ucontext_helper), 4, _fn_ptr[0], _fn_ptr[1], _this_ptr[0], _this_ptr[1]);
  };
  template<typename Func, typename ...ARGS>
  explicit Context(Func &&func, ARGS &&...args) :
    _fn(std::bind(func, args...)) {
    divptr(reinterpret_cast<std::intptr_t>(&_fn), _fn_ptr[0], _fn_ptr[1]);
    divptr(reinterpret_cast<std::intptr_t>(this), _this_ptr[0], _this_ptr[1]);
    _ucp = new ucontext_t;
    getcontext(_ucp);
    _ucp->uc_stack.ss_sp = new char[kStackSize];
    _ucp->uc_stack.ss_size = kStackSize;
    auto mng = Scheduler::get_current_manager();
    if (mng == nullptr) {
      _ucp->uc_link = nullptr;
    } else {
      _ucp->uc_link = mng->manager()->_ucp;
    }
    makecontext(_ucp, helper(ucontext_helper), 4, _fn_ptr[0], _fn_ptr[1], _this_ptr[0], _this_ptr[1]);
  };
  void resume(Context *from);
  inline Status status() {return _status;}
  inline void set_status(Status st) {_status = st;}
};

template<typename Func, typename ...ARGS>
void go(Func &&func, ARGS &&...args) {
  _scheduler->push_func(std::bind(func, args...));
};

#endif
