#include "Server.h"
#include <atomic>
#include <chrono>
#include <iostream>
#include <signal.h>
#include <thread>
#include <unistd.h>

using namespace Turkey;

std::atomic<bool> quit(false); // signal flag

void got_signal(int) { quit.store(true); }

int main(int argc, char* argv[]) {
  // Set up signal handler.
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_handler = got_signal;
  sigfillset(&sa.sa_mask);
  sigaction(SIGINT, &sa, NULL);

  using namespace std::chrono_literals;
  Server server;
  std::cout << "Starting server" << std::endl;
  while (true) {
    // do real work here...
    std::this_thread::sleep_for(1s);
    server.poll();
    if (quit.load())
      break; // exit normally after SIGINT
  }

  return 0;
}
