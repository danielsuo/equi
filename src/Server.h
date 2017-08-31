#pragma once
#include "Common.h"
namespace Turkey {
class Server {
public:
  Server();
  ~Server();

  Server(const Server &) = delete;
  Server &operator=(const Server &) = delete;

  void poll();

private:
};
}
