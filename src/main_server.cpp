#include "Server.h"
#include <atomic>
#include <chrono>
#include <glog/logging.h>
#include <iostream>
#include <queue>
#include <signal.h>
#include <thread>
#include <unistd.h>

using namespace Turkey;

std::atomic<bool> quit(false); // signal flag
std::queue<pid_t> to_register; // list of client PIDs to register

void signal_handler(int sig, siginfo_t* info, void* vp) {
  LOG(INFO) << "Signal from " << info->si_pid << " to " << getpid();
  if (info->si_signo == SIGQUIT) {
    quit.store(true);
  } else {
    LOG(INFO) << "Processing...";
    to_register.push(info->si_pid);
  }
}

int main(int argc, char* argv[]) {
  Server server;

  // Set up signal handler.
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_sigaction = signal_handler;
  sigfillset(&sa.sa_mask);
  sigaction(SIGINT, &sa, NULL);

  using namespace std::chrono_literals;
  std::cout << "Starting server" << std::endl;
  while (true) {
    // do real work here...
    std::this_thread::sleep_for(1s);
    server.poll();
    if (quit.load())
      break; // exit normally after SIGINT

    // TODO: lots of bad stuff can happen between the time we pop and the
    // time we process. For now, we ignore. Robustness is not our concern.
    while (to_register.size() > 0) {
      pid_t pid = to_register.front();
      LOG(INFO) << pid;
      server.process_client(pid);
      to_register.pop();
    }
  }

  return 0;
}
