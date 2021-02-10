// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Resource.h"

#include "frc/ErrorBase.h"
#include "frc/WPIErrors.h"

using namespace frc;

wpi::mutex Resource::m_createMutex;

void Resource::CreateResourceObject(std::unique_ptr<Resource>& r,
                                    uint32_t elements) {
  std::scoped_lock lock(m_createMutex);
  if (!r) {
    r = std::make_unique<Resource>(elements);
  }
}

Resource::Resource(uint32_t elements) {
  m_isAllocated = std::vector<bool>(elements, false);
}

uint32_t Resource::Allocate(const std::string& resourceDesc) {
  std::scoped_lock lock(m_allocateMutex);
  for (uint32_t i = 0; i < m_isAllocated.size(); i++) {
    if (!m_isAllocated[i]) {
      m_isAllocated[i] = true;
      return i;
    }
  }
  wpi_setWPIErrorWithContext(NoAvailableResources, resourceDesc);
  return std::numeric_limits<uint32_t>::max();
}

uint32_t Resource::Allocate(uint32_t index, const std::string& resourceDesc) {
  std::scoped_lock lock(m_allocateMutex);
  if (index >= m_isAllocated.size()) {
    wpi_setWPIErrorWithContext(ChannelIndexOutOfRange, resourceDesc);
    return std::numeric_limits<uint32_t>::max();
  }
  if (m_isAllocated[index]) {
    wpi_setWPIErrorWithContext(ResourceAlreadyAllocated, resourceDesc);
    return std::numeric_limits<uint32_t>::max();
  }
  m_isAllocated[index] = true;
  return index;
}

void Resource::Free(uint32_t index) {
  std::unique_lock lock(m_allocateMutex);
  if (index == std::numeric_limits<uint32_t>::max()) {
    return;
  }
  if (index >= m_isAllocated.size()) {
    wpi_setWPIError(NotAllocated);
    return;
  }
  if (!m_isAllocated[index]) {
    wpi_setWPIError(NotAllocated);
    return;
  }
  m_isAllocated[index] = false;
}
