#pragma once

#include <memory>
#include "wpi/DenseMap.h"
#include "wpi/mutex.h"

namespace wpi {
template<typename KeyType, typename ValueType>
class RefCountedSingleton {
 public:
  wpi::mutex m_mutex;
  wpi::DenseMap<KeyType, std::weak_ptr<ValueType>> m_storeMap;
  template<typename Function>
  std::shared_ptr<ValueType> GetStorage(KeyType key, Function allocFunc) {
    std::scoped_lock(m_mutex);
    auto& dataStoreWeak = m_storeMap[key];
    auto dataStore = dataStoreWeak.lock();
    if (!dataStore) {
      dataStore = allocFunc();
      dataStoreWeak = dataStore;
    }
    return dataStore;
  }
};
}
