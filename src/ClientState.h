#pragma once
#include "Common.h"

namespace Turkey {
class ClientState {
public:
  boost::uuids::uuid id = boost::uuids::random_generator()();
  pid_t pid;
  bool registered = false;
};
}
