/*----------------------------------------------------------------------------*/
/* Copyright (c) FIRST 2016-2017. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "HAL/handles/HandlesInternal.h"

#include <algorithm>

#include "HAL/cpp/priority_mutex.h"
#include "llvm/SmallVector.h"

namespace hal {
static llvm::SmallVector<HandleBase*, 32> globalHandles;
static priority_mutex globalHandleMutex;

HandleBase::HandleBase() {
  std::lock_guard<priority_mutex> lock(globalHandleMutex);
  auto index = std::find(globalHandles.begin(), globalHandles.end(), this);
  if (index == globalHandles.end()) {
    globalHandles.push_back(this);
  } else {
    *index = this;
  }
}

HandleBase::~HandleBase() {
  std::lock_guard<priority_mutex> lock(globalHandleMutex);
  auto index = std::find(globalHandles.begin(), globalHandles.end(), this);
  if (index != globalHandles.end()) {
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
  std::unique_lock<priority_mutex> lock(globalHandleMutex);
  for (auto&& i : globalHandles) {
    if (i != nullptr) {
      lock.unlock();
      i->ResetHandles();
      lock.lock();
    }
  }
}

HAL_PortHandle createPortHandle(uint8_t channel, uint8_t module) {
  // set last 8 bits, then shift to first 8 bits
  HAL_PortHandle handle = static_cast<HAL_PortHandle>(HAL_HandleEnum::Port);
  handle = handle << 24;
  // shift module and add to 3rd set of 8 bits
  int32_t temp = module;
  temp = (temp << 8) & 0xff00;
  handle += temp;
  // add channel to last 8 bits
  handle += channel;
  return handle;
}

HAL_PortHandle createPortHandleForSPI(uint8_t channel) {
  // set last 8 bits, then shift to first 8 bits
  HAL_PortHandle handle = static_cast<HAL_PortHandle>(HAL_HandleEnum::Port);
  handle = handle << 16;
  // set second set up bits to 1
  int32_t temp = 1;
  temp = (temp << 8) & 0xff00;
  handle += temp;
  // shift to last set of bits
  handle = handle << 8;
  // add channel to last 8 bits
  handle += channel;
  return handle;
}

HAL_Handle createHandle(int16_t index, HAL_HandleEnum handleType,
                        int16_t version) {
  if (index < 0) return HAL_kInvalidHandle;
  uint8_t hType = static_cast<uint8_t>(handleType);
  if (hType == 0 || hType > 127) return HAL_kInvalidHandle;
  // set last 8 bits, then shift to first 8 bits
  HAL_Handle handle = hType;
  handle = handle << 8;
  handle += static_cast<uint8_t>(version);
  handle = handle << 16;
  // add index to set last 16 bits
  handle += index;
  return handle;
}
}  // namespace hal
