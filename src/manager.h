#ifndef NAIVECORO_MANAGER_H
#define NAIVECORO_MANAGER_H

#include <list>
#include <vector>

class Context;

class Timer;


class ContextManager {
private:
  std::list<Context *> CtxQueue;
  Context *manager;
  Context *cur;
  std::vector<Context *> waiting_for_erase;
  Timer *timer;
public:
  ContextManager();
  void push(Context *context) {CtxQueue.push_back(context);}
  void start();
  void erase(Context *ctx);
  Context *main() {return manager;}
  static void alarm(int signo);
  static void manage();
};

extern ContextManager *mng;

#endif
