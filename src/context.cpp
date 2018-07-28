#include "context.h"

//std::map<std::function<void()> *, Context *> fp_to_ctx;
const int max_open_files = 1024;
bool is_regular[1024] = {true, true, true};
void Context::ucontext_helper(int fn_ptr1, int fn_ptr2, int this_ptr1, int this_ptr2) {
  auto pf = reinterpret_cast<std::function<void()> *>(makeptr(fn_ptr1, fn_ptr2));
  auto ctx = reinterpret_cast<Context *>(makeptr(this_ptr1, this_ptr2));
  (*pf)();
  ctx->set_status(Context::Status::finished);
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
