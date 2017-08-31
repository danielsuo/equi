#include "Client.h"
#include <glog/logging.h>
#include <iostream>

using namespace boost::interprocess;
using namespace boost::uuids;

namespace Turkey {
Client::Client(size_t defaultRec)
    :id_(boost::uuids::random_generator()()) {
  RecInfo rec;
  rec.rec = defaultRec;
  rec_ = rec;

  registerWithServer();
  pollServer();
}

void Client::registerWithServer() {
  try {
    managed_shared_memory segment(open_only, "TurkeySharedMemory");
    named_mutex mutex(open_only, "TurkeyMutex");
    scoped_lock<named_mutex> lock(mutex);

    // Get default recommendation to use as starting value
    const auto defaultRec = segment.find<size_t>("DefaultRec").first;
    RecInfo rec;
    rec.rec = *defaultRec;
    rec_ = rec;

    // Register client in the vector
    auto recMap = segment.find<RecMap>("RecMap").first;
    recMap->insert(std::pair<const uuid, RecInfo>(id_, rec_));

    registered_ = true;
    LOG(INFO) << "Client registered. UUID: " << id_ << ". Rec: " << rec_.rec;
  } catch (const std::exception& ex) {
    LOG(INFO) << "Interprocess exception: " << ex.what();
    // TODO any remediation?
  }
}

size_t Client::pollServer() {
  if (!registered_) {
    registerWithServer();
  }
  try {
    managed_shared_memory segment(open_only, "TurkeySharedMemory");
    named_mutex mutex(open_only, "TurkeyMutex");
    scoped_lock<named_mutex> lock(mutex);

    auto recMap = segment.find<RecMap>("RecMap").first;

    if (registered_) {
      // TODO take the default
      const auto defaultRec = segment.find<size_t>("DefaultRec").first;
      RecInfo rec;
      rec.rec = *defaultRec;
      rec_ = rec;
    }
  } catch (const std::exception& ex) {
    LOG(INFO) << "Interprocess exception: " << ex.what();
    // TODO any remediation?
  }
  return rec_.rec;
}
}
