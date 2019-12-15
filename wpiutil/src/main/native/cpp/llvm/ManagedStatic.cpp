//===-- ManagedStatic.cpp - Static Global wrapper -------------------------===//
//
//                     The LLVM Compiler Infrastructure
//
// This file is distributed under the University of Illinois Open Source
// License. See LICENSE.TXT for details.
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
static wpi::mutex *ManagedStaticMutex = nullptr;
static std::once_flag mutex_init_flag;

static void initializeMutex() {
  ManagedStaticMutex = new wpi::mutex();
}

static wpi::mutex* getManagedStaticMutex() {
  std::call_once(mutex_init_flag, initializeMutex);
  return ManagedStaticMutex;
}

void ManagedStaticBase::RegisterManagedStatic(void* created,
                                              void (*Deleter)(void*)) const {
  std::scoped_lock Lock(*getManagedStaticMutex());

  if (!Ptr.load(std::memory_order_relaxed)) {
    void *Tmp = created;

    Ptr.store(Tmp, std::memory_order_release);
    DeleterFn = Deleter;

    // Add to list of managed statics.
    Next = StaticList;
    StaticList = this;
  }
}

void ManagedStaticBase::RegisterManagedStatic(void *(*Creator)(),
                                              void (*Deleter)(void*)) const {
  assert(Creator);
  std::scoped_lock Lock(*getManagedStaticMutex());

  if (!Ptr.load(std::memory_order_relaxed)) {
    void *Tmp = Creator();

    Ptr.store(Tmp, std::memory_order_release);
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
void wpi::wpi_shutdown() {
  std::scoped_lock Lock(*getManagedStaticMutex());

  while (StaticList)
    StaticList->destroy();
}
