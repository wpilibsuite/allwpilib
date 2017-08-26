/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2017 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <memory>
#include <string>
#include <vector>

#include <HAL/cpp/priority_mutex.h>

#include "ErrorBase.h"

namespace frc {

/**
 * The Resource class is a convenient way to track allocated resources.
 * It tracks them as indicies in the range [0 .. elements - 1].
 * E.g. the library uses this to track hardware channel allocation.
 *
 * The Resource class does not allocate the hardware channels or other
 * resources; it just tracks which indices were marked in use by
 * Allocate and not yet freed by Free.
 */
class Resource : public ErrorBase {
 public:
  virtual ~Resource() = default;

  Resource(const Resource&) = delete;
  Resource& operator=(const Resource&) = delete;

  static void CreateResourceObject(std::unique_ptr<Resource>& r,
                                   uint32_t elements);
  explicit Resource(uint32_t size);
  uint32_t Allocate(const std::string& resourceDesc);
  uint32_t Allocate(uint32_t index, const std::string& resourceDesc);
  void Free(uint32_t index);

 private:
  std::vector<bool> m_isAllocated;
  hal::priority_recursive_mutex m_allocateLock;

  static hal::priority_recursive_mutex m_createLock;
};

}  // namespace frc
