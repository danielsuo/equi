#pragma once
#include "ClientState.h"

namespace Turkey {
class Client {
public:
  explicit Client(size_t defaultRec);
  ~Client();

  Client(const Client&) = delete;
  Client& operator=(const Client&) = delete;

  size_t pollServer();

private:
  ClientState state_;
  RecInfo rec_;
  pid_t serverpid_;

  void registerWithServer();
};
}
