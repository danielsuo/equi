#pragma once
#include <boost/interprocess/allocators/allocator.hpp>
#include <boost/interprocess/containers/map.hpp>
#include <boost/interprocess/containers/vector.hpp>
#include <boost/interprocess/managed_shared_memory.hpp>
#include <boost/interprocess/sync/named_mutex.hpp>
#include <boost/uuid/uuid.hpp>
#include <boost/uuid/uuid_generators.hpp>
#include <boost/uuid/uuid_io.hpp>
#include <utility>

struct RecInfo {
  size_t rec;
  size_t STUB_appStats;
};

typedef boost::interprocess::allocator<
    std::pair<const boost::uuids::uuid, RecInfo>,
    boost::interprocess::managed_shared_memory::segment_manager>
    ShmemAllocator;
typedef boost::interprocess::map<boost::uuids::uuid, RecInfo,
                                 std::less<boost::uuids::uuid>, ShmemAllocator>
    RecMap;
