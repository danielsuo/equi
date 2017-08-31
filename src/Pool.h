#pragma once
#include "Client.h"
#include <folly/experimental/FunctionScheduler.h>
#include <iostream>
#include <wangle/concurrent/CPUThreadPoolExecutor.h>

namespace Turkey {
class DynamicThreadPool {
public:
  explicit DynamicThreadPool(size_t defaultNumThreads);

  void start();
  size_t updatePoolSize();
  wangle::CPUThreadPoolExecutor& getPool() { return pool_; }

private:
  size_t currentNumThreads_;
  Client client_;
  wangle::CPUThreadPoolExecutor pool_;
  folly::FunctionScheduler fs_;
};
}
