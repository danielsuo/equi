#pragma once
#include "ClientState.h"
#include <queue>
namespace Turkey {
class Server {
public:
  Server();
  ~Server();

  Server(const Server&) = delete;
  Server& operator=(const Server&) = delete;

  pid_t getpid();
  void poll();
  void process_client(pid_t pid);

private:
  std::vector<ClientState> clients_;
  pid_t pid_;

  // TODO: For now, make these private because we're using a single signal
  // type to register / remove
  void register_client(pid_t pid);
  void remove_client(pid_t pid);
};
}
