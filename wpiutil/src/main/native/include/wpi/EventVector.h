#pragma once

#include "wpi/Synchronization.h"
#include "wpi/SmallVector.h"
#include "wpi/mutex.h"

namespace wpi {
struct EventVector {
  wpi::mutex mutex;
  wpi::SmallVector<WPI_EventHandle, 4> events;

  void add(WPI_EventHandle handle) {
    std::scoped_lock lock{mutex};
    events.emplace_back(handle);
  }
  void remove(WPI_EventHandle handle) {
    std::scoped_lock lock{mutex};
    auto it = std::find_if(
        events.begin(), events.end(),
        [=](const WPI_EventHandle fromHandle) { return fromHandle == handle; });
    if (it != events.end()) {
      events.erase(it);
    }
  }
  void wakeup() {
    std::scoped_lock lock{mutex};
    for (auto&& handle : events) {
      wpi::SetEvent(handle);
    }
  }
};
}
