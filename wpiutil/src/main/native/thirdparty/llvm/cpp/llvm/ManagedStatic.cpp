//===-- ManagedStatic.cpp - Static Global wrapper -------------------------===//
//
// Part of the LLVM Project, under the Apache License v2.0 with LLVM Exceptions.
// See https://llvm.org/LICENSE.txt for license information.
// SPDX-License-Identifier: Apache-2.0 WITH LLVM-exception
//
//===----------------------------------------------------------------------===//
//
// This file implements the ManagedStatic class and wpi_shutdown().
//
//===----------------------------------------------------------------------===//

#include "wpi/ManagedStatic.h"
#include "wpi/mutex.h"
#include <cassert>
#include <mutex>
using namespace wpi;

static const ManagedStaticBase *StaticList = nullptr;

static wpi::mutex *getManagedStaticMutex() {
  static wpi::mutex m;
  return &m;
}

void ManagedStaticBase::RegisterManagedStatic(void *(*Creator)(),
                                              void (*Deleter)(void*)) const {
  assert(Creator);
  if (1) {
    std::scoped_lock Lock(*getManagedStaticMutex());

    if (!Ptr.load(std::memory_order_relaxed)) {
      void *Tmp = Creator();

      Ptr.store(Tmp, std::memory_order_release);
      DeleterFn = Deleter;

      // Add to list of managed statics.
      Next = StaticList;
      StaticList = this;
    }
  } else {
    assert(!Ptr && !DeleterFn && !Next &&
           "Partially initialized ManagedStatic!?");
    Ptr = Creator();
    DeleterFn = Deleter;

    // Add to list of managed statics.
    Next = StaticList;
    StaticList = this;
  }
}

void ManagedStaticBase::destroy() const {
  assert(DeleterFn && "ManagedStatic not initialized correctly!");
  assert(StaticList == this &&
         "Not destroyed in reverse order of construction?");
  // Unlink from list.
  StaticList = Next;
  Next = nullptr;

  // Destroy memory.
  DeleterFn(Ptr);

  // Cleanup.
  Ptr = nullptr;
  DeleterFn = nullptr;
}

/// wpi_shutdown - Deallocate and destroy all ManagedStatic variables.
/// IMPORTANT: it's only safe to call wpi_shutdown() in single thread,
/// without any other threads executing LLVM APIs.
/// wpi_shutdown() should be the last use of LLVM APIs.
void wpi::wpi_shutdown() {
  std::scoped_lock Lock(*getManagedStaticMutex());
  while (StaticList)
    StaticList->destroy();
}
