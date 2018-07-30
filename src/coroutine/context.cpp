#include "context.h"

void Context::ucontext_helper(int fn_ptr1, int fn_ptr2, int this_ptr1, int this_ptr2) {
  auto pf = reinterpret_cast<std::function<void()> *>(makeptr(fn_ptr1, fn_ptr2));
  auto ctx = reinterpret_cast<Context *>(makeptr(this_ptr1, this_ptr2));
  (*pf)();
  ctx->set_status(Context::Status::finished);
}
Context::Context(std::function<void()> &&fn, size_t stack_size) : _fn(std::move(fn)) {
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
}
Context::~Context() {
  if (_ucp != nullptr) {
    delete[]char_p(_ucp->uc_stack.ss_sp);
  }
  delete _ucp;
}
void Context::resume(Context *from) {
  if (from == nullptr) {
    setcontext(_ucp);
  } else {
    swapcontext(from->_ucp, _ucp);
  }
}
