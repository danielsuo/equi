#include "Server.h"
#include <glog/logging.h>
#include <glog/stl_logging.h>
#include <iostream>
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
  // Delete the shared memory object if one already exists
  named_mutex::remove("TurkeyMutex");
  shared_memory_object::remove("TurkeySharedMemory");

  managed_shared_memory segment(create_only, "TurkeySharedMemory",
                                kSharedMemorySizeBytes);
  ShmemAllocator allocator(segment.get_segment_manager());
  named_mutex mutex(create_only, "TurkeyMutex");

  {
    scoped_lock<named_mutex> lock(mutex);
    segment.construct<size_t>("DefaultRec")(kDefaultRec);
    segment.construct<RecMap>("RecMap")(std::less<boost::uuids::uuid>(),
                                        allocator);
  }
}

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

Server::~Server() {
  LOG(INFO) << "Quitting server";
  named_mutex::remove("TurkeyMutex");
  shared_memory_object::remove("TurkeySharedMemory");
}
}
