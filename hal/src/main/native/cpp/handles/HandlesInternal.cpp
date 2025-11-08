// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "wpi/hal/handles/HandlesInternal.h"

#include <algorithm>

#include "wpi/util/SmallVector.hpp"
#include "wpi/util/mutex.hpp"

namespace wpi::hal {
static wpi::util::SmallVector<HandleBase*, 32>* globalHandles = nullptr;
static wpi::util::mutex globalHandleMutex;
HandleBase::HandleBase() {
  static wpi::util::SmallVector<HandleBase*, 32> gH;
  std::scoped_lock lock(globalHandleMutex);
  if (!globalHandles) {
    globalHandles = &gH;
  }

  auto index = std::find(globalHandles->begin(), globalHandles->end(), this);
  if (index == globalHandles->end()) {
    globalHandles->push_back(this);
  } else {
    *index = this;
  }
}
HandleBase::~HandleBase() {
  std::scoped_lock lock(globalHandleMutex);
  auto index = std::find(globalHandles->begin(), globalHandles->end(), this);
  if (index != globalHandles->end()) {
    *index = nullptr;
  }
}
void HandleBase::ResetHandles() {
  m_version++;
  if (m_version > 255) {
    m_version = 0;
  }
}
void HandleBase::ResetGlobalHandles() {
  std::unique_lock lock(globalHandleMutex);
  for (auto&& i : *globalHandles) {
    if (i != nullptr) {
      lock.unlock();
      i->ResetHandles();
      lock.lock();
    }
  }
}
HAL_Handle createHandle(int16_t index, HAL_HandleEnum handleType,
                        int16_t version) {
  if (index < 0) {
    return HAL_kInvalidHandle;
  }
  uint8_t hType = static_cast<uint8_t>(handleType);
  if (hType == 0 || hType > 127) {
    return HAL_kInvalidHandle;
  }
  // set last 8 bits, then shift to first 8 bits
  HAL_Handle handle = hType;
  handle = handle << 8;
  handle += static_cast<uint8_t>(version);
  handle = handle << 16;
  // add index to set last 16 bits
  handle += index;
  return handle;
}
}  // namespace wpi::hal
