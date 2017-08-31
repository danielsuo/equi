#include "Client.h"
#include <glog/logging.h>
#include <iostream>
#include <signal.h>
#include <thread>

using namespace boost::interprocess;
using namespace boost::uuids;

pid_t serverpid;
bool registered = false;

void signal_handler(int sig, siginfo_t* info, void* vp) {
  LOG(INFO) << "Signal from " << info->si_pid << " to " << getpid();
  if (info->si_pid == serverpid) {
    LOG(INFO) << "Registered!";
    registered = true;
  }
}

namespace Turkey {
Client::Client(size_t defaultRec) {
  RecInfo rec;
  rec.rec = defaultRec;
  rec_    = rec;

  state_.id  = boost::uuids::random_generator()();
  state_.pid = getpid();

  // Set up signal handler.
  struct sigaction sa;
  memset(&sa, 0, sizeof(sa));
  sa.sa_sigaction = signal_handler;
  sigfillset(&sa.sa_mask);
  sigaction(SIGINT, &sa, NULL);

  registerWithServer();
  pollServer();
}

Client::~Client() {
  LOG(INFO) << "DECONSTRUCTING CLIENT!!!!!";
  // TODO: This should really use a different signal than for registration
  if (kill(serverpid_, SIGINT) != 0) {
    LOG(INFO) << "Failed to signal server.";
  }
}

void Client::registerWithServer() {
  try {
    managed_shared_memory segment(open_only, "TurkeySharedMemory");
    named_mutex mutex(open_only, "TurkeyMutex");

    // TODO: We should make this bit of shared memory read only
    // scoped_lock<named_mutex> lock(mutex);

    // Find server PID
    serverpid = serverpid_ = *segment.find<pid_t>(kServerPIDString).first;
    LOG(INFO) << "Found server with PID " << serverpid_;
    LOG(INFO) << "Client PID is " << state_.pid;

    // Ping server
    if (kill(serverpid_, SIGINT) != 0) {
      LOG(INFO) << "Failed to signal server.";
    }

    // Get default recommendation to use as starting value
    const auto defaultRec = segment.find<size_t>("DefaultRec").first;
    RecInfo rec;
    rec.rec = *defaultRec;
    rec_    = rec;

    // Register client in the vector
    auto recMap = segment.find<RecMap>("RecMap").first;
    recMap->insert(std::pair<const uuid, RecInfo>(state_.id, rec_));

    using namespace std::chrono_literals;
    while (true) {
      std::this_thread::sleep_for(0.5s);
      if (registered) {
        break;
      }
    }

    state_.registered = true;
    LOG(INFO) << "Client registered. UUID: " << state_.id
              << ". Rec: " << rec_.rec;
  } catch (const std::exception& ex) {
    LOG(INFO) << "Interprocess exception: " << ex.what();
    // TODO any remediation?
  }
}

size_t Client::pollServer() {
  if (!state_.registered) {
    registerWithServer();
  }
  try {
    managed_shared_memory segment(open_only, "TurkeySharedMemory");
    named_mutex mutex(open_only, "TurkeyMutex");
    scoped_lock<named_mutex> lock(mutex);

    auto recMap = segment.find<RecMap>("RecMap").first;

    if (state_.registered) {
      // TODO take the default
      const auto defaultRec = segment.find<size_t>("DefaultRec").first;
      RecInfo rec;
      rec.rec = *defaultRec;
      rec_    = rec;
    }
  } catch (const std::exception& ex) {
    LOG(INFO) << "Interprocess exception: " << ex.what();
    // TODO any remediation?
  }
  return rec_.rec;
}
}
