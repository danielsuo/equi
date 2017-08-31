#include "Server.h"
#include <glog/logging.h>
#include <glog/stl_logging.h>
#include <iostream>
#include <signal.h>
#include <thread>

using namespace boost::interprocess;

static constexpr int kDefaultRec            = 32;
static constexpr int kSharedMemorySizeBytes = 65536;
static constexpr int kMaxTimeSeriesSize     = 1024;

namespace Turkey {
namespace {
int kNum = 0;
int someAlgorithm(int runnableThreads) {
  kNum++;
  if (kNum > 20) {
    kNum = 0;
  }
  if (kNum > 10) {
    return 1;
  } else {
    return 32;
  }
}
} // anonymous
Server::Server() {
  pid_ = ::getpid();

  // Delete the shared memory object if one already exists
  named_mutex::remove("TurkeyMutex");
  shared_memory_object::remove("TurkeySharedMemory");

  managed_shared_memory segment(create_only, "TurkeySharedMemory",
                                kSharedMemorySizeBytes);
  ShmemAllocator allocator(segment.get_segment_manager());
  named_mutex mutex(create_only, "TurkeyMutex");

  {
    scoped_lock<named_mutex> lock(mutex);
    segment.construct<pid_t>(kServerPIDString)(pid_);
    std::cout << "PID: " << pid_ << std::endl;
    segment.construct<size_t>("DefaultRec")(kDefaultRec);
    segment.construct<RecMap>("RecMap")(std::less<boost::uuids::uuid>(),
                                        allocator);
  }
}

pid_t Server::getpid() { return pid_; }

void Server::poll() {
  const auto runnableThreads = std::thread::hardware_concurrency();
  LOG(INFO) << "r: " << runnableThreads;

  const auto newRec = someAlgorithm(runnableThreads);
  managed_shared_memory segment(open_only, "TurkeySharedMemory");
  named_mutex mutex(open_only, "TurkeyMutex");
  {
    scoped_lock<named_mutex> lock(mutex);
    // Update default recommendation
    auto defaultRec = segment.find<size_t>("DefaultRec").first;
    *defaultRec     = newRec;
    LOG(INFO) << "rec: " << newRec;
  }
}

void Server::process_client(pid_t pid) {
  LOG(INFO) << "Processing client!";
  for (int i = 0; i < clients_.size(); i++) {
    if (clients_[i].pid == pid) {
      // TODO: Remove client
      remove_client(pid);
      return;
    }
  }

  register_client(pid);
}

void Server::register_client(pid_t pid) {
  // TODO: Register client
  // TODO: This is by no means atomic
  ClientState client;
  client.pid        = pid;
  client.registered = true;
  LOG(INFO) << "Registering client " << pid << " uuid: " << client.id;
  clients_.push_back(client);

  try {
    managed_shared_memory segment(open_only, "TurkeySharedMemory");
    named_mutex mutex(open_only, "TurkeyMutex");
    scoped_lock<named_mutex> lock(mutex);

    // TODO: Should probably have better naming than process id...
    segment.construct<ClientState>(std::to_string(pid).c_str())(client);

    if (kill(pid, SIGINT) != 0) {
      LOG(INFO) << "Failed to signal client";
    }
  } catch (const std::exception& ex) {
    LOG(INFO) << "Interprocess exception: " << ex.what();
    // TODO any remediation?
  }
}

void Server::remove_client(pid_t pid) {
  LOG(INFO) << "Removing client " << pid;

  shared_memory_object::remove(std::to_string(pid).c_str());
}

Server::~Server() {
  LOG(INFO) << "Quitting server";
  named_mutex::remove("TurkeyMutex");
  shared_memory_object::remove("TurkeySharedMemory");
}
}
