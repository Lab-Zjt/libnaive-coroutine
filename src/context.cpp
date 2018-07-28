#include "context.h"

std::map<std::function<void()> *, Context *> fp_to_ctx;
void Context::ucontext_helper(int fn_ptr1, int fn_ptr2, int this_ptr1, int this_ptr2) {
  auto pf = reinterpret_cast<std::function<void()> *>(makeptr(fn_ptr1, fn_ptr2));
  auto ctx = reinterpret_cast<Context *>(makeptr(this_ptr1, this_ptr2));
  (*pf)();
  ctx->status = Context::Status::finished;
}
Context::~Context() {
  if (ucp != nullptr) {
    delete[]char_p(ucp->uc_stack.ss_sp);
  }
  delete ucp;
}
void Context::resume(Context *from) {
  if (from == nullptr) {
    setcontext(ucp);
  } else {
    swapcontext(from->ucp, ucp);
  }
}
void Context::setlink(ucontext_t *uc_link) {
  ucp->uc_link = uc_link;
}