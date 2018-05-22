/*----------------------------------------------------------------------------*/
/* Copyright (c) 2018 FIRST. All Rights Reserved.                             */
/* Open Source Software - may be modified and shared by FRC teams. The code   */
/* must be accompanied by the FIRST BSD license file in the root directory of */
/* the project.                                                               */
/*----------------------------------------------------------------------------*/

#include "wpi/memory.h"

#include <exception>

#include "wpi/raw_ostream.h"

namespace wpi {

void* CheckedCalloc(size_t num, size_t size) {
  void* p = std::calloc(num, size);
  if (!p) {
    errs() << "FATAL: failed to allocate " << (num * size) << " bytes\n";
    std::terminate();
  }
  return p;
}

void* CheckedMalloc(size_t size) {
  void* p = std::malloc(size == 0 ? 1 : size);
  if (!p) {
    errs() << "FATAL: failed to allocate " << size << " bytes\n";
    std::terminate();
  }
  return p;
}

void* CheckedRealloc(void* ptr, size_t size) {
  void* p = std::realloc(ptr, size == 0 ? 1 : size);
  if (!p) {
    errs() << "FATAL: failed to allocate " << size << " bytes\n";
    std::terminate();
  }
  return p;
}

}  // namespace wpi
