// Copyright (c) FIRST and other WPILib contributors.
// Open Source Software; you can modify and/or share it under the terms of
// the WPILib BSD license file in the root directory of this project.

#include "frc/Resource.h"

#include <limits>
#include <memory>
#include <string>
#include <vector>

#include <wpi/deprecated.h>

#include "frc/Errors.h"

WPI_IGNORE_DEPRECATED
using namespace frc;

wpi::mutex Resource::m_createMutex;

void Resource::CreateResourceObject(std::unique_ptr<Resource>& r,
                                    uint32_t elements) {
  std::scoped_lock lock(m_createMutex);
  if (!r) {
    r = std::make_unique<Resource>(elements);
  }
}
WPI_UNIGNORE_DEPRECATED

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
  throw FRC_MakeError(err::NoAvailableResources, "{}", resourceDesc);
}

uint32_t Resource::Allocate(uint32_t index, const std::string& resourceDesc) {
  std::scoped_lock lock(m_allocateMutex);
  if (index >= m_isAllocated.size()) {
    throw FRC_MakeError(err::ChannelIndexOutOfRange, "{}", resourceDesc);
  }
  if (m_isAllocated[index]) {
    throw FRC_MakeError(err::ResourceAlreadyAllocated, "{}", resourceDesc);
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
    throw FRC_MakeError(err::NotAllocated, "index {}", index);
  }
  if (!m_isAllocated[index]) {
    throw FRC_MakeError(err::NotAllocated, "index {}", index);
  }
  m_isAllocated[index] = false;
}
