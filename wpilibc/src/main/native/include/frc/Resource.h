/*----------------------------------------------------------------------------*/
/* Copyright (c) 2008-2018 FIRST. All Rights Reserved.                        */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#pragma once

#include <stdint.h>

#include <memory>
#include <string>
#include <vector>

#include <wpi/mutex.h>

#include "frc/ErrorBase.h"

namespace frc {

/**
 * The Resource class is a convenient way to track allocated resources.
 *
 * It tracks them as indicies in the range [0 .. elements - 1]. E.g. the library
 * uses this to track hardware channel allocation.
 *
 * The Resource class does not allocate the hardware channels or other
 * resources; it just tracks which indices were marked in use by Allocate and
 * not yet freed by Free.
 */
class Resource : public ErrorBase {
 public:
  virtual ~Resource() = default;

  Resource(Resource&&) = default;
  Resource& operator=(Resource&&) = default;

  /**
   * Factory method to create a Resource allocation-tracker *if* needed.
   *
   * @param r        address of the caller's Resource pointer. If *r == nullptr,
   *                 this will construct a Resource and make *r point to it. If
   *                 *r != nullptr, i.e. the caller already has a Resource
   *                 instance, this won't do anything.
   * @param elements the number of elements for this Resource allocator to
   *                 track, that is, it will allocate resource numbers in the
   *                 range [0 .. elements - 1].
   */
  static void CreateResourceObject(std::unique_ptr<Resource>& r,
                                   uint32_t elements);

  /**
   * Allocate storage for a new instance of Resource.
   *
   * Allocate a bool array of values that will get initialized to indicate that
   * no resources have been allocated yet. The indicies of the resources are
   * [0 .. elements - 1].
   */
  explicit Resource(uint32_t size);

  /**
   * Allocate a resource.
   *
   * When a resource is requested, mark it allocated. In this case, a free
   * resource value within the range is located and returned after it is marked
   * allocated.
   */
  uint32_t Allocate(const std::string& resourceDesc);

  /**
   * Allocate a specific resource value.
   *
   * The user requests a specific resource value, i.e. channel number and it is
   * verified unallocated, then returned.
   */
  uint32_t Allocate(uint32_t index, const std::string& resourceDesc);

  /**
   * Free an allocated resource.
   *
   * After a resource is no longer needed, for example a destructor is called
   * for a channel assignment class, Free will release the resource value so it
   * can be reused somewhere else in the program.
   */
  void Free(uint32_t index);

 private:
  std::vector<bool> m_isAllocated;
  wpi::mutex m_allocateMutex;

  static wpi::mutex m_createMutex;
};

}  // namespace frc
