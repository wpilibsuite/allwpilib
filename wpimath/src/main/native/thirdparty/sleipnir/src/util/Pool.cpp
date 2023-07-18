// Copyright (c) Sleipnir contributors

#include "sleipnir/util/Pool.hpp"

namespace sleipnir {

PoolResource& GlobalPoolResource() {
  thread_local PoolResource pool{16384};
  return pool;
}

}  // namespace sleipnir
