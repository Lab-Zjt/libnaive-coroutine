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
#define divptr(p, p1, p2) (p2) = (int)((p)&0x00000000ffffffff),(p1) = (int)(((p)>>32)&0x00000000ffffffff)
#define makeptr(p1, p2) ((((std::uint64_t)(p1))<<32)|(((std::uint64_t)(p2))&0x00000000ffffffff))

const int normal_stack_size = 32 * 1024;

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
  Context(std::function<void()> &&fn, size_t stack_size = normal_stack_size);
  void resume(Context *from);
  inline Status status() {return _status;}
  inline void set_status(Status st) {_status = st;}
};

template<typename Func, typename ...ARGS>
void go(Func &&func, ARGS &&...args) {
  _scheduler->push_func(std::bind(func, args...));
};

#endif
