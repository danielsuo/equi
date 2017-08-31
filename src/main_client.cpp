#include "Client.h"
#include <chrono>
#include <glog/logging.h>
#include <iostream>
#include <thread>
using namespace Turkey;

int main(int argc, char* argv[]) {
  using namespace std::chrono_literals;
  std::cout << "Starting client" << std::endl;
  Client client(1);
  for (int i = 0; i < 2; i++) {
    std::this_thread::sleep_for(1s);
    LOG(INFO) << client.pollServer();
  }
  return 0;
}
