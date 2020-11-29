/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018-2020 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <cstring>
#include <functional>

#include <wpi/spinlock.h>

namespace hal {

class SimDisplayName {
 public:
  void Reset() {
    std::scoped_lock lock(m_mutex);
    displayName[0] = '\0';
  }

  const char* Get() {
    std::scoped_lock lock(m_mutex);
    return displayName;
  }

  void Set(const char* newDisplayName) {
    std::scoped_lock lock(m_mutex);

    std::strncpy(displayName, newDisplayName, sizeof(displayName) - 1);
    *(std::end(displayName) - 1) = '\0';
  }

 protected:
  mutable wpi::recursive_spinlock m_mutex;
  char displayName[256];
};

}  // namespace hal
