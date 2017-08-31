#pragma once
#include "Common.h"
#include <folly/Optional.h>

namespace Turkey {
class Client {
public:
  explicit Client(size_t defaultRec);

  Client(const Client&) = delete;
  Client& operator=(const Client&) = delete;

  size_t pollServer();

private:
  void registerWithServer();
  boost::uuids::uuid id_;
  RecInfo rec_;
  bool registered_ = false;
};
}
