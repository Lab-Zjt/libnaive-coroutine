#include "context.h"

std::map<std::function<void()> *, Context *> fp_to_ctx;
void Context::ucontext_helper(std::function<void()> *pf) {
  (*pf)();
  Scheduler::getCurrentManager()->erase(fp_to_ctx[pf]);
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